CC=pgcc
OBJ=imgproc_mem
FLAGS= -fast -Minfo=all  
all: $(OBJ).c
	$(CC) $(FLAGS) -o $(OBJ) $<
clean:
	rm $(OBJ)
cleanall:
	rm $(OBJ) *.err *.out
