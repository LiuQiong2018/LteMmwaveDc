
#include "ns3/point-to-point-module.h"
#include "ns3/mmwave-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
#include <ns3/buildings-module.h>
#include <ns3/packet.h>
#include <ns3/tag.h>
#include <ns3/lte-helper.h>
#include <ns3/lte-module.h>

#include <ns3/spectrum-channel.h>

//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * A script to simulate the DOWNLINK TCP data over mmWave links
 * with the mmWave devices and the LTE EPC.
 */
NS_LOG_COMPONENT_DEFINE ("mmWaveTCPExample");
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void SetDataRate (DataRate r); // woody

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  uint32_t         m_sent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0),
	m_sent(0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

bool isTcp_for_MyApp=false;

void
MyApp::SendPacket (void)
{

  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  if (!isTcp_for_MyApp)
  {
	  SeqTsHeader seqTs;  // for udp sequence number sjkang 0703
 	  seqTs.SetSeq (m_sent); // for udp sequence number sjkang 0703
 	  m_sent++;  // for udp sequence number sjkang
 	  packet->AddHeader(seqTs); // for udp sequence number sjkang
  }
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

void
MyApp::SetDataRate (DataRate r) // woody
{
        m_dataRate = r;
}

/*static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RttChange (Ptr<OutputStreamWrapper> stream, Time oldRtt, Time newRtt)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldRtt.GetSeconds () << "\t" << newRtt.GetSeconds () << std::endl;
}*/

double instantPacketSize[100], packetRxTime[100], lastPacketRxTime[100];
double sumPacketSize[100];

static void
Rx (Ptr<OutputStreamWrapper> stream, uint16_t i, Ptr<const Packet> packet, const Address &from){
  packetRxTime[i] = Simulator::Now().GetSeconds();
  if (lastPacketRxTime[i] == packetRxTime[i]){
    instantPacketSize[i] += packet->GetSize();
    return;
  }
  else{
    sumPacketSize[i] += instantPacketSize[i];
    *stream->GetStream () << lastPacketRxTime[i] << "\t" << instantPacketSize[i] << "\t" << sumPacketSize[i]
    		<< std::endl;
    lastPacketRxTime[i] =  packetRxTime[i];
    instantPacketSize[i] = packet->GetSize();
  }
}

int previousLoss[100];

void
Loss(Ptr<OutputStreamWrapper> stream, uint16_t i, uint64_t received, uint32_t loss ){
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" <<received<< "\t"<<
			loss << "\t" << loss-previousLoss[i] <<std::endl;
	previousLoss[i]=loss;
}

uint64_t lastTotalRx[100];

void
CalculateThroughput (Ptr<OutputStreamWrapper> stream, Ptr<PacketSink> sink, uint16_t i)
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx[i]) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  *stream->GetStream()  << now.GetSeconds () << "\t" << cur <<std::endl;
  lastTotalRx[i] = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput,stream,sink,i);
}

/*static void Sstresh (Ptr<OutputStreamWrapper> stream, uint32_t oldSstresh, uint32_t newSstresh)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldSstresh << "\t" << newSstresh << std::endl;
}*/

void
ChangeSpeed(Ptr<Node>  n, Vector speed)
{
	n->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (speed);
}


/*static void
Traces(uint16_t nodeNum)
{
	AsciiTraceHelper asciiTraceHelper;

	std::ostringstream pathCW;
	pathCW<<"/NodeList/"<<nodeNum+1<<"/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow";

	std::ostringstream fileCW;
	fileCW<<"UE-"<<nodeNum+1<<"-TCP-CWND.txt";

	std::ostringstream pathRTT;
	pathRTT<<"/NodeList/"<<nodeNum+1<<"/$ns3::TcpL4Protocol/SocketList/0/RTT";

	std::ostringstream fileRTT;
	fileRTT<<"UE-"<<nodeNum+1<<"-TCP-RTT.txt";

	std::ostringstream pathRCWnd;
	pathRCWnd<<"/NodeList/"<<nodeNum+1<<"/$ns3::TcpL4Protocol/SocketList/0/RWND";

	std::ostringstream fileRCWnd;
	fileRCWnd<<"UE-"<<nodeNum+1<<"-TCP-RCWND.txt";

	Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (fileCW.str ().c_str ());
	Config::ConnectWithoutContext (pathCW.str ().c_str (), MakeBoundCallback(&CwndChange, stream1));

	Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (fileRTT.str ().c_str ());
	Config::ConnectWithoutContext (pathRTT.str ().c_str (), MakeBoundCallback(&RttChange, stream2));

	Ptr<OutputStreamWrapper> stream4 = asciiTraceHelper.CreateFileStream (fileRCWnd.str ().c_str ());
	Config::ConnectWithoutContext (pathRCWnd.str ().c_str (), MakeBoundCallback(&CwndChange, stream4));

}
*/

int
main (int argc, char *argv[])
{
//	LogComponentEnable("LteHelper", LOG_FUNCTION);
//	LogComponentEnable("MmWaveHelper", LOG_FUNCTION);
//	LogComponentEnable("MmWavePointToPointEpcHelper", LOG_FUNCTION);
//	LogComponentEnable("MobilityBuildingInfo", LOG_FUNCTION);
//	LogComponentEnable("BuildingsHelper", LOG_FUNCTION);
//	LogComponentEnable("MmWaveEnbNetDevice", LOG_FUNCTION);
//	LogComponentEnable("MmWaveUeNetDevice", LOG_FUNCTION);
//	LogComponentEnable("LteUeNetDevice", LOG_FUNCTION);
//	LogComponentEnable("LteEnbNetDevice", LOG_FUNCTION);
//	LogComponentEnable("LteEnbRrc", LOG_FUNCTION);
//	LogComponentEnable("LteUeRrc", LOG_FUNCTION);
//	LogComponentEnable("EpcMme", LOG_FUNCTION);
//	LogComponentEnable("EpcUeNas", LOG_FUNCTION);
//	LogComponentEnable("EpcSgwPgwApplication", LOG_FUNCTION);
//	LogComponentEnable("EpcEnbApplication", LOG_FUNCTION);
//	LogComponentEnable("MmWaveEnbMac", LOG_FUNCTION);
//	LogComponentEnable("MmWaveUeMac", LOG_FUNCTION);
//	LogComponentEnable("MmWaveUePhy", LOG_FUNCTION);
//	LogComponentEnable("MmWaveEnbPhy", LOG_FUNCTION);
//	LogComponentEnable("mmWaveRrcProtocolIdeal", LOG_FUNCTION);
//	LogComponentEnable("LteRrcProtocolIdeal", LOG_FUNCTION);
//	LogComponentEnable("LtePdcp", LOG_FUNCTION);
//	LogComponentEnable("LteRlcAm", LOG_FUNCTION);
//	LogComponentEnable("LteRlcUm",LOG_FUNCTION);
//	LogComponentEnable("LteRlc", LOG_FUNCTION);
//	LogComponentEnable("Simulator", LOG_FUNCTION);
//	LogComponentEnable("LteEnbMac", LOG_FUNCTION);
//	LogComponentEnable("LteUeMac", LOG_FUNCTION);
//	LogComponentEnable("LteEnbPhy", LOG_FUNCTION);
//	LogComponentEnable("LteUePhy", LOG_FUNCTION);
//	LogComponentEnable("MmWaveSpectrumPhy", LOG_FUNCTION);
//	LogComponentEnable("MmWaveHarqPhy", LOG_FUNCTION);
//	LogComponentEnable("mmWaveChunkProcessor", LOG_FUNCTION);
//	LogComponentEnable("TcpSocketBase", LOG_INFO);
//	LogComponentEnable("MmWaveChannelMatrix", LOG_FUNCTION);
//	LogComponentEnable("MmWaveChannelRaytracing", LOG_FUNCTION);
//	LogComponentEnable("MmWaveBeamforming", LOG_FUNCTION);
//	LogComponentEnable("MmWave3gppChannel", LOG_FUNCTION);
//	LogComponentEnable("EpcTftClassifier", LOG_LEVEL_ALL);
//	LogComponentEnable("Ipv4L3Protocol", LOG_FUNCTION);
//	LogComponentEnable("TcpL4Protocol", LOG_FUNCTION);
	LogComponentEnable("LtePdcp", LOG_INFO);

	bool log_packetflow = false;
	bool enablePDCPReordering =true;
	int nodeNum_t = 1;
	double simStopTime = 6;
	bool rlcAmEnabled = true;
	std::string protocol = "TcpCubic";
	int bufferSize = 1024*1024*100;
	uint16_t downlinkRb = 100;
	int dcType_t = 2; // (1:1A, 2:3C, 3:1X)
	bool isTcp = false;
	isTcp_for_MyApp = isTcp;
	int PacketSize = 1400; //60000;
	int splitAlgorithm_t = 6; // (0:MeNB only, 1:SeNB only, 2:alternative, 3:Delay-based, 4:Queue-based, 6:NewQueue-based)
	int pdcpReorderingTimer_t = 50;
	int pdcpEarlyRetTimer_t = 40;
//	uint16_t x2LinkDelay =0;
        std::string tcpDataRate = "800Mb/s";
	int splitTimerInterval = 10;
	double alpha = 1/10.0;
	double beta = 1/10.0;
	int x2delay_t = 10;

//	Config::SetDefault ("ns3::TcpSocketBase::ReTxThreshold", UintegerValue (1));

	std::string outputName;
	uint8_t dcType;
	uint16_t pdcpReorderingTimer, splitAlgorithm, pdcpEarlyRetTimer;
	uint16_t nodeNum;
	uint16_t x2delay;

	//The available channel scenarios are 'RMa', 'UMa', 'UMi-StreetCanyon', 'InH-OfficeMixed', 'InH-OfficeOpen', 'InH-ShoppingMall'
	std::string scenario = "UMa";
	std::string condition = "n";

	CommandLine cmd;
	cmd.AddValue("nodeNum", "Number of UEs", nodeNum_t);
	cmd.AddValue("simTime", "Total duration of the simulation [s])", simStopTime);
//	cmd.AddValue("interPacketInterval", "Inter-packet interval [us])", interPacketInterval);
//	cmd.AddValue("harq", "Enable Hybrid ARQ", harqEnabled);
//	cmd.AddValue("rlcAm", "Enable RLC-AM", rlcAmEnabled);
//	cmd.AddValue("protocol", "TCP protocol", protocol);
	cmd.AddValue("bufferSize", "buffer size", bufferSize);
	cmd.AddValue("pdcpReorderingTimer", "PDCP reordering timer [ms])", pdcpReorderingTimer_t);
	cmd.AddValue("pdcpEarlyRetTimer", "PDCP early retransmission timer [ms])", pdcpEarlyRetTimer_t);
//	cmd.AddValue("outputName", "Output file name prefix", outputName);
	cmd.AddValue("splitAlgorithm", "Selecting splitting algorithm", splitAlgorithm_t);
	cmd.AddValue("dcType", "Select DC Type", dcType_t);
	cmd.AddValue("tcpDataRate", "Data rate for TCP application", tcpDataRate);
	cmd.AddValue("splitTimerInterval", "split timer interval for 1X architecture", splitTimerInterval);
	cmd.AddValue("TCP", "TCP protocol", protocol);
	cmd.AddValue("alpha", "alpha value for averaging etha", alpha);
	cmd.AddValue("beta", "beta value for averaging data rate", beta);
	cmd.AddValue("x2delay", "x2 delay", x2delay_t);

	cmd.Parse(argc, argv);
	nodeNum = (unsigned) nodeNum_t;
	dcType = (unsigned) dcType_t;
	pdcpReorderingTimer = (unsigned) pdcpReorderingTimer_t;
	splitAlgorithm = (unsigned) splitAlgorithm_t;
	pdcpEarlyRetTimer = (unsigned) pdcpEarlyRetTimer_t;
	x2delay = (unsigned) x2delay_t;

	NS_LOG_UNCOND("Simulation Setting");
	NS_LOG_UNCOND(" -simTime(s) = " << simStopTime);
	NS_LOG_UNCOND(" -nodeNum = " << nodeNum);
	if (isTcp)  NS_LOG_UNCOND(" -App = TCP");
	else NS_LOG_UNCOND(" -App = UDP");
	NS_LOG_UNCOND(" -dcType = " << (unsigned) dcType);
	NS_LOG_UNCOND(" -splitAlgorithm = " << (unsigned) splitAlgorithm);
	NS_LOG_UNCOND(" -pdcpReorderingTimer(ms) = " << (unsigned) pdcpReorderingTimer);
	NS_LOG_UNCOND(" -pdcpEarlyRetTimer(ms) = " << (unsigned) pdcpEarlyRetTimer);
//	NS_LOG_UNCOND(" -x2LinkDelay(ms) = " << x2LinkDelay);
	NS_LOG_UNCOND(" -tcpDataRate = " << tcpDataRate);

	if (log_packetflow){
		LogComponentEnable ("EpcEnbApplication", LOG_INFO);
		LogComponentEnable ("EpcSgwPgwApplication", LOG_INFO);
		LogComponentEnable ("LteEnbRrc", LOG_INFO);
		LogComponentEnable ("PacketSink", LOG_INFO);
		LogComponentEnable ("UdpClient", LOG_INFO);
	}

	Config::SetDefault ("ns3::MmWavePointToPointEpcHelper::X2LinkDelay", TimeValue(MilliSeconds(x2delay)));
	Config::SetDefault ("ns3::UeManager::X2Delay", UintegerValue (x2delay));

	Config::SetDefault ("ns3::UeManager::SplitAlgorithm", UintegerValue (splitAlgorithm));
	Config::SetDefault ("ns3::UeManager::SplitTimerInterval", UintegerValue (splitTimerInterval));
	Config::SetDefault ("ns3::LtePdcp::ExpiredTime",TimeValue(MilliSeconds(pdcpReorderingTimer)));
	Config::SetDefault ("ns3::LtePdcp::EarlyRetTime",TimeValue(MilliSeconds(pdcpEarlyRetTimer)));

//	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (65535));
	Config::SetDefault ("ns3::TcpSocketBase::MinRto", TimeValue (MilliSeconds (200)));
	Config::SetDefault ("ns3::Ipv4L3Protocol::FragmentExpirationTimeout", TimeValue (Seconds (1)));
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (PacketSize));
	Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (1));
	Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (131072*200*40));
	Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (131072*200*40));
//        Config::SetDefault ("ns3::TcpCubic::HyStart", BooleanValue (false));
//        Config::SetDefault ("ns3::TcpCubic::C", DoubleValue (300.0));
//        Config::SetDefault ("ns3::TcpSocket::InitialSlowStartThreshold", UintegerValue (7000000));
	if(protocol == "TcpNewReno")
	{
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
	}
	else if (protocol == "TcpVegas")
	{
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
	}
	else if (protocol == "TcpCubic")
	{
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpCubic::GetTypeId ()));
	}
	else
	{
		std::cout<<protocol<<" Unkown protocol.\n";
		return 1;
	}

//	if(isTcp){
		Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue (ns3::LteEnbRrc::RLC_AM_ALWAYS));
//		Config::SetDefault ("ns3::LteRlcAm::PollRetransmitTimer", TimeValue(MilliSeconds(2.0)));
//		Config::SetDefault ("ns3::LteRlcAm::ReorderingTimer", TimeValue(MilliSeconds(1.0)));
//		Config::SetDefault ("ns3::LteRlcAm::StatusProhibitTimer", TimeValue(MilliSeconds(1.0)));
//		Config::SetDefault ("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue(MilliSeconds(2.0)));
		Config::SetDefault ("ns3::LteRlcAm::EnableAQM", BooleanValue (false));
		Config::SetDefault ("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue (bufferSize));
		Config::SetDefault ("ns3::LteRlcAm::SplitTimerInterval", UintegerValue (splitTimerInterval));
		Config::SetDefault ("ns3::LtePdcp::EnablePDCPReordering", BooleanValue (enablePDCPReordering));
		Config::SetDefault ("ns3::LtePdcp::ExpiredTime",TimeValue(MilliSeconds(pdcpReorderingTimer)));
		Config::SetDefault ("ns3::LtePdcp::EarlyRetTime",TimeValue(MilliSeconds(pdcpEarlyRetTimer)));
/*	}
	else {
		Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue (ns3::LteEnbRrc::RLC_AM_ALWAYS)); // RLC_UM not verififed
		Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (200 * 1024 * 1024));
		Config::SetDefault ("ns3::LtePdcp::EnablePDCPReordering", BooleanValue (false));
	}*/

	Config::SetDefault ("ns3::Queue::MaxPackets", UintegerValue (1000*1000));
	Config::SetDefault ("ns3::CoDelQueueDisc::Mode", StringValue ("QUEUE_MODE_PACKETS"));
	Config::SetDefault ("ns3::CoDelQueueDisc::MaxPackets", UintegerValue (50000));
	Config::SetDefault ("ns3::CoDelQueueDisc::Interval", StringValue ("100ms"));
	Config::SetDefault ("ns3::CoDelQueueDisc::Target", StringValue ("5ms"));

	Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue(rlcAmEnabled));
	Config::SetDefault ("ns3::MmWaveHelper::HarqEnabled", BooleanValue(true));
	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));
	Config::SetDefault ("ns3::MmWaveFlexTtiMaxWeightMacScheduler::HarqEnabled", BooleanValue(true));
	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));

//	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::ChannelCondition", StringValue(condition));
//	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::Scenario", StringValue(scenario));
//	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::OptionalNlos", BooleanValue(false));
//	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::Shadowing", BooleanValue(true)); // enable or disable the shadowing effect

	Config::SetDefault ("ns3::MmWave3gppChannel::UpdatePeriod", TimeValue (MilliSeconds (100))); // Set channel update period, 0 stands for no update.
	Config::SetDefault ("ns3::MmWave3gppChannel::CellScan", BooleanValue(false)); // Set true to use cell scanning method, false to use the default power method.
	Config::SetDefault ("ns3::MmWave3gppChannel::Blockage", BooleanValue(true)); // use blockage or not
	Config::SetDefault ("ns3::MmWave3gppChannel::PortraitMode", BooleanValue(true)); // use blockage model with UT in portrait mode
	Config::SetDefault ("ns3::MmWave3gppChannel::NumNonselfBlocking", IntegerValue(1)); // number of non-self blocking obstacles
	Config::SetDefault ("ns3::MmWave3gppChannel::BlockerSpeed", DoubleValue(1)); // speed of non-self blocking obstacles
	Config::SetDefault ("ns3::MmWavePhyMacCommon::NumHarqProcess", UintegerValue(100));
        Config::SetDefault ("ns3::MmWaveBeamforming::LongTermUpdatePeriod", TimeValue (MilliSeconds (100.0)));
        Config::SetDefault ("ns3::MmWave3gppBuildingsPropagationLossModel::UpdateCondition", BooleanValue(true)); // enable or disable the LOS/NLOS update when the UE moves

        NS_LOG_UNCOND("# Set LteHelper, mmwaveHelper, MmWavePointToPointEpcHelper");
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (downlinkRb));
	lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (downlinkRb));
	lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::LogDistancePropagationLossModel"));
	lteHelper->Initialize();

	Ptr<MmWaveHelper> mmwaveHelper = CreateObject<MmWaveHelper> ();
	mmwaveHelper->SetAttribute ("PathlossModel", StringValue ("ns3::MmWave3gppBuildingsPropagationLossModel"));
	mmwaveHelper->SetAttribute ("ChannelModel", StringValue ("ns3::MmWave3gppChannel"));
	mmwaveHelper->Initialize();
	mmwaveHelper->SetHarqEnabled(true);

	Ptr<MmWavePointToPointEpcHelper>  epcHelper = CreateObject<MmWavePointToPointEpcHelper> ();
	lteHelper->SetEpcHelper (epcHelper);
	mmwaveHelper->SetEpcHelper (epcHelper);
	Ptr<Node> pgw = epcHelper->GetPgwNode ();

	pgw->GetApplication (0) -> GetObject<EpcSgwPgwApplication> () -> SetSplitAlgorithm(splitAlgorithm);
	pgw->GetApplication (0) -> GetObject<EpcSgwPgwApplication> () -> SetSplitTimerInterval(splitTimerInterval);
	pgw->GetApplication (0) -> GetObject<EpcSgwPgwApplication> () -> SetParameters(alpha, beta);

	// Create a single RemoteHost
	NS_LOG_UNCOND("# Create a remote host, and connect with P-GW");
	NodeContainer remoteHostContainer;
	remoteHostContainer.Create (nodeNum);
	InternetStackHelper internet;
	internet.Install (remoteHostContainer);
	Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ipv4InterfaceContainer internetIpIfaces;

	for (uint16_t i = 0; i < nodeNum; i++)
	{
		Ptr<Node> remoteHost = remoteHostContainer.Get (i);
		// Create the Internet
		PointToPointHelper p2ph;
		p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
		p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1600));
		if (i==0)
		{
			p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.01)));
		}
		else
		{
			p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.02)));

		}

		NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

		Ipv4AddressHelper ipv4h;
		std::ostringstream subnet;
		subnet << i << ".1.0.0";
		ipv4h.SetBase (subnet.str ().c_str (), "255.255.0.0");
		internetIpIfaces = ipv4h.Assign (internetDevices);
		// interface 0 is localhost, 1 is the p2p device
		Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
		remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.255.0.0"), 1);
	}

	NS_LOG_UNCOND("# Create UE, eNB nodes");
	NodeContainer ueNodes;
	NodeContainer enbNodes;
	NodeContainer senbNodes;
	enbNodes.Create(1);
	senbNodes.Create(1);
	ueNodes.Create(nodeNum);

        NS_LOG_UNCOND("# Mobility set up");
        Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
        enbPositionAlloc->Add (Vector (0.0, 0.0, 10.0));
        MobilityHelper enbmobility;
        enbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
        enbmobility.SetPositionAllocator(enbPositionAlloc);
        enbmobility.Install (enbNodes);
        BuildingsHelper::Install (enbNodes);

        Ptr<ListPositionAllocator> senbPositionAlloc = CreateObject<ListPositionAllocator> ();
        senbPositionAlloc->Add (Vector (350.0, 0.0, 10.0));
        MobilityHelper senbmobility;
        senbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
        senbmobility.SetPositionAllocator(senbPositionAlloc);
        senbmobility.Install (senbNodes);
        BuildingsHelper::Install (senbNodes);

	Ptr < Building > building;
	building = Create<Building> ();
	building->SetBoundaries (Box (150.0, 200.0,
	                              0.0, 25.0,
	                              0.0, 25.0));
	building->SetBuildingType (Building::Residential);
	building->SetExtWallsType (Building::ConcreteWithWindows);
	building->SetNFloors (1);
	building->SetNRoomsX (1);
	building->SetNRoomsY (1);

	MobilityHelper uemobility;
	uemobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	uemobility.Install (ueNodes);
	ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (50, -20, 3.0));
	ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
	BuildingsHelper::Install (ueNodes);

/*	MobilityHelper uemobility;
	Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator> ();
	uePositionAlloc->Add (Vector (40.0, 0.0, 3.0));
	uemobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	uemobility.SetPositionAllocator(uePositionAlloc);
	uemobility.Install (ueNodes);
	BuildingsHelper::Install (ueNodes);*/

	// Install LTE Devices to the nodes
	NS_LOG_UNCOND("# Install LTE device to the nodes");
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
	NetDeviceContainer senbMmWaveDevs = mmwaveHelper->InstallSenbDevice (senbNodes, 2);
	mmwaveHelper->SetLteChannel (lteHelper->GetDlChannel(), lteHelper->GetUlChannel());
	NetDeviceContainer ueDevs = mmwaveHelper->InstallDcUeDevice (ueNodes);

        mmwaveHelper->NotifyEnbNeighbor (enbNodes.Get(0), senbNodes.Get(0));

	if (dcType == 2 || dcType == 3) mmwaveHelper->ConnectAssistInfo (enbNodes.Get(0), senbNodes.Get(0), ueNodes.Get(0), dcType);

	// Install the IP stack on the UEs
	// Assign IP address to UEs, and install applications
	NS_LOG_UNCOND("# Install the IP stack on the UE");
	internet.Install (ueNodes);
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

	NS_LOG_UNCOND("# Attach UE to eNB");
//	mmwaveHelper->AttachToClosestEnb (ueDevs, senbMmWaveDevs);
//	mmwaveHelper->EnableTraces ();
	uint16_t sinkPort = 1235;
	Ptr<EpcTft> tftDc = Create<EpcTft> ();
	EpcTft::PacketFilter tftPacketFilter;
	tftPacketFilter.localPortStart = sinkPort;
	tftPacketFilter.localPortEnd = sinkPort;
	tftDc->Add (tftPacketFilter);

	for (uint8_t i =0 ;i<ueDevs.GetN(); i++){
	lteHelper->Attach (ueDevs.Get(i), enbLteDevs.Get(0));
	mmwaveHelper->AttachDc (ueDevs.Get(i), senbMmWaveDevs.Get(0), tftDc, dcType); // woody, woody3C
	}
	// Add X2 interface
	mmwaveHelper->AddX2Interface (enbNodes.Get(0), senbNodes.Get(0)); // woody

	enbLteDevs.Get(0)->GetObject<LteEnbNetDevice>()->GetRrc()->totalUe =nodeNum;

	NS_LOG_UNCOND("# Install application");
	ApplicationContainer sourceAppsForUDP;
	ApplicationContainer sinkApps;

//	if (isTcp)
//	{
//		for (uint16_t i = 0; i < ueNodes.GetN (); i++)
/*		{
			uint16_t i = 0;
			// Set the default gateway for the UE
			Ptr<Node> ueNode = ueNodes.Get (i);
			Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
			ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
	
			// Install and start applications on UEs and remote host
			PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
			sinkApps.Add (packetSinkHelper.Install (ueNodes.Get (i)));
	
			Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (remoteHostContainer.Get (i), TcpSocketFactory::GetTypeId ());
			Ptr<MyApp> app = CreateObject<MyApp> ();
			Address sinkAddress (InetSocketAddress (ueIpIface.GetAddress (i), sinkPort));
			app->Setup (ns3TcpSocket, sinkAddress, 1400, 5000000, DataRate (tcpDataRate));//sychoi, tcp data rate config

			std::ostringstream fileName;
			fileName<<"UE-"<<i+1<<"-TCP-DATA.txt";

			AsciiTraceHelper asciiTraceHelper;

			Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ().c_str ());
			sinkApps.Get(i)->TraceConnectWithoutContext("Rx",MakeBoundCallback (&Rx, stream,i));

			std::ostringstream fileName_2;
			fileName_2<<"UE-" << i+1 <<"-TCP-Throughput.txt";
			AsciiTraceHelper asciiTraceHelper_2;
			Ptr<OutputStreamWrapper> stream_2 = asciiTraceHelper_2.CreateFileStream(fileName_2.str().c_str());
			Simulator::Schedule (Seconds (0.1), &CalculateThroughput,stream_2,sinkApps.Get(i)->GetObject<PacketSink>(),i);

			app->SetStartTime(Seconds(0.01*i + 0.1));
			Simulator::Schedule (Seconds (0.1001+0.01*i), &Traces, i);
			app->SetStopTime(Seconds(simStopTime));
				//sourceApps.Get(i)->SetStopTime (Seconds (10-1.5*i));
			 remoteHostContainer.Get(i)->AddApplication(app);

		}*/
//	}
//	else
//	{
//		for(uint16_t i=0 ; i<ueNodes.GetN() ; i++)
		{

			uint16_t i = 0;
			// UdpServerHelper UdpServer(sinkPort);
			//ApplicationContainer sinkApps = UdpServer.Install (ueNodes.Get (i));

			PacketSinkHelper packetSinkHelperUdp ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
			sinkApps.Add (packetSinkHelperUdp.Install (ueNodes.Get (i)));

			Address sinkAddress (InetSocketAddress (ueIpIface.GetAddress (i), sinkPort));
			Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (remoteHostContainer.Get (i), UdpSocketFactory::GetTypeId ());
			Ptr<MyApp> app = CreateObject<MyApp> ();
			app->Setup (ns3UdpSocket, sinkAddress, 1400, 5000000, DataRate (tcpDataRate));
//			Simulator::Schedule (Seconds (2.0), &MyApp::SetDataRate, app, DataRate("1000Mb/s"));
//			Simulator::Schedule (Seconds (4.0), &MyApp::SetDataRate, app, DataRate("200Mb/s"));

			std::ostringstream fileName_3;
			fileName_3<<"UE-" << i+1 <<"-UDP-DATA.txt";
			AsciiTraceHelper asciiTraceHelper_3;
			Ptr<OutputStreamWrapper> stream = asciiTraceHelper_3.CreateFileStream (fileName_3.str().c_str());
			sinkApps.Get(i)->TraceConnectWithoutContext("Rx",MakeBoundCallback (&Rx, stream,i));

			std::ostringstream fileName_4;
			fileName_4<<"UE-" << i+1 <<"-UDP-DATALOSS.txt";
			AsciiTraceHelper asciiTraceHelper_4;
			Ptr<OutputStreamWrapper> stream_2 = asciiTraceHelper_4.CreateFileStream (fileName_4.str().c_str());
			*stream_2->GetStream() << "time  " << "  \t " << "number of received packets" <<"   \t  "
				<< "number of Loss packet " << " \t  "<< " amount of Loss  " <<std::endl;
			sinkApps.Get(i)->TraceConnectWithoutContext("Loss",MakeBoundCallback (&Loss, stream_2,i));

			app->SetStartTime(Seconds(0.05));
			app->SetStopTime(Seconds(simStopTime));
			remoteHostContainer.Get (i)->AddApplication (app);
		}
//	}

	sinkApps.Start (Seconds (0.));
	sinkApps.Stop (Seconds (simStopTime));
//	sourceAppsUL.Start (Seconds (0.1));
//	sourceApps.Stop (Seconds (simStopTime));

	NS_LOG_UNCOND("# Mobility verification");
	//p2ph.EnablePcapAll("mmwave-sgi-capture");
	BuildingsHelper::MakeMobilityModelConsistent ();

	Config::Set ("/NodeList/*/DeviceList/*/TxQueue/MaxPackets", UintegerValue (1000*1000));
	Config::Set ("/NodeList/*/DeviceList/*/TxQueue/MaxBytes", UintegerValue (1500*1000*1000));

	NS_LOG_UNCOND("# Run simulation");
	Simulator::Stop (Seconds (simStopTime+2));
	Simulator::Run ();

	for (uint16_t i=0 ; i<ueNodes.GetN() ; i++){
		double lteThroughput = sinkApps.Get(i)->GetObject<PacketSink>()->GetTotalRx () * 8.0 / (1000000.0*(simStopTime - (0.01*i+0.1)));
//		NS_LOG_UNCOND ("LastPacket " << packetRxTime << " TotalFlow " << sumPacketSize << "Mb");
		NS_LOG_UNCOND ("UE(" << ueIpIface.GetAddress(i) <<") AverageLteThroughput: " << lteThroughput << "Mbps");
	}

	Simulator::Destroy ();

	return 0;

}
