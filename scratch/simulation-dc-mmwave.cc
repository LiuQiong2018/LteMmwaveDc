
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



static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RttChange (Ptr<OutputStreamWrapper> stream, Time oldRtt, Time newRtt)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldRtt.GetSeconds () << "\t" << newRtt.GetSeconds () << std::endl;
}



static void Rx (Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet, const Address &from)
{
	*stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << packet->GetSize()<< std::endl;
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


static void
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


int
main (int argc, char *argv[])
{
	LogComponentEnable("LteHelper", LOG_FUNCTION);
	LogComponentEnable("MmWaveHelper", LOG_FUNCTION);
	LogComponentEnable("MmWavePointToPointEpcHelper", LOG_FUNCTION);
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
//	LogComponentEnable("LteRlc", LOG_FUNCTION);
//	LogComponentEnable("Simulator", LOG_FUNCTION);
//	LogComponentEnable("LteEnbMac", LOG_FUNCTION);
//	LogComponentEnable("LteUeMac", LOG_FUNCTION);
//	LogComponentEnable("LteEnbPhy", LOG_FUNCTION);
//	LogComponentEnable("LteUePhy", LOG_FUNCTION);
/*	LogComponentEnable("MmWaveSpectrumPhy", LOG_FUNCTION);
	LogComponentEnable("MmWaveHarqPhy", LOG_FUNCTION);
	LogComponentEnable("mmWaveChunkProcessor", LOG_FUNCTION);
	LogComponentEnable("TcpSocketBase", LOG_FUNCTION);*/
//	LogComponentEnable("MmWaveChannelMatrix", LOG_FUNCTION);
//	LogComponentEnable("MmWaveChannelRaytracing", LOG_FUNCTION);
//	LogComponentEnable("MmWaveBeamforming", LOG_FUNCTION);
//	LogComponentEnable("MmWave3gppChannel", LOG_FUNCTION);
//	LogComponentEnable("EpcTftClassifier", LOG_LEVEL_ALL);
//	LogComponentEnable("Ipv4L3Protocol", LOG_FUNCTION);
//	LogComponentEnable("TcpL4Protocol", LOG_FUNCTION);

	uint16_t nodeNum = 1;
	double simStopTime = 5;
	bool harqEnabled = true;
	bool rlcAmEnabled = true;
	std::string protocol = "TcpNewReno";
	int bufferSize = 1000 *1000 * 3.5 * 0.4;
	uint16_t downlinkRb = 100;
	uint8_t dcType = 3; // (1:1A, 2:3C, 3:1X)
	bool log_packetflow = false;
	bool isTcp = true;
	int PacketSize = 1400; //60000;
	int splitAlgorithm = 2;
	// This 3GPP channel model example only demonstrate the pathloss model. The fast fading model is still in developing.

	//The available channel scenarios are 'RMa', 'UMa', 'UMi-StreetCanyon', 'InH-OfficeMixed', 'InH-OfficeOpen', 'InH-ShoppingMall'
	std::string scenario = "UMa";
	std::string condition = "n";

	CommandLine cmd;
//	cmd.AddValue("numEnb", "Number of eNBs", numEnb);
//	cmd.AddValue("numUe", "Number of UEs per eNB", numUe);
	cmd.AddValue("simTime", "Total duration of the simulation [s])", simStopTime);
//	cmd.AddValue("interPacketInterval", "Inter-packet interval [us])", interPacketInterval);
	cmd.AddValue("harq", "Enable Hybrid ARQ", harqEnabled);
	cmd.AddValue("rlcAm", "Enable RLC-AM", rlcAmEnabled);
	cmd.AddValue("protocol", "TCP protocol", protocol);
	cmd.AddValue("bufferSize", "buffer size", bufferSize);
	cmd.Parse(argc, argv);

	if (log_packetflow){
		LogComponentEnable ("EpcEnbApplication", LOG_INFO);
		LogComponentEnable ("EpcSgwPgwApplication", LOG_INFO);
		LogComponentEnable ("LteEnbRrc", LOG_INFO);
		LogComponentEnable ("PacketSink", LOG_INFO);
		LogComponentEnable ("UdpClient", LOG_INFO);
	}

	//Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (65535));
	Config::SetDefault ("ns3::TcpSocketBase::MinRto", TimeValue (MilliSeconds (200)));
	Config::SetDefault ("ns3::Ipv4L3Protocol::FragmentExpirationTimeout", TimeValue (Seconds (1)));
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (PacketSize));
	Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (1));

	Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (131072*200));
	Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (131072*200));

	Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue(rlcAmEnabled));
	Config::SetDefault ("ns3::MmWaveHelper::HarqEnabled", BooleanValue(harqEnabled));
	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));
	Config::SetDefault ("ns3::MmWaveFlexTtiMaxWeightMacScheduler::HarqEnabled", BooleanValue(true));
	Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));

	Config::SetDefault ("ns3::UeManager::SplitAlgorithm", UintegerValue (splitAlgorithm));
	Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue (ns3::LteEnbRrc::RLC_AM_ALWAYS));
	Config::SetDefault ("ns3::LteRlcAm::PollRetransmitTimer", TimeValue(MilliSeconds(2.0)));
	Config::SetDefault ("ns3::LteRlcAm::ReorderingTimer", TimeValue(MilliSeconds(1.0)));
	Config::SetDefault ("ns3::LteRlcAm::StatusProhibitTimer", TimeValue(MilliSeconds(1.0)));
	Config::SetDefault ("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue(MilliSeconds(2.0)));
	Config::SetDefault ("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue (bufferSize));
	Config::SetDefault ("ns3::Queue::MaxPackets", UintegerValue (100*1000));

	Config::SetDefault ("ns3::CoDelQueueDisc::Mode", StringValue ("QUEUE_MODE_PACKETS"));
	Config::SetDefault ("ns3::CoDelQueueDisc::MaxPackets", UintegerValue (50000));
//	Config::SetDefault ("ns3::CoDelQueue::Interval", StringValue ("500ms"));
//	Config::SetDefault ("ns3::CoDelQueue::Target", StringValue ("50ms"));

//	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpCubic::GetTypeId ()));
	if(protocol == "TcpNewReno")
	{
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
	}
	else if (protocol == "TcpVegas")
	{
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
	}
	else
	{
		std::cout<<protocol<<" Unkown protocol.\n";
		return 1;
	}

	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::ChannelCondition", StringValue(condition));
	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::Scenario", StringValue(scenario));
	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::OptionalNlos", BooleanValue(false));
	Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::Shadowing", BooleanValue(true)); // enable or disable the shadowing effect

	Config::SetDefault ("ns3::MmWave3gppChannel::UpdatePeriod", TimeValue (MilliSeconds (100))); // Set channel update period, 0 stands for no update.
	Config::SetDefault ("ns3::MmWave3gppChannel::CellScan", BooleanValue(false)); // Set true to use cell scanning method, false to use the default power method.
	Config::SetDefault ("ns3::MmWave3gppChannel::Blockage", BooleanValue(false)); // use blockage or not
	Config::SetDefault ("ns3::MmWave3gppChannel::PortraitMode", BooleanValue(true)); // use blockage model with UT in portrait mode
	Config::SetDefault ("ns3::MmWave3gppChannel::NumNonselfBlocking", IntegerValue(4)); // number of non-self blocking obstacles
	Config::SetDefault ("ns3::MmWave3gppChannel::BlockerSpeed", DoubleValue(1)); // speed of non-self blocking obstacles

	Config::SetDefault ("ns3::MmWavePhyMacCommon::NumHarqProcess", UintegerValue(100));

	double hBS = 0; //base station antenna height in meters;
	double hUT = 0; //user antenna height in meters;
	if(scenario.compare("RMa")==0)
	{
		hBS = 35;
		hUT = 1.5;
	}
	else if(scenario.compare("UMa")==0)
	{
		hBS = 25;
		hUT = 1.5;
	}
	else if (scenario.compare("UMi-StreetCanyon")==0)
	{
		hBS = 10;
		hUT = 1.5;
	}
	else if (scenario.compare("InH-OfficeMixed")==0 || scenario.compare("InH-OfficeOpen")==0 || scenario.compare("InH-ShoppingMall")==0)
	{
		hBS = 3;
		hUT = 1;
	}
	else
	{
		std::cout<<"Unkown scenario.\n";
		return 1;
	}

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
		p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
		if (i==0)
		{
			p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.009)));
		}
		else if(i==1)
		{
			p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.004)));
		}
		else
		{
			p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0015)));

		}
		//p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010+i*0.0025)));

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

/*	NS_LOG_UNCOND("# Install an obstacle block");
	Ptr < Building > building4d;
	building4d = Create<Building> ();
	building4d->SetBoundaries (Box (30,60.0, -40.0, 40, 0.0, 50));*/

	NS_LOG_UNCOND("# Create UE, eNB nodes");
	NodeContainer ueNodes;
	NodeContainer enbNodes;
	NodeContainer senbNodes;
	enbNodes.Create(1);
	senbNodes.Create(1);
	ueNodes.Create(nodeNum);

	NS_LOG_UNCOND("# Mobility set up");
	Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
	enbPositionAlloc->Add (Vector (0.0, 0.0, hBS));
	MobilityHelper enbmobility;
	enbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	enbmobility.SetPositionAllocator(enbPositionAlloc);
	enbmobility.Install (enbNodes);
	enbmobility.Install (senbNodes);
	BuildingsHelper::Install (enbNodes);
	BuildingsHelper::Install (senbNodes);

	MobilityHelper uemobility;
	uemobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator> ();
	uePositionAlloc->Add (Vector (50, 0, hUT));
	uemobility.SetPositionAllocator(uePositionAlloc);
/*	uemobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
	Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator> ();
	uePositionAlloc->Add (Vector (41.0, 10.0, hUT));
	uePositionAlloc->Add (Vector (80.0, 20.0, hUT));
	uePositionAlloc->Add (Vector (-100.0, 53.0, hUT));
	uemobility.SetPositionAllocator(uePositionAlloc);*/
	uemobility.Install (ueNodes);
	BuildingsHelper::Install (ueNodes);

	// Install LTE Devices to the nodes
	NS_LOG_UNCOND("# Install LTE device to the nodes");
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
	NetDeviceContainer senbMmWaveDevs = mmwaveHelper->InstallSenbDevice (senbNodes, 2);
	mmwaveHelper->SetLteChannel (lteHelper->GetDlChannel(), lteHelper->GetUlChannel());
	NetDeviceContainer ueDevs = mmwaveHelper->InstallDcUeDevice (ueNodes);

        mmwaveHelper->NotifyEnbNeighbor (enbNodes.Get(0), senbNodes.Get(0));

	// Install the IP stack on the UEs
	// Assign IP address to UEs, and install applications
	NS_LOG_UNCOND("Install the IP stack on the UE");
	internet.Install (ueNodes);
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

	NS_LOG_UNCOND("# Attach UE to eNB");
//	mmwaveHelper->AttachToClosestEnb (ueDevs, senbMmWaveDevs);
	mmwaveHelper->EnableTraces ();
	uint16_t sinkPort = 1235;
	Ptr<EpcTft> tftDc = Create<EpcTft> ();
	EpcTft::PacketFilter tftPacketFilter;
	tftPacketFilter.localPortStart = sinkPort;
	tftPacketFilter.localPortEnd = sinkPort;
	tftDc->Add (tftPacketFilter);

	lteHelper->Attach (ueDevs.Get(0), enbLteDevs.Get(0));
	mmwaveHelper->AttachDc (ueDevs.Get(0), senbMmWaveDevs.Get(0), tftDc, dcType); // woody, woody3C

	// Add X2 interface
	mmwaveHelper->AddX2Interface (enbNodes.Get(0), senbNodes.Get(0)); // woody

	NS_LOG_UNCOND("# Install application");
	ApplicationContainer sourceApps;
	ApplicationContainer sinkApps;

	if (isTcp)
	{
		for (uint16_t i = 0; i < ueNodes.GetN (); i++)
		{
			// Set the default gateway for the UE
			Ptr<Node> ueNode = ueNodes.Get (i);
			Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
			ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
	
			// Install and start applications on UEs and remote host
			PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
			sinkApps.Add (packetSinkHelper.Install (ueNodes.Get (i)));
	
			BulkSendHelper ftp ("ns3::TcpSocketFactory", InetSocketAddress (ueIpIface.GetAddress (i), sinkPort));
			sourceApps.Add (ftp.Install (remoteHostContainer.Get (i)));

			std::ostringstream fileName;
			fileName<<"UE-"<<i+1<<"-TCP-DATA.txt";

			AsciiTraceHelper asciiTraceHelper;

			Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ().c_str ());
			sinkApps.Get(i)->TraceConnectWithoutContext("Rx",MakeBoundCallback (&Rx, stream));
			sourceApps.Get(i)->SetStartTime(Seconds (0.1+0.01*i));
			Simulator::Schedule (Seconds (0.1001+0.01*i), &Traces, i);
				//sourceApps.Get(i)->SetStopTime (Seconds (10-1.5*i));
			sourceApps.Get(i)->SetStopTime (Seconds (simStopTime));

			sinkPort++;
		}
	}
	else
	{
		PacketSinkHelper dlPacketSinkHelperDc ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
		sinkApps.Add (dlPacketSinkHelperDc.Install (ueNodes.Get(0)));

		UdpClientHelper dlClientDc (ueIpIface.GetAddress (0), sinkPort);
		dlClientDc.SetAttribute ("Interval", TimeValue (MilliSeconds(1000)));
		dlClientDc.SetAttribute ("MaxPackets", UintegerValue(1000000));
		sourceApps.Add (dlClientDc.Install (remoteHostContainer.Get (0)));

		sourceApps.Start (Seconds (0.1));
		sourceApps.Stop (Seconds (simStopTime));
	}

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
	Simulator::Stop (Seconds (simStopTime));
	Simulator::Run ();

	double lteThroughput = sinkApps.Get(0)->GetObject<PacketSink>()->GetTotalRx () * 8.0 / (1000000.0*(simStopTime - 0.1));
	NS_LOG_UNCOND ("AverageLteThroughput: " << lteThroughput << "Mbps");

	Simulator::Destroy ();

	return 0;

}
