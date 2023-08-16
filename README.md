# TCP/IP Chat System

Welcome to the TCP/IP Chat System, a practical exploration of socket-based TCP/IP communications through a simple chat program. This assignment challenges you to delve into C programming techniques for socket-level programming, multi-threaded solutions, and integration of the NCURSES library for enhanced user interfaces.

## Objectives

- Develop proficiency in C programming techniques for socket-level communication.
- Explore multi-threading concepts and apply them to your solution.
- Integrate the NCURSES library for a more interactive user interface.

## Requirements

In this assignment, you'll create a CHAT-SYSTEM consisting of two applications: chat-server (server) and chat-client (client). Your design must adhere to the following guidelines:

### Server (chat-server)

- Implement a central server model in ANSI C.
- Enable multi-threading to handle new user connections.
- Maintain a shared data structure to manage client information (IP Address, user name, etc.).
- Support a maximum of 10 clients.
- Ensure proper shutdown and cleanup when threads reach zero.

### Client (chat-client)

- Utilize the NCURSES library for a user-friendly interface.
- Implement basic UI components, including a message input area and conversation display.
- Support threading for outgoing and incoming messages.
- Enforce a 40-character boundary for message input.
- Ensure smooth message reception and composition without interruptions.
- Gracefully shut down the client application when the user enters ">>bye<<".

### Networking and Messaging

- Implement a messaging protocol for communication between clients and the server.
- Include IP address, user name, message content, and timestamp in the message format.
- Document your messaging scheme and data structure used in the server code's file header comments.

## Getting Started

1. Set up the chat-server application on a Linux VM (MACHINE-A).
2. Run a chat-client application on a different Linux VM (MACHINE-B).
3. Optionally, run another chat-client application on the same Linux VM as the server (MACHINE-A).
4. Utilize command-line switches for the chat-client application (`--user<userID> --server<server name>`).
5. Experiment with the NCURSES library to enhance the user interface.
6. Ensure error handling and graceful shutdowns are implemented.

## UI Examples

Your chat-client application's UI should include basic components for message input and conversation display. Feel free to explore additional UI enhancements using the NCURSES library.

## Incoming Message Formatting

Each chat message follows a specific format for clarity:

