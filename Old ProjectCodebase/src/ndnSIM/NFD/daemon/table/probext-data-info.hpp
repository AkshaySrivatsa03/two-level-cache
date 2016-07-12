/**
* This is header file for custom caching
* Our solution is based on probabilistic caching 
*/

#ifndef PROB_CACHE_EXT_DATAINFO
#define PROB_CACHE_EXT_DATAINFO

class ProbCacheExtDataInfo
{
  double popularity;
  double probability;
  int frequency;
  
public:
  ProbCacheExtDataInfo();
  double getPopularity();
  void setPopularity(double pop);
  double getProbability();
  void setProbability(double prob);
  int getFrequency();
  void setFrequency(int n);
  
};   


#endif

