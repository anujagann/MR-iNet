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
** Author: Kian Hamedani, Collin Farquhar
** Email:  khamedani@androcs.com, cfarquhar@androcs.com
** File description: Testing how to swtich RXs in simulation
** File: test/different_rx.cc
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


std::vector<Ipv4Address> rx_addresses;
std::vector<Ptr<Socket>> source_sockets;

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
                             uint32_t pktCount, Time pktInterval, Ipv4Address addr2, uint32_t port)
{
  if (pktCount > 0)
    {

      socket->SendTo (Create<Packet> (pktSize), 0, InetSocketAddress (addr2, port));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize, pktCount - 1, pktInterval, addr2, port);
    }
  
  // socket->Close() will prevent a TX from being able to connect to a new RX
  /*
  else
    {
      socket->Close (); // the culprit
    }
  */
}

int main (int argc, char *argv[])
{
  uint numNodes = 6;
  uint nTx = 3;

  received_vector.resize(numNodes, 0);

  uint32_t packetSize = 1024; // bytes
  uint32_t numPackets = 100;
  double interval = 0.05; // seconds

  Time interPacketInterval = Seconds (interval);

  NodeContainer cdmaNodes;
  cdmaNodes.Create (numNodes);

  // --------------------------------> Set up channel <---------------------------------
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetAttribute ("Exponent", DoubleValue (0.5));
  /*
  // Adds randomness to the throughput results
  Ptr<NakagamiPropagationLossModel> nak = CreateObject<NakagamiPropagationLossModel> ();
  lossModel -> SetNext (nak);
  */
  channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  CdmaHelper cdma;
  NetDeviceContainer CdmaDevice;

  CdmaDevice = cdma.Install (cdmaNodes,channel);
  // --------------------------------> End Set up channel <---------------------------------

  // --------------------------------> Mobility <---------------------------------
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
  mobilityHelper.Install (cdmaNodes);
  // --------------------------------> End Mobility <---------------------------------

  /* Internet stack*/
  InternetStackHelper stack;
  stack.Install (cdmaNodes);


  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface;
  apInterface = address.Assign (CdmaDevice);

  NS_LOG_INFO ("Configure unicasting.");

  uint32_t sink_idx;
  uint32_t source_idx;
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  std::vector <uint32_t> nodeIDRx;
  std::vector <uint32_t> nodeIDTx;

  for (uint32_t i = 0; i < nTx; i++)
  {
    source_idx = 2*i + 1;
    sink_idx = 2*i;

    uint32_t port = 1; // test 

    // Set multicast route
    Ipv4StaticRoutingHelper multicast;
    Ptr<Node> sender = cdmaNodes.Get (source_idx);
    Ptr<NetDevice> senderNetDevice = CdmaDevice.Get (source_idx);
    multicast.SetDefaultMulticastRoute (sender, senderNetDevice);

    // Create sockets
    Ptr<Socket> sinkSocket = Socket::CreateSocket (cdmaNodes.Get (sink_idx), tid);
    Ptr<Socket> sourceSocket = Socket::CreateSocket (cdmaNodes.Get (source_idx), tid);
    source_sockets.push_back(sourceSocket);

    // Create internet socket address for the destination with the form: 224.1.<something>.<sinkd_idx + 1>
    string destAddr = "224.1.0." + std::to_string(sink_idx + 1);
    InetSocketAddress local = InetSocketAddress (Ipv4Address (destAddr.c_str()), port);

    rx_addresses.push_back(Ipv4Address(destAddr.c_str()));

    // Bind destination address to destination socket
    sinkSocket->Bind (local);

    sinkSocket->SetRecvCallback (MakeCallback (&ReceiveCallback));

    Simulator::ScheduleWithContext (sourceSocket->GetNode ()->GetId (), Seconds (0.0), &GenerateTraffic, sourceSocket, packetSize,
                                    numPackets, interPacketInterval, Ipv4Address (destAddr.c_str()), port);

    nodeIDTx.push_back(source_idx);
    nodeIDRx.push_back(sink_idx);
    Simulator::Stop (Seconds (100));
  }

  // ------------------------> Have TX's connect to different RXs <----------------------------
  int port = 1;
  for (uint i=0; i < nTx; i++)
  {
    Ptr<Socket> sourceSocket = source_sockets[i];
    Ipv4Address nextAddr = rx_addresses[((i + 1) % 3)]; // Get next (in terms of node ID) RX. 

    Simulator::ScheduleWithContext (sourceSocket->GetNode ()->GetId (), Seconds (6.0), &GenerateTraffic, sourceSocket, packetSize,
                                    numPackets, interPacketInterval, nextAddr, port);
  }
  // ------------------------------------------------------------------------------------------

  Simulator::Run ();
  Simulator::Destroy (); 

  for (size_t i = 0; i < nodeIDRx.size(); i++)
  {
    std::cout<< "Throughput of Rx " + std::to_string(nodeIDRx[i]) + " is = " << received_vector.at(nodeIDRx[i]) << std::endl;
  }
  received_vector.clear();

  return 0;
}
