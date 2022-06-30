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
** File: test/cdma-uni-clean.cc
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
#include "ns3/names.h"


using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("CdmaMulticast");

int main (int argc, char *argv[])
{ string applicationType = "onoff";             /* Type of the Tx application */
  uint32_t payloadSize = 1060; //bytes
  double simulationTime = 9; //seconds
  uint32_t number;
std::cout << "Please enter the number of nodes: " << std::endl;
 std::cin >> number;
  uint32_t nTx;
std::cout << "Please enter the number of TXs: " << std::endl;
 std::cin >> nTx;
  uint32_t nRx= number - nTx;
std::cout << "The number of RXs is equal to: " << nRx << std::endl;
  CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.Parse (argc, argv);


  NodeContainer cdmaNode;
  cdmaNode.Create (number);

  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (0.5));
  Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);
  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());



  CdmaHelper cdma;

  
  NetDeviceContainer CdmaDevice;
  

  CdmaDevice = cdma.Install (cdmaNode,channel);
   
   MobilityHelper mobilityHelper;
  mobilityHelper.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper.Install (cdmaNode);

      /* Internet stack*/
      InternetStackHelper stack;
      stack.Install (cdmaNode);


      Ipv4AddressHelper address;
      address.SetBase ("10.0.0.0", "255.255.255.0");
      Ipv4InterfaceContainer apInterface;
      apInterface = address.Assign (CdmaDevice);

      NS_LOG_INFO ("Configure uniicasting.");

      
      uint32_t sink_idx;
      uint32_t nSlots =3.0;
    //  double step = simulationTime/double(nSlots);
      std::vector <Ptr<PacketSink>> sinkRX;
      std::vector <uint32_t> nodeID;
      std::vector <uint32_t> nodeIDTx;
      std::vector <uint32_t> Slotnum;
 for (uint32_t j=0; j<nSlots ; j++){                
      for (uint32_t i=0; i<nTx; i++){
             nodeIDTx.push_back(i);
             Slotnum.push_back(j);
             Ipv4StaticRoutingHelper multicast;
             Ptr<Node> sender = cdmaNode.Get (i);
             Ptr<NetDevice> senderIf = CdmaDevice.Get (i);
             multicast.SetDefaultMulticastRoute (sender, senderIf);
             string addr1="224.1.2."+std::to_string(i+4);
             std::cout << "Group Address=" << addr1 << std::endl;
             OnOffHelper onoff ("ns3::UdpSocketFactory", 
             Address (InetSocketAddress (Ipv4Address (addr1.c_str()), i+1)));                                  ////// Configuring TXs and assigning a group address to each TX
	     onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	     onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	     onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("0.600Mbps")));
	     onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));
             ApplicationContainer srcC = onoff.Install (cdmaNode.Get(i));


             srcC.Start(Seconds(0.));
             srcC.Stop (Seconds(simulationTime));
          

             std::cout << "Enter the node id of the Rx connecting to "+ std::to_string(i)+ " Tx in "+std::to_string(j)+ " time slot=";
             cin >> sink_idx; 
             nodeID.push_back(sink_idx);
             PacketSinkHelper sink ("ns3::UdpSocketFactory",
             InetSocketAddress (Ipv4Address (addr1.c_str()), i+1));
             ApplicationContainer sinkC = sink.Install (cdmaNode.Get(sink_idx)); 
             Ptr<PacketSink> sinkPacketInterf = StaticCast<PacketSink> (sinkC.Get(0)); 
             if (j==0){
	     sinkC.Start (Seconds (0.0));
	     sinkC.Stop (Seconds (2.0));}
	     else if(j==1){
                      sinkC.Start (Seconds (2.0));
	              sinkC.Stop (Seconds (4.0));
                         }else if(j==2){
                       sinkC.Start (Seconds (4.0));
	              sinkC.Stop (Seconds (9.0));
                                       }
             sinkRX.push_back(sinkPacketInterf);  ////// nTx <= sink_idx =< number
             cout << "Hello ANtonio" << sinkRX.size() << std::endl;

            }
}

       Simulator::Stop (Seconds (simulationTime));
       Simulator::Run ();

       auto sinkIterator = sinkRX.begin();

       uint32_t counter=0;
       for (; sinkIterator!=sinkRX.end() ; sinkIterator++){

      cout  << "Number of Packets Received from TX"+std::to_string(nodeIDTx[counter])+ " by RX"+std::to_string(nodeID[counter]-1)+" in time slot "+ std::to_string(Slotnum[counter])+ " is = " << int(((*sinkIterator)->GetTotalRx ())/payloadSize) << endl;
             counter+=1;
     }



      Simulator::Destroy ();
  

  return 0;
}
