
#include "common.hpp"
#include "cs-decision-policy.hpp"
#include "ns3/random-variable-stream.h"
//#include "table/pit.hpp"

#ifndef NFD_DAEMON_TABLE_CS_PROBCACHEEXT_DECISION_POLICY_HPP
#define NFD_DAEMON_TABLE_CS_PROBCACHEEXT_DECISION_POLICY_HPP

namespace nfd {
namespace cs{

class ProbCacheExtDecisionPolicy : public DecisionPolicy
{
public:
  ProbCacheExtDecisionPolicy()
  {
    m_policyName = std::string("ProbCacheExt");
    m_ranVar = ns3::CreateObject<ns3::UniformRandomVariable>();
  };

  virtual bool admitPacket(const Data & data);
  

private:
  bool randAccept(int hopCount);
  //Pit m_pit; 
  

private:
  ns3::Ptr<ns3::UniformRandomVariable>  m_ranVar;
};


} // namespace cs
} // namespace nfd

#endif
