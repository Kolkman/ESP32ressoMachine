
all:include/pages/index.html.h include/pages/update.html.h  \
	include/pages/button.css.h include/pages/ESPresso.css.h \
	include/pages/gauge.min.js.h include/pages/EspressoMachine.svg.h\
	include/pages/test.html.h include/pages/test2.html.h \
	include/pages/drawtimeseries.js.h	include/pages/firmware.js.h \
	include/pages/configuration.html.h 
	@exec true 



include/pages/test.html.h: WEBSources/test.html 
	@./ProduceDefine.py $< text/html  > $@

include/pages/test2.html.h: WEBSources/test2.html 
	@./ProduceDefine.py $< text/html  > $@

include/pages/index.html.h: WEBSources/index.html 
	@./ProduceDefine.py $< text/html  > $@

include/pages/configuration.html.h: WEBSources/configuration.html 
	@./ProduceDefine.py $< text/html  > $@

include/pages/button.css.h: WEBsources/button.css
	@./ProduceDefine.py $< text/css  > $@

include/pages/ESPresso.css.h: WEBsources/ESPresso.css
	@./ProduceDefine.py $< text/css  > $@

include/pages/gauge.min.js.h: WEBSources/gauge.min.js
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/drawtimeseries.js.h: WEBSources/drawtimeseries.js
	@./ProduceDefine.py $< text/javascript  > $@

include/pages/firmware.js.h: WEBSources/firmware.js
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/EspressoMachine.svg.h: WEBSources/EspressoMachine.svg
	@./ProduceDefine.py $< image/svg+xml  > $@


include/pages/update.html.h: WEBsources/update.html
	@./ProduceDefine.py $< text/html  > $@


clean:
	rm include/pages/*

