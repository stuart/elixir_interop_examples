EIDIR=`elixir ../script/erlang_config.exs ei`
ERTSDIR=`elixir ../script/erlang_config.exs erts`

PGLIBS=-L$(shell pg_config --libdir)
PGINC=-I$(shell pg_config --includedir)

CC=gcc
BINDIR=../bin
LIBS=-lgsl -lerl_interface -lei -lerts
CPPFLAGS=-I../include -I$(EIDIR)/include -I$(ERTSDIR)/include -Wall -Wmissing-prototypes -Wstrict-prototypes
LFLAGS=-L$(EIDIR)/lib -L$(ERTSDIR)/lib -L$(EIDIR)/lib
SOFLAGS=-fPIC -shared -flat_namespace -undefined suppress

_OBJECTS=bessel polynomial example_drv.so complex_nif.so pg_async.so cnode
OBJECTS = $(patsubst %,$(BINDIR)/%,$(_OBJECTS))

all: $(OBJECTS)

$(BINDIR)/bessel: port.c erl_comm.c
	$(CC) -o $(BINDIR)/bessel erl_comm.c port.c  $(CPPFLAGS) $(LFLAGS) $(LIBS)

$(BINDIR)/polynomial: eiport.c erl_comm.c
	$(CC) -o $(BINDIR)/polynomial erl_comm.c eiport.c $(CPPFLAGS) $(LFLAGS) $(LIBS)

$(BINDIR)/example_drv.so: port_driver.c
	$(CC) -o $(BINDIR)/example_drv.so port_driver.c $(CPPFLAGS) $(LFLAGS) $(LIBS) $(SOFLAGS)

$(BINDIR)/complex_nif.so: nif.c
	$(CC) -o $(BINDIR)/complex_nif.so nif.c $(CPPFLAGS) $(LFLAGS) $(LIBS) $(SOFLAGS)

$(BINDIR)/pg_async.so: pg_async.c pg_encode.c
	$(CC) -o $(BINDIR)/pg_async.so pg_async.c pg_encode.c $(CPPFLAGS) $(PGINC) $(LFLAGS) $(PGLIBS) $(LIBS) -lpq $(SOFLAGS)

$(BINDIR)/cnode: node.c
	$(CC) -o $(BINDIR)/cnode node.c $(CPPFLAGS) $(LFLAGS) $(LIBS)

clean:
	rm $(OBJECTS)

.PHONY: all clean
