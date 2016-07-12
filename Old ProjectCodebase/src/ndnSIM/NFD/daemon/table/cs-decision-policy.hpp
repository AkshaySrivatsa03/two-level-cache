#include "common.hpp"
#include <string>
#include <map>
#include <array>


#include "probext-data-info.hpp"

#ifndef NFD_DAEMON_TABLE_CS_DECISION_POLICY_HPP
#define NFD_DAEMON_TABLE_CS_DECISION_POLICY_HPP

namespace nfd {
namespace cs{

/*
 * \brief Represents the CS cache decision policy.
 */
class DecisionPolicy
{
public:
  DecisionPolicy() {};

  /**
   * \brief Decides if to cache an incoming data packet.
   *
   * Returns true if the packet should be cached, false otherwise.
   */
  virtual bool admitPacket(const Data & data) = 0;

  // Fields required for probCache calculations
  int numOfRequests;
  int totalrequests;
  
  /*
   * in the array value of below map index 0: frequency, index 1: Popularity, index 2: Probability
   */
  std::map<std::string, ProbCacheExtDataInfo*> dataInfoMap;

  /**
   * PIT Entry for extracting 
   */
   

  // Access specifiers for above two fields
  void setNumOfRequests(int n) { 
    numOfRequests = n;
  }
  
  int getnumOfRequests() {
    return numOfRequests;
  }
  
  void setTotalRequests(int tr){
    totalrequests = tr;
  }

  int getTotalRequests() {
    return totalrequests;
  }

  const
  std::string& getName()
  {
    return m_policyName;
  }
  
  Interest interest;
  
  void setInterest(Interest _interest) {
    interest = _interest;
  }

protected:
  std::string m_policyName;

};

} // namespace cs
} // namespace nfd


#endif
