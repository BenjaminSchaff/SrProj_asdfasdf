TARGET=fat_test
OBJS=fat_test.o f16_fs.o 

CC=gcc

CFLAGS=-Wall -g

all:$(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)


clean:
	rm -f $(TARGET) $(OBJS)
