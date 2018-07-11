TARGET:=netdet_server
#CC:=mipsel-openwrt-linux-gcc

INCLUDE_PATH:=/opt/toolchan/target/usr/include
LIBRARY_PATH:=/opt/toolchan/target/usr/lib
INCLUDE_FLAG:=-I$(INCLUDE_PATH)
OPENWRT_LD_FLAGS:= -L$(LIBRARY_PATH) -levent -lm
#-levent-core
LD_FLAGS:= -levent -lm

#OBJECTS:=$(patsubst %.c, %.o, $(wildcard *.c))BJECTS:=server.o cjson.o
OBJECTS:=server.o 
$(TARGET):$(OBJECTS)
ifeq ("$(OPENWRT)", "1")
	$(CC) -o $@ $^ $(OPENWRT_LD_FLAGS) $(INCLUDE_FLAG)
else
	gcc -o $@ $^ $(LD_FLAGS)
endif


clean:
	rm -rf tags  $(TARGET) $(OBJECTS) 
