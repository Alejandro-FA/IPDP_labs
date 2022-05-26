CC=pgcc
OBJ=imgproc
FLAGS= -fast -Minfo=all  
all: $(OBJ).c
	$(CC) $(FLAGS) -o $(OBJ) $<
clean:
	rm $(OBJ)
cleanall:
	rm $(OBJ) *.err *.out
