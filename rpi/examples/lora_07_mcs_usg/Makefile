
CROSS_COMPILER_PATH=YOURPATH/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
CROSS_COMPILER=${CROSS_COMPILER_PATH}/arm-linux-gnueabihf-

export CC=${CROSS_COMPILER}gcc
export AS=${CROSS_COMPILER}as
export CXX=${CROSS_COMPILER}g++
export AR=${CROSS_COMPILER}ar
export LD=${CROSS_COMPILER}ld
export OBJCOPY=${CROSS_COMPILER}objcopy
export RANLIB=${CROSS_COMPILER}ranlib
export STRIP=${CROSS_COMPILER}strip

LORA_DIR = ../../../
RPI_DIR  = ../../

LORA_SRC = $(wildcard $(LORA_DIR)*.cpp)
RPI_SRC = $(wildcard $(RPI_DIR)*.cpp)

SRC = main.cpp
SRC += $(LORA_SRC)
SRC += $(RPI_SRC)

OBJS = $(patsubst %.cpp, %.o, $(SRC))

LORA_HDR = $(wildcard $(LORA_DIR)*.h)
RPI_HDR = $(wildcard $(RPI_DIR)*.h)

HEADERS = $(LORA_HDR)
HEADERS += $(RPI_HDR)

EXE=ex7McsUsg

CFLAGS:= -fPIC -D__RASPBERRYPI__ -I$(RPI_DIR) -I$(RPI_DIR)include -I$(LORA_DIR) -I/usr/local/include 
LDFLAGS:= -L/usr/local/lib -L$(RPI_DIR)lib -lwiringPi -lpthread -lssl -lcrypto

all: $(OBJS) $(HEADERS) Makefile
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS)

$(OBJS): %.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm *.[oa] $(EXE) $(RPI_DIR)*.o $(LORA_DIR)*.o

