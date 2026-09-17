#ifndef EXPOFALLBACK_H
#define EXPOFALLBACK_H

// Default cap on the retry interval: 5 minutes.
#define DEFAULT_MAXINTERVAL 300000UL
// Interval used for the first retry after a failure.
#define DEFAULT_BASEINTERVAL 5000UL


/**
 * @brief Class that keeps time based state for exponential fallback.
 *
 * Tracks the time of the last attempt and a retry interval that doubles
 * on every failure (capped at maxInterval) and resets on success.
 */
class ExpFallback

{
public:
    /**
     * @brief Construct a new Exp Fallback object
     *
     * @param maxInterval retry interval will never grow beyond this value, in milliseconds (default 5 minutes)
     * @param baseInterval retry interval used right after the first failure, in milliseconds (default 5 seconds)
     */
    ExpFallback(unsigned long maxInterval = DEFAULT_MAXINTERVAL,
               unsigned long baseInterval = DEFAULT_BASEINTERVAL);
    /**
     * @brief Call after a successful attempt, resets the backoff interval.
     *
     * @param reset If true, resets the backoff interval to baseInterval.
     */
    void hadSuccess(bool reset);
    /**
     * @brief Call after a failed attempt, doubles the backoff interval (capped at maxInterval)
     * and records the time of the failure.
     */
    void hadFailure();
    /**
     * @brief Indicates whether enough time has elapsed since the last attempt
     * to allow a new attempt.
     */
    bool mustAttempt();


private:
    unsigned long lastAttemptMillis;
    unsigned long interval;
    unsigned long maxInterval;
    unsigned long baseInterval;
    bool everAttempted;
};

#endif