TARGET = dev

# Compiler options.
CC_DEV = g++
CC_ARM = /usr/bin/arm-linux-gnueabihf-g++

# Header includes.
INCLUDEPATH = -Idkm/include -Istb

# Flags.
CFLAGS = -Wall -static -O3 -std=c++14 


# Source directory and files.
SOURCEDIR = src
HEADERS := $(wildcard $(SOURCEDIR)/*.hpp)
SOURCES := $(wildcard $(SOURCEDIR)/*.cpp)

# Target output.
BUILDTARGET = image_segmentation

# Target compiler environment.
ifeq ($(TARGET),arm)
	CC = $(CC_ARM)
else
	CC = $(CC_DEV)
endif

all:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(HEADERS) $(SOURCES) -o $(BUILDTARGET)

clean:
	rm -f $(SOURCEDIR)/*.o
	rm -f $(BUILDTARGET)