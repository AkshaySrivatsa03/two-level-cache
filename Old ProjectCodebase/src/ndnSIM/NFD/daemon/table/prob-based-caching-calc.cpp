/**
* Class file for custom caching class.
* This class hass methods to do probability calculations
*/

#include "prob-based-caching-calc.hpp"
#include <iostream>

ProbCacheCalc::ProbCacheCalc(double tsi, double tsb): tsi(tsi), tsb(tsb) {}

double ProbCacheCalc::getFinalProb() {
  return finalProb;
}

void ProbCacheCalc::calcPop(double poptdelta, double maxPop) {
  if(totalRequests == 0) {
    totalRequests = 1;
  }
//  std::cout << "alpha: " << alpha << " poptdelta: " << poptdelta << " numRequests: " << numRequests << " totalRequests: " << totalRequests << " maxPop: " << maxPop << "\n" << std::flush;
  popt = (alpha * poptdelta + (1 - alpha) * numRequests/totalRequests)/maxPop;
}

double ProbCacheCalc::getPop() {
  return popt;
}

double ProbCacheCalc::calcCacheCap() {
  if(tsi == 0) {
    tsi = 1;
  }
  if(tsb == 0) {
    tsb = 1;
  }
  int numIter = tsi - (tsb - 1);
  int currCap = numIter * cacheCap;  
//  std::cout << "currCap: " << currCap << " numIter " << numIter << " cacheCap " << cacheCap << " tsi: " << tsi << 
 //   " tsb: " << tsb << "\n" << std::flush;
  timesIn = currCap / (ttw * cacheCap);
  return timesIn;    
}

void ProbCacheCalc::calcFinalProb() {
  if(tsi == 0) {
    tsi = 1;
  }
  cacheWeight = tsb/tsi;
//  std::cout << "timesIn: " << timesIn << " cacheWeight: " << cacheWeight << " popt: " << popt << "\n" << std::flush;
  finalProb = timesIn * cacheWeight * popt;         
}

void ProbCacheCalc::setNumRequests(int n) {
  numRequests = n;
}

void ProbCacheCalc::setTotalRequests(int tr) {
  totalRequests = tr;
}
