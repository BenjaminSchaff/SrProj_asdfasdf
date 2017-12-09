TARGET=sim_ui
OBJS=sensors.o sim_ui.o ui.o

CC=gcc

CFLAGS=-Wall -g

all:$(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)


clean:
	rm -f $(TARGET) $(OBJS)
