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
** File description: CDMA Interference 
** File: test/cdma-interference-multiple-pairs.cc
** Date Modified: Sep 2020
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
static void 
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
 // Vector pos = mobility->GetPosition ();
 // Vector vel = mobility->GetVelocity ();
 // std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
          //  << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
          //  << ", z=" << vel.z << std::endl;
}

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
              tt+=1;
    }


}

int tt2=0;
void ReceiveCallback2 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt2+=1;
    }


}
int tt3=0;
void ReceiveCallback3 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt3+=1;
    }


}

int tt4=0;
void ReceiveCallback4 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt4+=1;
    }


}

int tt5=0;
void ReceiveCallback5 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt5+=1;
    }


}

int tt6=0;
void ReceiveCallback6 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt6+=1;
    }


}

int tt7=0;
void ReceiveCallback7 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
             tt7+=1;
    }


}

int tt8=0;
void ReceiveCallback8 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
              tt8+=1;
    }


}
int tt9=0;
void ReceiveCallback9 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt9+=1;
    }
        

}

int tt10=0;
void ReceiveCallback10 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt10+=1;
    }
        

}

int tt11=0;
void ReceiveCallback11 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt11+=1;
    }
        

}

int tt12=0;
void ReceiveCallback12 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt12+=1;
    }
        

}

int tt13=0;
void ReceiveCallback13 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt13+=1;
    }
        

}

int tt14=0;
void ReceiveCallback14 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt14+=1;
    }
        

}

int tt15=0;
void ReceiveCallback15 (Ptr<Socket> socket)
{ 
 // std::cout << "Hello1"<<std::endl;
  while (socket->Recv ())
    {
      //NS_LOG_UNCOND ("Received one packet!");
      tt15+=1;
    }
        

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
  uint32_t numPackets =200;
  double interval = 0.05; // seconds
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
  Time interPacketInterval1 = Seconds (0.0005);
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
  //mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
 // mobilityHelper.Install (nodes);
 //nodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
  //nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.0, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));

   mobilityHelper.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper.Install (nodes);
    Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));

  InternetStackHelper internet;
  internet.Install (nodes);

  NodeContainer nodes1;
  nodes1.Create (2);
  NetDeviceContainer d1 = cdma.Install (nodes1,channel);
  MobilityHelper mobilityHelper1;
/*
  mobilityHelper1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper1.Install (nodes1);
  nodes1.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (-1, 0, 0));
  nodes1.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (-3, 0, 0));
*/
   // MobilityHelper mobilityHelper1;
  //mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper.Install (nodes);
 // nodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
  //nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.0, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper1.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
 mobilityHelper1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper1.Install (nodes1);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet1;
  internet1.Install (nodes1);
  
    NodeContainer nodes2;
  nodes2.Create (2);
  NetDeviceContainer d2 = cdma.Install (nodes2,channel);
    MobilityHelper mobilityHelper2;
  //mobilityHelper2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
 // mobilityHelper2.Install (nodes2);
 //nodes2.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes2.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (1.5, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper2.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper2.Install (nodes2);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet2;
  internet2.Install (nodes2);

    NodeContainer nodes3;
  nodes3.Create (2);
  NetDeviceContainer d3 = cdma.Install (nodes3,channel);
    MobilityHelper mobilityHelper3;
 // mobilityHelper3.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper3.Install (nodes3);
 //nodes3.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes3.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.5, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper3.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper3.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                            "Mode", StringValue ("Time"),
                            "Time", StringValue ("2s"),
                            "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                            "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper3.Install (nodes3);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet3;
  internet3.Install (nodes3);
      NodeContainer nodes4;
  nodes4.Create (2);
  NetDeviceContainer d4 = cdma.Install (nodes4,channel);
    MobilityHelper mobilityHelper4;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper4.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper4.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper4.Install (nodes4);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet4;
  internet4.Install (nodes4);

       NodeContainer nodes5;
  nodes5.Create (2);
  NetDeviceContainer d5 = cdma.Install (nodes5,channel);
    MobilityHelper mobilityHelper5;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper5.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper5.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper5.Install (nodes5);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet5;
  internet5.Install (nodes5);

  NodeContainer nodes6;
  nodes6.Create (2);
  NetDeviceContainer d6 = cdma.Install (nodes6,channel);
    MobilityHelper mobilityHelper6;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper6.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper6.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper6.Install (nodes6);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet6;
  internet6.Install (nodes6);

   NodeContainer nodes7;
  nodes7.Create (2);
  NetDeviceContainer d7 = cdma.Install (nodes7,channel);
    MobilityHelper mobilityHelper7;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper7.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper7.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper7.Install (nodes7);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet7;
  internet7.Install (nodes7);


   NodeContainer nodes8;
  nodes8.Create (2);
  NetDeviceContainer d8 = cdma.Install (nodes8,channel);
    MobilityHelper mobilityHelper8;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper8.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper8.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper8.Install (nodes8);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet8;
  internet8.Install (nodes8);

   NodeContainer nodes9;
  nodes9.Create (2);
  NetDeviceContainer d9 = cdma.Install (nodes9,channel);
    MobilityHelper mobilityHelper9;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper9.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper9.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper9.Install (nodes9);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet9;
  internet9.Install (nodes9);

  NodeContainer nodes10;
  nodes10.Create (2);
  NetDeviceContainer d10 = cdma.Install (nodes10,channel);
    MobilityHelper mobilityHelper10;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper10.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper10.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper10.Install (nodes10);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet10;
  internet10.Install (nodes10);

  NodeContainer nodes11;
  nodes11.Create (2);
  NetDeviceContainer d11 = cdma.Install (nodes11,channel);
    MobilityHelper mobilityHelper11;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper11.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper11.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper11.Install (nodes11);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet11;
  internet11.Install (nodes11);

  NodeContainer nodes12;
  nodes12.Create (2);
  NetDeviceContainer d12 = cdma.Install (nodes12,channel);
    MobilityHelper mobilityHelper12;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper12.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper12.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper12.Install (nodes12);

  InternetStackHelper internet12;
  internet12.Install (nodes12);

  NodeContainer nodes13;
  nodes13.Create (2);
  NetDeviceContainer d13 = cdma.Install (nodes13,channel);
    MobilityHelper mobilityHelper13;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper13.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper13.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper13.Install (nodes13);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet13;
  internet13.Install (nodes13);

  NodeContainer nodes14;
  nodes14.Create (2);
  NetDeviceContainer d14 = cdma.Install (nodes14,channel);
    MobilityHelper mobilityHelper14;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper14.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper14.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper14.Install (nodes14);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet14;
  internet14.Install (nodes14);

  NodeContainer nodes15;
  nodes15.Create (2);
  NetDeviceContainer d15 = cdma.Install (nodes15,channel);
    MobilityHelper mobilityHelper15;
  //mobilityHelper4.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobilityHelper4.Install (nodes4);
 //nodes4.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
 //nodes4.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2.75, 0, 0));
   // nodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (50, 50, 0));
   mobilityHelper15.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("100.0"),
                                 "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
  mobilityHelper15.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("0|200|0|200"));
  mobilityHelper15.Install (nodes15);
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
  InternetStackHelper internet15;
  internet15.Install (nodes15);
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

  Ipv4InterfaceContainer i2 = ipv41.Assign (d2);
   TypeId tid2 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink2 = Socket::CreateSocket (nodes2.Get (0), tid2);
  InetSocketAddress local2 = InetSocketAddress (Ipv4Address::GetAny (), 2);
  recvSink2->Bind (local2);
  recvSink2->SetRecvCallback (MakeCallback (&ReceiveCallback2));

  Ptr<Socket> interferer2 = Socket::CreateSocket (nodes2.Get (1), tid2);
  InetSocketAddress interferingAddr2 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 2);
 interferer2->SetAllowBroadcast (true);
  interferer2->Connect (interferingAddr2);

  Ipv4InterfaceContainer i3 = ipv41.Assign (d3);
   TypeId tid3 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink3 = Socket::CreateSocket (nodes3.Get (0), tid3);
  InetSocketAddress local3 = InetSocketAddress (Ipv4Address::GetAny (), 4);
  recvSink3->Bind (local3);
  recvSink3->SetRecvCallback (MakeCallback (&ReceiveCallback3));

  Ptr<Socket> interferer3 = Socket::CreateSocket (nodes3.Get (1), tid3);
  InetSocketAddress interferingAddr3 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 4);
 interferer3->SetAllowBroadcast (true);
  interferer3->Connect (interferingAddr3);

   Ipv4InterfaceContainer i4 = ipv41.Assign (d4);
   TypeId tid4 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink4 = Socket::CreateSocket (nodes4.Get (0), tid4);
  InetSocketAddress local4 = InetSocketAddress (Ipv4Address::GetAny (), 6);
  recvSink4->Bind (local4);
  recvSink4->SetRecvCallback (MakeCallback (&ReceiveCallback4));

  Ptr<Socket> interferer4 = Socket::CreateSocket (nodes4.Get (1), tid4);
  InetSocketAddress interferingAddr4 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 6);
 interferer4->SetAllowBroadcast (true);
  interferer4->Connect (interferingAddr4);

   Ipv4InterfaceContainer i5 = ipv41.Assign (d5);
   TypeId tid5 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink5 = Socket::CreateSocket (nodes5.Get (0), tid5);
  InetSocketAddress local5 = InetSocketAddress (Ipv4Address::GetAny (), 8);
  recvSink5->Bind (local5);
  recvSink5->SetRecvCallback (MakeCallback (&ReceiveCallback5));

  Ptr<Socket> interferer5= Socket::CreateSocket (nodes5.Get (1), tid5);
  InetSocketAddress interferingAddr5 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 8);
 interferer5->SetAllowBroadcast (true);
  interferer5->Connect (interferingAddr5);

   Ipv4InterfaceContainer i6 = ipv41.Assign (d6);
   TypeId tid6 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink6 = Socket::CreateSocket (nodes6.Get (0), tid6);
  InetSocketAddress local6 = InetSocketAddress (Ipv4Address::GetAny (), 10);
  recvSink6->Bind (local6);
  recvSink6->SetRecvCallback (MakeCallback (&ReceiveCallback6));

  Ptr<Socket> interferer6= Socket::CreateSocket (nodes6.Get (1), tid6);
  InetSocketAddress interferingAddr6 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 10);
 interferer6->SetAllowBroadcast (true);
  interferer6->Connect (interferingAddr6);

 
   Ipv4InterfaceContainer i7 = ipv41.Assign (d7);
   TypeId tid7 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink7 = Socket::CreateSocket (nodes7.Get (0), tid7);
  InetSocketAddress local7 = InetSocketAddress (Ipv4Address::GetAny (), 12);
  recvSink7->Bind (local7);
  recvSink7->SetRecvCallback (MakeCallback (&ReceiveCallback7));

  Ptr<Socket> interferer7= Socket::CreateSocket (nodes7.Get (1), tid7);
  InetSocketAddress interferingAddr7 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 12);
 interferer7->SetAllowBroadcast (true);
  interferer7->Connect (interferingAddr7);

   Ipv4InterfaceContainer i8 = ipv41.Assign (d8);
   TypeId tid8 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink8 = Socket::CreateSocket (nodes8.Get (0), tid8);
  InetSocketAddress local8 = InetSocketAddress (Ipv4Address::GetAny (), 14);
  recvSink8->Bind (local8);
  recvSink8->SetRecvCallback (MakeCallback (&ReceiveCallback8));

  Ptr<Socket> interferer8= Socket::CreateSocket (nodes8.Get (1), tid8);
  InetSocketAddress interferingAddr8 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 14);
 interferer8->SetAllowBroadcast (true);
  interferer8->Connect (interferingAddr8);

   Ipv4InterfaceContainer i9 = ipv41.Assign (d9);
   TypeId tid9 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink9 = Socket::CreateSocket (nodes9.Get (0), tid9);
  InetSocketAddress local9 = InetSocketAddress (Ipv4Address::GetAny (), 16);
  recvSink9->Bind (local9);
  recvSink9->SetRecvCallback (MakeCallback (&ReceiveCallback9));

  Ptr<Socket> interferer9= Socket::CreateSocket (nodes9.Get (1), tid9);
  InetSocketAddress interferingAddr9 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 16);
 interferer9->SetAllowBroadcast (true);
  interferer9->Connect (interferingAddr9);

   Ipv4InterfaceContainer i10 = ipv41.Assign (d10);
   TypeId tid10 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink10 = Socket::CreateSocket (nodes10.Get (0), tid10);
  InetSocketAddress local10 = InetSocketAddress (Ipv4Address::GetAny (), 18);
  recvSink10->Bind (local10);
  recvSink10->SetRecvCallback (MakeCallback (&ReceiveCallback10));

  Ptr<Socket> interferer10= Socket::CreateSocket (nodes10.Get (1), tid10);
  InetSocketAddress interferingAddr10 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 18);
 interferer10->SetAllowBroadcast (true);
  interferer10->Connect (interferingAddr10);

   Ipv4InterfaceContainer i11 = ipv41.Assign (d11);
   TypeId tid11 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink11 = Socket::CreateSocket (nodes11.Get (0), tid11);
  InetSocketAddress local11 = InetSocketAddress (Ipv4Address::GetAny (), 20);
  recvSink11->Bind (local11);
  recvSink11->SetRecvCallback (MakeCallback (&ReceiveCallback11));

  Ptr<Socket> interferer11= Socket::CreateSocket (nodes11.Get (1), tid11);
  InetSocketAddress interferingAddr11 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 20);
 interferer11->SetAllowBroadcast (true);
  interferer11->Connect (interferingAddr11);

   Ipv4InterfaceContainer i12 = ipv41.Assign (d12);
   TypeId tid12 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink12 = Socket::CreateSocket (nodes12.Get (0), tid12);
  InetSocketAddress local12 = InetSocketAddress (Ipv4Address::GetAny (), 22);
  recvSink12->Bind (local12);
  recvSink12->SetRecvCallback (MakeCallback (&ReceiveCallback12));

  Ptr<Socket> interferer12= Socket::CreateSocket (nodes12.Get (1), tid12);
  InetSocketAddress interferingAddr12 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 22);
 interferer12->SetAllowBroadcast (true);
  interferer12->Connect (interferingAddr12);

   Ipv4InterfaceContainer i13 = ipv41.Assign (d13);
   TypeId tid13 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink13 = Socket::CreateSocket (nodes13.Get (0), tid13);
  InetSocketAddress local13 = InetSocketAddress (Ipv4Address::GetAny (), 24);
  recvSink13->Bind (local13);
  recvSink13->SetRecvCallback (MakeCallback (&ReceiveCallback13));

  Ptr<Socket> interferer13= Socket::CreateSocket (nodes13.Get (1), tid13);
  InetSocketAddress interferingAddr13 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 24);
 interferer13->SetAllowBroadcast (true);
  interferer13->Connect (interferingAddr13);

   Ipv4InterfaceContainer i14 = ipv41.Assign (d14);
   TypeId tid14 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink14 = Socket::CreateSocket (nodes14.Get (0), tid14);
  InetSocketAddress local14 = InetSocketAddress (Ipv4Address::GetAny (), 26);
  recvSink14->Bind (local14);
  recvSink14->SetRecvCallback (MakeCallback (&ReceiveCallback14));

  Ptr<Socket> interferer14= Socket::CreateSocket (nodes14.Get (1), tid14);
  InetSocketAddress interferingAddr14 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 26);
 interferer14->SetAllowBroadcast (true);
  interferer14->Connect (interferingAddr14);

   Ipv4InterfaceContainer i15 = ipv41.Assign (d15);
   TypeId tid15 = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink15 = Socket::CreateSocket (nodes15.Get (0), tid15);
  InetSocketAddress local15 = InetSocketAddress (Ipv4Address::GetAny (), 28);
  recvSink15->Bind (local15);
  recvSink15->SetRecvCallback (MakeCallback (&ReceiveCallback15));

  Ptr<Socket> interferer15= Socket::CreateSocket (nodes15.Get (1), tid15);
  InetSocketAddress interferingAddr15 = InetSocketAddress (Ipv4Address ("255.255.255.255"), 28);
 interferer15->SetAllowBroadcast (true);
  interferer15->Connect (interferingAddr15);
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                        Seconds (7.0), &GenerateTraffic,
                           source, packetSize, numPackets, interPacketInterval);

 Simulator::ScheduleWithContext (interferer->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                            interferer, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer2->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                             interferer2, packetSize, numPackets, interPacketInterval);



 Simulator::ScheduleWithContext (interferer3->GetNode ()->GetId (),
                         Seconds (7.0), &GenerateTraffic,
                             interferer3, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer4->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer4, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer5->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer5, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer6->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer6, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer7->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer7, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer8->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer8, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer9->GetNode ()->GetId (),
                             Seconds (7.0), &GenerateTraffic,
                             interferer9, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer10->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer10, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer11->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer11, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer12->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer12, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer13->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer13, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer14->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer14, packetSize, numPackets, interPacketInterval);
 Simulator::ScheduleWithContext (interferer15->GetNode ()->GetId (),
                            Seconds (7.0), &GenerateTraffic,
                            interferer15, packetSize, numPackets, interPacketInterval);

  Simulator::Stop (Seconds (27.0));
  Simulator::Run ();
  Simulator::Destroy ();
      std::cout<< "Throughput of the first Tx-Rx pair=" << p << std::endl;
      std::cout<< "Throughput of the second Tx-Rx pair=" << tt << std::endl;
      std::cout<< "Throughput of the third Tx-Rx pair=" << tt2 << std::endl;
      std::cout<< "Throughput of the fourth Tx-Rx pair=" << tt3 << std::endl;
      std::cout<< "Throughput of the fifth Tx-Rx pair=" << tt4 << std::endl;
      std::cout<< "Throughput of the sixth Tx-Rx pair=" << tt5 << std::endl;
      std::cout<< "Throughput of the seventh Tx-Rx pair=" << tt6 << std::endl;
      std::cout<< "Throughput of the eights Tx-Rx pair=" << tt7 << std::endl;
      std::cout<< "Throughput of the ninth Tx-Rx pair=" << tt8 << std::endl;
      std::cout<< "Throughput of the tenth Tx-Rx pair=" << tt9 << std::endl;
      std::cout<< "Throughput of the eleventh Tx-Rx pair=" << tt10 << std::endl;
      std::cout<< "Throughput of the twelfth Tx-Rx pair=" << tt11 << std::endl;
          std::cout<< "Throughput of the thirteenth Tx-Rx pair=" << tt12 << std::endl;
          std::cout<< "Throughput of the fourtheenth Tx-Rx pair=" << tt13 << std::endl;
          std::cout<< "Throughput of the fifteenth Tx-Rx pair=" << tt14 << std::endl;
          std::cout<< "Throughput of the sixteenth Tx-Rx pair=" << tt15 << std::endl;

  return 0;
}

