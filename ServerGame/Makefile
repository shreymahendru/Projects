PORT=42001
CFLAGS= -DPORT=\$(PORT) -g -Wall

battle : battleserver.o
	gcc ${CFLAGS} -o $@ battleserver.o

.c.o: %.c $(DEPS)
	gcc ${CFLAGS} -c $< 

clean:
	rm *.o