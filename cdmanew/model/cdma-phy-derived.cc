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
** Author(s): Anu Jagannath, Collin Farquhar, Kian Hamedani, Swatantra Kafle
** Email:  ajagannath@androcs.com, cfarquhar@androcs.com, khamedani@androcs.com, skafle@androcs.com 
** File description: CDMA Phy layer
** File: model/cdma-phy-derived.cc
*/
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/mac48-address.h"
#include "ns3/integer.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "cdma-mac-header.h"
#include <vector>
#include <complex>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "cdma-phy.h"
#include "cdma-phy-derived.h"
#include "cdma-channel.h"
#include "cdma-mac.h"
#include "cdma-net-device.h"

#include "ns3/object-factory.h"
#include "ns3/boolean.h"

#include <fstream>

double G_MIN_SINR = 0;
double G_MAX_SINR = 0;
double G_MIN_INTERFERENCE_CAUSED = 0;
double G_MAX_INTERFERENCE_CAUSED = 0;
double G_MIN_INTERFERENCE_SENSED = 0;
double G_MAX_INTERFERENCE_SENSED = 0;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CdmaPhyDerived");

/****************************************************************
 *       The actual CdmaPhyDerived class
 ****************************************************************/

NS_OBJECT_ENSURE_REGISTERED (CdmaPhyDerived);

TypeId
CdmaPhyDerived::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CdmaPhyDerived")
    .SetParent<CdmaPhy> ()
    .AddConstructor<CdmaPhyDerived> ()
    .SetGroupName ("Cdma")
    .AddAttribute ("Frequency",
                   "The operating center frequency (MHz)",
                   DoubleValue (915e6),
                   MakeDoubleAccessor (&CdmaPhyDerived::GetFrequency,
                                         &CdmaPhyDerived::SetFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RxSensitivity",
                   "The energy of a received signal should be higher than "
                   "this threshold (dB) for the PHY to detect the signal.",
                   DoubleValue (0),
                   MakeDoubleAccessor (&CdmaPhyDerived::SetRxSensitivity,
                                       &CdmaPhyDerived::GetRxSensitivity),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxGain",
                   "Transmission gain (dB).",
                   DoubleValue (10.0),
                   MakeDoubleAccessor (&CdmaPhyDerived::SetTxGain,
                                       &CdmaPhyDerived::GetTxGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RxGain",
                   "Reception gain (dB).",
                   DoubleValue (10.0),
                   MakeDoubleAccessor (&CdmaPhyDerived::SetRxGain,
                                       &CdmaPhyDerived::GetRxGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxPower",
                   "Minimum available transmission level (dBm).",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&CdmaPhyDerived::SetTxPower,
                                       &CdmaPhyDerived::GetTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SpreadingGain",
                   "Length of spreading sequence",
                   IntegerValue (4),
                   MakeIntegerAccessor (&CdmaPhyDerived::SetSpreadingGain,
                                       &CdmaPhyDerived::GetSpreadingGain),
                   MakeIntegerChecker<int> ())

  ;
  return tid;
}

CdmaPhyDerived::CdmaPhyDerived()
: m_RxPkt(0),
  m_mac(0)

{
  NS_LOG_FUNCTION (this);
  m_freq  = 915e6;
  m_txgain = 10;
  m_rxgain = 10;
  m_txpower = 0.5;
  m_ack_txpower = 0.5;
  m_data_txpower = 0.5;
  m_rxSensitivityW = 0; // dB 
  
  m_rxnoisefig = 5;
  m_device = 0;
  m_channel = 0;
  m_mac = 0;
  m_state = IDLE;
  m_bitRate = 2; // Mbps

  m_frequency_id = 0;

  /* m_state_delay is set to 3335ns. This is equal to the propagation delay over 1000m
  when using the ConstantSpeedPropagationDelayModel. 
  */
  m_state_delay = NanoSeconds(3335); 

  // temporary initialization
  boost::numeric::ublas::matrix<std::complex<double>> m_SPREADING_MATRIX (2,4);
  m_SPREADING_MATRIX(0,0) = std::complex<double>(1,0);m_SPREADING_MATRIX(0,1) = std::complex<double>(1,0);
  m_SPREADING_MATRIX(0,2) = std::complex<double>(1,0);m_SPREADING_MATRIX(0,3) = std::complex<double>(1,0);
  m_SPREADING_MATRIX(1,0) = std::complex<double>(-1,0);m_SPREADING_MATRIX(1,1) = std::complex<double>(1,0);
  m_SPREADING_MATRIX(1,2) = std::complex<double>(-1,0);m_SPREADING_MATRIX(1,3) = std::complex<double>(1,0);
}

CdmaPhyDerived::~CdmaPhyDerived ()
{
  Clear();
}

// get frequency of operation
double
CdmaPhyDerived::GetFrequency (void) const
{
  return this->m_freq;
}
// NS-3 doesnt allow for nodeid intialization but this a placeholder
// this is for NW layer to parse nodeid (retrieved from ipv4 address)
void 
CdmaPhyDerived::SetDstNodeid(uint32_t nodeid)
{
  m_dstnodeid = nodeid;
}
// get dst nodeid
double
CdmaPhyDerived::GetDstNodeid(void)
{
  return this->m_dstnodeid;
}
// clear variables
void CdmaPhyDerived::Clear(void)
{
  m_freq  = 0;
  m_txgain = 0;
  m_rxgain = 0;
  m_txpower = 0;
  m_rxSensitivityW = 0;
  m_rxnoisefig = 0;
  m_spreading_gain = 0;
  m_RxPkt = 0;
}
// set spreading gain/length
void 
CdmaPhyDerived::SetSpreadingGain(int spreading_gain)
{
  this->m_spreading_gain = spreading_gain;
}
void
CdmaPhyDerived::SetMobility (const Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
}
Ptr<MobilityModel>
CdmaPhyDerived::GetMobility (void) const
{
  if (m_mobility != 0)
    {
      return m_mobility;
    }
  else
    {
      return m_device->GetNode ()->GetObject<MobilityModel> ();
    }
}

// get spreading gain/length
int
CdmaPhyDerived::GetSpreadingGain (void) const
{
  return this->m_spreading_gain;
}
// set spreading sequence
void 
CdmaPhyDerived::SetSpreadingSeq (boost::numeric::ublas::vector<std::complex<double>>& seq)
{
  this->m_spreading_seq = seq;
}
// get spreading seq
boost::numeric::ublas::vector<std::complex<double>>
CdmaPhyDerived::GetSpreadingSeq (void) const
{
  return this->m_spreading_seq;
}
// set frequency
void
CdmaPhyDerived::SetFrequency (double freq)
{
  this->m_freq = freq;
}
// get spreading seq w.r.t specific user

boost::numeric::ublas::vector<std::complex<double>>
CdmaPhyDerived::GetUserSignature(uint32_t usernodeid)
{
  boost::numeric::ublas::vector<std::complex<double>> user_sig(m_spreading_gain);
  boost::numeric::ublas::matrix_row<boost::numeric::ublas::matrix<std::complex<double>> > mr(m_SPREADING_MATRIX,usernodeid);
  std::cout << "SIGNAL" << mr.size() <<std::endl;
  for(int i=0;i<m_spreading_gain;++i)
    user_sig(i) = mr(i);
  return user_sig;
}
// set rx sensitivity
void
CdmaPhyDerived::SetRxSensitivity (double threshold)
{
  this->m_rxSensitivityW  = threshold;
}
// get rx sensitivity
double
CdmaPhyDerived::GetRxSensitivity (void) const
{
  return this->m_rxSensitivityW;
}
// set rx noise figure
void
CdmaPhyDerived::SetRxNoiseFigure (double noiseFigureDb)
{
  this->m_rxnoisefig  =  noiseFigureDb;
}
// get rx noise fig
double
CdmaPhyDerived::GetRxNoiseFigure (void) const
{
  return this->m_rxnoisefig;
}
// set current tx power
void
CdmaPhyDerived::SetTxPower (double txpower)
{
  this->m_txpower = txpower;
}
// get current tx power
double
CdmaPhyDerived::GetTxPower (void) const
{
  return this->m_txpower;
}
// set tx power to be used for ACK transmissions
void
CdmaPhyDerived::SetAckTxPower (double txpower)
{
  this->m_ack_txpower = txpower;
}
// get tx power to be used for ACK transmissions
double
CdmaPhyDerived::GetAckTxPower (void) const
{
  return this->m_ack_txpower;
}
// set tx power to be used for data transmissions
void
CdmaPhyDerived::SetDataTxPower (double txpower)
{
  this->m_data_txpower = txpower;
}
// get tx power to be used for data transmissions
double
CdmaPhyDerived::GetDataTxPower (void) const
{
  return this->m_data_txpower;
}
// set tx gain
void
CdmaPhyDerived::SetTxGain (double gain)
{
  this->m_txgain = gain;
}
// get tx gain
double
CdmaPhyDerived::GetTxGain (void) const
{
  return this->m_txgain;
}
// set rx gain
void
CdmaPhyDerived::SetRxGain (double gain)
{
  this->m_rxgain = gain;
}
// get rx gain
double
CdmaPhyDerived::GetRxGain (void) const
{
  return this->m_rxgain;
}
// set net device
void
CdmaPhyDerived::SetDevice (const Ptr<CdmaNetDevice> device)
{
  this->m_device = device;
}
// set mac layer
void
CdmaPhyDerived::SetMac (Ptr<CdmaMac> mac)
{
  m_mac = mac;
}
// get net device
Ptr<CdmaNetDevice>
CdmaPhyDerived::GetDevice (void)
{
  return this->m_device;
}
//get node id
uint32_t
CdmaPhyDerived::GetNodeID (void)
{
  /*
   obtain node id of this node
  */
  return m_device->GetNode()->GetId();
}
void 
CdmaPhyDerived::AddNodeIdToNeighList(uint32_t id)
{
  /*
    Add node id (id) of neighboring node to populate self list
  */
  m_neighlist.push_back(id);
}
uint32_t
CdmaPhyDerived::GetNeigh(uint32_t idx)
{
  /*
    Get node id of neighbor at index idx of neighbor list
  */
  return m_neighlist.at(idx);
}
// set channel
void
CdmaPhyDerived::SetChannel (Ptr<CdmaChannel> channel)
{
  m_channel = channel;
}
// get channel
Ptr<CdmaChannel>
CdmaPhyDerived::GetChannel ()
{
  return m_channel;
}

bool
CdmaPhyDerived::IsStateIdle (void)
{
  return m_state == IDLE;
}

bool
CdmaPhyDerived::IsStateRx (void)
{
  return m_state == RX;
}
bool
CdmaPhyDerived::IsStateTx (void)
{
  return m_state == TX;
}

void 
CdmaPhyDerived::ChangeState(State new_state)
{
  m_state = new_state;
}
void 
CdmaPhyDerived::SetStateToIdle()
{
  m_state = IDLE;
}
void 
ScheduledSetStateToIdle(Ptr<CdmaPhyDerived> phy)
{
  phy->SetStateToIdle();
}

Time
CdmaPhyDerived::GetPktDuration (uint32_t n_bytes)
{
  double bitrate_in_bps = m_bitRate * 1e6; // Convert from Mega-bits-per-second to bits-per-second.
  double n_bits = n_bytes * 8;
  double pkt_duration = n_bits / bitrate_in_bps;

  Time pkt_duration_time = Seconds (pkt_duration);
  return pkt_duration_time;
}

// send packet and attach to channel
bool
CdmaPhyDerived::SendPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION ("from node " << m_device->GetNode()->GetId() << " state " << (m_state));
  NS_LOG_FUNCTION("CdmaPhyDerived::SendPacket");

  ChangeState(TX);

  Time txDuration = GetPktDuration (packet->GetSize ());
  NS_LOG_DEBUG ("Tx will finish at " << (Simulator::Now () + txDuration).GetNanoSeconds () << "(ns) txPower=" << m_txpower << " dBm");

  // Initialize transmission struct
  transmission txStruct;
  txStruct.txStartTime = Simulator::Now ();
  txStruct.txDuration = txDuration;
  txStruct.txEndTime = Simulator::Now() + txDuration;
  txStruct.packet = packet;
  txStruct.txPower = m_txpower;
  m_transmission.push_back (txStruct);
  txpkt++;

  // Forward to channel.
  m_channel->SendPacket (packet, m_device->GetPhy(), m_txpower, txDuration);

  return true;
}

// send pkt done
void
CdmaPhyDerived::SendPacketDone(Ptr<Packet> packet)
{
  Simulator::Schedule(m_state_delay, &ScheduledSetStateToIdle, this); 

  NS_LOG_FUNCTION ("from node " << m_device->GetNode ()->GetId () << " state " << (m_state));
}

// receive pkt
void
CdmaPhyDerived::ReceivePacket (Ptr<Packet> packet, Time txDuration, double_t rxPower, Ptr<CdmaPhy> sender)
{
  NS_LOG_FUNCTION ("at node " << m_device->GetNode ()->GetId () << " rxPower " << rxPower << " dBm" <<" state " << (m_state));
  // Initialize reception struct. Add to m_recepetion list. 

  reception Rxvd;
  Rxvd.rxStartTime = Simulator::Now ();
  Rxvd.rxDuration = txDuration;
  Rxvd.packet = packet;
  Rxvd.rxPower = rxPower;
  m_reception.push_back (Rxvd);
  rxpkt++;

  if (m_state == TX)
  {
    NS_LOG_INFO ("Half-duplex system cannot receive while transmission in progress");
    return;
  }
  else
  {
    // forward to mac
    m_RxPkt = packet;
    m_mac->ReceivePacket (this, packet); // Original

    ChangeState(RX);

    NS_LOG_INFO ("Received Packet at PHY");
    return;
  }
}  


bool
CdmaPhyDerived::IsTransmissionSuccessul(double sinrW, Ptr<CdmaPhy> sender)
/*
* Compares the sinr in W to m_rxSensitivityW. If sinr exceeds the threshold set by the Rx Sensitivity,
* the tranmsission is succesul.
*
* Assumption: Ack messages are always successfully transmitted. Calling this function for ACK messages may
* violate that assumption and return false.
*/
{
  if (sinrW > m_rxSensitivityW)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
Finds and prints the min/max sinr, interference-sensed, and interference-caused based
on global vectors defined at the top of the file.
*/
void
MinMax(double sinr, double interferenceSensed, double interferenceCaused)
{
  G_MIN_SINR = (sinr < G_MIN_SINR) ? sinr : G_MIN_SINR;
  G_MAX_SINR = (sinr > G_MAX_SINR) ? sinr : G_MAX_SINR;
  G_MIN_INTERFERENCE_SENSED = (interferenceSensed < G_MIN_INTERFERENCE_SENSED) ? interferenceSensed : G_MIN_INTERFERENCE_SENSED;
  G_MAX_INTERFERENCE_SENSED = (interferenceSensed > G_MAX_INTERFERENCE_SENSED) ? interferenceSensed : G_MAX_INTERFERENCE_SENSED;
  G_MIN_INTERFERENCE_CAUSED = (interferenceCaused < G_MIN_INTERFERENCE_CAUSED) ? interferenceCaused : G_MIN_INTERFERENCE_CAUSED;
  G_MAX_INTERFERENCE_CAUSED = (interferenceCaused > G_MAX_INTERFERENCE_CAUSED) ? interferenceCaused : G_MAX_INTERFERENCE_CAUSED;

  std::cout << "min sinr: " << G_MIN_SINR << " max sinr: " << G_MAX_SINR <<
  " min interference-sensed: " << G_MIN_INTERFERENCE_SENSED << " max interference-sensed: " << G_MAX_INTERFERENCE_SENSED <<
  " min interference-caused: " << G_MIN_INTERFERENCE_CAUSED << " max interference-caused: " << G_MAX_INTERFERENCE_CAUSED << "\n";
}

/*
Writes SINR, interference-sensed, and interference-caused to a file.
For logging and testing.
*/
void
WriteTransmissionInfo(double sinr, double interferenceSensed, double interferenceCaused)
{
    std::ofstream outfile;
    outfile.open("transmission_info.csv", std::ios_base::app);
    outfile << sinr << "," << interferenceSensed << "," << interferenceCaused << "\n";
}

/*
Update PHY member variables that are used for the ns3-gym simulations
*/
void
UpdateNs3Variables(Ptr<CdmaPhy> receiver, Ptr<CdmaPhy> sender, double sinr, 
std::vector<double> interferenceCaused, double interferenceCausedSum, double interferenceReceived)
{
  receiver->source_id_check = sender->GetNodeID();
  sender->dest_id_check = receiver->GetNodeID();
  receiver->sinr += sinr;
  receiver->interference = interferenceReceived; 
  sender->interference_caused.push_back(interferenceCaused); 
  sender->interference_caused_sum += interferenceCausedSum; 
}

// receive pkt done 
void
CdmaPhyDerived::ReceivePacketDone (Ptr<Packet> packet, double_t rxPower, Ptr<CdmaPhy> sender)
/* 
* Calculates sinr and interference. Uses sinr to determine with the packet is received successfuly.
* Erases received packet from m_reception.
* Switches m_state to IDLE.
* Calls ReceivePacketDone in the MAC layer.
*
* Assumption: Ack messages are sent in a perfect channel. They are always received. 
*/
{
  if (m_state != RX)
  {
    NS_LOG_INFO ("Dropping packet. Incorrect RX state");
    NS_LOG_DEBUG ("Current Status " << (m_state) << " Need Status RX ");
    return;
  }
  // Update m_state.
  ChangeState(IDLE);
  
  CdmaMacHeader h; 
  packet->PeekHeader(h);
  int type = h.GetType();

  if (type == 0) // ACK
  {
    m_mac->ReceivePacketDone (this, packet, sender);//ACK always received
  }
  else // Data
  {
    // Interference sensed by the intended receiver from other concurrent transmissions
    double noiseWatt = 1e-13; // 1e-10 mW 
    double interfW  = m_channel->GetInterference (this, sender);

    // Interference caused by the sender
    uint32_t intended_receiver_id = this->GetDevice()->GetNode()->GetId();
    uint32_t sender_id = sender->GetDevice()->GetNode()->GetId();
    source_id_check = sender_id;

    std::vector<double> interferenceCaused = m_channel->GetInterferenceCaused(sender_id, intended_receiver_id);
    double interferenceCausedSum = 0;
    for (auto n : interferenceCaused)
    {
      interferenceCausedSum += n;
    }
    
    // Calculate sinr in Watts and Db.
    double rxPowerW = m_channel->DbmToW (rxPower);
    double spreadingFactor = 8; // original

    double sinr = (spreadingFactor * rxPowerW) / (interfW + noiseWatt); // linear scale 
    double sinrDb = 10 * std::log10 (sinr); 

    // Update variables for NS3-Gym observations.
    UpdateNs3Variables(this, sender, sinr, interferenceCaused, interferenceCausedSum, interfW); // change for future version

    // Determine if transmission is successful and pass information to the MAC layer.
    bool isSuccess = IsTransmissionSuccessul(sinrDb, sender);

    //pass to mac only if detection is valid
    if (isSuccess == true)
    {
      m_mac->ReceivePacketDone (this, packet, sender);

      // Erase received packet from m_reception.
      std::list<reception>::iterator it = m_reception.begin ();
      for (; it != m_reception.end (); ++it)
      {
        if (it->packet == m_RxPkt)
        {
          m_reception.erase (it);
          break;
        }
      }
    }
    else
    {
      NS_LOG_DEBUG ("Noisy reception. Drop it!");
    }
  }

  
}

} //namespace ns3

