TARGET = libskmsq.so

CC = g++
CPP = g++
LINK = g++

CCFLAGS = -O2 -W -Wall -fPIC
CPPFLAGS = -O2 -W -Wall -fPIC

INCLUDES = -I./../libsrc/

LIBS =


CFILES = ./../libsrc/remsg.c \
	./../libsrc/msgsocket.c

CPPFILES = 

OBJFILE = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

all:$(TARGET)
$(TARGET):$(OBJFILE)
	$(AR) cq $@ $^

%.o:%.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(LIBS) -c $<

%.o:%.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(LIBS) -c $<

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)


