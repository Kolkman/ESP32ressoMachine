#ifndef EXPOFALLBACK_H
#define EXPOFALLBACK_H

#define DEFAULT_MAXVAL 0x800


/**
 * @brief Class that keeps state for exponential fallback.
 *
 */
class ExpFallback

{
public:
    /**
     * @brief Construct a new Exp Fallback object
     *
     * @param maxval exponential fallback not bigger than this value (default 0xFFF)
     */
    ExpFallback(unsigned int maxval=DEFAULT_MAXVAL);
    /**
     * @brief Indicates if the internal state is in fallback mode.
     *
     * @param reset  If true: resets all counters and the function returns true,
     *            if false the output will reflect the internal state.
     * @return true
     * @return false
     */
    void hadSuccess(bool reset);
    /**
     * @brief suggest whether a function in exp fall back should attempt of wait
     * 
     */
    bool mustAttempt();


private:
    unsigned int itterator;
    unsigned int fallback;
    unsigned int maxval;
    void increment();
};

#endif