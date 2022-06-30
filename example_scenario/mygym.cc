/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Technische Universität Berlin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Piotr Gawlowicz <gawlowicz@tkn.tu-berlin.de>
 */

#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <sstream>
#include <iostream>
#include <string.h>

namespace ns3 {

void Transmit (Ptr<Socket> socket, uint32_t pktSize)
{
  //std::cout << "Transmit \n";
  socket->Send (Create<Packet> (pktSize));
}

void 
MyGymEnv::ScheduleTransmissions (Ptr<CdmaMac> mac, Ptr<Socket> socket, uint32_t pktSize, Time time_slot)
{
  //std::cout << "ScheduleTransmissions " << "buffer: " << mac->m_buffer << "\n";

  Time now = Simulator::Now();
  if (0 < mac->m_buffer)
  {
    //std::cout << "Scheduled for: " << now + time_slot << "\n";
    Simulator::Schedule (time_slot, &Transmit, socket, m_packet_size);
  }
}

NS_LOG_COMPONENT_DEFINE ("MyGymEnv");

NS_OBJECT_ENSURE_REGISTERED (MyGymEnv);

MyGymEnv::MyGymEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(0.1);

  Simulator::Schedule (Seconds(0.0), &MyGymEnv::ScheduleNextStateRead, this);
}

MyGymEnv::MyGymEnv (Time stepTime, NodeContainer nodes, NetDeviceContainer devs, double simulationTime,
            Time packetInterval, uint32_t packetSize, std::vector<Ptr<Socket>> socket_vector)
{
  NS_LOG_FUNCTION (this);


  m_nodes = nodes;
  m_devs = devs;
  m_simulationTime = simulationTime;
  m_packet_interval = packetInterval;
  m_packet_size = packetSize; 

  m_interval = stepTime;

  m_n_nodes = m_nodes.GetN();
  m_n_pairs = m_n_nodes / 2;

  // Set values for feature normalization
  m_max_interference_caused = 1.75686e-07;
  m_max_interference_sensed = (m_n_pairs - 1) * m_max_interference_caused;
  // Setting min_sinr to 0 since, in ScheduleNextStateRead, we set the sinr feature to 0 if it's less 0.
  m_min_sinr = 0; 
  m_max_sinr = 20;

  m_sinr_vector.resize(m_n_nodes);
  m_interference_vector.resize(m_n_pairs);
  m_interference_sensed.resize(m_n_pairs);
  m_rx_phy_vector.resize(m_n_pairs);
  m_tx_phy_vector.resize(m_n_pairs);
  m_mobility_vector.resize(m_n_nodes); 
  m_position_vector.resize(m_n_nodes);
  m_tx_mac_vector.resize(m_n_pairs);
  m_reward_vector.resize(m_n_pairs);
  m_buffer_vector.resize(m_n_pairs);

  for (uint i = 0; i < m_n_pairs; i++)
  {
    m_n_acks_received_vector.push_back(std::vector<int> {0});
    m_n_successful_transmissions.push_back(0);
  }

  for (uint32_t i = 0; i < m_n_pairs; i++)
  {
    int rx = 2*i;
    int tx = 2*i + 1;

    // Mobility 
    m_mobility_vector.at(rx) = m_nodes.Get(rx)->GetObject<MobilityModel> ();
    m_mobility_vector.at(tx) = m_nodes.Get(tx)->GetObject<MobilityModel> ();

    // Phy
    m_rx_phy_vector.at(i) = m_devs.Get(rx)->GetObject<CdmaNetDevice>()->GetPhy(); 
    m_tx_phy_vector.at(i) = m_devs.Get(tx)->GetObject<CdmaNetDevice>()->GetPhy(); 

    // Tx Mac
    m_tx_mac_vector.at(i) = m_devs.Get(tx)->GetObject<CdmaNetDevice>()->GetMac(); 
  }

  m_n_observations = 2*m_n_nodes + 5*m_n_pairs + 2; // Make sure this matches GetObservationSpace

  m_source_socket_vector = socket_vector;
  m_n_agents = m_n_pairs;
  m_action_vector.resize(m_n_pairs);

  Simulator::Schedule (Seconds(0.0), &MyGymEnv::ScheduleNextStateRead, this);
}


void
MyGymEnv::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  for (size_t i = 0; i < m_n_pairs; i++)
  {
    // Binning sinr to the range [0, m_max_sinr]
    if (m_rx_phy_vector.at(i)->sinr <= 0) 
    {
      m_sinr_vector.at(i) = 0;
    }
    else if (m_rx_phy_vector.at(i)->sinr >= m_max_sinr)
    {
      m_sinr_vector.at(i) = m_max_sinr;
    }
    else
    {
      m_sinr_vector.at(i) = m_rx_phy_vector.at(i)->sinr;
    }

    m_interference_vector.at(i) = m_tx_phy_vector.at(i)->interference_caused_sum;
    m_interference_sensed.at(i) = m_rx_phy_vector.at(i)->interference_sensed_sum;
    //std::cout << "if " << m_interference_sensed.at(i) << "\n";

    m_rx_phy_vector.at(i)->sinr = 0;
    m_tx_phy_vector.at(i)->interference_caused_sum = 0;
    m_rx_phy_vector.at(i)->interference_sensed_sum = 0;

    int last_n_acks = m_n_acks_received_vector.at(i).back();
    int current_n_acks = m_tx_mac_vector.at(i)->m_n_acks_received;
    m_n_acks_received_vector.at(i).push_back(current_n_acks);
    m_n_successful_transmissions.at(i) = current_n_acks - last_n_acks;  

    m_buffer_vector.at(i) = m_tx_mac_vector.at(i)->m_buffer;
  }

  for (size_t i = 0; i < m_mobility_vector.size(); i++)
  {
    m_position_vector.at(i) = m_mobility_vector.at(i)->GetPosition();
  }

  Simulator::Schedule (m_interval, &MyGymEnv::ScheduleNextStateRead, this);
  Notify();
}


MyGymEnv::~MyGymEnv ()
{
  NS_LOG_FUNCTION (this);
}


TypeId
MyGymEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyGymEnv")
    .SetParent<OpenGymEnv> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<MyGymEnv> ()
  ;
  return tid;
}


void
MyGymEnv::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}


Ptr<OpenGymSpace>
MyGymEnv::GetObservationSpace()
/*
* Let x be the number of transmitting nodes and y be the total number of nodes. 
* For each node there are 2 position cooridates. 
* 
* For each transmitting node there is 1 SINR value, 1 interference-caused value, 
* 2 buffer values, 1 Tx Power
*

* 1 interference-sensed value
* 1 buffer value
* 
* Thus, the number of observations is 2y + (1 + 1 + 2 + 1)x + 2 = 2y + 5x + 2.
*/
{
  float low = std::numeric_limits<double>::min();
  float high = std::numeric_limits<double>::max();

  std::cout << "OBS: " << m_n_observations << std::endl;

  std::vector<uint32_t> shape = {m_n_observations,};
  std::string dtype = TypeNameGet<double> ();

  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  //NS_LOG_UNCOND ("MyGetObservationSpace: " << box);

  return box;
}

/*
Define action space

Description: 
*/
Ptr<OpenGymSpace>
MyGymEnv::GetActionSpace()
{
  float low = -1;
  float high = 100;

  std::vector<uint32_t> shape = {m_n_pairs,};
  std::string dtype = TypeNameGet<float> ();

  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  //NS_LOG_UNCOND ("MyGetActionSpace: " << box); 
  return box;
 }

void
write_througput(std::vector<std::vector<int>> throughput)
{
  std::ofstream myFile("througput.csv");
  
  for(auto& mac:throughput)
  {
   for(auto& t:mac){ // t is the number of acks received by this MAC at a particular time step
      myFile << t << ",";
   }
   myFile << "\n";
  }

  myFile.close();
}

/*
Define game over condition
*/
bool
MyGymEnv::GetGameOver()
{
  bool isGameOver = false;
  static int stepCounter = 0.0; 

  stepCounter += 1;
  if (stepCounter == m_simulationTime) {
    isGameOver = true;
    write_througput(m_n_acks_received_vector); 
  }

  //NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations.
*/
Ptr<OpenGymDataContainer>
MyGymEnv::GetObservation()
{
  std::vector<uint32_t> shape = {m_n_observations,};
  Ptr<OpenGymBoxContainer<double> > box = CreateObject<OpenGymBoxContainer<double> >(shape);

  for (uint32_t i = 0; i < m_n_nodes; i++)
  {
    box->AddValue(m_position_vector.at(i).x);
    box->AddValue(m_position_vector.at(i).y);
  }
  // Normal loop
  for (uint32_t i = 0; i < m_n_pairs; i++)
  {
    if (m_action_vector.at(i) != -1) // Transmitted on last agent-step
    {
      box->AddValue( (m_sinr_vector.at(i) - m_min_sinr) / (m_max_sinr - m_min_sinr) );
      box->AddValue(m_interference_vector.at(i) / m_max_interference_caused);
      box->AddValue(m_n_successful_transmissions.at(i)); 
      box->AddValue(m_tx_phy_vector.at(i)->GetTxPower());
    }
    else // Did not transmit on last agent-step
    {
      box->AddValue(0); // SINR
      box->AddValue(0); // interference-caused
      box->AddValue(0); // n_successful_transmissions
      box->AddValue(-1); // Tx power
    }

    // interference-sensed
    box->AddValue(m_interference_sensed.at(i) / m_max_interference_sensed);
    
    // Buffer size must be the last element in the box to know whether or not to use the RL agent
    box->AddValue(m_buffer_vector.at(i));
  }

  //NS_LOG_UNCOND ("GetObservation Box: " << box);
  return box;
}

/*
Define reward function
* Ns3-gym requires that GetReward function returns a float. This is incompatible with the distributed setting.
* This implementation updates the m_reward_vector memeber variable. This variable is then passed to the Python-side
* as a string using the GetExtraInfo function.
*/
float
MyGymEnv::GetReward()
{
  //static float reward = 0.0;
  //reward += 1;

  std::vector<double> reward_vector;
  float reward;
  
  // Loop over transmitter
  for (size_t i = 0; i < m_n_pairs; i++)
  {
    // Action taken with RL
    if (m_action_vector.at(i) >= 0)
    {
      //std::cout << "m_n_successful_transmissions: " << m_n_successful_transmissions.at(i) <<"\n";
      //
      if (m_n_successful_transmissions.at(i) > 0)
      { 
        reward = (m_sinr_vector.at(i) - m_min_sinr) / (m_max_sinr - m_min_sinr) -
        (m_interference_vector.at(i) / m_max_interference_caused); 
        NS_LOG_DEBUG("Reward case successful: " << reward);
      }
      // Unsuccessful transmission
      else
      {
        reward = -1 * (m_interference_vector.at(i) / m_max_interference_caused); 
        NS_LOG_DEBUG("Reward case not successful: " << reward);
      }
    }
    else // No RL
    {
      reward = 0;
      NS_LOG_DEBUG("Reward: No RL" << reward);
    }
    NS_LOG_INFO("reward: " << reward);
    m_reward_vector.at(i) = reward;
  }

  //NS_LOG_UNCOND("Current Reward: " << reward);
  return 0;
}

/*
Define reward function -- Copy
* Ns3-gym requires that GetReward function returns a float. This is incompatible with the distributed setting.
* This implementation updates the m_reward_vector memeber variable. This variable is then passed to the Python-side
* as a string using the GetExtraInfo function.
*/
/*
float
MyGymEnv::GetReward()
{
  //static float reward = 0.0;
  //reward += 1;

  std::vector<double> reward_vector;
  float reward;
  
  // Loop over transmitter
  for (size_t i = 0; i < m_n_pairs; i++)
  {
    // Action taken with RL
    if (m_action_vector.at(i) >= 0)
    {
      //std::cout << "m_n_successful_transmissions: " << m_n_successful_transmissions.at(i) <<"\n";
      //
      if (m_n_successful_transmissions.at(i) > 0)
      { 
        reward = (m_sinr_vector.at(i) - m_min_sinr) / (m_max_sinr - m_min_sinr) -
        (m_interference_vector.at(i) / m_max_interference_caused); 
        NS_LOG_DEBUG("Reward case successful: " << reward);
      }
      // Unsuccessful transmission
      else
      {
        reward = -1 * (m_interference_vector.at(i) / m_max_interference_caused); 
        NS_LOG_DEBUG("Reward case not successful: " << reward);
      }
    }
    else // No RL
    {
      reward = 0;
      NS_LOG_DEBUG("Reward: No RL" << reward);
    }
    NS_LOG_INFO("reward: " << reward);
    m_reward_vector.at(i) = reward;
  }

  //NS_LOG_UNCOND("Current Reward: " << reward);
  return 0;
}
*/

/*
Define extra info. Optional
*/
std::string
MyGymEnv::GetExtraInfo()
// Using this function to pass reward information
{
  std::string reward_string = "";
 
  for (size_t i = 0; i < m_reward_vector.size(); i++)
  {
    reward_string += std::to_string(m_reward_vector[i]);
    if (i + 1 != m_reward_vector.size())
    {
      reward_string += ",";
    }
  }

  //NS_LOG_UNCOND("MyGetExtraInfo: " << reward_string);
  return reward_string;
}


/*
Execute received action with the mapping:
* Action -1 --> buffer = 0, RL not used
* Action 0 --> transmit now at 1 dBm
* Action 1 --> transmit now at 10 dBm
* Action 2 --> transmit now at 20 dBm
*/
bool
MyGymEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  // Change data type from int to float
  Ptr<OpenGymBoxContainer<float> > box = DynamicCast<OpenGymBoxContainer<float> >(action);
  std::vector<float> actionVector = box->GetData();
  m_action_vector = actionVector;
  
  double tx_power;
  int agent_action;
  for (std::size_t i = 0; i < m_n_agents; i++)
  {
    agent_action = int(actionVector.at(i)); 
    NS_LOG_DEBUG("agent_action: " << agent_action);

    if (agent_action != -1)
    {
      // Assign TX power
      if (agent_action == 0)
      {
        tx_power = 1;
      }
      else if (agent_action == 1)
      {
        tx_power = 10;
      }
      else 
      {
        tx_power = 20;
      }
      m_tx_phy_vector.at(i)->SetDataTxPower(tx_power);
    }

    // TEST
    /*
    if (i == 0)
    {
      tx_power = 10;   
    }
    else if (i == 1)
    {
      tx_power = 1;
    }
    else
    {
      tx_power = 1;
    }
    tx_power = 10;
    */
    m_tx_phy_vector.at(i)->SetDataTxPower(tx_power);
    Ptr<CdmaMac> mac = m_tx_mac_vector.at(i);
    Ptr<Socket> socket = m_source_socket_vector.at(i);
    ScheduleTransmissions(mac, socket, m_packet_size, Seconds(0)); 
  }

  return true;
}

} // ns3 namespace
