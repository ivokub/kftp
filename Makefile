ARGS = -g -pthread
compiled = serve.o kftp.o handlers.o

all: kftp

kftp: kftp.o serve.o handlers.o
	gcc $(ARGS) $(compiled) -o kftp

kftp.o: kftp.c serve.c
	gcc $(ARGS) -c kftp.c

serve.o: serve.c kftp.c
	gcc $(ARGS) -c serve.c
	
handlers.o: handlers.c serve.c
	gcc $(ARGS) -c handlers.c
