all: kftp

kftp: kftp.o serve.o

kftp.o: kftp.c serve.c
