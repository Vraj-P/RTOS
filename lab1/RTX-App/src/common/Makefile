CC = gcc 
CFLAGS = -Wall -std=gnu99 -g # "curl-config --cflags" output is empty  
LD = gcc
LDFLAGS = -std=gnu99 -g 

SRCS   = list_test.c freelist.c
OBJS1  = list_test.o freelist.o
TARGETS= list_test

all: ${TARGETS}

list_test: $(OBJS1) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *~ *.d *.o $(TARGETS) *.png