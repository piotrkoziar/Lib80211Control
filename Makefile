CC ?= "g++"
PKG_CONFIG ?= pkg-config
NLLIBNAME = libnl-3.0

CFLAGS ?= -O0 -g
CFLAGS += -Wall -Wextra -Wundef -Wno-trigraphs -fno-strict-aliasing -fno-common \
	  -Werror-implicit-function-declaration -Wsign-compare -Wno-unused-parameter
CFLAGS += -DCONFIG_LIBNL30
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(NLLIBNAME))
# debug level
# 0 [Debugging disabled] ... 5 [less important messages]
CFLAGS += -DNLDBG=4 -DNLCB=debug

LIBS += -lnl-genl-3
LIBS += $(shell $(PKG_CONFIG) --libs $(NLLIBNAME))
LIBS += -lstdc++

#
TARGET1 = mynl

# directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

INCLUDES1 := $(wildcard $(SRCDIR)/*.h)
SOURCES1 := $(wildcard $(SRCDIR)/*.cpp)
# OBJECTS1 := $(patsubst %.cpp, %.o, $(SOURCES1))
OBJECTS1 := $(SOURCES1:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# rule for mynl
$(BINDIR)/$(TARGET1): $(OBJECTS1)
	@echo LINKER
	@echo $(OBJECTS1)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# rule for objects (.o)
$(OBJECTS1):$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDES1)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(TARGET1)
