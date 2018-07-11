SOURCE = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SOURCE))
TARGER = get_interface

CFLAGS = -g -O0 -Wall


all:$(TARGER)
$(TARGER):$(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -rf *.o $(TARGER)
