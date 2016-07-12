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
#include <random>
#include <time.h>

using namespace std;

namespace ns3 {

int l[1000] = {};

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
  double init_s = clock();
  AnnotatedTopologyReader topologyReader("", 1);
  std::cout << "Reading topology file"<<endl;
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-250Clients_64_caches.txt");
  topologyReader.Read();

  vector<Ptr<Node>> clients;
  for(int i = 1; i <= 250; i++) {
    std::string cli = "client" + std::to_string(i);
    clients.push_back(Names::Find<Node>(cli));    
  }
  std::cout << "Added "<< clients.size()<<" clients in vector"<<endl;

  vector<Ptr<Node>> cacheServers;
  for(int i = 1; i <= 63; i++) {
    std::string cs = "cache" + std::to_string(i);
    cacheServers.push_back(Names::Find<Node>(cs));    
  }
  std::cout << "Added "<< cacheServers.size()<<" cache servers in vector"<<endl;

  ndn::StackHelper ndnHelperCache;
//  ndnHelperCache.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "150");
  ndnHelperCache.setCsSize(150);
  for(int i = 0; i < (int)cacheServers.size(); i++) {
    ndnHelperCache.Install(cacheServers[i]);
  } 

// Install NDN stack on rest of the nodes
  ndn::StackHelper ndnHelper;
  for(int i = 0; i < (int)clients.size(); i++) {
    ndnHelper.Install(clients[i]);
  }
  ndnHelper.Install(Names::Find<Node>("originServer"));


// Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");

  Ptr<Node> producer = Names::Find<Node>("originServer");

  cout << "Installing Global routing interface on all nodes";

// Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  cout << "Creating app1...."<<endl;

  std::default_random_engine generator;
  std::lognormal_distribution<double> distribution(0.0,1.0);

/*
  int j = 0;
  for (int k = 0; k < 1; k++) {
      for (int i = 0; i < 256; i++) {
          for (int l = i; l < 2560; l = l + 10) {
              j = int(distribution(generator));
              ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
              consumerHelper.SetAttribute("Frequency", StringValue("1"));
              consumerHelper.SetAttribute("Content", IntegerValue(j));
              int temp = l;
              consumerHelper.SetPrefix("/root/" + Names::FindName(clients[temp]));
              ns3::ApplicationContainer ac = consumerHelper.Install(clients[temp]);
              ac.Start(Seconds(double(k * 1000 + ((i * 10) + 1))));
              ac.Stop(Seconds(double(k * 1000 + ((i * 10) + 2))));
          }
      }
  }
  */
      int j = 0;
      for (int k = 0; k < 1; k++) {
        for (int i = 0; i < 50; i++) {
          j = int(distribution(generator));
          std::cout<<"j is : "<<j<<endl;
          ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
          consumerHelper.SetAttribute("Frequency", StringValue("1"));
          consumerHelper.SetAttribute("Content", IntegerValue(j));
          consumerHelper.SetPrefix("/root/" + Names::FindName(clients[i]));
          ns3::ApplicationContainer ac = consumerHelper.Install(clients[i]);
          ac.Start(Seconds (double(k * 1000 + ( (i * 10) + 1))));
          ac.Stop(Seconds (double(k * 1000 + ( (i * 10) + 2))));
        }
      }

      cout <<"Creating Producer"<<endl;

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute("Freshness", TimeValue (Seconds (2.0)));
  ndnGlobalRoutingHelper.AddOrigins("/root", producer);
  producerHelper.SetPrefix("/root");
  producerHelper.Install(producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

//  Simulator::Stop(Seconds(2564.0));
  Simulator::Stop(Seconds(19993.0));
  Simulator::GetContext();      
  Simulator::Run();
  Simulator::Destroy();
  double end_s = clock();
  std::cout<<"Time taken by the program is : "<<( (end_s - init_s)/CLOCKS_PER_SEC)<<" seconds"<<endl;
  std::cout<<"Program successfully completed"<<endl;
  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
