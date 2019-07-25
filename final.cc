#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include <ns3/propagation-loss-model.h>
#include <ns3/single-model-spectrum-channel.h>
#include "ns3/netanim-module.h"
using namespace ns3;
int main (){
  printf("    Hello,please input the parameters about the WSN and algorithm you want to simulate! First,input the number of nodes n and the x and y position for each node,totally 2*n+1 numbers. Then input the speed of sink and the route waypoints x and y, totally 2*n+1 numbers. Finally, input the max communication range of nodes and the packets for each node to send!\n    Like this,  4 20 20 50 70 110 110 273 273\n2.0  110  110  50  70  273  273  20  20       80 100\n    In these numbers,the first number 4 means there are four nodes and the first node(20,20)is mobile sink node. 2.0 is the speed of sink node, the following 4*2 numbers are the waypoint route. 80 is the max communication range,100 is the packets that every node will send to the sink.\nNow,please input your data!\n");
  int posnum;
  float pos1,pos2,bef1,bef2;
  scanf("%d",&posnum);
  NodeContainer nodes;
  nodes.Create (posnum);  
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for(int i=0;i<posnum;i++){
    scanf("%f %f",&pos1,&pos2);
    if(i==0){bef1=pos1;bef2=pos2;}
    positionAlloc->Add(Vector(pos1,pos2,0.0)); }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::WaypointMobilityModel", "InitialPositionIsWaypoint", BooleanValue (true));
  mobility.Install(nodes);
  Ptr<WaypointMobilityModel> mob = nodes.Get(0)->GetObject<WaypointMobilityModel> ();
  float speed, distance, arrivetime=0;
  scanf("%f",&speed);
  for(int i=0;i<posnum;i++){
    scanf("%f %f",&pos1,&pos2);
    distance = sqrt((bef1-pos1)*(bef1-pos1)+(bef2-pos2)*(bef2-pos2));
    bef1=pos1; bef2=pos2;
    arrivetime=arrivetime + distance/speed;
    Waypoint m_waypoint (Seconds (arrivetime), Vector (pos1, pos2, 0.0));
    mob->AddWaypoint (m_waypoint); }
  Simulator::Schedule (Seconds (0.0), &Object::Initialize, mob);
  float maxrange;
  int packets;
  scanf("%f %d",&maxrange,&packets);

  LrWpanHelper lrWpanHelper;
  NetDeviceContainer devContainer;
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  ns3::Config::SetDefault("ns3::RangePropagationLossModel::MaxRange", ns3::DoubleValue(maxrange));
  Ptr<RangePropagationLossModel> model = CreateObject<RangePropagationLossModel> ();
  channel->AddPropagationLossModel(model);
  Ptr<LrWpanNetDevice> dev[posnum];
  char str[6]="00:01";
  for(int i=0;i<posnum;i++){
    dev[i] = CreateObject<LrWpanNetDevice> ();
    dev[i]->AssignStreams(0);
    dev[i]->SetAddress (Mac16Address (str));
    dev[i]->SetChannel(channel);
    nodes.Get(i)->AddDevice(dev[i]);
    dev[i]->SetNode (nodes.Get(i));
    devContainer.Add(dev[i]); }
  lrWpanHelper.AssociateToPan (devContainer, 10);
  SixLowPanHelper sixlowpan;
  NetDeviceContainer six = sixlowpan.Install (devContainer);
  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.Install (nodes);
  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer i = ipv6.Assign (six);
  //3 20 20 50 70 273 273 3 2 100 100 3.5 253 300 5 40 60
  //4 20 20 50 70 110 110 273 273 10.0 110 110 50 70 273 273 20 20 80 100
  //8 20 20 50 70 110 110 273 273 90 36 27 180 1 1 345 220 3.0 1 1 345 220 110 110 50 70 90 36 273 273 27 180 20 20 50 100
  /*  17   20 20 50 70 110 110 273 273   90 36 27 180 1 1 345 220    30 30 70 29 67 135 356 234   59 324 145 23 245 89 37 267   98 251
  2.0 1 1 345 220 110 110 50 70 90 36 273 273 27 180   37 267 59 324 145 23 245 89 98 251   67 135 30 30 70 29 356 234 20 20   50 100 */
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer app1 = echoServer.Install (nodes.Get(0));
  app1.Start (Seconds (0));
  app1.Stop (Seconds (3600));
  UdpEchoClientHelper echoClient (i.GetAddress (0,1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (packets+10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (64));
  ApplicationContainer app2;
  for(int i=1;i<posnum;i++)
    app2.Add(echoClient.Install(nodes.Get(i)));
  app2.Start (Seconds (0));
  app2.Stop (Seconds (3600));
  Simulator::Stop(Seconds(3600));
  AsciiTraceHelper ascii;
  lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream ("final.tr"));
  AnimationInterface anim ("final.xml");
  anim.SetMobilityPollInterval(Seconds(0.01));
  anim.SetMaxPktsPerTraceFile(99999999999999);
  Simulator::Run ();
  Simulator::Destroy ();
}
