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
** Author: Kian Hamedani
** Email:  khamedani@androcs.com
** File description: example interference
** File: test/cdma-interference.cc
** Date Modified: Aug 2020
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
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CDMASimple");
  //LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

int p=0;
void ReceiveCallback (Ptr<Socket> socket)
{ 
  //std::cout << "Hello"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
    }
    p+=1;

}
int tt=0;
void ReceiveCallback1 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
    }
        tt+=1;

}
static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{
  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

int main (int argc, char *argv[])
{
  double rss = 10;  // -dBm
  uint32_t packetSize = 1024; // bytes
  uint32_t numPackets =4000;
  double interval = 0.0005; // seconds
  bool verbose = false;

  CommandLine cmd;
  cmd.AddValue ("rss", "received signal strength", rss);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);
  NodeContainer nodes;
  nodes.Create (2);


  NS_LOG_INFO ("Create channels.");
//
// Explicitly create the channels required by the topology (shown above).
//
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
     // 2. Place nodes somehow, this is required by every wireless simulation
 // for (uint8_t i = 0; i < 2; ++i)
    // {
      // nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
   //  }
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (0.5));
   Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);
  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());
  CdmaHelper cdma;
  NetDeviceContainer d = cdma.Install (nodes,channel);

  MobilityHelper mobilityHelper;
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper.Install (nodes);
  nodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
  nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));

  InternetStackHelper internet;
  internet.Install (nodes);

  NodeContainer nodes1;
  nodes1.Create (2);
  NetDeviceContainer d1 = cdma.Install (nodes1,channel);
  MobilityHelper mobilityHelper1;
  mobilityHelper1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper1.Install (nodes1);
  nodes1.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (-1, 0, 0));
  nodes1.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (1000, 0, 0));
  InternetStackHelper internet1;
  internet1.Install (nodes1);

  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (d);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceiveCallback));

  Ptr<Socket> source = Socket::CreateSocket (nodes.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  Ipv4AddressHelper ipv41;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv41.SetBase ("10.1.0.0", "255.255.0.0");
  Ipv4InterfaceContainer i1 = ipv41.Assign (d1);
   TypeId tid1 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink1 = Socket::CreateSocket (nodes1.Get (0), tid1);
  InetSocketAddress local1 = InetSocketAddress (Ipv4Address::GetAny (), 49000);
  recvSink1->Bind (local1);
  recvSink1->SetRecvCallback (MakeCallback (&ReceiveCallback1));

    Ptr<Socket> interferer = Socket::CreateSocket (nodes1.Get (1), tid1);
  InetSocketAddress interferingAddr = InetSocketAddress (Ipv4Address ("255.255.255.255"), 49000);
 interferer->SetAllowBroadcast (true);
  interferer->Connect (interferingAddr);


  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                         Seconds (7.0), &GenerateTraffic,
                             source, packetSize, numPackets, interPacketInterval);
  Simulator::ScheduleWithContext (interferer->GetNode ()->GetId (),
                              Seconds (7.5), &GenerateTraffic,
                              interferer, packetSize, numPackets, interPacketInterval);


  Simulator::Stop (Seconds (9.0));
  Simulator::Run ();
  Simulator::Destroy ();
      std::cout<< "Throughput of the first Tx-Rx pair=" << p << std::endl;
          std::cout<< "Throughput of the second Tx-Rx pair=" << tt << std::endl;

  return 0;
}
