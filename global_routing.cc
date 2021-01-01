#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <stdio.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/stats-module.h"
#include "ns3/aodv-module.h"


using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("SandBox");

int 
main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse(argc, argv);

    NS_LOG_INFO("CREATE NODES");
    NodeContainer c;
    c.Create(7);
    NodeContainer n0n1 = NodeContainer(c.Get(0), c.Get(1));
    NodeContainer n0n2 = NodeContainer(c.Get(0), c.Get(2));
    NodeContainer n3n1 = NodeContainer(c.Get(3), c.Get(1));
    NodeContainer n3n2 = NodeContainer(c.Get(3), c.Get(2));
    NodeContainer n2n4 = NodeContainer(c.Get(2), c.Get(4));
    NodeContainer n2n6 = NodeContainer(c.Get(2), c.Get(6));
    NodeContainer n4n5 = NodeContainer(c.Get(4), c.Get(5));
    NodeContainer n4n6 = NodeContainer(c.Get(4), c.Get(6));

    InternetStackHelper internet;
    

    NS_LOG_INFO("CREATE CHANNELS");
    PointToPointHelper p2p;

    p2p.SetDeviceAttribute("DataRate", StringValue("20Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer d0d1 = p2p.Install(n0n1);
    NetDeviceContainer d0d2 = p2p.Install(n0n2);
    NetDeviceContainer d3d1 = p2p.Install(n3n1);
    NetDeviceContainer d3d2 = p2p.Install(n3n2);
    NetDeviceContainer d2d4 = p2p.Install(n2n4);
    NetDeviceContainer d2d6 = p2p.Install(n2n6);
    NetDeviceContainer d4d5 = p2p.Install(n4n5);
    NetDeviceContainer d4d6 = p2p.Install(n4n6);

    // // Routing
    // Ipv4ListRoutingHelper list;
    // AodvHelper aodv;
    // list.Add(aodv, 100);
    // internet.SetRoutingHelper(list);
    internet.Install(c);

    NS_LOG_INFO("Assigning IP Addresses");
    Ipv4AddressHelper ipv4;

    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i1 = ipv4.Assign(d0d1);

    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);

    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i1 = ipv4.Assign(d3d1);

    ipv4.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i2 = ipv4.Assign(d3d2);

    ipv4.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i4 = ipv4.Assign(d2d4);

    ipv4.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i6 = ipv4.Assign(d2d6);

    ipv4.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i5 = ipv4.Assign(d4d5);

    ipv4.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i6 = ipv4.Assign(d4d6);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    

    NS_LOG_INFO("Create Application");

    uint16_t port = 9;

    OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(i2i4.GetAddress(1), port)));
    onoff.SetConstantRate(DataRate("800kbps"));
    onoff.SetAttribute("MaxBytes", UintegerValue(15000 * 1024));
    ApplicationContainer apps = onoff.Install(c.Get(0));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(8.0));
    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    apps = sink.Install(c.Get(4));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(8.0));

    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(i4i5.GetAddress(1), port)));
    onoff.SetConstantRate(DataRate("500kbps"));
    onoff.SetAttribute("MaxBytes", UintegerValue(16800 * 1024));
    apps = onoff.Install(c.Get(3));
    apps.Start(Seconds(4.0));
    apps.Stop(Seconds(9.0));
    apps = sink.Install(c.Get(5));
    apps.Start(Seconds(4.0));
    apps.Stop(Seconds(9.0));

    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(i3i2.GetAddress(0), port)));
    onoff.SetConstantRate(DataRate("500kbps"));
    onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.3]"));
    apps = onoff.Install(c.Get(0));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(9.0));
    apps = sink.Install(c.Get(3));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(9.0));

    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(i2i6.GetAddress(1), port)));
    onoff.SetConstantRate(DataRate("500kbps"));
    onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.3]"));
    apps = onoff.Install(c.Get(0));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(9.0));
    apps = sink.Install(c.Get(6));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(9.0));

    Ptr<Node> n4 = c.Get(2);
    Ptr<Ipv4> ipv24 = n4->GetObject<Ipv4>();
    uint32_t ipv4ifIndex1 = 3;
    Simulator::Schedule(Seconds(5), &Ipv4::SetDown, ipv24, ipv4ifIndex1);
    Simulator::Schedule(Seconds(7), &Ipv4::SetUp, ipv24, ipv4ifIndex1);

    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("sandbox.tr"));
    p2p.EnablePcapAll("sandbox");

    NS_LOG_INFO("RUN SIMULATION");
    Simulator::Stop(Seconds(10));

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    AnimationInterface anim("global.xml");
    anim.SetConstantPosition(c.Get(0), 1.0, 1.0);
    anim.SetConstantPosition(c.Get(1), 1.0, 10.0);
    anim.SetConstantPosition(c.Get(2), 10.0, 1.0);
    anim.SetConstantPosition(c.Get(3), 10.0, 10.0);
    anim.SetConstantPosition(c.Get(4), 20.0, 1.0);
    anim.SetConstantPosition(c.Get(5), 30.0, 10.0);
    anim.SetConstantPosition(c.Get(6), 20.0, 10.0);

    Simulator::Run();
    NS_LOG_INFO("DONE");

    flowMonitor->SerializeToXmlFile("flow_global.xml", true, true);
    Simulator::Destroy();
    return 0;
}

