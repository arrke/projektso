OBJ = main.c projectfunctions.c
all: projekt
projekt: $(OBJ)
	gcc $(OBJ) -o projekt
$(OBJ): projectfunctions.h
.PHONY: clean
clean:
	rm -f *.o projekt
