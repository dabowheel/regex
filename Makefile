LDPATH = -L/usr/local/lib
LDFLAGS = -laqua
CFLAGS = -Wall -O3
installpath = /usr/local

regex: regex.o
	gcc -o regex regex.o  $(LDPATH) $(LDFLAGS)
regex.o: regex.c
	gcc -c regex.c $(CFLAGS)
install:
	cp regex $(installpath)/bin
uninstall:
	rm -f $(installpath)/bin/regex
clean:  
	rm -f regex *.o
