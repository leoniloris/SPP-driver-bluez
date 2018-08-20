CC = gcc
CC_FLAGS = -shared -fPIC -I/usr/include/python3.6 \
	-Wall -Wextra -Wno-unused-parameter -O2 -g

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.so)

all: $(OBJECTS)

%.so: %.c
	$(CC) $(CC_FLAGS) $< -o $@

test: $(OBJECTS)
	python3.6 -c "import ble_serial; \
				  import numpy as np; \
				  print(ble_serial_module.notify(np.array((1, 2, 3, 4), dtype=np.dtype(>B))))"

# To remove generated files
clean:
	rm -f *.so

.PHONY: all run clean
