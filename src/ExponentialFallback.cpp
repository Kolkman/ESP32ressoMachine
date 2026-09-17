#include "ExponentialFallback.h"
#include "Arduino.h"

ExpFallback::ExpFallback(unsigned long _maxInterval, unsigned long _baseInterval)
{
  lastAttemptMillis = 0;
  maxInterval = _maxInterval;
  baseInterval = _baseInterval;
  interval = baseInterval;
  everAttempted = false;
}

void ExpFallback::hadSuccess(bool reset)
{
  if (reset)
  {
    interval = baseInterval;
  }
  return;
}

void ExpFallback::hadFailure()
{
  lastAttemptMillis = millis();
  interval *= 2;
  if (interval > maxInterval)
    interval = maxInterval;
}

bool ExpFallback::mustAttempt()
{
  if (!everAttempted)
  {
    everAttempted = true;
    return true;
  }
  return (millis() - lastAttemptMillis) >= interval;
}