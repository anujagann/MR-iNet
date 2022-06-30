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
** Author(s):Anu Jagannath
** Email:  ajagannath@androcs.com
** File description: CDMA Phy base class
** File: model/cdma-phy.h
*/
#ifndef CDMA_PHY_H
#define CDMA_PHY_H

#include "ns3/type-id.h"
#include "ns3/packet.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/pointer.h"
#include "ns3/traced-value.h"
#include "ns3/mobility-model.h"
#include <list>
#include <vector>
#include <complex>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include "cdma-channel.h"
#include "cdma-mac.h"
#include "cdma-net-device.h"

namespace ns3 {

class CdmaPhy : public Object
{
   public:
  // Adding for interference calculations
  typedef struct
  {
    Time rxStartTime;        //!< start time of the ongoing transmission
    Time rxDuration;     //!< duration of the ongoing transmission
    Ptr<Packet> packet;  //!< the ongoing packet
    double rxPower;    //!< the receiving power
    double interference;
  } reception;

  typedef struct
  {
    Time txStartTime;        //!< start time of the ongoing transmission
    Time txDuration;     //!< duration of the ongoing transmission
    Time txEndTime; 
    Ptr<Packet> packet;  //!< the ongoing packet
    double txPower;    //!< the receiving power
    double interference;
  } transmission;
  std::list<reception> m_reception;
  std::list<transmission> m_transmission;

  // For NS3-Gym
  double sinr = 0; 
  double interference = 0; // remove after interference_sensed_sum is verified
  double interference_sensed_sum = 0;

  // For 2-freq spectrum sensed
  double interference_sensed_sum_freq_0 = 0;
  double interference_sensed_sum_freq_1 = 0;

  std::vector<std::vector<double>> interference_caused;
  double interference_caused_sum = 0;
  int m_frequency_id = 0;

  int txpkt = 0;
  int rxpkt = 0;
  int source_id_check = 0 ;
  int dest_id_check = 0 ;
  std::vector<uint32_t> m_neighlist;
  /// Enum defining possible Phy states.
  enum State
  {
    IDLE,     //!< Idle state.
    CCABUSY,  //!< Channel busy.
    RX,       //!< Receiving.
    TX,       //!< Transmitting.
    SLEEP,    //!< Sleeping.
    DISABLED  //!< Disabled.
  };

  virtual void Clear () = 0;
  virtual void SetRxSensitivity (double threshold) = 0;
  virtual void SetRxNoiseFigure (double noiseFigureDb) = 0;
  virtual void SetTxPower (double start) = 0;

  virtual void SetAckTxPower (double start) = 0; 
  virtual void SetDataTxPower (double start) = 0;

  virtual void SetTxGain (double gain) = 0;
  virtual void SetRxGain (double gain) = 0;
  virtual void SetDevice (const Ptr<CdmaNetDevice> device) = 0;
  virtual void SetMac (Ptr<CdmaMac> mac) = 0;
  virtual void SetChannel (Ptr<CdmaChannel> channel) = 0;
  virtual void SetFrequency (double freq) = 0;
  virtual void SetSpreadingGain(int spreading_gain) = 0;
  virtual void SetSpreadingSeq (boost::numeric::ublas::vector<std::complex<double>>& seq) = 0;
  virtual void SetDstNodeid(uint32_t nodeid) = 0;
  virtual void SetMobility (const Ptr<MobilityModel> mobility)=0;
  virtual double GetRxSensitivity (void) const = 0;
  virtual double GetRxNoiseFigure (void) const = 0;
  virtual double GetTxPower (void) const = 0;

  virtual double GetAckTxPower (void) const = 0;
  virtual double GetDataTxPower (void) const = 0;

  virtual double GetTxGain (void) const = 0;
  virtual double GetRxGain (void) const = 0;
  virtual double GetDstNodeid(void) = 0;
  virtual uint32_t GetNodeID (void) = 0;
  virtual void AddNodeIdToNeighList(uint32_t id) = 0;
  virtual uint32_t GetNeigh(uint32_t idx) = 0;
  virtual Ptr<CdmaNetDevice> GetDevice (void) = 0;
  virtual Ptr<CdmaChannel> GetChannel () = 0;
  virtual boost::numeric::ublas::vector<std::complex<double>> GetUserSignature(uint32_t usernodeid) = 0;
  virtual Time GetPktDuration (uint32_t numbytes)=0;
  virtual bool SendPacket (Ptr<Packet> packet) = 0;
  virtual void SendPacketDone(Ptr<Packet> packet) = 0;
  virtual void ReceivePacket (Ptr<Packet> packet,Time txDuration,double_t rxPower, Ptr<CdmaPhy> sender) = 0;
  virtual int GetSpreadingGain (void) const = 0;
  virtual boost::numeric::ublas::vector<std::complex<double>> GetSpreadingSeq (void) const = 0;
  virtual void ReceivePacketDone (Ptr<Packet> packet,double_t rxPower, Ptr<CdmaPhy> sender) = 0;
  virtual bool IsStateIdle (void)=0;
  virtual   Ptr<MobilityModel> GetMobility (void) const = 0;
  virtual bool IsStateRx (void)=0;
  virtual bool IsStateTx (void)=0;

  void NotifyTxBegin (Ptr<const Packet> packet);

  /**
   * Called when the transducer finishes transmitting a packet.
   *
   * This fires a PhyTxEnd trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet The packet.
   */
  void NotifyTxEnd (Ptr<const Packet> packet);

  /**
   * Called when the transducer attempts to transmit a new packet while
   * already transmitting a prior packet.
   *
   * This fires a PhyTxDrop trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet The packet.
   */
  void NotifyTxDrop (Ptr<const Packet> packet);

  /**
   * Called when the Phy begins to receive a packet.
   *
   * This fires a PhyRxBegin trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet The packet.
   */
  void NotifyRxBegin (Ptr<const Packet> packet);

  /**
   * Called when a packet is received without error.
   *
   * This fires a PhyRxEnd trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet The packet.
   */
  void NotifyRxEnd (Ptr<const Packet> packet);

  /**
   * Called when the Phy drops a packet.
   *
   * This fires a PhyRxDrop trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet The packet.
   */
  void NotifyRxDrop (Ptr<const Packet> packet);

 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream First stream index to use.
  * \return The number of stream indices assigned by this model.
  */


  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);

private:
  /**
   * Trace source indicating a packet has begun transmitting
   * over the channel medium.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyTxBeginTrace;

  /**
   * Trace source indicating a packet has been completely transmitted
   * over the channel.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyTxEndTrace;

  /**
   * Trace source indicating a packet has been dropped by the device
   * during transmission.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyTxDropTrace;

  /**
   * Trace source indicating a packet has begun being received
   * from the channel medium by the device.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyRxBeginTrace;

  /**
   * Trace source indicating a packet has been completely received
   * from the channel medium by the device.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyRxEndTrace;

  /**
   * Trace source indicating a packet has been dropped by the device
   * during reception.
   *
   * \see class CallBackTraceSource
   */
  ns3::TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;


};

} //namespace ns3

#endif /* CDMA_PHY_H */
