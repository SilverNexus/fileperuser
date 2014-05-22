CC     = gcc -std=gnu99
OBJ    = ErrorLog.o main.o parseArgs.o settings.o dir_list.o
BIN    = FilePeruser
CFLAGS = -O3 -Wall -Wextra -Wmain
RM     = rm -f

.PHONY: all all-before all-after clean clean-custom cleanall

all: all-before FilePeruser all-after

cleanall: clean

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "$(BIN)"

ErrorLog.o: ErrorLog.c ErrorLog.h
	$(CC) -c ErrorLog.c -o ErrorLog.o $(CFLAGS)

main.o: main.c main.h parseArgs.h ErrorLog.h
	$(CC) -c main.c -o main.o $(CFLAGS)

parseArgs.o: parseArgs.c parseArgs.h settings.h ErrorLog.h
	$(CC) -c parseArgs.c -o parseArgs.o $(CFLAGS)

settings.o: settings.c settings.h ErrorLog.h
	$(CC) -c settings.c -o settings.o $(CFLAGS)

dir_list.o: dir_list.c dir_list.h
	$(CC) -c dir_list.c -o dir_list.o $(CFLAGS)
