# TODO

* Most of the code has be refactored to be object orriented. The OO design could do with a review.
* The simmulation code has not been integratd or tested. Behavior is unpredicted.
* More and better error/bounds checking. At this point **DO NOT RUN ON NETWORKS WITH UNTRUSTED ENTITIES**
* Configuration page rewrite.
* Cleanup, removing commented out code and stale pieces.
* WIFI managment: password management and act as basestation when WIFI fails so reconfiguration can happen.
* Redo the updater (AsyncElegantOTA is nice, but I want better visual integration) (see e.g. https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino)