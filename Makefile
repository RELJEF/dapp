
.SUFFIXES:
.SUFFIXES: .c .o

GCC_DIR =  ~/ti/gcc/bin
SUPPORT_FILE_DIRECTORY = ~/ti/gcc/include
OBJDIR=obj
vpath %.c src
vpath %.h src

OBJECTS = $(addprefix $(OBJDIR)/, main.o rf.o)

DEVICE  = cc430f5137
CC      = $(GCC_DIR)/msp430-elf-gcc

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O2 -g
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -T $(DEVICE).ld


all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(DEVICE).out

$(OBJDIR) obj/%.o : %.c rf.h
	@mkdir -p obj
	@echo $<
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJECTS)
	rm -f $(DEVICE).out
