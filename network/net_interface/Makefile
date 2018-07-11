TARGET:=netspace

COMPILE_FLAGS:= -g -Wall -O3
OBJECT:=$(patsubst %.c, %.o, $(wildcard *.c))

$(TARGET):$(OBJECT)
	gcc -o $@ $^ $(COMPILE_FLAGS)

clean:
	rm -rf $(TARGET) $(OBJECT)

%.o:%.c
	$(CC) -o $@ -c $<  $(COMPILE_FLAGS) 
