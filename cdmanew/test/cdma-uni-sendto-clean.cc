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
** File: test/cdma-uni-sendto-clean.cc
*/


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
#include <iostream>
#include "ns3/names.h"
#include "ns3/mac48-address.h"

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("CdmaMulticast");

std::vector<int> received_vector;
uint32_t nTx;
void ReceiveCallback (Ptr<Socket> socket)
{ 

  while (socket->Recv ())
  {
    //NS_LOG_UNCOND ("Received one packet!");
  }
  int node_id = socket->GetNode ()->GetId ();
  int pair_id = node_id;
  received_vector.at(pair_id) += 1;
}

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval, Ipv4Address addr2,  uint32_t port )
{
  if (pktCount > 0)
    {

      socket->SendTo (Create<Packet> (pktSize),0,  InetSocketAddress (addr2, port));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize, pktCount - 1, pktInterval, addr2, port);
    }
  else
    {
      socket->Close ();
    }
}

int main (int argc, char *argv[])
{ 
  uint32_t number;
std::cout << "Please enter the number of nodes: " << std::endl;
 std::cin >> number;
std::cout << "Please enter the number of TXs: " << std::endl;
 std::cin >> nTx;
  uint32_t nRx= number - nTx;

std::cout << "The number of RXs is equal to: " << nRx << std::endl;
  uint32_t nSlots;
std::cout << "Please enter the number of time slots: " << std::endl;
 std::cin >> nSlots;
for(uint32_t ii=0;ii<nSlots ;ii++){
  received_vector.resize(number, 0);
  //received_vector1.resize(nTx, 0); 
  double simulationTime = 10; //seconds

  uint32_t packetSize = 1024; // bytes
  uint32_t numPackets = 100;
  double interval = 0.05; // seconds

  Time interPacketInterval = Seconds (interval);



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
      uint32_t source_idx;
      double step = simulationTime/double(nSlots);
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      std::vector <uint32_t> nodeIDRx;
      std::vector <uint32_t> nodeIDTx;
      for (uint32_t i=0; i<nTx; i++){

              std::cout << "Enter the node id of the Tx in "+std::to_string(ii)+ " time slot=";
	      cin >> source_idx;
	      uint32_t port = i+ii+1;
              Ipv4StaticRoutingHelper multicast;
              Ptr<Node> sender = cdmaNode.Get (source_idx);
              Ptr<NetDevice> senderIf = CdmaDevice.Get (source_idx);
              multicast.SetDefaultMulticastRoute (sender, senderIf);
	      std::cout << "Enter the node id of the Rx connecting to "+ std::to_string(i)+ " Tx in "+std::to_string(ii)+ " time slot=";
	      cin >> sink_idx; 

	      string addr1="224.1."+std::to_string(ii+2)+"."+std::to_string(sink_idx+1);
	      Ptr<Socket> recvSink = Socket::CreateSocket (cdmaNode.Get (sink_idx), tid);
	      InetSocketAddress local = InetSocketAddress (Ipv4Address (addr1.c_str()), port);
	      recvSink->Bind (local);
	      recvSink->SetRecvCallback (MakeCallback (&ReceiveCallback));
	      Ptr<Socket> source = Socket::CreateSocket (cdmaNode.Get (source_idx), tid);
	      Simulator::ScheduleWithContext (source->GetNode ()->GetId (), Seconds (0.0+ii*(step)), &GenerateTraffic, source, packetSize, numPackets, interPacketInterval, Ipv4Address (addr1.c_str()), port);
              nodeIDTx.push_back(source_idx);
              nodeIDRx.push_back(sink_idx);
              Simulator::Stop (Seconds ((ii+1)*(step)));



            }


                     Simulator::Run ();
              Simulator::Destroy (); 
        for (size_t i = 0; i < nodeIDRx.size(); i++)
  {
    std::cout<< "Throughput of Tx "+ std::to_string(nodeIDTx[i])+ " Rx "+std::to_string(nodeIDRx[i])+ " pair at time slot " +std::to_string(ii) + " is = " << received_vector.at(nodeIDRx[i]) << std::endl;
  }
  received_vector.clear();
}
//cout << "Throu size" << throughput[2][0] << endl;
  return 0;
}
