COMPILER	= gcc
CFLAGS		= -std=c99 -Wall -fopenmp -pg
LIBS		= -lm
OBJ		= nqueens
TARGET		= a.out

.PHONY: all

all:
	$(COMPILER) $(CFLAGS) $(OBJ).c $(LIBS) -o $(TARGET)
	
	
.PHONY: clean
	
clean:
	rm -f $(TARGET)
