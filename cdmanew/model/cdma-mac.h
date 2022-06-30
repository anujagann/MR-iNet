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
** File description: MAC layer header 
** File: model/cdma-mac.h
*/
#ifndef CDMA_MAC_H
#define CDMA_MAC_H

#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/event-id.h"
#include "ns3/traced-value.h"
#include "cdma-phy.h"
#include "cdma-net-device.h"
#include "ns3/mac48-address.h"
#include <list>
#include <deque>

namespace ns3 {
    class CdmaPhy;
    class CdmaNetDevice;

    class CdmaMac: public Object
    {
        typedef struct
            {
                uint16_t sequence;  //!< Packet sequence number.
                uint16_t retry;     //!< Retry number.
                Ptr<Packet> packet;
                Time tstart;        //!< Transmission start time.
                Mac48Address destination; //!< Transmission start time.
                bool backoff;
            } PktTx;
            /** Data structure for tracking ACK timeout of transmitted packets */
        typedef struct
        {
            uint16_t sequence;    //!< Packet sequence number.
            EventId m_ackTimeoutEvent;
            Ptr<Packet> packet;
        }   AckTimeouts;

        public:
            CdmaMac ();
            virtual ~CdmaMac ();
            void Clear ();
            static TypeId GetTypeId (void);
              // Inherited methods from CdmaMac
            virtual void AttachPhy (Ptr<CdmaPhy> phy);
            void SetDevice (Ptr<CdmaNetDevice> dev);
            Ptr<CdmaNetDevice> GetDevice (void) const;
            virtual void SetAddress (Mac48Address addr);
            virtual Mac48Address GetAddress () const;
            virtual Mac48Address GetBroadcast (void) const;
            virtual void SendPacketDone (Ptr<Packet> packet);
            virtual void ReceivePacket (Ptr<CdmaPhy> phy, Ptr<Packet> packet);
            virtual void ReceivePacketDone (Ptr<CdmaPhy> phy, Ptr<Packet> packet, Ptr<CdmaPhy> sender);
            virtual void SetForwardUpCb (Callback<void, Ptr<Packet>, Mac48Address, Mac48Address> cb);
            bool Enqueue (Ptr<Packet> pkt, Mac48Address dest);
            void NotifyTx (Ptr<const Packet> packet);
	        void Enqueue (Ptr<const Packet> packet, Mac48Address to, Mac48Address from);

            std::list<Ptr<Packet> > m_pktQueue;  //!< Queue to hold the enqueued packets, that is, packets which are about to be sent.
            void Dequeue (Ptr<Packet> packet);

            int m_buffer; // Number of packets in the buffer

            typedef std::deque<std::pair<int, Time>> buffer_queue; // Defining type helps initialization with size=0 
            buffer_queue m_buffer_queue; // RX ID and Time the packet was added to the buffer

            int m_n_acks_received;
            int m_max_buffer;
            Time m_guard; // Acts as an RX processing time before sending ACKS

            void ReceiveData (Ptr<Packet> packet); // Public for guard

        private:
            void TransmitFirstPacket (); //
            void AckTimeout (uint16_t sequence);
            Time GetCtrlDuration (uint16_t type);
            Time GetDataDuration (Ptr<Packet> p);
            Time m_tend;
            Time m_timeRec;
            uint16_t m_dataRetryLimit;    //!< Maximum number of retry before dropping the packet.
            uint16_t m_FrameLength;       //!< Packet length at the MAC layer
            uint16_t m_sequence;   
            double m_throughput; //!< Throughput of acked packet.
            double m_throughputAll;
            double m_throughputavg;
            int m_discarded;
            int m_ite;
            Time m_ackTimeout;
            /** Send Data packet. */
            Ptr<Packet> m_pktDataTx;
            void SendData (Ptr<Packet> packet);
            bool SendPacket (Ptr<Packet> packet);
            void SendDataDone (bool success, Ptr<Packet> packet);
            void SendAck (Mac48Address source, Mac48Address dest, uint16_t sequence);
            void ReceiveAck (Ptr<Packet> packet);
            bool IsNewSequence (Mac48Address addr, uint16_t seq);
            Callback <void, Ptr<Packet>, Mac48Address, Mac48Address> m_forwardUpCb;
            Mac48Address m_addr; //!< The MAC address.
            Ptr<CdmaPhy> m_phy;     //!< PHY layer attached to this MAC.
            Ptr<CdmaNetDevice> m_device;  //!< Device attached to this MAC.
            uint32_t m_queueLimit;
            std::list<std::pair<Mac48Address, uint16_t> > m_seqList;
            TracedCallback<uint32_t, uint32_t> m_traceEnqueue;
            TracedCallback<uint32_t, uint32_t, bool> m_traceSendDataDone;
            TracedCallback<uint32_t, uint32_t> m_traceAckTimeout;
            TracedCallback<Ptr<const Packet> > m_macTxTrace;
            //add trace throughput
            TracedCallback<double> m_traceThroughput;
            std::list<PktTx> m_pktTx;
            std::list<AckTimeouts> m_ackTimeouts;
            typedef std::vector<Ptr<CdmaPhy>> CdmaDeviceList1;
            CdmaDeviceList1 m_devList1;

    };
}
#endif // CDMA_MAC_H
