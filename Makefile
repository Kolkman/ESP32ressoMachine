
all: include/webpages.h include/pages/test_html.h include/pages/update_html.h 
	@exec true 



include/webpages.h: include/pages/button_css.h include/pages/index_html.h  include/pages/gauge_min_js.h include/pages/ESPresso_css.h include/pages/EspressoMachine_svg.h
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
	@echo "#ifndef PAGES_TEST_HEADER" > $@
	@echo "#define PAGES_TEST_HEADER" >> $@
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz >> $@  
	@echo "#endif" >> $@
	@rm $<.gz


include/pages/update_html.h: WEBsources/update.html 
	@echo "#ifndef PAGES_UPDATE_HEADER" > $@
	@echo "#define PAGES_UPDATE_HEADER" >> $@
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz >> $@  
	@echo "#endif" >> $@
	@rm $<.gz



include/pages/EspressoMachine_svg.h: WEBsources/EspressoMachine.svg 
	@echo "#ifndef PAGES_ESPSVG_HEADER" > $@
	@echo "#define PAGES_ESPSVG_HEADER" >> $@
	@cat $< | gzip -c > $<.gz &&  xxd -i $<.gz >> $@  
	@echo "#endif" >> $@
	@rm $<.gz



clean:
	rm include/pages/*
	rm include/webpages.h
