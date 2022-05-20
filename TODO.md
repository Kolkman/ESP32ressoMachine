# TODO and known bugs

* Most of the code has be refactored to be object orriented. The OO design could do with a review.
* The simmulation code has not been integratd or tested. Behavior is unpredicted.
* More and better error/bounds checking. At this point **DO NOT RUN ON NETWORKS WITH UNTRUSTED ENTITIES**
* Configuration page rewrite needs finalizing
* Cleanup, removing commented out code and stale pieces.
* WIFI managment: password management and act as basestation when WIFI fails so reconfiguration can happen.
* Carefully go over the use of the String object - can/should char * be used?
* WebAPI crashes server - needs fixing.
* TuningMode is now broken.
* Make MQTT topic configurable 
* timeseries management after PidInterval changed (graph flows over to the left or doesn't occupy whole with.)


