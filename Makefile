CFLAGS = -pthread -g -w -Wall


test:test.o 
	gcc -o test test.o $(CFLAGS)


test.o:test.c
	gcc -o $@ -c $^ $(CFLAGS) 
	
#	@echo start 

run:test
	./test

clean:
	rm -f *.o

.PHONY: clean run
