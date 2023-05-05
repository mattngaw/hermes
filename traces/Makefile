.PHONY: all build clean

PINTOOL := ../pin/obj-intel64/pintool.so
TRACE_SCRIPT := trace.py

CC := gcc
CFLAGS := -Wall -pthread
C_SOURCES := $(wildcard *.c)
C_EXECUTABLE := $(C_SOURCES:.c=)
C_TRACES := $(C_SOURCES:.c=.trace)
C_TRACE := $(addsuffix _traces, $(C_EXECUTABLE))
 
all: build pin traces

build: $(C_EXECUTABLE)

pin: $(C_TRACES)

traces: $(C_TRACE)

$(addsuffix _traces, %): %.trace
	python3 $(TRACE_SCRIPT) $< &> /dev/null

%.trace: %
	pin -t $(PINTOOL) -o $@ -- ./$< &> /dev/null

%: %.c
	$(CC) $< $(CFLAGS) -o $@ &> /dev/null

clean:
	rm -rf $(C_EXECUTABLE) $(C_TRACES) $(C_TRACE)