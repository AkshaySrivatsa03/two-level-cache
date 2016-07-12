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
//  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-custom2.txt");
 // topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-custom3.txt");
//  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-project.txt");
  std::cout << "Reading topology file";
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-project-2000-255.txt");
  topologyReader.Read();
  
  std::cout << "Adding clients in vector";
  vector<Ptr<Node>> clients;
  for(int i = 1; i <= 2000; i++) {
    std::string cli = "client" + std::to_string(i);
    clients.push_back(Names::Find<Node>(cli));    
  }

  std::cout << "Adding Cache servers in vector";
  vector<Ptr<Node>> cacheServers;
  for(int i = 1; i <= 255; i++) {
    std::string cs = "cache" + std::to_string(i);
    cacheServers.push_back(Names::Find<Node>(cs));    
  }
  
  // Creating nodes
//  Ptr<Node> node = CreateObject<Node>();
  // Install Content Store stack on cache node
  ndn::StackHelper ndnHelperCache;
  ndnHelperCache.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "2");
  //ndnHelperCache.SetOldContentStore("ns3::ndn::cs::Nocache");
//  ndnHelperCache.SetOldContentStore("ns3::ndn::cs::ProbCacheExt::Lru", "MaxSize", "2");
//  ndnHelperCache.setCsSize(2); 
  for(int i = 0; i < (int)cacheServers.size(); i++) {
    ndnHelperCache.Install(cacheServers[i]);
  } 

  // Install NDN stack on all nodes
/*  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();*/

// Install NDN stack on rest of the nodes
  ndn::StackHelper ndnHelper;
  for(int i = 0; i < (int)clients.size(); i++) {
    ndnHelper.Install(clients[i]);
  }
//  ndnHelper.Install(Names::Find<Node>("client2"));
//  ndnHelper.Install(Names::Find<Node>("client3"));
  ndnHelper.Install(Names::Find<Node>("originServer"));


// Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");

//  Ptr<Node> consumers1 = Names::Find<Node>("client1");
//  Ptr<Node> consumers2 = Names::Find<Node>("client2");
//  Ptr<Node> consumers3 = Names::Find<Node>("client3");
//  Ptr<Node> consumers[3] = {Names::Find<Node>("client1"), Names::Find<Node>("client2"), Names::Find<Node>("client3")};
  Ptr<Node> producer = Names::Find<Node>("originServer");

  cout << "Installing Global routing interface on all nodes";

// Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  cout << " Creating app1....";
  // App1
  /*ndn::AppHelper app1("CustomApp");
  app1.SetPrefix("/root");
  app1.Install(consumers1);
  app1.Install(consumers2);
  app1.Install(consumers3);*/
  
//  std::string content[] = {"alpha", "beta", "gamma", "delta", "epsilon"};

//  const int nrolls=100000;  // number of experiments
//  const int nstars=100;    // maximum number of stars to distribute

  std::default_random_engine generator;
  std::lognormal_distribution<double> distribution(0.0,1.0);

  //int p[100]={};

/*  for (int i=0; i<nrolls; ++i) {
    double number = distribution(generator);
    if ((number>=0.0)&&(number<10.0)) ++p[int(number)];
  }*/

  int j = 0;
/*  int alphaCount = 1;
  int betaCount = 2;
  int gammaCount = 3;
  int deltaCount = 4;
  int epsilonCount = 10;  */
  for (int k = 0; k < 1 ; k++) {
    for (int i = 0; i < 400; i++) {
      for(int l = 0; l < 2; l++) {
      j = int(distribution(generator));
     // j = p[i]*nstars/nrolls;
     //   std::cout << "i: " << i;
      ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
      consumerHelper.SetAttribute("Frequency", StringValue("1")); 
      consumerHelper.SetAttribute("Content", IntegerValue(j)); 
      int temp = 0;
      if(l == 0) {
        temp = i;
      } else {
        temp = i + 400;
      }
      consumerHelper.SetPrefix("/root/" + Names::FindName(clients[temp]));
      ns3::ApplicationContainer ac = consumerHelper.Install(clients[temp]);
      ac.Start(Seconds (double(k * 1000 + ( (i * 10) + 1))));
      ac.Stop(Seconds (double(k * 1000 + ( (i * 10) + 2))));
      }
    /*  if((j % 5) == 0) {
        if(alphaCount == 0) {
          alphaCount++;
        } else {
          alphaCount--;
        }
        j++;
      } else if ((j % 5) == 1) {
        if(betaCount >= 1) {
          betaCount--;
        } else {
          betaCount = 2;
          j++;
        }
      } else if ((j % 5) == 2) {
        if(gammaCount >= 1) {
          gammaCount--;
        } else {
          gammaCount = 3;
          j++;
        }
      } else if ((j % 5) == 3) {
        if(deltaCount >= 1) {
          deltaCount--;
        } else {
          deltaCount = 4;
          j++;
        }
      } else if ((j % 5) == 4) {
        if(epsilonCount >= 1) {
          epsilonCount--;
        } else {
          epsilonCount = 10;
          j++;
        }
      } */  
    }
  }
    
/*    ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
    consumerHelper2.SetAttribute("Frequency", StringValue("1")); 
    consumerHelper2.SetPrefix("/root/" + Names::FindName(consumers[1]));
//    consumerHelper.Install(consumers[i]);
    //consumerHelper2.Install(consumers[1]).Start(Seconds (2.0));
    ns3::ApplicationContainer ac2 = consumerHelper2.Install(consumers[1]);
    ac2.Start(Seconds (2.0));
    ac2.Stop(Seconds (3.0));
    //consumerHelper2.Install(consumers[0]).Stop(Seconds (3.0));
    
    ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
    consumerHelper3.SetAttribute("Frequency", StringValue("1")); 
    consumerHelper3.SetPrefix("/root/" + Names::FindName(consumers[2]));
//    consumerHelper.Install(consumers[i]);
    //consumerHelper3.Install(consumers[2]).Start(Seconds (3.0));
    ns3::ApplicationContainer ac3 = consumerHelper3.Install(consumers[2]);
    ac3.Start(Seconds (3.0));
    ac3.Stop(Seconds (4.0)); */
    //consumerHelper3.Install(consumers[0]).Stop(Seconds (4.0));

  // App2
  //ndn::AppHelper app2("Hijacker");
  //app2.Install(producer); // last node

  cout << " Creating Producer";

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute("Freshness", TimeValue (Seconds (2.0)));
  ndnGlobalRoutingHelper.AddOrigins("/root", producer);
  producerHelper.SetPrefix("/root");
  producerHelper.Install(producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(2004.0));
//  Simulator::Stop(Seconds(9993.0));

  Simulator::Run();
  Simulator::Destroy();
  std::cout<<"Program terminated successfully"<<std::endl;
  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
