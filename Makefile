CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

SRCS = main.c screen.c keyboard.c timer.c
OBJS = $(SRCS:.c=.o)
EXEC = arkanoid

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
		$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		$(RM) $(OBJS) $(EXEC)
