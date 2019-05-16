TARGET = server

CC = g++
CPP = g++
LINK = g++

CCFLAGS = -O2 -Wall
CPPFLAGS = -O2 -Wall

INCLUDES = -I../libsrc

LIBS = -lpthread -lrt


CFILES = $(wildcard *.c) \
	./../libsrc/remsgque.c \
	./../libsrc/msgsocket.c \
	./../libsrc/reque.c \
	./../libsrc/mempool.c 

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
