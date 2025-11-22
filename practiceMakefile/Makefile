CC = gcc
CFLAGS = -Wall -g
OBJS = main.o hello.o
TARGET = myapp

$(TARGET): $(OBJS)
	@echo "ターゲット(\$@) は: $@"
	@echo "全部の材料(\$^) は: $^"
	$(CC) -o $@ $^

%.o: %.c
	@echo "ターゲット(\$@) は: $@"
	@echo "最初の材料(\$<) は: $<"
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)