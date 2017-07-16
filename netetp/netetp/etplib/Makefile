
include ../make.include

CPPFLAGS  =   -ggdb -DCORE_DUMP  -O2 -pthread -Wall

CPPFLAGS += -I./ -I./src/ -I../

CPPFLAGS += -rdynamic -fPIC
LDFLAGS += -lpthread 

SOURCES = $(wildcard ./*.cpp ./*.c ./src/*.c ./src/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS = $(patsubst %.cpp,%.d,$(SOURCES))
ASMFILE = $(patsubst %.cpp,%.s,$(SOURCES))

.PHONY: all clean

target = libetp.so
all: $(target)

$(target): $(OBJECTS)	
	g++ -shared -o $(target)  $(OBJECTS)  $(LDFLAGS)
	mv $(target) $(LIB_LOCATION)
clean:
	@rm -fr $(OBJECTS) $(DEPENDS) $(ASMFILE)
	@rm -fr *.d *.o *.s 
	@rm $(LIB_LOCATION)/$(target)

install:
uninstall:
