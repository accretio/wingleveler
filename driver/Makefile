IDIR =../include
CC=cc
CFLAGS=-I$(IDIR) -DMPU9250 -DRASPBERRY_PI

ODIR=obj
LDIR =../lib

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  linux_glue.o inv_mpu.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

wingleveler: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lm -lpigpio -lrt

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ wingleveler


send:
	rsync -a . pi@10.0.0.247:/tmp/wingleveler
