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


#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/nstime.h"
#include <ns3/cdma-phy-derived.h>
#include <ns3/socket.h>
#include "ns3/net-device-container.h"

namespace ns3 {

class MyGymEnv : public OpenGymEnv
{
public:
  MyGymEnv ();
  MyGymEnv (Time stepTime, NodeContainer nodes, NetDeviceContainer devs, double simulationTime,
            Time packetInterval, uint32_t packetSize, std::vector<Ptr<Socket>> socket_vector);
  virtual ~MyGymEnv ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  bool GetGameOver();
  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);
  
  void ScheduleTransmissions (Ptr<CdmaMac> mac, Ptr<Socket> socket, uint32_t pktSize, Time time_slot);

  // Adding attributes
  double m_max_interference_caused;
  double m_max_interference_sensed;
  double m_min_sinr;
  double m_max_sinr; 
  uint32_t m_n_nodes;
  uint32_t m_n_pairs;
  uint32_t m_n_agents;
  std::vector<double> m_sinr_vector;
  std::vector<double> m_interference_vector;
  std::vector<double> m_interference_sensed;
  std::vector< Ptr<CdmaPhy> > m_rx_phy_vector;
  std::vector< Ptr<CdmaPhy> > m_tx_phy_vector;
  std::vector< Ptr<MobilityModel> > m_mobility_vector;
  std::vector<ns3::Vector> m_position_vector;
  std::vector< Ptr<CdmaMac> > m_tx_mac_vector;
  uint32_t m_n_observations;
  std::vector<double> m_reward_vector;
  std::vector< std::vector<int> > m_n_acks_received_vector;
  std::vector<int> m_n_successful_transmissions;

  std::vector<Ptr<Socket>> m_source_socket_vector;
  Time m_packet_interval;
  uint32_t m_packet_size; 
  std::vector<float> m_action_vector;

  // Refactor
  NodeContainer m_nodes;
  NetDeviceContainer m_devs;
  double m_simulationTime;

  std::vector<int> m_buffer_vector;
  

private:
  void ScheduleNextStateRead();

  Time m_interval;
};

}

#endif // MY_GYM_ENTITY_H
