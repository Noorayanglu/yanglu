PROG ?= ./meter       # Program we are building
PACK ?= ./pack          # Packing executable
DELETE = rm -rf         # Command to remove files
GZIP ?= gzip            # For compressing files in web_root/
OUT ?= -o $(PROG)       # Compiler argument for output file
SOURCES = main.c mongoose.c net.c appl.c\
		./libmodbus/src/modbus.c \
		./libmodbus/src/modbus-data.c \
		./libmodbus/src/modbus-rtu.c \
		./libmodbus/src/modbus-tcp.c \
		./paho.mqtt.c-1.3.9/src/Base64.c \
		./paho.mqtt.c-1.3.9/src/Clients.c \
		./paho.mqtt.c-1.3.9/src/Heap.c \
		./paho.mqtt.c-1.3.9/src/LinkedList.c \
		./paho.mqtt.c-1.3.9/src/Log.c \
		./paho.mqtt.c-1.3.9/src/Messages.c \
		./paho.mqtt.c-1.3.9/src/MQTTClient.c \
		./paho.mqtt.c-1.3.9/src/MQTTPacket.c \
		./paho.mqtt.c-1.3.9/src/MQTTPacketOut.c \
		./paho.mqtt.c-1.3.9/src/MQTTPersistence.c \
		./paho.mqtt.c-1.3.9/src/MQTTPersistenceDefault.c \
		./paho.mqtt.c-1.3.9/src/MQTTProperties.c \
		./paho.mqtt.c-1.3.9/src/MQTTProtocolClient.c \
		./paho.mqtt.c-1.3.9/src/MQTTProtocolOut.c \
		./paho.mqtt.c-1.3.9/src/MQTTReasonCodes.c \
		./paho.mqtt.c-1.3.9/src/MQTTTime.c \
		./paho.mqtt.c-1.3.9/src/OsWrapper.c \
		./paho.mqtt.c-1.3.9/src/SHA1.c \
		./paho.mqtt.c-1.3.9/src/Socket.c \
		./paho.mqtt.c-1.3.9/src/SocketBuffer.c \
		./paho.mqtt.c-1.3.9/src/SSLSocket.c \
		./paho.mqtt.c-1.3.9/src/StackTrace.c \
		./paho.mqtt.c-1.3.9/src/Thread.c \
		./paho.mqtt.c-1.3.9/src/Tree.c \
		./paho.mqtt.c-1.3.9/src/utf-8.c \
		./paho.mqtt.c-1.3.9/src/WebSocket.c
CFLAGS = -w -g -I. -I./libmodbus -I./libmodbus/src -I./paho.mqtt.c-1.3.9/src
CFLAGS_EXTRA = -lpthread
NPX ?= npx
# em-500
CC=arm-linux-gnueabihf-gcc
# HTNICE GT6517
# CC=arm-linux-gcc -std=c99
# HTNICE GT6757
#CC=aarch64-linux-gcc

# Mongoose build options. See https://mongoose.ws/documentation/#build-options
CFLAGS_MONGOOSE += -DMG_ENABLE_PACKED_FS=0

ifeq ($(OS),Windows_NT)         # Windows settings. Assume MinGW compiler. To use VC: make CC=cl CFLAGS=/MD OUT=/Feprog.exe
  PROG = meter.exe            # Use .exe suffix for the binary
  PACK = pack.exe               # Packing executable
  CC = gcc                      # Use MinGW gcc compiler
  CFLAGS += -lws2_32            # Link against Winsock library
  DELETE = cmd /C del /Q /F /S  # Command prompt command to delete files
  GZIP = echo                   # No gzip on Windows
endif

# Default target. Build and run program
all: $(PROG)
#	$(RUN) $(PROG) $(ARGS)

# Build program from sources
$(PROG): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(CFLAGS_MONGOOSE) $(CFLAGS_EXTRA) $(OUT)

# Cleanup. Delete built program and all build artifacts
clean:
	$(DELETE) $(PROG) $(PACK) *.o *.obj *.exe *.dSYM mbedtls
