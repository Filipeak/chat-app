# Chat App

## Overview
A simple chat app built in Visual Studio C++ using Winsock library

## Features
- Client-Server architecture
- Peer-to-peer architecture
- Multithreading

## Parts (Modules):
1. **Client** - Client for Client-Server architecture
2. **Server** - Server for Client-Server architecture
3. **Client P2P** - Client for Peer-to-Peer architecture
4. **Network Base** - Base library including utils functions, etc...

## Usage
Before using remember to build the project!

#### Warning: There isn't any kind of encryption or security!

### Client-Server
To use the client-server architecture run the ```Server.exe```, and set the port you with to start on. Then start a few instances of ```Client.exe```, provide their nicknames, set ip (you can 127.0.0.1 or localhost if server is on the same machine) and port of the server. Clients will connect to the server and now you can exchange messages by simply typing them to terminal.

### Peer-to-peer (P2P)
To start, open the ```ClientP2P.exe``` app. Then provide your client's port. Leave the "Enter ip to start connection" and "Enter port to start connection" fields blank. Next, open the ```ClientP2P.exe``` again and provide ip and port of the previous client (you can 127.0.0.1 or localhost if server is on the same machine). You should now be connected. There is no support for nicknames in P2P, but you can freely send messages to each other. You can join as many clients as you wish and they will all be synced.

Note: If you connect clients on your local machine using localhost or 127.0.0.1, you won't be able to connect client from outside.

#### How does syncing work?
Since P2P is decentrialized, there is no simple way of obtaining all users. When the new user connects to any of users already connected to the network it asks for its' list of connected clients. Then it tries to connect to them. This process continues until it connects to all users in network.