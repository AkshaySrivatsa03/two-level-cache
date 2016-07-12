/**
* Class file for custom caching class.
* This class hass methods to do probability calculations
*/

#include "probext-data-info.hpp"

ProbCacheExtDataInfo::ProbCacheExtDataInfo() {
  frequency = 0;
  popularity = 0;
  probability = 0;
}

double ProbCacheExtDataInfo::getPopularity() {
  return popularity;
}

void ProbCacheExtDataInfo::setPopularity(double pop) {
  popularity = pop;
}

double ProbCacheExtDataInfo::getProbability() {
  return probability;
}

void ProbCacheExtDataInfo::setProbability(double prob) {
  probability = prob;
}

int ProbCacheExtDataInfo::getFrequency() {
  return frequency;
}

void ProbCacheExtDataInfo::setFrequency(int n) {
  frequency = n;
}
