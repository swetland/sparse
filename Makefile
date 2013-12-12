
all: mksparse unsparse

mksparse: mksparse.c util.c util.h
	gcc -Wall -O2 -o mksparse mksparse.c util.c

unsparse: unsparse.c util.c util.h
	gcc -Wall -O2 -o unsparse unsparse.c util.c

clean:
	rm -f unsparse mksparse
