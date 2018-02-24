#config
GCC=gcc

#include directories
INCLUDES+=

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=

#flags
CFLAGS+=$(addprefix -I,$(INCLUDES))

#source
CSRC=$(wildcard *.c)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CSRC:.c=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,toybox)

#targets
all: $(OBJ) $(OUT)
	$(GCC) $(CFLAGS) -o $(OUT) $(OBJ) $(LIBS)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.c
	$(GCC) $(CFLAGS) -c -o $@ $<

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o *.a *.exe
else ifeq ($(shell uname), Linux)
	find . -type f -name *.o -exec rm -f -r -v {} \;
	find . -type f -name *.a -exec rm -f -r -v {} \;
	rm -f -v out/client out/server
endif

rebuild: clean all
