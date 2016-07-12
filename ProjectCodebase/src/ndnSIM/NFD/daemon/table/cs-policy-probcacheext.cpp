/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cs-policy-probcacheext.hpp"
#include "cs.hpp"
#include <ndn-cxx/util/signal.hpp>
#include <map>
#include "../fw/face-table.hpp"
#include <float.h>

// Himanshu: Added for finding number of packets at this interface
//#include "face-counters.hpp" 

//#include <ndn-cxx/management/nfd-face-status.hpp>

namespace nfd {
namespace cs {
namespace ProbCacheExtPolicy {

const std::string ProbCacheExtPolicy::POLICY_NAME = "probcacheext";

ProbCacheExtPolicy::ProbCacheExtPolicy()
  : Policy(POLICY_NAME)
{
}

void
ProbCacheExtPolicy::doAfterInsert(iterator i)
{
  std::cout << "i->getName(): " << i->getName() << " " << std::flush;
  this->attachQueue(i);
  this->evictEntries();
}

void
ProbCacheExtPolicy::doAfterRefresh(iterator i)
{
  this->detachQueue(i);
  this->attachQueue(i);
}

void
ProbCacheExtPolicy::doBeforeErase(iterator i)
{
  this->detachQueue(i);
}

void
ProbCacheExtPolicy::doBeforeUse(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) != m_entryInfoMap.end());
}

void
ProbCacheExtPolicy::evictEntries()
{
  std::cout << "ProbCacheExtPolicy::evictEntries() \n" << std::flush;

  BOOST_ASSERT(this->getCs() != nullptr);

  while (this->getCs()->size() > this->getLimit()) {
//    std::cout << "ProbCacheExtPolicy::evictEntries(): Inside while loop \n" << std::flush;
    this->evictOne();
  }
}

void
ProbCacheExtPolicy::evictOne()
{
  BOOST_ASSERT(!m_queues[QUEUE_UNSOLICITED].empty() ||
               !m_queues[QUEUE_STALE].empty() ||
               !m_queues[QUEUE_FIFO].empty());

  iterator i;
//  std::cout << "ProbCacheExtPolicy::evictOne(): \n" << std::flush;
  if (!m_queues[QUEUE_UNSOLICITED].empty()) {
//    std::cout << "ProbCacheExtPolicy::evictOne(): QUEUE_UNSOLICITED \n" << std::flush;
    i = m_queues[QUEUE_UNSOLICITED].front();
  }
/*  else if (!m_queues[QUEUE_STALE].empty()) {
    std::cout << "ProbCacheExtPolicy::evictOne(): QUEUE_STALE\n" << std::flush;
    i = m_queues[QUEUE_STALE].front();
  }*/ 
//  else if (!m_queues[QUEUE_FIFO].empty()) {
  else {
 //   std::cout << "ProbCacheExtPolicy::evictOne(): QUEUE_FIFO \n" << std::flush;
    std::map<std::string, ProbCacheExtDataInfo*> dataInfoMap = this->getCs()->getDataInfoMap();
    double minProb = DBL_MAX;
    std::string minName;
    // Finding the number of Requests for each content
    for (std::map<std::string, ProbCacheExtDataInfo*>::iterator it = dataInfoMap.begin(); it != dataInfoMap.end(); ++it) {
      std::cout << it->first << " => " << it->second->getProbability() << '\n';
      double currProb = it->second->getProbability();
  //    std::cout << "currProb: " << currProb << "minProb" << minProb << "\n" << std::flush;
      if (currProb < minProb) {
        minProb = currProb;
        minName = it->first;
      } 
    }
    
 //   std::cout << "minName: " << minName << "\n" << std::flush;
    bool iSet = false; 
    //std::for_each ( m_queues[QUEUE_FIFO].begin(),std::next(m_queues[QUEUE_FIFO].begin(),1),[&minName, &i, &iSet](iterator &it) {
    std::for_each ( m_queues[QUEUE_FIFO].begin(),m_queues[QUEUE_FIFO].end(),[&minName, &i, &iSet](iterator &it) {
/*      std::cout << "Inside for_each\n" << std::flush;
      std::cout << "Getting Name..." << it->getName() << "\n" << std::flush;
      std::cout << "Getting URI..." << it->getName().toUri() << "\n" << std::flush;*/
      //if(it->getName().toUri().find("client") != std::string::npos) {
        if(it->getName().toUri().compare(minName) == 0) {
    //      std::cout << "Assisning it to i\n" << std::flush;
          i = it;
          iSet = true;
        }
      //}
    });
    if(iSet) {
  //   std::cout << "i.getName: " << i->getName() << "\n" << std::flush;
    } else {
  //   std::cout << "i is NULL\n" << std::flush;
     i = m_queues[QUEUE_FIFO].front();
//     i = m_queues[QUEUE_FIFO].find();
    } 
  }
  std::cout << "Data getting evicted: " << i->getName() << "\n" << std::flush;

  this->detachQueue(i);
  this->emitSignal(beforeEvict, i);
}

void
ProbCacheExtPolicy::attachQueue(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) == m_entryInfoMap.end());

  EntryInfo* entryInfo = new EntryInfo();
  if (i->isUnsolicited()) {
    entryInfo->queueType = QUEUE_UNSOLICITED;
  }
  else if (i->isStale()) {
    entryInfo->queueType = QUEUE_STALE;
  }
  else {
    entryInfo->queueType = QUEUE_FIFO;

    if (i->canStale()) {
      entryInfo->moveStaleEventId = scheduler::schedule(i->getData().getFreshnessPeriod(),
                                              bind(&ProbCacheExtPolicy::moveToStaleQueue, this, i));
    }
  }

  Queue& queue = m_queues[entryInfo->queueType];
  entryInfo->queueIt = queue.insert(queue.end(), i);
  m_entryInfoMap[i] = entryInfo;
}

void
ProbCacheExtPolicy::detachQueue(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) != m_entryInfoMap.end());

  EntryInfo* entryInfo = m_entryInfoMap[i];
  if (entryInfo->queueType == QUEUE_FIFO) {
    scheduler::cancel(entryInfo->moveStaleEventId);
  }

  m_queues[entryInfo->queueType].erase(entryInfo->queueIt);
  m_entryInfoMap.erase(i);
}

void
ProbCacheExtPolicy::moveToStaleQueue(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) != m_entryInfoMap.end());

  EntryInfo* entryInfo = m_entryInfoMap[i];
  BOOST_ASSERT(entryInfo->queueType == QUEUE_FIFO);

  m_queues[QUEUE_FIFO].erase(entryInfo->queueIt);

  entryInfo->queueType = QUEUE_STALE;
  Queue& queue = m_queues[QUEUE_STALE];
  entryInfo->queueIt = queue.insert(queue.end(), i);
  m_entryInfoMap[i] = entryInfo;
}

} // namespace priorityfifo
} // namespace cs
} // namespace nfd
