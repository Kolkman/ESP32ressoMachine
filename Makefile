
all: include/webpages.h
	@exec true 



include/webpages.h: include/pages/button_css.h include/pages/index_html.h include/pages/test_html.h  include/pages/gauge_min_js.h include/pages/ESPresso_css.h
	@echo "#ifndef PAGES_HEADER" > $@
	@echo "#define PAGES_HEADER" >> $@
	@cat $^ >> $@
	@echo "#endif" >> $@
	



include/pages/button_css.h: WEBsources/button.css
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz > $@   && rm $<.gz

include/pages/index_html.h: WEBsources/index.html
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz > $@   && rm $<.gz

include/pages/gauge_min_js.h: WEBsources/gauge.min.js
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz > $@   && rm $<.gz

include/pages/ESPresso_css.h: WEBsources/ESPresso.css
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz > $@   && rm $<.gz

include/pages/test_html.h: WEBsources/test.html
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz > $@   && rm $<.gz




clean:
	rm include/pages/*
	rm include/webpages.h
