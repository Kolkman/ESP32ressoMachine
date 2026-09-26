
###
### Make to manipulate webpages.
### Note that code relies on MinifyAll extension to first minify the raw source

include/pages:
	@mkdir -p $@

all: include/pages include/pages/index.html.h include/pages/update.html.h  \
	include/pages/switch.css.h include/pages/ESPresso.css.h \
	include/pages/gauge.min.js.h include/pages/EspressoMachine.svg.h\
	include/pages/drawtimeseries.js.h	include/pages/firmware.js.h \
	include/pages/configuration.html.h include/pages/configuration_helper.js.h \
	include/pages/index_helper.js.h include/pages/ESPconfig.css.h \
	include/pages/networkSetup.html.h \
	include/pages/captivePortal.html.h \
	include/pages/configDone.html.h \
	include/pages/networkConfigPage.js.h \
	include/pages/WebLogin.html.h \
	include/pages/redCircleCrossed.svg.h
	@exec true 





include/pages/test2.html.h: WEBsources/test2.html | include/pages
	@./ProduceDefine.py $< text/html this > $@



include/pages/WebLogin.html.h: WEBsources/WebLogin.html | include/pages
	@./ProduceDefine.py $< text/html serverscope > $@

include/pages/index.html.h: WEBsources/index.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8  > $@


include/pages/configuration.html.h: WEBsources/configuration-min.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8  > $@

include/pages/switch.css.h: WEBsources/switch-min.css | include/pages
	@./ProduceDefine.py $< text/css serverscope > $@

include/pages/ESPresso.css.h: WEBsources/ESPresso-min.css | include/pages
	@./ProduceDefine.py $< text/css serverscope  > $@


include/pages/index_helper.js.h: WEBsources/index_helper-min.js | include/pages
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/networkConfigPage.js.h: WEBsources/networkConfigPage-min.js | include/pages
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/configuration_helper.js.h: WEBsources/configuration_helper-min.js | include/pages
	@./ProduceDefine.py $< text/javascript  > $@

include/pages/gauge.min.js.h: WEBsources/gauge.min.js | include/pages
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/drawtimeseries.js.h: WEBsources/drawtimeseries-min.js | include/pages
	@./ProduceDefine.py $< text/javascript  > $@

include/pages/firmware.js.h: WEBsources/firmware-min.js | include/pages
	@./ProduceDefine.py $< text/javascript serverscope > $@


include/pages/EspressoMachine.svg.h: WEBsources/EspressoMachine.svg | include/pages
	@./ProduceDefine.py $< image/svg+xml serverscope > $@

include/pages/redCircleCrossed.svg.h: WEBsources/redCircleCrossed.svg | include/pages
	@./ProduceDefine.py $< image/svg+xml serverscope > $@


include/pages/captivePortal.html.h: WEBsources/captivePortal.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/networkSetup.html.h: WEBsources/networkSetup.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/update.html.h: WEBsources/update.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/configDone.html.h: WEBsources/configDone.html | include/pages
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/ESPconfig.css.h: WEBsources/ESPconfig-min.css | include/pages
	@./ProduceDefine2.py $<  > $@




clean:
	rm include/pages/*

