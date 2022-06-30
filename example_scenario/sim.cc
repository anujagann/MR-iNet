/* ------------------------------------------------------------------------------
* Copyright 2020 ANDRO Computational Solutions, LLC.  All Rights Reserved
 ------------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------------
* Distribution Statement D:
*  Distribution authorized to Department of Defense and U.S. DoD contractors only (reason) (June 26, 2020).
*  Other requests for this document shall
*  be referred to (controlling DoD office)
* ------------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------------
* Government Rights: GOVERNMENT PURPOSE RIGHTS
* Contract Number: W15P7T-20-C-0006
* Contractor Name: ANDRO Computational Solutions, LLC.
* Contractor Address: The Beeches Professional Campus
*                     One Beeches Place 7980 Turin Rd. Bldg. 1
*                     Rome, NY 13440
* Expiration Date: July 2022
* The Government’s rights to use, modify, reproduce, release, perform, display,
* or disclose these technical data are restricted by paragraph (b)(2) of the Rights
* in Technical Data-Noncommercial Items clause contained in the above
* identified contract. No restrictions apply after the expiration date shown
* above. Any reproduction of technical data or portions thereof marked with
* this legend must also reproduce the markings.
* ------------------------------------------------------------------------------
*/
/*
* I-ROAM
** Author: Collin Farquhar
** Email: cfarquhar@androcs.com
** File description: NS3 simulation for ns3-gym scenario.
** File: sim.cc
** Date Modified: March 2021
*/

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/cdmanew-module.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/cdmanew-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"

#include "ns3/opengym-module.h"
#include "mygym.h"
#include <typeinfo>
#include <iomanip>

/* Node topology
    RX0 --10-- RX2 ------90------RX4
     |          |                 |
     10         10                10
     |          |                 |
    TX1 --10-- TX3 ------90------TX5
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CDMASimple");
//LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

std::vector<int> received_vector;
void ReceiveCallback (Ptr<Socket> socket)
{ 
  while (socket->Recv ())
  {
    //NS_LOG_UNCOND ("Received one packet!");
  }

  int node_id = socket->GetNode ()->GetId ();
  int pair_id = node_id / 2;
  received_vector.at(pair_id) += 1;
}

void BufferSchedule(Ptr<CdmaMac> mac, Time generationInterval)
{

  if (mac->m_buffer < mac->m_max_buffer)
  {
    mac->m_buffer += 1; 
  }
  Simulator::Schedule(generationInterval, &BufferSchedule, mac, generationInterval);
}

int main (int argc, char *argv[])
{
  //LogComponentEnable ("MyGymEnv", LOG_LEVEL_DEBUG); // Useful for debugging

  uint32_t n_pairs = 3;
  uint32_t n_nodes = 2 * n_pairs;
  uint32_t packetSize = 1024; // bytes
  uint32_t numPackets = 1e6;
  double generation_interval = 1; // Add to the buffer every x seconds 
  double packet_interval = 1; // Packet interval (seconds).
  bool verbose = false;
  uint32_t openGymPort = 5555;
  uint32_t simSeed = 1;
  double simulationTime = 1e4; //seconds
  double envStepTime = 1; //seconds, ns3gym env step time interval

  received_vector.resize(n_pairs, 0); // fills with zeros

  CommandLine cmd;
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", packet_interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  // for OpenGym interface
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 5555", openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  cmd.AddValue ("stepTime", "Gym Env step time in seconds. Default: 0.1s", envStepTime);
  cmd.Parse (argc, argv);

  // Convert to time objects
  Time interPacketInterval = Seconds (packet_interval);
  Time generation_interval_time = Seconds(generation_interval);

  // Create nodes
  NodeContainer nodes;
  nodes.Create (n_nodes);

  // <----------------> Create channels and net devices <---------------->
  NS_LOG_INFO ("Create channels.");
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (1)); 
  /* // Turning off the added randomness
  Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);
  */
  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());
  CdmaHelper cdma;
  NetDeviceContainer devs = cdma.Install (nodes, channel);

  // <----------------> Mobility <---------------->
  MobilityHelper mobilityHelper;

  // for constant mobility model
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); 
  
  mobilityHelper.Install (nodes); // This line is required for any mobility model

  // To set positions 
  nodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 10, 0));
  nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
  nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (10, 10, 0));
  nodes.Get (3)->GetObject<MobilityModel> ()->SetPosition (Vector (10, 0, 0));
  nodes.Get (4)->GetObject<MobilityModel> ()->SetPosition (Vector (100, 10, 0));
  nodes.Get (5)->GetObject<MobilityModel> ()->SetPosition (Vector (100, 0, 0));

  // For mobility tracing:
  //Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback (&CourseChange));

  // <----------------> Install internet with helper <---------------->
  InternetStackHelper internet;
  internet.Install (nodes);

  // <----------------> Set sources and sinks <---------------->
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

  Ipv4AddressHelper ipv41;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv41.SetBase ("10.1.0.0", "255.255.0.0");
  Ipv4InterfaceContainer Ipv4_containter = ipv41.Assign (devs);

  std::vector<Ptr<Socket>> socket_vector; 
  for (uint32_t i = 0; i < n_pairs; i++)
  {
    uint32_t sink_idx = 2*i;
    uint32_t source_idx = 2*i + 1;
    uint32_t port = i + 1;

    Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get (sink_idx), tid);
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), port);
    recvSink->Bind (local);
    recvSink->SetRecvCallback (MakeCallback (&ReceiveCallback));

    Ptr<Socket> source = Socket::CreateSocket (nodes.Get (source_idx), tid);
    InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), port);
    source->SetAllowBroadcast (true);
    source->Connect (remote);

    socket_vector.push_back(source); 

    Ptr<CdmaNetDevice> transmit_dev = devs.Get(source_idx)->GetObject<CdmaNetDevice> ();
    Ptr<CdmaMac> transmit_mac = transmit_dev->GetMac();  
    // Packet generation for agent (increments m_buffer)
    Simulator::Schedule (Seconds(1.0), &BufferSchedule, transmit_mac, generation_interval_time);

    // Set TX power
    transmit_dev->GetPhy()->SetAttribute("TxPower", DoubleValue (10));  
  }
  // ------------------------------------------------------------

  // OpenGym Env
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
  Ptr<MyGymEnv> myGymEnv = CreateObject<MyGymEnv> (Seconds(envStepTime), nodes, devs, simulationTime,
                                                   Seconds(packet_interval), packetSize, socket_vector);

  myGymEnv->SetOpenGymInterface(openGymInterface);

  Simulator::Stop (Seconds (simulationTime));
  Simulator::Run ();
  Simulator::Destroy ();

  for (size_t i = 0; i < received_vector.size(); i++)
  {
    std::cout<< "Throughput of Tx-Rx pair " << i << " = " << received_vector.at(i) << std::endl;
  }
  return 0;
}