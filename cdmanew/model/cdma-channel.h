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
** File description: Cdma Channel class header
** File: model/cdma-channel.h
*/
#ifndef CDMA_CHANNEL_H
#define CDMA_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/net-device.h"

#include <list>
#include <vector>
namespace ns3 {

class NetDevice;
class CdmaNetDevice;
class PropagationLossModel;
class PropagationDelayModel;
class CdmaPhy;
class Packet;
class Time;
class CdmaPhyDerived;

class CdmaChannel : public Channel
{
  public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  CdmaChannel ();
  virtual ~CdmaChannel ();
  void Clear ();

  //inherited from Channel.
  virtual std::size_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (std::size_t i) const;
  void AddDevice ( Ptr<CdmaPhy> phy);
  /**
   * \param loss the new propagation loss model.
   */
  void SetPropagationLossModel (Ptr<PropagationLossModel> loss);
  ns3::Ptr<ns3::PropagationLossModel> GetPropagationLossModel();
  /**
   * \param delay the new propagation delay model.
   */
  void SetPropagationDelayModel (Ptr<PropagationDelayModel> delay);
  ns3::Ptr<ns3::PropagationDelayModel> GetPropagationDelayModel();
  bool SendPacket (Ptr<Packet> txPkt,Ptr <CdmaPhy> sender,double txPowerDbm, Time txDuration);

  void ReceivePacket (uint32_t RxNodeId, Ptr<Packet> rxPkt, Time txDuration, double rxPowerDbm, Ptr<CdmaPhy> sender);

  void ReceivePacketDone (uint32_t RxNodeId, Ptr<Packet> rxPkt,double rxPowerDbm,Ptr<CdmaPhy> sender);
  double GetInterference (Ptr <CdmaPhy> receiver, Ptr <CdmaPhy> sender);
  std::vector< std::tuple< Ptr<CdmaPhy>, double> > GetInterferingNodes(Ptr <CdmaPhy> receiver, Ptr <CdmaPhy> sender);
  std::vector<double> GetInterferenceCaused (uint32_t sender_id, uint32_t receiver_id);

  double DbmToW (double dbm);
  double m_noiseFloor;
  typedef std::vector<Ptr<CdmaPhy>> CdmaDeviceList;
  CdmaDeviceList m_devList;

  private:

  Ptr<PropagationLossModel> m_loss;    //!< Propagation loss model
  Ptr<PropagationDelayModel> m_delay;  //!< Propagation delay model

protected:
};


} // ns3
#endif //CDMA_CHANNEL_H
