/* ------------------------------------------------------------------------------ 
* Copyright 2022 ANDRO Computational Solutions, LLC.  All Rights Reserved
 ------------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------------
This file is part of the MR-iNET software codebase released for public use under
the [Creative Commons Attribution - NonCommercial - ShareAlike 4.0 (CC BY-NC-SA 4.0) License]
(https://creativecommons.org/licenses/by-nc-sa/4.0/).
 
This material is based upon work supported by the US Army Contract No. W15P7T-20-C-0006.
Any opinions, findings, and conclusions or recommendation expressed in this material
are those of the author(s) and do not necessarily reflect the views of the US Army.
* ------------------------------------------------------------------------------
*/

/*
* I-ROAM
** Author: Collin Farquhar
** Email: cfarquhar@androcs.com
** File description: Using this simulation to find SINR and interference bounds
** File: test/simple_test.cc
*/
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/cdmanew-module.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
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

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval) 
{
  if (pktCount > 0)
  {
    //socket->Send (packet); // Use with the CdmaMacHeader code block 

    socket->Send (Create<Packet> (pktSize));
    Simulator::Schedule (pktInterval, &GenerateTraffic,
                         socket, pktSize, pktCount - 1, pktInterval);
  }
  else
  {
    socket->Close ();
  }
}

int main (int argc, char *argv[])
{
  RngSeedManager::SetSeed (1);

  uint32_t n_pairs = 3;
  uint32_t packetSize = 1024; // bytes
  uint32_t numPackets = 10;
  double interval = 0.05; // seconds
  bool verbose = false;

  received_vector.resize(n_pairs, 0); // fills with zeros

  CommandLine cmd;
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.Parse (argc, argv);

  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // Create nodes
  NodeContainer nodes;
  uint32_t n_nodes = n_pairs * 2;
  nodes.Create (n_nodes);

  // <----------------> Create channels and net devices <---------------->
  NS_LOG_INFO ("Create channels.");
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (7));

  Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);

  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());
  CdmaHelper cdma;
  NetDeviceContainer d = cdma.Install (nodes, channel);

  // <----------------> Mobility <---------------->
  MobilityHelper mobilityHelper;

  // for constant mobility model
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); 
  
  mobilityHelper.Install (nodes); // This line is required for any mobility model

  std::vector<int> x_positions = {0, 100, 1000};

  // To set positions 
  for (uint32_t i = 0; i < x_positions.size(); i++)
  {
    int rxNodeId = 2*i;
    int txNodeId = 2*i + 1;
    int x = x_positions.at(i);
    nodes.Get (rxNodeId)->GetObject<MobilityModel> ()->SetPosition (Vector (x, 10, 0));
    nodes.Get (txNodeId)->GetObject<MobilityModel> ()->SetPosition (Vector (x, 0, 0));
  }
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
  Ipv4InterfaceContainer Ipv4_containter = ipv41.Assign (d);

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

    Simulator::ScheduleWithContext (nodes.Get (source_idx)->GetId (),
                                    Seconds (0.0), &GenerateTraffic,
                                    source, packetSize, numPackets, interPacketInterval);
  }

  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();
  Simulator::Destroy ();

  std::ofstream outfile;
  outfile.open("transmission_info.csv", std::ios_base::app);

  for (size_t i = 0; i < received_vector.size(); i++)
  {
    std::cout<< "Throughput of Tx-Rx pair " << i << " = " << received_vector.at(i) << std::endl;
    outfile << received_vector.at(i);
    if (i != (received_vector.size() - 1))
    {
      outfile << ",";
    }
  }

  return 0;
}