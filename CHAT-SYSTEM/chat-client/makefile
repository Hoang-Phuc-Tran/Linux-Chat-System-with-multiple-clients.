#	
#   FILE			: makefile
#	PROJECT			: PROG1970 - Assignment #4
#	PROGRAMMER(S)	: Caine Phung - ID: 6983324
#                     Hoang Phuc Tran - ID: 8789102
#				      Philip Wojdyna - ID: 8640180
#	FIRST VERSION	: 2023-27-03
#	DESCRIPTION		: This makefile will compile all the source files for chat-client.
#
./bin/chat-client : ./obj/chat-client.o 
	cc ./obj/chat-client.o   -o ./bin/chat-client -lpthread -lncurses

#Dependencies                                        
./obj/chat-client.o : ./src/chat-client.c ./inc/chat-client.h 
	cc -c ./src/chat-client.c -o ./obj/chat-client.o

#Other targets
all : ./bin/chat-client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	



