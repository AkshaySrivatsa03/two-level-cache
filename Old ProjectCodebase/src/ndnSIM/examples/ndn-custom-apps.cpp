/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-custom-apps.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include <iostream>

using namespace std;

namespace ns3 {

/**
 * This scenario simulates a one-node two-custom-app scenario:
 *
 *   +------+ <-----> (CustomApp)
 *   | Node |
 *   +------+ <-----> (Hijacker)
 *
 *     NS_LOG=CustomApp ./waf --run=ndn-custom-apps
 */
int
main(int argc, char* argv[])
{
  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
//  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-custom1.txt");
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-custom2.txt");
  topologyReader.Read();

  // Creating nodes
//  Ptr<Node> node = CreateObject<Node>();
  // Install Content Store stack on cache node
/*  ndn::StackHelper ndnHelperCache;
  ndnHelperCache.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "10");
  ndnHelperCache.Install(Names::Find<Node>("cache1")); */

  // Install NDN stack on all nodes
/*  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();*/

// Install NDN stack on rest of the nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.Install(Names::Find<Node>("client"));
  ndnHelper.Install(Names::Find<Node>("originServer"));


// Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");

  Ptr<Node> consumers = Names::Find<Node>("client");
  Ptr<Node> producer = Names::Find<Node>("originServer");

  cout << "Installing Global routing interface on all nodes";

// Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  cout << " Creating app1....";
  // App1
  ndn::AppHelper app1("CustomApp");
  app1.SetPrefix("/root");
  app1.Install(consumers);

  // App2
  //ndn::AppHelper app2("Hijacker");
  //app2.Install(producer); // last node

  cout << " Creating Producer";

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper.SetPrefix("/prefix");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

  ndnGlobalRoutingHelper.AddOrigins("/root", producer);
  producerHelper.SetPrefix("/root");
  producerHelper.Install(producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(3.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
