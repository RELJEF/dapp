
.SUFFIXES:
.SUFFIXES: .c .o

GCC_DIR =  ~/ti/gcc/bin
SUPPORT_FILE_DIRECTORY = ~/ti/gcc/include
OBJDIR=obj
vpath %.c src
vpath %.h src

OBJECTS = $(addprefix $(OBJDIR)/, main.o rf.o rf1a.o board.o pmm.o adpcm.o)

DEVICE  = cc430f5137
CC      = $(GCC_DIR)/msp430-elf-gcc

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O2 -g -Wa,-a,-ad
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -T $(DEVICE).ld


all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(DEVICE).out

$(OBJDIR) obj/%.o : %.c rf.h rf1a.h board.h main.h smartrf.h pmm.h adpcm.h
	@mkdir -p $(OBJDIR)
	@echo $<
	$(CC) $(CFLAGS) -c $< -o $@ > $*.lst

.PHONY: clean

clean:
	rm -f *.lst
	rm -f $(OBJECTS)
	rm -f $(DEVICE).out
