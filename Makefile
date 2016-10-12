LDPATH = -L/usr/local/lib
LDFLAGS = -laqua
CFLAGS = -Wall -O3

regex: regex.o
	gcc -o regex regex.o  $(LDPATH) $(LDFLAGS)
regex.o: regex.c
	gcc -c regex.c $(CFLAGS)
clean:  
	rm -f regex *.o
