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
 * Author: Jaume Nin <jnin@cttc.cat>
 *         Nicola Baldo <nbaldo@cttc.cat>
 */


#include "epc-sgw-pgw-application.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4.h"
#include "ns3/inet-socket-address.h"
#include "ns3/epc-gtpu-header.h"
#include "ns3/abort.h"
#include <fstream>
#include "ns3/simulator.h"
#include "Gtpu_SN_Header.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EpcSgwPgwApplication");

/////////////////////////
// UeInfo
/////////////////////////


EpcSgwPgwApplication::UeInfo::UeInfo ()
{
  NS_LOG_FUNCTION (this);
}

void
EpcSgwPgwApplication::UeInfo::AddBearer (Ptr<EpcTft> tft, uint8_t bearerId, uint32_t teid)
{
  NS_LOG_FUNCTION (this << tft << teid);
  m_teidByBearerIdMap[bearerId] = teid;
  return m_tftClassifier.Add (tft, teid);
}

void
EpcSgwPgwApplication::UeInfo::RemoveBearer (uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << bearerId);
  m_teidByBearerIdMap.erase (bearerId);
}

uint32_t
EpcSgwPgwApplication::UeInfo::Classify (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  // we hardcode DOWNLINK direction since the PGW is espected to
  // classify only downlink packets (uplink packets will go to the
  // internet without any classification). 
  return m_tftClassifier.Classify (p, EpcTft::DOWNLINK);
}

Ipv4Address 
EpcSgwPgwApplication::UeInfo::GetEnbAddr ()
{
  return m_enbAddr;
}

void
EpcSgwPgwApplication::UeInfo::SetEnbAddr (Ipv4Address enbAddr)
{
  m_enbAddr = enbAddr;
}


Ipv4Address 
EpcSgwPgwApplication::UeInfo::GetSenbAddr () // sychoi, woody inserted
{
  return m_senbAddr;
}

void
EpcSgwPgwApplication::UeInfo::SetSenbAddr (Ipv4Address senbAddr) // sychoi, woody inserted
{
  m_senbAddr = senbAddr;
}


Ipv4Address 
EpcSgwPgwApplication::UeInfo::GetUeAddr ()
{
  return m_ueAddr;
}

void
EpcSgwPgwApplication::UeInfo::SetUeAddr (Ipv4Address ueAddr)
{
  m_ueAddr = ueAddr;
}

/////////////////////////
// EpcSgwPgwApplication
/////////////////////////


TypeId
EpcSgwPgwApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcSgwPgwApplication")
    .SetParent<Object> ()
    .SetGroupName("Lte");
  return tid;
}

void
EpcSgwPgwApplication::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_s1uSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_s1uSocket = 0;
  delete (m_s11SapSgw);
}

  
//std::ofstream OutFile_forEtha1X ("etha_At_1X.txt");

EpcSgwPgwApplication::EpcSgwPgwApplication (const Ptr<VirtualNetDevice> tunDevice, const Ptr<Socket> s1uSocket)
  : m_s1uSocket (s1uSocket),
    m_tunDevice (tunDevice),
    m_gtpuUdpPort (2152), // fixed by the standard
    m_teidCount (0),
    m_s11SapMme (0)
{
  NS_LOG_FUNCTION (this << tunDevice << s1uSocket);
  m_s1uSocket->SetRecvCallback (MakeCallback (&EpcSgwPgwApplication::RecvFromS1uSocket, this));
  m_s11SapSgw = new MemberEpcS11SapSgw<EpcSgwPgwApplication> (this);
//  OutFile_forEtha1X << "time\tteid\tMenb_etha_delay\tSenb_etha_delay\tMenb_etha_Queue\tSenb_etha_Queue" << std::endl;
}

  
EpcSgwPgwApplication::~EpcSgwPgwApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
EpcSgwPgwApplication::SetSplitAlgorithm (uint16_t splitAlgorithm) // woody
{
  m_splitAlgorithm = splitAlgorithm;
}

//LteRrcSap::AssistInfo info1X[3];

void
EpcSgwPgwApplication::RecvAssistInfo (LteRrcSap::AssistInfo assistInfo){ // woody
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_isAssistInfoSink == true, "Not a assist info sink");

  int nodeNum;
  if (assistInfo.is_enb && assistInfo.is_menb) nodeNum = 0;
  else if (assistInfo.is_enb) nodeNum = 1;
  else nodeNum = 2;

//NS_LOG_UNCOND("nodeNum " << nodeNum << " pdcp_sn " << assistInfo.pdcp_sn << " pdcp_delay " << assistInfo.pdcp_delay << " rlc_avg_buffer " << assistInfo.rlc_avg_buffer << " rlc_tx_queue " << assistInfo.rlc_tx_queue << " rlc_retx_queue " << assistInfo.rlc_retx_queue << " rlc_tx_queue_hol_delay " << assistInfo.rlc_tx_queue_hol_delay << " rlc_retx_queue_hol_delay " << assistInfo.rlc_retx_queue_hol_delay );

  std::map<uint32_t, TeidInfo>::iterator teidit = m_teidInfoMap.find (assistInfo.bearerId); // woody
  NS_ASSERT_MSG (teidit != m_teidInfoMap.end (), "unknown TEID " << assistInfo.bearerId); // woody

  teidit->second.info1X[nodeNum] = assistInfo;
//NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\tSgwPgw\t" << nodeNum << "\t" << assistInfo.rlc_average_delay << "\t" << teidit->second.info1X[nodeNum].rlc_average_delay << "\t" << (unsigned) assistInfo.bearerId << "\t" << assistInfo.is_enb << "\t" << assistInfo.is_menb);

  return;
}

//double pre_QueueSize=0;
bool ToSenb=false, ToMenb=false;
int p1,p2;
int ChunkSize = 50;

//std::ofstream OutFile_test ("test_algorithm.txt");

void
EpcSgwPgwApplication::UpdateEthas(TeidInfo *t, uint32_t teid){
/// the split algorithm using RLC AM queuing delay
	double delayAtMenb, delayAtSenb;//sjkang
	delayAtMenb = t->info1X[0].rlc_tx_queue_hol_delay;// + t->info1X[0].rlc_retx_queue_hol_delay;
	delayAtSenb = t->info1X[1].rlc_tx_queue_hol_delay;// + t->info1X[1].rlc_retx_queue_hol_delay;
	double DelayDifferenceAtMenb = std::max (targetDelay - delayAtMenb,sigma);
	double DelayDifferenceAtSenb = std::max (targetDelay - delayAtSenb,sigma);

	t->etha_AtMenbFromDelay= DelayDifferenceAtMenb / (DelayDifferenceAtMenb+DelayDifferenceAtSenb);
	t->etha_AtSenbFromDelay = DelayDifferenceAtSenb / (DelayDifferenceAtMenb+DelayDifferenceAtSenb);
//	t->pastEthaAtMenbFromDelay = (1-alpha)*t->pastEthaAtMenbFromDelay + alpha*t->etha_AtMenbFromDelay;
//	t->pastEthaAtSenbFromDelay = (1-alpha)*t->pastEthaAtSenbFromDelay + alpha*t->etha_AtSenbFromDelay;
	t->pastEthaAtMenbFromDelay = t->pastEthaAtMenbFromDelay * (alpha * 2 * t->etha_AtMenbFromDelay);
	t->pastEthaAtSenbFromDelay = 1 - t->pastEthaAtMenbFromDelay;


        //targetDelay = std::max(delayAtMenb, delayAtSenb);    
        //targetDelay += targetDelay*0.2;

/*
	double ThroughputAtSenb = t->info1X[1].averageThroughput;
        double ThroughputAtMenb = t->info1X[0].averageThroughput;
	double targetThroughput_AtMenb = 10000000;
	double targetThroughput_AtSenb = 9000000;
	double theSumOfThroughputRatio = targetThroughput_AtMenb/ThroughputAtMenb +targetThroughput_AtSenb/ThroughputAtSenb;

	t->etha_AtMenbFrom_Thr_ = (targetThroughput_AtMenb/ThroughputAtMenb) / theSumOfThroughputRatio;
	t->etha_AtSenbFrom_Thr_ = (targetThroughput_AtSenb/ThroughputAtSenb) / theSumOfThroughputRatio;
*/

	 double queueSizeAtMenb, queueSizeAtSenb;
 	 queueSizeAtMenb = t->info1X[0].rlc_retx_queue + t->info1X[0].rlc_tx_queue;
	 queueSizeAtSenb = t->info1X[1].rlc_retx_queue + t->info1X[1].rlc_tx_queue;

 	double QueueDifferenceAtMenb = std::max (t->targetQueueSize - queueSizeAtMenb, sigma*1000);
 	double QueueDifferenceAtSenb = std::max (t->targetQueueSize - queueSizeAtSenb, sigma*1000);

 	t->etha_AtMenbFromQueueSize = QueueDifferenceAtMenb / (QueueDifferenceAtMenb+QueueDifferenceAtSenb);
 	t->etha_AtSenbFromQueueSize = QueueDifferenceAtSenb / (QueueDifferenceAtMenb+QueueDifferenceAtSenb);
	t->pastEthaAtMenbFromQueueSize = (1-alpha)*t->pastEthaAtMenbFromQueueSize + alpha*t->etha_AtMenbFromQueueSize;
	t->pastEthaAtSenbFromQueuesize = (1-alpha)*t->pastEthaAtSenbFromQueuesize + alpha*t->etha_AtSenbFromQueueSize;
	t->targetQueueSize = std::max(queueSizeAtMenb, queueSizeAtSenb);
	t->targetQueueSize += t->targetQueueSize*0.2;

//OutFile_test << Simulator::Now().GetSeconds()<< "\t"
//<< teid << "\t"
//<< delayAtMenb << "\t"
//<< delayAtSenb << "\t"
//<< DelayDifferenceAtMenb << "\t"
//<< DelayDifferenceAtSenb << "\t"
//<< t->etha_AtMenbFromDelay << "\t"
//<< t->etha_AtSenbFromDelay << "\t"
//<< queueSizeAtMenb << "\t"
//<< queueSizeAtSenb << "\t"
//<< QueueDifferenceAtMenb << "\t"
//<< QueueDifferenceAtSenb << "\t"
//<< t->etha_AtMenbFromQueueSize << "\t"
//<< t->etha_AtSenbFromQueueSize << "\t"
//<< t->targetQueueSize << std::endl;

//targetQueueSize = std::max(2.5*t_targetQueueSize - 1.5*pre_QueueSize,t_targetQueueSize+t_targetQueueSize*0.2); 
        //pre_QueueSize = t_targetQueueSize; 
    //  std::cout << ThroughputAtMenb << "\t" << ThroughputAtSenb << std::endl;
           /* if ( delayAtSenb - delayAtMenb > 1.0 && queueSizeAtMenb !=0 && queueSizeAtSenb!=0){
            ToMenb=true;
            ToSenb=false;
                
                      
           }
        else if(delayAtMenb - delayAtSenb >1.0 && queueSizeAtMenb !=0 && queueSizeAtSenb!=0 ){
                ToMenb=false;
                ToSenb=true;
   
                }
        else*/ 
	if (t->info1X[0].rlc_retx_queue > 10000 ) {
			ToSenb=true;
			ToMenb= false;
		}
	else if (t->info1X[1].rlc_retx_queue > 10000) {
		ToMenb= true;        
		ToSenb=false ; 
		}
       else{
                ToSenb=false;
                ToMenb=false;
        }
//  	OutFile_forEtha1X << Simulator::Now().GetSeconds()<< "\t"
//		<< teid << "\t"
//		<< t->pastEthaAtMenbFromDelay << "\t"
//		<< t->pastEthaAtSenbFromDelay << "\t"
//		<< t->pastEthaAtMenbFromQueueSize << "\t"
//		<< t->pastEthaAtSenbFromQueuesize
//		<< std::endl;
  		//	<< pastEthaAtSenbFromDelay << "\t" << ThroughputAtMenb << "\t" << ThroughputAtSenb << std::endl;
}

//int count_forSplitting_At_SgwPgw=0;

//std::ofstream OutFile_test5 ("test_algorithm5.txt");
//std::ofstream OutFile_test6 ("test_algorithm6.txt");

void
EpcSgwPgwApplication::SplitTimer (TeidInfo *t, uint32_t teid) // woody
{
	NS_LOG_FUNCTION (this);
	
	if (m_splitAlgorithm == 6)
	{
		double queueSizeMenb, queueSizeSenb;
		queueSizeMenb = t->info1X[0].rlc_average_queue;
		queueSizeSenb = t->info1X[1].rlc_average_queue;

		if (queueSizeMenb == 0 && queueSizeSenb == 0 && t->prevQueueSizeMenb == 0 && t->prevQueueSizeSenb == 0 && t->packetNum == 0)
		{
		}
		else
		{

		double dataRateMenb, dataRateSenb;
		dataRateMenb = std::max(t->prevQueueSizeMenb - queueSizeMenb + t->sumPacketSizeMenb, 0.0);
		dataRateSenb = std::max(t->prevQueueSizeSenb - queueSizeSenb + t->sumPacketSizeSenb, 0.0);
		t->cumDataRateMenb = (1-beta)*t->cumDataRateMenb + beta*dataRateMenb;
		t->cumDataRateSenb = (1-beta)*t->cumDataRateSenb + beta*dataRateSenb;

		double tempt, nextEthaMenb;
		tempt = std::max(t->packetNum * 1400.0 * (t->cumDataRateMenb + t->cumDataRateSenb), 0.000001);
		nextEthaMenb = std::max(std::min(((t->cumDataRateMenb * (queueSizeSenb + (t->packetNum * 1400.0))) - (t->cumDataRateSenb * queueSizeMenb))
			/ tempt, 1.0), 0.02);
/*	
		double dataRateMenb, dataRateSenb;
		dataRateMenb = std::max(t->prevQueueSizeMenb - queueSizeMenb + (t->ethaMenb * t->packetNum), 0.0);
		dataRateSenb = std::max(t->prevQueueSizeSenb - queueSizeSenb + ((1 - t->ethaMenb) * t->packetNum), 0.0);

		double tempt, nextEthaMenb;
		tempt = std::max(t->packetNum * (dataRateMenb + dataRateSenb), 0.000001);
		nextEthaMenb = std::max(std::min(((dataRateMenb * (queueSizeSenb + t->packetNum)) - (dataRateSenb * queueSizeMenb))
			/ tempt, 1.0), 0.0);
*/
		t->ethaMenb = (1-alpha)*t->ethaMenb + alpha*nextEthaMenb;
		t->prevQueueSizeMenb = queueSizeMenb;
		t->prevQueueSizeSenb = queueSizeSenb;

//OutFile_test6
//	<< Simulator::Now().GetSeconds() << "\t"
//	<< teid << "\t"
//	<< queueSizeMenb << "\t"
//	<< queueSizeSenb << "\t"
//	<< dataRateMenb << "\t"
//	<< dataRateSenb << "\t"
//	<< t->cumDataRateMenb << "\t"
//	<< t->cumDataRateSenb << "\t"
//	<< t->packetNum << "\t"
//	<< nextEthaMenb << "\t"
//	<< t->ethaMenb << "\t"
//	<< t->sumPacketSizeMenb << "\t"
//	<< t->sumPacketSizeSenb <<  std::endl;

		}
	}
	else if (m_splitAlgorithm == 5)
	{
//NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << teid << "\t" << t->info1X[0].rlc_average_delay << "\t" << this);
		double delayMenb, delaySenb;
		delayMenb = t->info1X[0].rlc_average_delay;
		delaySenb = t->info1X[1].rlc_average_delay;

		double temptM, temptS;
		temptM = std::max(delayMenb - t->prevDelayMenb + splitTimerInterval, 0.000001);
		temptS = std::max(delaySenb - t->prevDelaySenb + splitTimerInterval, 0.000001);
		double dataRateMenb, dataRateSenb;
		dataRateMenb = t->ethaMenb * t->packetNum * splitTimerInterval / temptM;
		dataRateSenb = (1 - t->ethaMenb) * t->packetNum * splitTimerInterval / temptS;

		double nextEthaMenb;
		nextEthaMenb = (delaySenb - delayMenb + (t->packetNum * splitTimerInterval / dataRateSenb)) / t->packetNum / splitTimerInterval / ((1 / dataRateMenb) + (1 / dataRateSenb));
		t->ethaMenb = (1-alpha)*t->ethaMenb + alpha*nextEthaMenb;
		t->prevDelayMenb = delayMenb;
		t->prevDelaySenb = delaySenb;

//OutFile_test5
//	<< Simulator::Now().GetSeconds() << "\t"
//	<< teid << "\t"
//	<< delayMenb << "\t"
//	<< delaySenb << "\t"
//	<< dataRateMenb << "\t"
//	<< dataRateSenb << "\t"
//	<< t->packetNum << "\t"
//	<< nextEthaMenb << "\t"
//	<< t->ethaMenb << std::endl;

	}

	t->packetNum = 0;
	t->sumPacketSizeMenb = 0;
	t->sumPacketSizeSenb = 0;
	Simulator::Schedule (MilliSeconds (splitTimerInterval), &EpcSgwPgwApplication::SplitTimer, this, t, teid);
}

void
EpcSgwPgwApplication::SetSplitTimerInterval (int i) // woody
{
  splitTimerInterval = i;
}

void
EpcSgwPgwApplication::SetParameters (double a, double b) // woody
{
  alpha = a;
  beta = b;
}

int
EpcSgwPgwApplication::SplitAlgorithm (TeidInfo *teidInfo, uint32_t teid) // woody
{
  NS_LOG_FUNCTION (this);
/*
 0: MeNB only
 1: SeNB only
 2: alternative splitting

*/

  if (teidInfo->isInitialized == false)
  {
    teidInfo->countChunk = 0;
    teidInfo->etha_AtMenbFromDelay = 0.5;
    teidInfo->etha_AtSenbFromDelay = 0.5;
    teidInfo->etha_AtMenbFromQueueSize = 0.5;
    teidInfo->etha_AtSenbFromQueueSize = 0.5;
    teidInfo->pastEthaAtMenbFromDelay = 0.5;
    teidInfo->pastEthaAtSenbFromDelay = 0.5;
    teidInfo->pastEthaAtMenbFromQueueSize = 0.5;
    teidInfo->pastEthaAtSenbFromQueuesize = 0.5;
    teidInfo->isInitialized = true;
/*    if (m_splitAlgorithm == 6 || m_splitAlgorithm == 5)
    {
      Simulator::Schedule (MilliSeconds (splitTimerInterval), &EpcSgwPgwApplication::SplitTimer, this, teidInfo, teid);
    }*/
    Simulator::Schedule (MilliSeconds (splitTimerInterval), &EpcSgwPgwApplication::SplitTimer, this, teidInfo, teid);
    teidInfo->packetNum = 0;
    teidInfo->ethaMenb = 0.5;
    teidInfo->prevQueueSizeMenb = 0;
    teidInfo->prevQueueSizeSenb = 0;
    teidInfo->prevDelayMenb = 0;
    teidInfo->prevDelaySenb = 0;
    teidInfo->info1X[0].rlc_average_delay = 0;
    teidInfo->info1X[1].rlc_average_delay = 0;
    teidInfo->info1X[0].rlc_average_queue = 0;
    teidInfo->info1X[1].rlc_average_queue = 0;
    teidInfo->cumDataRateMenb = 0;
    teidInfo->cumDataRateSenb = 0;
    teidInfo->sumPacketSizeMenb = 0;
    teidInfo->sumPacketSizeSenb = 0;
  }

  // return 0 for Tx through MeNB &  return 1 for Tx through SeNB
  switch (m_splitAlgorithm)
  {
    case 0:
      return 0;
      break;

    case 1:
      return 1;
      break;

    case 2:
      if (teidInfo->lastDirection1X == 0) return 1;
      else return 0;
      break;
    case 3:
 		if (teidInfo->countChunk > ChunkSize * (teidInfo->pastEthaAtSenbFromDelay + teidInfo->pastEthaAtMenbFromDelay)){
    	        	UpdateEthas(teidInfo, teid);

    	        	teidInfo->countChunk = 0;
    	        	return 0;
    	        }
    	        else if (teidInfo->countChunk < teidInfo->pastEthaAtMenbFromDelay * ChunkSize)
    	        {

    	        	teidInfo->countChunk++;
    	        	return 0;

    	        }
    	        else if (teidInfo->countChunk >= teidInfo->pastEthaAtMenbFromDelay * ChunkSize
    	        		&& teidInfo->countChunk <= ChunkSize * (teidInfo->pastEthaAtMenbFromDelay + teidInfo->pastEthaAtSenbFromDelay))
    	        {
    	          	teidInfo->countChunk++;
    	        	return 1;
    	        }
    break;
   case 4: 
         
   /*      if (ToSenb == true && ToMenb==false ) {
                UpdateEthas();
                return 1;
                }
        else if (ToMenb == true && ToSenb==false) {
                UpdateEthas();
                return 0;
                }
 	 else*/ 
	{		
		if (teidInfo->countChunk > ChunkSize * (teidInfo->pastEthaAtSenbFromQueuesize + teidInfo->pastEthaAtMenbFromQueueSize)){
  	        	UpdateEthas(teidInfo, teid);

  	        	teidInfo->countChunk =0;
  	        	return 0;
  	        }
  	        else if (teidInfo->countChunk < teidInfo->pastEthaAtMenbFromQueueSize * ChunkSize)
  	        {

  	        	teidInfo->countChunk++;
  	        	return 0;

  	        }
  	        else if (teidInfo->countChunk >= teidInfo->pastEthaAtMenbFromQueueSize * ChunkSize
  	        		&& teidInfo->countChunk <= ChunkSize * (teidInfo->pastEthaAtMenbFromQueueSize + teidInfo->pastEthaAtSenbFromQueuesize))
  	        {
  	          	teidInfo->countChunk++;
  	        	return 1;
  	        }
	}
  	        break;
    case 5:
      teidInfo->packetNum++;
      teidInfo->countChunk++;

      if (teidInfo->countChunk < teidInfo->ethaMenb * ChunkSize) return 0;
      else if ((teidInfo->countChunk >= teidInfo->ethaMenb * ChunkSize) && (teidInfo->countChunk <= ChunkSize)) return 1;
      else
      {
        teidInfo->countChunk = 0;
        return 1;
      }

      break;

   case 6:
      teidInfo->packetNum++;
      teidInfo->countChunk++;

      if (teidInfo->countChunk < (1 - teidInfo->ethaMenb) * ChunkSize) return 1;
      else if ((teidInfo->countChunk >= (1 - teidInfo->ethaMenb) * ChunkSize) && (teidInfo->countChunk <= ChunkSize)) return 0;
      else
      {
        teidInfo->countChunk = 1;
        return 1;
      }

      break;

  }
  return -1;
}

bool
EpcSgwPgwApplication::RecvFromTunDevice (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << source << dest << packet << packet->GetSize ());

  // get IP address of UE
  Ptr<Packet> pCopy = packet->Copy ();
  Ipv4Header ipv4Header;
  pCopy->RemoveHeader (ipv4Header);
  Ipv4Address ueAddr =  ipv4Header.GetDestination ();
  NS_LOG_LOGIC ("packet addressed to UE " << ueAddr);

  // woody, for observing packet flow
  NS_LOG_INFO ("***SgwPgw, " << Simulator::Now ().GetSeconds () << "s "
              <<  packet->GetSize () << " bytes from "
              << ipv4Header.GetSource () << " to " << ueAddr );


  // find corresponding UeInfo address
  std::map<Ipv4Address, Ptr<UeInfo> >::iterator it = m_ueInfoByAddrMap.find (ueAddr);
  if (it == m_ueInfoByAddrMap.end ())
    {        
      NS_LOG_WARN ("unknown UE address " << ueAddr);
NS_LOG_UNCOND ("unknown UE address " << ueAddr);
    }
  else
    {
      Ipv4Address enbAddr = it->second->GetEnbAddr ();      
      uint32_t teid = it->second->Classify (packet);   
      if (teid == 0)
        {
          NS_LOG_WARN ("no matching bearer for this packet");                   
NS_LOG_UNCOND ("no matching bearer for this packet");                   
        }
      else
        {
    	  /** sychoi, modified by woody
    	   * Here, we need to implement the branch point for selecting the next eNB to steer DL packet.
    	   * Using newly defined map, m_dcEnbAddrByTeidMap, find the SeNB address, senbAddr.
    	   * Because of backward compatibility, m_dcEnbAddrByTeidMap is used only for DC packet routing.
    	   * If there is no SeNB Address, then the packet is forwarded to MeNB.
    	   * Otherwise, the packet is forwarded to SeNB.
    	   */
          if(it->second->dcType == 0 || it->second->dcType == 2){
            NS_LOG_INFO ("***SgwPgw send to MeNB " << enbAddr << " with teid " << teid);
            SendToS1uSocket (packet, enbAddr, teid);
          }
          else if(it->second->dcType == 1){
            std::map<uint32_t, Ipv4Address>::iterator senbAddrIt = m_dcEnbAddrByTeidMap.find (teid);
            if (senbAddrIt == m_dcEnbAddrByTeidMap.end ())
            {
              NS_LOG_INFO ("***SgwPgw send to MeNB " << enbAddr << " with teid " << teid);
              SendToS1uSocket (packet, enbAddr, teid);
            }
            else
            {
              Ipv4Address senbAddr = senbAddrIt->second;
              NS_LOG_INFO ("***SgwPgw send to SeNB " << senbAddr << " with teid " << teid);
              SendToS1uSocket (packet, senbAddr, teid);
            }
          }
          else if(it->second->dcType == 3){
            std::map<uint32_t, TeidInfo>::iterator teidit = m_teidInfoMap.find (teid);
            NS_ASSERT_MSG (teidit != m_teidInfoMap.end (), "unknown TEID " << teid);

            int t_splitter = SplitAlgorithm(&teidit->second, teid);

            std::map<uint32_t, Ipv4Address>::iterator senbAddrIt = m_dcEnbAddrByTeidMap.find (teid);
            Ipv4Address senbAddr = senbAddrIt->second;
            if (senbAddrIt == m_dcEnbAddrByTeidMap.end ()) NS_FATAL_ERROR("Cannot find senbAddr");

            if (t_splitter == 1){
              NS_LOG_INFO ("***SgwPgw send to SeNB " << senbAddr << " with teid " << teid);
              teidit->second.lastDirection1X = 1;
              teidit->second.sumPacketSizeSenb += packet->GetSize();
              SendToS1uSocket (packet, senbAddr, teid);
            }
            else if (t_splitter == 0) {
              NS_LOG_INFO ("***SgwPgw send to MeNB " << enbAddr << " with teid " << teid);
              teidit->second.lastDirection1X = 0;
              teidit->second.sumPacketSizeMenb += packet->GetSize();
              SendToS1uSocket (packet, enbAddr, teid);
            }
            else NS_FATAL_ERROR ("unknwon t_splitter value");

          }
          else NS_FATAL_ERROR("Unimplemented DC type");

        }
    }
  // there is no reason why we should notify the TUN
  // VirtualNetDevice that he failed to send the packet: if we receive
  // any bogus packet, it will just be silently discarded.
  const bool succeeded = true;
  return succeeded;
}

void 
EpcSgwPgwApplication::RecvFromS1uSocket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);  
  NS_ASSERT (socket == m_s1uSocket);
  Ptr<Packet> packet = socket->Recv ();
  GtpuHeader gtpu;
  packet->RemoveHeader (gtpu);
  uint32_t teid = gtpu.GetTeid ();

  SendToTunDevice (packet, teid);
}

void 
EpcSgwPgwApplication::SendToTunDevice (Ptr<Packet> packet, uint32_t teid)
{
  NS_LOG_FUNCTION (this << packet << teid);
  NS_LOG_LOGIC (" packet size: " << packet->GetSize () << " bytes");
  m_tunDevice->Receive (packet, 0x0800, m_tunDevice->GetAddress (), m_tunDevice->GetAddress (), NetDevice::PACKET_HOST);
}

void 
EpcSgwPgwApplication::SendToS1uSocket (Ptr<Packet> packet, Ipv4Address enbAddr, uint32_t teid)
{
  NS_LOG_FUNCTION (this << packet << enbAddr << teid);

  GtpuHeader gtpu;
  gtpu.SetTeid (teid);
  // From 3GPP TS 29.281 v10.0.0 Section 5.1
  // Length of the payload + the non obligatory GTP-U header
  std::map<uint32_t, TeidInfo>::iterator teidit = m_teidInfoMap.find (teid); // woody
  NS_ASSERT_MSG (teidit != m_teidInfoMap.end (), "unknown TEID " << teid); // woody

  gtpu.SetSequenceNumber(teidit->second.gtpu_SN); //sjkang0601
  teidit->second.gtpu_SN ++; //sjkang0601

  gtpu.SetLength (packet->GetSize () + gtpu.GetSerializedSize () - 8);  
  packet->AddHeader (gtpu);
  uint32_t flags = 0;
  m_s1uSocket->SendTo (packet, flags, InetSocketAddress (enbAddr, m_gtpuUdpPort));
}


void 
EpcSgwPgwApplication::SetS11SapMme (EpcS11SapMme * s)
{
  m_s11SapMme = s;
}

EpcS11SapSgw* 
EpcSgwPgwApplication::GetS11SapSgw ()
{
  return m_s11SapSgw;
}

void 
EpcSgwPgwApplication::AddEnb (uint16_t cellId, Ipv4Address enbAddr, Ipv4Address sgwAddr)
{
  NS_LOG_FUNCTION (this << cellId << enbAddr << sgwAddr);
  EnbInfo enbInfo;
  enbInfo.enbAddr = enbAddr;
  enbInfo.sgwAddr = sgwAddr;
  m_enbInfoByCellId[cellId] = enbInfo;
}

void 
EpcSgwPgwApplication::AddUe (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  Ptr<UeInfo> ueInfo = Create<UeInfo> ();
  m_ueInfoByImsiMap[imsi] = ueInfo;
}

void 
EpcSgwPgwApplication::SetUeAddress (uint64_t imsi, Ipv4Address ueAddr)
{
  NS_LOG_FUNCTION (this << imsi << ueAddr);
  std::map<uint64_t, Ptr<UeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi); 
  m_ueInfoByAddrMap[ueAddr] = ueit->second;
  ueit->second->SetUeAddr (ueAddr);
}

void 
EpcSgwPgwApplication::DoCreateSessionRequest (EpcS11SapSgw::CreateSessionRequestMessage req) // woody, modified
{
  NS_LOG_FUNCTION (this << req.imsi);
  std::map<uint64_t, Ptr<UeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (req.imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << req.imsi); 
  uint16_t cellId = req.uli.gci;
  std::map<uint16_t, EnbInfo>::iterator enbit = m_enbInfoByCellId.find (cellId);
  NS_ASSERT_MSG (enbit != m_enbInfoByCellId.end (), "unknown CellId " << cellId); 
  Ipv4Address enbAddr = enbit->second.enbAddr;
  //ueit->second->SetEnbAddr (enbAddr);

  EpcS11SapMme::CreateSessionResponseMessage res;
  res.teid = req.imsi; // trick to avoid the need for allocating TEIDs on the S11 interface

  for (std::list<EpcS11SapSgw::BearerContextToBeCreated>::iterator bit = req.bearerContextsToBeCreated.begin ();
       bit != req.bearerContextsToBeCreated.end ();
       ++bit)
    {
      // simple sanity check. If you ever need more than 4M teids
      // throughout your simulation, you'll need to implement a smarter teid
      // management algorithm. 
      NS_ABORT_IF (m_teidCount == 0xFFFFFFFF);
      uint32_t teid = ++m_teidCount;  
      ueit->second->AddBearer (bit->tft, bit->epsBearerId, teid);
      ueit->second->dcType = bit->dcType;
      // if the bearer type is DC, add list eNB addr into SenbMap
      if(bit->dcType == 0 || bit->dcType == 2) { // woody3C
          NS_LOG_FUNCTION ("SetEnbAddr " << enbAddr << " dcType " << (unsigned)bit->dcType << " teid " << teid);
          ueit->second->SetEnbAddr (enbAddr);
      }
      else if(bit->dcType == 1) {  // woody
          NS_LOG_FUNCTION ("SetSenbAddr " << enbAddr << " dcType " << (unsigned)bit->dcType << " teid " << teid);
          ueit->second->SetSenbAddr (enbAddr); // woody, actually SetSenbAddr is currently not utilized
          m_dcEnbAddrByTeidMap[teid] = enbAddr;
      }
      else if(bit->dcType == 3) {  // woody1X
          if (req.isSenb == 1)
          {
            NS_LOG_FUNCTION ("SetSenbAddr " << enbAddr << " dcType " << (unsigned)bit->dcType << " teid " << teid);
            ueit->second->SetSenbAddr (enbAddr); // woody, actually SetSenbAddr is currently not utilized
            m_dcEnbAddrByTeidMap[teid] = enbAddr;
 
          }
          else
          {
            NS_LOG_FUNCTION ("SetEnbAddr " << enbAddr << " dcType " << (unsigned)bit->dcType << " teid " << teid);
            ueit->second->SetEnbAddr (enbAddr);
          }
      }
      else {
          NS_FATAL_ERROR("unimplemented DC type");
      }
      EpcS11SapMme::BearerContextCreated bearerContext;
      bearerContext.sgwFteid.teid = teid;
      bearerContext.sgwFteid.address = enbit->second.sgwAddr;
      bearerContext.epsBearerId =  bit->epsBearerId; 
      bearerContext.bearerLevelQos = bit->bearerLevelQos; 
      bearerContext.tft = bit->tft;
      bearerContext.dcType = bit->dcType; // woody
      res.bearerContextsCreated.push_back (bearerContext);

      // woody
      TeidInfo teidInfo;
      teidInfo.gtpu_SN = 0;
      m_teidInfoMap[teid] = teidInfo;

    }
  m_s11SapMme->CreateSessionResponse (res);
  
}

void 
EpcSgwPgwApplication::DoModifyBearerRequest (EpcS11SapSgw::ModifyBearerRequestMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<UeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi); 
  uint16_t cellId = req.uli.gci;
  std::map<uint16_t, EnbInfo>::iterator enbit = m_enbInfoByCellId.find (cellId);
  NS_ASSERT_MSG (enbit != m_enbInfoByCellId.end (), "unknown CellId " << cellId); 
  Ipv4Address enbAddr = enbit->second.enbAddr;
  ueit->second->SetEnbAddr (enbAddr);
  // no actual bearer modification: for now we just support the minimum needed for path switch request (handover)
  EpcS11SapMme::ModifyBearerResponseMessage res;
  res.teid = imsi; // trick to avoid the need for allocating TEIDs on the S11 interface
  res.cause = EpcS11SapMme::ModifyBearerResponseMessage::REQUEST_ACCEPTED;
  m_s11SapMme->ModifyBearerResponse (res);
}
 
void
EpcSgwPgwApplication::DoDeleteBearerCommand (EpcS11SapSgw::DeleteBearerCommandMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<UeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi);

  EpcS11SapMme::DeleteBearerRequestMessage res;
  res.teid = imsi;

  for (std::list<EpcS11SapSgw::BearerContextToBeRemoved>::iterator bit = req.bearerContextsToBeRemoved.begin ();
       bit != req.bearerContextsToBeRemoved.end ();
       ++bit)
    {
      EpcS11SapMme::BearerContextRemoved bearerContext;
      bearerContext.epsBearerId =  bit->epsBearerId;
      res.bearerContextsRemoved.push_back (bearerContext);
    }
  //schedules Delete Bearer Request towards MME
  m_s11SapMme->DeleteBearerRequest (res);
}

void
EpcSgwPgwApplication::DoDeleteBearerResponse (EpcS11SapSgw::DeleteBearerResponseMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<UeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi);

  for (std::list<EpcS11SapSgw::BearerContextRemovedSgwPgw>::iterator bit = req.bearerContextsRemoved.begin ();
       bit != req.bearerContextsRemoved.end ();
       ++bit)
    {
      //Function to remove de-activated bearer contexts from S-Gw and P-Gw side
      ueit->second->RemoveBearer (bit->epsBearerId);
    }
}

void
EpcSgwPgwApplication::IsAssistInfoSink (){ // woody
  NS_LOG_FUNCTION (this);
  m_isAssistInfoSink = true;
}

}  // namespace ns3
