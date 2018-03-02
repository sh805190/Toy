#Windows 7:
#RM=del /y

#Windows 8.1:
#RM=del /S

OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C src

tool: $(OUTDIR)
	$(MAKE) -C tool

debug: clean all

release: clean all package

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
ifeq ($(OS),Windows_NT)
	del /S /Q *.o *.a *.exe $(OUTDIR)\*.class
#	rmdir /S /Q $(OUTDIR)
else ifeq ($(shell uname), Linux)
	find . -type f -name '*.class' -exec rm -f -r -v {} \;
	find . -type f -name '*.jar' -exec rm -f -r -v {} \;
#	rm $(OUTDIR)/* -f
	find . -empty -type d -delete
endif

rebuild: clean all
