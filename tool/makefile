#output
OBJDIR=obj
OUTDIR=../out
OUT=GenerateAst.jar

#targets
all: $(OBJDIR)
	javac -d $(OBJDIR) *.java
	jar cfm $(OUTDIR)/$(OUT) manifest.txt -C $(OBJDIR) .

$(OBJDIR):
	mkdir $(OBJDIR)

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
