CC ?= "gcc"
PKG_CONFIG ?= pkg-config
NLLIBNAME = libnl-3.0

CFLAGS ?= -O2 -g
CFLAGS += -Wall -Wextra -Wundef -Wno-trigraphs -fno-strict-aliasing -fno-common \
	  -Werror-implicit-function-declaration -Wsign-compare -Wno-unused-parameter
CFLAGS += -DCONFIG_LIBNL30
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(NLLIBNAME))
# debug level
# 0 [Debugging disabled] ... 5 [less important messages]
CFLAGS += -DNLDBG=4

LIBS += -lnl-genl-3
LIBS += $(shell $(PKG_CONFIG) --libs $(NLLIBNAME))

# dependencies (header files)
DEPS1 =

# objects we want to build
OBJS1 := mynl.o

#
TARGET1 = mynl

# create objects (.o)
%.o: %.cpp $(DEPS1)
	$(CC) -c -o $@ $< $(CFLAGS)

# build from objects
$(TARGET1): $(OBJS1)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o $(TARGET1)

# but you can use this as well:
# sudo gcc mynl.cpp -o mynl $(pkg-config --cflags --libs libnl-3.0)