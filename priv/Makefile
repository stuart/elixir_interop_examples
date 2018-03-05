EIDIR=`elixir script/erlang_config.exs ei`
ERTSDIR=`elixir script/erlang_config.exs erts`

PGLIBS=-L$(shell pg_config --libdir)
PGINC=-I$(shell pg_config --includedir)

CC=gcc
SRCDIR=./src
BINDIR=./bin
LIBS=-lgsl -lerl_interface -lei -lerts
CPPFLAGS=-I./include -I$(EIDIR)/include -I$(ERTSDIR)/include -Wall -Wmissing-prototypes -Wstrict-prototypes
LFLAGS=-L$(EIDIR)/lib -L$(ERTSDIR)/lib -L$(EIDIR)/lib
SOFLAGS=-fPIC -shared -flat_namespace -undefined suppress

_OBJECTS=bessel polynomial example_drv.so complex_nif.so pg_async.so
OBJECTS = $(patsubst %,$(BINDIR)/%,$(_OBJECTS))

all: $(OBJECTS)

bin/bessel: $(SRCDIR)/port.c $(SRCDIR)/erl_comm.c
	$(CC) -o $(BINDIR)/bessel $(SRCDIR)/erl_comm.c $(SRCDIR)/port.c  $(CPPFLAGS) $(LFLAGS) $(LIBS)

bin/polynomial: $(SRCDIR)/eiport.c $(SRCDIR)/erl_comm.c
	$(CC) -o $(BINDIR)/polynomial $(SRCDIR)/erl_comm.c $(SRCDIR)/eiport.c $(CPPFLAGS) $(LFLAGS) $(LIBS)

bin/example_drv.so: $(SRCDIR)/port_driver.c
	$(CC) -o $(BINDIR)/example_drv.so $(SRCDIR)/port_driver.c $(CPPFLAGS) $(LFLAGS) $(LIBS) $(SOFLAGS)

bin/complex_nif.so: $(SRCDIR)/nif.c
	$(CC) -o $(BINDIR)/complex_nif.so $(SRCDIR)/nif.c $(CPPFLAGS) $(LFLAGS) $(LIBS) $(SOFLAGS)

bin/pg_async.so: $(SRCDIR)/pg_async.c $(SRCDIR)/pg_encode.c
	$(CC) -o $(BINDIR)/pg_async.so $(SRCDIR)/pg_async.c $(SRCDIR)/pg_encode.c $(CPPFLAGS) $(PGINC) $(LFLAGS) $(PGLIBS) $(LIBS) -lpq $(SOFLAGS)

clean:
	rm $(OBJECTS)

.PHONY: all clean
