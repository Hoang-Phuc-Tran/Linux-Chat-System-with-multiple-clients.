#	
#   FILE			: makefile
#	PROJECT			: PROG1970 - Assignment #4
#	PROGRAMMER(S)	: Caine Phung - ID: 6983324
#                     Hoang Phuc Tran - ID: 8789102
#				      Philip Wojdyna - ID: 8640180
#	FIRST VERSION	: 2023-27-03
#	DESCRIPTION		: This makefile will compile all the source files for chat-server.
#
./bin/chat-server : ./obj/chat-server.o 
	cc ./obj/chat-server.o   -o ./bin/chat-server -lpthread

#Dependencies                     
./obj/chat-server.o : ./src/chat-server.c ./inc/chat-server.h 
	cc -c ./src/chat-server.c -o ./obj/chat-server.o

#Other targets
all : ./bin/chat-server

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	



