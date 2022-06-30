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
** Author: Collin Farquhar, Kian Hamedani
** Email:  cfarquhar@androcs.com, khamedani@androcs.com
** File description: test file for the merged module that should act the same as simple_test.cc
does for the pre-merged module.
** File: test/simple_test_merged.cc
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
  uint32_t number = 6;
  uint32_t nTx = 3;
  uint32_t nSlots = 1;

  for(uint32_t ii = 0; ii < nSlots; ii++)
  {
    received_vector.resize(number, 0);
    double simulationTime = 100; //seconds
    uint32_t packetSize = 1024; // bytes
    uint32_t numPackets = 10; // was 10
    double interval = 0.05; // seconds

    Time interPacketInterval = Seconds (interval);

    NodeContainer cdmaNode;
    cdmaNode.Create (number);

    Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
    Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
    lossModel->SetAttribute ("Exponent", DoubleValue (7));
    Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
    lossModel -> SetNext (nak);
    channel->SetPropagationLossModel (lossModel);
    channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

    CdmaHelper cdma;
    NetDeviceContainer CdmaDevice;
    CdmaDevice = cdma.Install (cdmaNode, channel);
    
    // -----------------------> Mobility <-----------------------
    MobilityHelper mobilityHelper;
    mobilityHelper.Install (cdmaNode);
    
    std::vector<int> x_positions = {0, 100, 1000};
    for (uint32_t i = 0; i < nTx; i++)
    {
      int rxNodeId = 2*i;
      int txNodeId = 2*i + 1;
      int x = x_positions.at(i);
      cdmaNode.Get (rxNodeId)->GetObject<MobilityModel> ()->SetPosition (Vector (x, 10, 0));
      cdmaNode.Get (txNodeId)->GetObject<MobilityModel> ()->SetPosition (Vector (x, 0, 0));
    }
    // -----------------------> End Mobility <-----------------------

    /* Internet stack*/
    InternetStackHelper stack;
    stack.Install (cdmaNode);

    Ipv4AddressHelper address;
    address.SetBase ("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign (CdmaDevice);


    uint32_t sink_idx;
    uint32_t source_idx;
    double step = simulationTime/double(nSlots);
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    std::vector <uint32_t> nodeIDRx;
    std::vector <uint32_t> nodeIDTx;
    for (uint32_t i = 0; i < nTx; i++)
    {
      source_idx = 2*i + 1;
      sink_idx = 2*i;
      uint32_t port = i+ii+1;
      Ipv4StaticRoutingHelper multicast;
      Ptr<Node> sender = cdmaNode.Get (source_idx);
      Ptr<NetDevice> senderIf = CdmaDevice.Get (source_idx);
      multicast.SetDefaultMulticastRoute (sender, senderIf);

      string addr1 = "224.1." + std::to_string(ii+2) + "." +std::to_string(sink_idx+1);
      Ptr<Socket> recvSink = Socket::CreateSocket (cdmaNode.Get (sink_idx), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address (addr1.c_str()), port);
      recvSink->Bind (local);
      recvSink->SetRecvCallback (MakeCallback (&ReceiveCallback));
      Ptr<Socket> source = Socket::CreateSocket (cdmaNode.Get (source_idx), tid);
      Simulator::ScheduleWithContext (source->GetNode ()->GetId (), Seconds (0.0+ii*(step)), 
        &GenerateTraffic, source, packetSize, numPackets, interPacketInterval, Ipv4Address (addr1.c_str()), port);

      nodeIDTx.push_back(source_idx);
      nodeIDRx.push_back(sink_idx);
    }

    Simulator::Stop (Seconds ((ii + 1)*(step)));
    Simulator::Run ();
    Simulator::Destroy (); 

    std::ofstream outfile;
    outfile.open("transmission_info.csv", std::ios_base::app);

    for (size_t i = 0; i < nodeIDRx.size(); i++)
    {
      std::cout<< "Throughput of Tx-Rx pair " << i << " = " << received_vector.at(nodeIDRx[i]) << std::endl;
      outfile << received_vector.at(nodeIDRx[i]);
      if (i != (nodeIDRx.size() - 1))
      {
        outfile << ",";
      }
    }
    received_vector.clear();
  }

  return 0;
}
