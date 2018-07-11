server:local_addr.o server.o
	gcc -o $@ $^ -levent

client:client.o local_addr.o
	gcc -o $@ $^

%.o:%.c
	gcc -o $@  -c $< -levent 

clean:
	rm -rf client.o local_addr.o server.o  server client
