CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = banker

all: $(TARGET)

$(TARGET): banker.c
	$(CC) $(CFLAGS) banker.c -o $(TARGET)

clean:
	rm -f $(TARGET) *.o result.txt

.PHONY: all clean