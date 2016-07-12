
#include "cs-probcache-decision-policy.hpp"
#include "prob-based-caching-calc.hpp"

#include "../../../utils/ndn-ns3-packet-tag.hpp"
#include "../../../utils/ndn-fw-hop-count-tag.hpp"
#include "../../../utils/ndn-consumer-hop-distance-tag.hpp"
#include <ndn-cxx/tag-host.hpp>
#include <cmath>
#include <float.h>

namespace nfd {
namespace cs{

bool
ProbCacheExtDecisionPolicy::admitPacket(const Data & data)
{
  using ns3::ndn::FwHopCountTag;
  using ns3::ndn::Ns3PacketTag;
  using ns3::ndn::ConsumerHopDistanceTag;

  uint32_t hopCount = 0;
  uint32_t interestDistance = 0;

  auto ns3PacketTag = data.getTag<ns3::ndn::Ns3PacketTag>();
/*  if (ns3PacketTag != nullptr) {
    FwHopCountTag hopCountTag;
    if (ns3PacketTag->getPacket()->PeekPacketTag(hopCountTag)) {
      hopCount = hopCountTag.Get();
    }
    ConsumerHopDistanceTag hopDistance;
    if (ns3PacketTag->getPacket()->PeekPacketTag(hopDistance)) {
      interestDistance = hopDistance.Get();
    }
  }*/
  double tsi = 2.0;
  double tsb = 1.0;
  
  if (ns3PacketTag != nullptr) {
    FwHopCountTag hopCountTag;
    if (ns3PacketTag->getPacket()->PeekPacketTag(hopCountTag)) {
      tsb = (double)hopCountTag.Get();
    }
   /* ConsumerHopDistanceTag hopDistance;
    if (ns3PacketTag->getPacket()->PeekPacketTag(hopDistance)) {
      tsi = (double)hopDistance.Get();
    }*/
    tsi = (double)data.getTSI();  
//    std::cout << "data.getTSI(): " << data.getTSI() << "\n" << std::flush;
//    tsi = 20;
  }
  
//  std::cout << "tsi: " << tsi << " tsb = " << tsb << "\n" << std::flush;
    
  ProbCacheCalc* pce = new ProbCacheCalc(tsi, tsb); 
  pce->setNumRequests(numOfRequests);
  pce->setTotalRequests(totalrequests);

  /*
  * TODO: This popularity should come from stored Entry class
  */
  double privPop = 0.5;
  std::string nameUri = data.getName().toUri();
  if(dataInfoMap.count(nameUri)) {
    privPop = dataInfoMap.find(nameUri)->second->getFrequency();
  }


  double maxPop = 1;
  for (std::map<std::string, ProbCacheExtDataInfo*>::iterator it = dataInfoMap.begin(); it != dataInfoMap.end(); ++it) {
//    std::cout << it->first << " => " << it->second << '\n';
    double currPop = it->second->getPopularity();
    if (currPop > maxPop) {
      maxPop = currPop;
    } 
  }
  if(maxPop == 0) {maxPop = 1;}
  pce->calcPop(privPop, maxPop); 
  double pop = pce->getPop();
//  std::cout << " popularity = " << pop << "\n" << std::flush;
  double cacheCap = pce->calcCacheCap();
  pce->calcFinalProb();
  double prob = pce->getFinalProb();
//  std::cout << "Final probability: " << prob << "\n" << std::flush;
  delete pce;
  
  if(dataInfoMap.find(nameUri) == dataInfoMap.end()) {
    dataInfoMap.insert(std::make_pair(nameUri, new ProbCacheExtDataInfo()));
    ProbCacheExtDataInfo* pcei = dataInfoMap.find(nameUri)->second;
    pcei->setProbability(prob);  
    pcei->setPopularity(pop);
  } else {
    ProbCacheExtDataInfo* pcei = dataInfoMap.find(nameUri)->second;
    pcei->setProbability(prob);
    pcei->setPopularity(pop); 
  }
  double maxProb = 0;

  // Finding the number of Requests for each content
  for (std::map<std::string, ProbCacheExtDataInfo*>::iterator it = dataInfoMap.begin(); it != dataInfoMap.end(); ++it) {
//    std::cout << it->first << " => " << it->second << '\n';
    double currProb = it->second->getProbability();
    if (currProb > maxProb) {
      maxProb = currProb;
    } 
  }
//  std::cout << "admitPacket(): maxProb: " << maxProb << " prob: " << prob << "\n" << std::flush;  
  return prob < maxProb; 
  //return randAccept(interestDistance - hopCount);
}

bool
ProbCacheExtDecisionPolicy::randAccept(int hopCount)
{
  if (hopCount == 0) {
    return true;
  }

  double acceptRatio = 1 / pow(2.0, double(hopCount - 1));

  if (m_ranVar->GetValue(double(0), double(1.0)) <= acceptRatio) {
    return true;
  }
  return false;
}

}
}
