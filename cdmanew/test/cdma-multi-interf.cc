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
** File description: test
** File: test/cdma-multi-interf.cc
*/
 
 // Network topology
 //
 //                  
 //                
 //                  
 //       n0   n1   n2   n3   n4
 //       |    |    |    |    |
 //       ======================
 //                 CDMA
 //
 // - Multicast source is at node n0 and n1;
 // - Nodes n1, n2, n3, and n4 receive the multicast frame.
 // - Node n4 listens for the data 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/cdmanew-module.h"
#include <string>


using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("CdmaMulticast");

int main (int argc, char *argv[])
{ string applicationType = "onoff";             /* Type of the Tx application */
  uint32_t payloadSize = 1024; //bytes
  uint64_t simulationTime = 10; //seconds
  double distance = 0.0000000001; //meters
  double number = 5;
 // string socketType = "ns3::UdpSocketFactory";  /* Socket Type (TCP/UDP) */

  CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("distance", "Distance in meters between the station and the access point", distance);
  cmd.AddValue ("numberStas", "Distance in meters between the station and the access point", number); 
  cmd.Parse (argc, argv);


  NodeContainer cdmaApNode;
  cdmaApNode.Create (1);
  NodeContainer cdmaInterf;
  cdmaInterf.Create(1);
  //NodeContainer c;
 // c.Create (number);
  NodeContainer c0;
  c0.Create(3);
  NodeContainer c1;
  c1.Create(2);
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (0.5));
  Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);
  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());



  CdmaHelper cdma;

  
  NetDeviceContainer apDevice;
   NetDeviceContainer interfDevice;
  NetDeviceContainer RxDevices1;
  NetDeviceContainer RxDevices2;

  apDevice = cdma.Install (cdmaApNode,channel);
   
  interfDevice = cdma.Install(cdmaInterf, channel);

  RxDevices1 = cdma.Install(c0, channel);
  RxDevices2 = cdma.Install(c1, channel);
  /* Setting mobility model, Initial Position 1 meter apart */
      MobilityHelper mobility;
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
     mobility.Install (c0);
      mobility.Install (c1);
      mobility.Install (cdmaApNode);
      mobility.Install (cdmaInterf);

	  cdmaApNode.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 450,0)); //TX1
	  cdmaInterf.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 250, 0));  //TX2

	  c0.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 10,0));  //RX1
	 c0.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 20, 0));    //RX2
	  c0.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 30, 0));   //RX3
	  c1.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 40, 0));  //RX4
	  c1.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 50, 0));  //RX5


      /* Internet stack*/
      InternetStackHelper stack;
      stack.Install (cdmaApNode);
      stack.Install (cdmaInterf);
      stack.Install (c0);
      stack.Install (c1);

      Ipv4AddressHelper address;
      address.SetBase ("10.0.0.0", "255.255.255.0");
      Ipv4InterfaceContainer apInterface;
      apInterface = address.Assign (apDevice);
      Ipv4InterfaceContainer staInterface1;
      staInterface1 = address.Assign (RxDevices1);
      NS_LOG_INFO ("Configure multicasting.");


      Ipv4Address multicastSource ("10.0.0.1");
      Ipv4Address multicastGroup ("224.1.2.4");

      Ipv4AddressHelper address1;
      address1.SetBase ("10.1.1.0", "255.255.255.0");
      Ipv4InterfaceContainer apInterface1;
      apInterface1 = address1.Assign (interfDevice);
      Ipv4InterfaceContainer staInterface2;
      staInterface2 = address1.Assign (RxDevices2);
      NS_LOG_INFO ("Configure multicasting.");
      Ipv4Address multicastSource1 ("10.1.1.1");

      Ipv4Address multicastGroupInterf ("225.1.2.5");
      
       Ipv4StaticRoutingHelper multicast;
        // 2) Set up a default multicast route on the sender n0 
     Ptr<Node> sender = cdmaApNode.Get (0);
     Ptr<NetDevice> senderIf = apDevice.Get(0);
     multicast.SetDefaultMulticastRoute (sender, senderIf);

       Ipv4StaticRoutingHelper multicastInterf;

        // 2) Set up a default multicast route on the sender n1
     Ptr<Node> interf = cdmaInterf.Get (0);

     Ptr<NetDevice> interfIf = interfDevice.Get(0);

     multicastInterf.SetDefaultMulticastRoute (interf, interfIf);

   //  Ptr<MobilityModel> model1 = cdmaApNode.Get(0) -> GetObject <MobilityModel>;
         //  std::cout << "Position=" << cdmaApNode.Get(0) -> GetPosition() << std::endl;
      /* Populate routing table */
     //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
     NS_LOG_INFO ("Create Applications.");

  uint16_t multicastPort = 9;   // Discard port (RFC 863)
     
  uint16_t multicastPortInterf = 10; 
      

      // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
  Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("0.600Mbps")));
  onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  ApplicationContainer srcC = onoff.Install (cdmaApNode);

      // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff1 ("ns3::UdpSocketFactory", 
  Address (InetSocketAddress (multicastGroupInterf, multicastPortInterf)));
  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("0.600Mbps")));
  onoff1.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  ApplicationContainer srcCInterf = onoff1.Install (cdmaInterf);

  //
  // Tell the application when to start and stop.
  //
  srcC.Start(Seconds(1.));
  srcC.Stop (Seconds(10.));

  srcCInterf.Start(Seconds(1.));
  srcCInterf.Stop (Seconds(10.));
// Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress ("224.1.2.4", multicastPort));
   
  ApplicationContainer sinkC = sink.Install (c0);
Ptr<PacketSink> sinkPacket = StaticCast<PacketSink> (sinkC.Get(2));

  PacketSinkHelper sinkInterf ("ns3::UdpSocketFactory",
                         InetSocketAddress ("225.1.2.5", multicastPortInterf));
   

  ApplicationContainer sinkCInterf = sinkInterf.Install (c1); 
Ptr<PacketSink> sinkPacketInterf = StaticCast<PacketSink> (sinkCInterf.Get(1));
  // Start the sink
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (10.0));
  sinkCInterf.Start (Seconds (1.0));
  sinkCInterf.Stop (Seconds (10.0));
      Simulator::Stop (Seconds (simulationTime));
      Simulator::Run ();

      /* Calculate Throughput */
      cout  << "Number of Packets Received from TX1 = " << int((sinkPacket->GetTotalRx ())/payloadSize) << endl;
      cout  << "Number of Packets Received from TX2 = " << int((sinkPacketInterf->GetTotalRx ())/payloadSize) << endl;
      Simulator::Destroy ();
  

  return 0;
}
