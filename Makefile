PROJECT=cs540p2_myakal_purva

SRC_FILES= Map.hpp README.txt Makefile

submit:	$(SRC_FILES)
	cp $(SRC_FILES) $(PROJECT)/
	tar -cvf $(PROJECT).tar $(PROJECT)
	gzip $(PROJECT).tar

clean:
	rm -f *~
