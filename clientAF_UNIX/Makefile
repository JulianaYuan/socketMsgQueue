TARGET = client

CC = g++
CPP = g++
LINK = g++

CCFLAGS = -O2 -Wall
CPPFLAGS = -O2 -Wall

INCLUDES = -I../libsrc/

LIBS = -L../libsrc -lpthread -lskmsq


CFILES = $(wildcard *.c) 

CPPFILES = $(wildcard *.cpp)

OBJFILE = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

all:$(TARGET)
$(TARGET):$(OBJFILE)
	$(LINK) $^ $(LIBS) -o $@

%.o:%.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@
 
%.o:%.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)

