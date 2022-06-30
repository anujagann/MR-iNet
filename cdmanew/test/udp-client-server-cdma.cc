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
** File description: test udp client server
** File: model/udp-client-server-cdma.cc
*/


// Network topology
//
//       n0    n1
//       |     |
//       =======
//         CDMA
//
// - UDP flows from n0 to n1

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/cdmanew-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpClientServerExample");

int
main (int argc, char *argv[])
{
//
// Enable logging for UdpClient and Server
//
  LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
  //LogComponentEnable ("CdmaChannel", LOG_LEVEL_INFO);
  bool useV6 = false;
  Address serverAddress;

  CommandLine cmd;
  cmd.AddValue ("useIpv6", "Use Ipv6", useV6);
  cmd.Parse (argc, argv);

//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (2);
  InternetStackHelper internet;
  internet.Install (nodes);
  NS_LOG_INFO ("Create channels.");
//
// Explicitly create the channels required by the topology (shown above).
//
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
     // 2. Place nodes somehow, this is required by every wireless simulation
  //for (uint8_t i = 0; i < 2; ++i)
  //   {
    //   nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
   //  }
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (1));
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
  nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (10, 0, 0));
 


// We've got the "hardware" in place.  Now we need to add IP addresses.
//


  NS_LOG_INFO ("Assign IP Addresses.");
  if (useV6 == false)
    {
      Ipv4AddressHelper ipv4;
      ipv4.SetBase ("10.1.1.0", "255.255.255.0");
      Ipv4InterfaceContainer i = ipv4.Assign (d);
      serverAddress = Address (i.GetAddress (0));
    }
  else
    {
      Ipv6AddressHelper ipv6;
      ipv6.SetBase ("2001:0000:f00d:cafe::", Ipv6Prefix (64));
      Ipv6InterfaceContainer i6 = ipv6.Assign (d);
      serverAddress = Address(i6.GetAddress (1,1));
    }

  NS_LOG_INFO ("Create Applications.");
//
// Create one udpServer applications on node one.
//
  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (nodes.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));
  
 // Ptr<MobilityModel> model1 = nodes.Get(0)->GetObject<MobilityModel>();

 // Ptr<MobilityModel> model2 = nodes.Get(1)->GetObject<MobilityModel>();

 // double distance = model1->GetDistanceFrom (model2);

 //std::cout << distance << std::endl;
//
// Create one UdpClient application to send UDP datagrams from node zero to
// node one.
//
  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (0.0005);
  uint32_t maxPacketCount = 320;
  UdpClientHelper client (serverAddress, port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = client.Install (nodes.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

//
// Now, do the actual simulation.
//

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}


