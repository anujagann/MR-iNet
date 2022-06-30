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
** Author(s): Anu Jagannath
** Email:  ajagannath@androcs.com
** File description: CDMA Phy layer utilities
** File: model/cdma-phy-derived.h
*/
#ifndef CDMA_PHY_DERIVED_H
#define CDMA_PHY_DERIVED_H

#include "ns3/type-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/pointer.h"
#include <list>
#include <vector>
#include <complex>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "cdma-phy.h"

namespace ns3 {

class CdmaChannel;
class CdmaMac;
class CdmaNetDevice;

class CdmaPhyDerived : public CdmaPhy
{
  public:

  typedef enum
  {
    IDLE, TX, RX, OFF
  } State;
  State m_state {OFF};
  void ChangeState (State new_state);
  void SetStateToIdle();
  Time m_state_delay; // Used to provide some time delay when switching from the TX state to IDLE.

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  CdmaPhyDerived ();
  virtual ~CdmaPhyDerived ();
  virtual bool IsStateIdle (void);
  virtual bool IsStateRx (void);
  virtual bool IsStateTx (void);

  
  Time GetPktDuration (uint32_t n_bytes);

  void Clear(void);

  void SetRxSensitivity (double threshold);

  double GetRxSensitivity (void) const;

  void SetRxNoiseFigure (double noiseFigureDb);

  double GetRxNoiseFigure (void) const;

  void SetTxPower (double start);
 
  double GetTxPower (void) const;

  void SetAckTxPower (double start);
 
  double GetAckTxPower (void) const;

  void SetDataTxPower (double start);
 
  double GetDataTxPower (void) const;

  void SetTxGain (double gain);

  double GetTxGain (void) const;

  void SetRxGain (double gain);

  double GetRxGain (void) const;

  void SetDevice (const Ptr<CdmaNetDevice> device);

  void SetMac (Ptr<CdmaMac> mac);

  Ptr<CdmaNetDevice> GetDevice (void);

  void SetChannel (Ptr<CdmaChannel> channel);
    
  Ptr<CdmaChannel> GetChannel ();

  void SetMobility (const Ptr<MobilityModel> mobility);

  Ptr<MobilityModel> GetMobility (void) const;

  void SetFrequency (double freq);

  double GetFrequency (void) const;

  void AddNodeIdToNeighList(uint32_t id);
  uint32_t GetNeigh(uint32_t idx);

  double GetDstNodeid(void);
  void SetDstNodeid(uint32_t nodeid);
  uint32_t GetNodeID (void);

  bool SendPacket (Ptr<Packet> packet);

  void SendPacketDone(Ptr<Packet> packet);

  void ReceivePacket (Ptr<Packet> packet,Time txDuration,double_t rxPower, Ptr<CdmaPhy> sender);

  void SetSpreadingGain(int spreading_gain);

  int GetSpreadingGain (void) const;

  void SetSpreadingSeq (boost::numeric::ublas::vector<std::complex<double>>& seq);

  boost::numeric::ublas::vector<std::complex<double>> GetSpreadingSeq (void) const;

  void ReceivePacketDone (Ptr<Packet> packet,double_t rxPower, Ptr<CdmaPhy> sender);

  bool IsTransmissionSuccessul(double sinrW, Ptr<CdmaPhy> sender);

  boost::numeric::ublas::vector<std::complex<double>> GetUserSignature(uint32_t usernodeid);
  boost::numeric::ublas::matrix<std::complex<double>>m_SPREADING_MATRIX;
  boost::numeric::ublas::vector<std::complex<double>>m_spreading_seq;

  

private:

  double   m_rxSensitivityW; //!< Receive sensitivity threshold in watts
  double   m_txgain; //!< Transmission gain (dB)
  double   m_rxgain; //!< Reception gain (dB)
  double   m_txpower; // (dBm)
  double   m_ack_txpower; // Switch m_txpower to this value for ACK transmissions
  double   m_data_txpower; // Switch m_txpower to this value for data transmissions
  double   m_rxnoisefig;
  double   m_freq; // Hz
  double   m_bitRate; // Mbps
  double   m_preambleBytes;
  double   m_HeaderBytes;
  int      m_spreading_gain; // length of spreading signature (number of chips)
  double   m_SINRth;
  uint32_t m_dstnodeid;
  Ptr<MobilityModel> m_mobility; //!< Pointer to the mobility model
  Ptr<Packet> m_RxPkt;
  Ptr<CdmaNetDevice> m_device; //!< Pointer to the device
  Ptr<CdmaChannel> m_channel;
  Ptr<CdmaMac> m_mac; // Original
  
  protected:

};

} //namespace ns3

#endif /* CDMA_PHY_DERIVED_H */
