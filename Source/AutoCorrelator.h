/*
  ==============================================================================

    AutoCorrelator.h
    Created: 28 May 2024 9:24:09pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include "XDDSP/XDDSP.h"

template <typename T, unsigned long N>
T autoCorrelateStaticSizeHalved(std::array<T, N> &data)
{
 constexpr unsigned long nHalved = N/2;
 
 XDDSP::fftStaticSize(data);
 
 // Multiply by conjugate
 for (unsigned long i = 0; i < nHalved; ++i)
 {
  data[i] = data[i]*data[i] + data[data.size() - i - 1]*data[data.size() - i - 1];
  data[data.size() - i - 1] = 0.;
 }
 data[0] = data[1] = 0.;
 XDDSP::ifftStaticSize(data);
 
 T norm = data[0];
 if (norm > 0.) norm = 1./norm;
 for (unsigned long i = 0; i < nHalved; ++i)
 {
  data[i] = data[i]*norm;
 }
 
 unsigned long maxima = 1;
 constexpr T threshold = 0.8;
 while (maxima < nHalved && data[maxima] >= threshold) ++ maxima;
 while (maxima < nHalved && data[maxima] < threshold) ++maxima;
 while (maxima < nHalved && data[maxima - 1] <= data[maxima]) ++ maxima;
 if (data[maxima-2] > data[maxima]) --maxima;
 
 T fMaxima = -1;
 
 if (maxima < nHalved - 1)
 {
  T maxima1;
  XDDSP::IntersectionEstimator intersect;
  intersect.setSampleValues(data[maxima - 2],
                            data[maxima - 1],
                            data[maxima],
                            data[maxima + 1]);
  
  intersect.calculateStationaryPoints(fMaxima, maxima1);
  fMaxima = maxima + maxima1 - 2.;
 }
 
 return fMaxima;
}
