#include "ExponentialFallback.h"
#include "Arduino.h"

ExpFallback::ExpFallback(unsigned int _maxval)
{
  itterator = 0;
  maxval = _maxval;
  fallback = 1;
}

void ExpFallback::hadSuccess(bool reset)
{
  if (reset)
  {
    itterator = 0;
    fallback = 1;
    return;
  } // else
    // else
  return;
}

bool ExpFallback::mustAttempt()
{
  if (itterator)
  {
    increment();
    return false;
  }
  increment();
  return true;
}

void ExpFallback::increment()
{
  itterator++;
  if (itterator > fallback)
  {

    fallback = fallback * 2;
    if (fallback > maxval)
      fallback = maxval;
    itterator = 0;
    return;
  }
}