CC = gcc
CFLAGS = -g -MD -MP
LFLAGS = -g

sources = $(wildcard *.c)

all: explicit-control

explicit-control: explicit-control.o hash.o
	$(CC) $(LFLAGS) -o $@ $^

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.c %.d
	$(CC) $(CFLAGS) -c $^

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

-include $(sources:.c=.d)

