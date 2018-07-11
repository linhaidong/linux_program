container:local_addr.o container.o
	gcc -o $@ $^ -levent

hello:hello.o local_addr.o
	gcc -o $@ $^

%.o:%.c
	gcc -o $@  -c $< -levent 

clean:
	rm -rf hello.o local_addr.o container.o
