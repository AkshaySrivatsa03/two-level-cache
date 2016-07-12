/**
* This is header file for custom caching
* Our solution is based on probabilistic caching 
*/

#ifndef PROB_CACHE_CALC
#define PROB_CACHE_CALC

class ProbCacheCalc
{
  double tsi;
  double tsb;
  int cacheCap = 2;
  int numOfCaches;
  double ttw = 10;
  double timesIn;
  double cacheWeight;
  double popt;
  double alpha = 0.25;
  int numRequests;
  int totalRequests;
  double tdelta;
  double poptdelta;
  double finalProb;
  
public:
  ProbCacheCalc(double tsi, double tsb);
  double getFinalProb();
  void calcPop(double poptdelta, double maxPop);
  double getPop();
  void calcFinalProb();
  double calcCacheCap();
  void setNumRequests(int n);
  void setTotalRequests(int tr);
  
};   


#endif

