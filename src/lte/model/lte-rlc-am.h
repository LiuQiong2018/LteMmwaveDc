/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef LTE_RLC_AM_H
#define LTE_RLC_AM_H

#include <ns3/event-id.h>
#include <ns3/lte-rlc-sequence-number.h>
#include <ns3/lte-rlc.h>
#include "ns3/codel-queue-disc.h"

#include "ns3/lte-rrc-sap.h" // woody
#include "ns3/lte-enb-rrc.h" // woody
#include "ns3/lte-ue-rrc.h" // woody
#include "ns3/network-module.h"
#include <vector>
#include <map>

namespace ns3 {

/**
 * LTE RLC Acknowledged Mode (AM), see 3GPP TS 36.322
 */
class LteRlcAm : public LteRlc
{
public:
  LteRlcAm ();
  virtual ~LteRlcAm ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  /**
   * RLC SAP
   */
  virtual void DoTransmitPdcpPdu (Ptr<Packet> p);

  /**
   * MAC SAP
   */
  virtual void DoNotifyTxOpportunity (uint32_t bytes, uint8_t layer, uint8_t harqId);
  virtual void DoNotifyHarqDeliveryFailure ();
  virtual void DoNotifyDlHarqDeliveryFailure (uint8_t harqId);
  virtual void DoNotifyUlHarqDeliveryFailure (uint8_t harqId);
  virtual void DoReceivePdu (Ptr<Packet> p);

  // sjkang
  virtual double GetBufferSize();
  void GetReportBufferStatus( LteMacSapProvider::ReportBufferStatusParameters r);
  int count=0, sum;
  int p;
  double averageBufferSize;
  Time SamplingTime;
  uint32_t ArrayInMovingWindow[11];

  // woody
  virtual void SetAssistInfoPtr (LteRrcSap::AssistInfo* assistInfoPtr);
  virtual void IsEnbRlc (void);
  virtual void SetRrc (Ptr<LteEnbRrc> enbRrc, Ptr<LteUeRrc> ueRrc);
  virtual void CalculatePathThroughput (std::ofstream *streamPathThroughput);
  void SetRlcAmIdentity(uint16_t imsi, uint16_t bearerId, bool isMenb, bool isMmenb);
  void ReportRlcBufferSizeForUE();
private:
  // woody
  LteRrcSap::AssistInfo *m_assistInfoPtr;
  bool m_isEnbRlc;
  Ptr<LteEnbRrc> m_enbRrc;
  Ptr<LteUeRrc> m_ueRrc;
  uint32_t sumPacketSize;
  uint32_t lastSumPacketSize;
  std::map<Ptr<LteRlcAm>,Ptr<OutputStreamWrapper>>FileStremFromRlcAddress;
  /**
   * This method will schedule a timeout at WaitReplyTimeout interval
   * in the future, unless a timer is already running for the cache,
   * in which case this method does nothing.
   */
  void ExpireReorderingTimer (void);
  void ExpirePollRetransmitTimer (void);
  void ExpireRbsTimer (void);

  /** 
   * method called when the T_status_prohibit timer expires
   * 
   */
  void ExpireStatusProhibitTimer (void);

  bool IsInsideReceivingWindow (SequenceNumber10 seqNumber);
// 
//   void ReassembleOutsideWindow (void);
//   void ReassembleSnLessThan (uint16_t seqNumber);
// 
  void ReassembleAndDeliver (Ptr<Packet> packet);

  void DoReportBufferStatus ();

private:
    std::vector < Ptr<Packet> > m_txonBuffer;       // Transmission buffer
    Ptr<CoDelQueueDisc> m_txonQueue; //the packets comming from PDCP first stored in this queue and move to m_txonBuffer during transmission.

    struct RetxPdu
    {
      Ptr<Packet> m_pdu;
      uint16_t    m_retxCount;
    };

    struct RetxSegPdu
		{
			Ptr<Packet> m_pdu;
			uint16_t    m_retxCount;
			bool			m_lastSegSent;		// all segments sent, waiting for ACK
		};

  std::vector <RetxPdu> m_txedBuffer;  ///< Buffer for transmitted and retransmitted PDUs 
                                       ///< that have not been acked but are not considered 
                                       ///< for retransmission 
  std::vector <RetxPdu> m_retxBuffer;  ///< Buffer for PDUs considered for retransmission
  std::vector <RetxSegPdu> m_retxSegBuffer;  // buffer for AM PDU segments

    uint32_t m_txonBufferSize;
    uint32_t m_retxBufferSize;
    uint32_t m_txedBufferSize;

    bool     m_statusPduRequested;
    uint32_t m_statusPduBufferSize;

    struct PduBuffer
    {
      SequenceNumber10  m_seqNumber;
      std::list < Ptr<Packet> >  m_byteSegments;

      bool      m_pduComplete;
    };

    std::map <uint16_t, PduBuffer > m_rxonBuffer; // Reception buffer

    Ptr<Packet> m_controlPduBuffer;               // Control PDU buffer (just one PDU)

    // SDU reassembly
//   std::vector < Ptr<Packet> > m_reasBuffer;     // Reassembling buffer
// 
    std::list < Ptr<Packet> > m_sdusBuffer;       // List of SDUs in a packet (PDU)

  /**
   * State variables. See section 7.1 in TS 36.322
   */
  // Transmitting side
  SequenceNumber10 m_vtA;                   // VT(A)
  SequenceNumber10 m_vtMs;                  // VT(MS)
  SequenceNumber10 m_vtS;                   // VT(S)
  SequenceNumber10 m_pollSn;                // POLL_SN

  // Receiving side
  SequenceNumber10 m_vrR;                   // VR(R)
  SequenceNumber10 m_vrMr;                  // VR(MR)
  SequenceNumber10 m_vrX;                   // VR(X)
  SequenceNumber10 m_vrMs;                  // VR(MS)
  SequenceNumber10 m_vrH;                   // VR(H)

  /**
   * Counters. See section 7.1 in TS 36.322
   */
  uint32_t m_pduWithoutPoll;
  uint32_t m_byteWithoutPoll;

  /**
   * Constants. See section 7.2 in TS 36.322
   */
  uint16_t m_windowSize;

  /**
   * Timers. See section 7.3 in TS 36.322
   */
  EventId m_pollRetransmitTimer;
  Time    m_pollRetransmitTimerValue;
  EventId m_reorderingTimer;
  Time    m_reorderingTimerValue;
  EventId m_statusProhibitTimer;
  Time    m_statusProhibitTimerValue;
  EventId m_rbsTimer;
  Time    m_rbsTimerValue;

  /**
   * Configurable parameters. See section 7.4 in TS 36.322
   */
  uint16_t m_maxRetxThreshold;  /// \todo How these parameters are configured???
  uint16_t m_pollPdu;
  uint16_t m_pollByte;
  
  bool m_txOpportunityForRetxAlwaysBigEnough;
  bool m_pollRetransmitTimerJustExpired;

  /**
   * SDU Reassembling state
   */
  typedef enum { NONE            = 0,
                 WAITING_S0_FULL = 1,
                 WAITING_SI_SF   = 2 } ReassemblingState_t;
  ReassemblingState_t m_reassemblingState;
  Ptr<Packet> m_keepS0;

  /**
   * Expected Sequence Number
   */
  SequenceNumber10 m_expectedSeqNumber;

  std::map <uint8_t, uint16_t> m_harqIdToSnMap;

  uint32_t m_maxTxBufferSize;
  bool m_enableAqm;
};


} // namespace ns3

#endif // LTE_RLC_AM_H
