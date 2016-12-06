SOURCE = main.c functions.c
OBJS = main.o functions.o 
HEADER = header.h
OUT = main
CC = gcc
FLAGS = -c -w

all: $(OBJS)
	$(CC) -g $(OBJS) -L . -static -o $(OUT)

main.o : main.c
	$(CC) $(FLAGS) main.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean :
	rm -f $(OBJS) $(OUT)