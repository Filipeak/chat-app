# Chat App

## Overview
A simple chat app built in Visual Studio C++ using Winsock library

## Features
- Client-Server architecture
- Peer-to-peer architecture
- Multithreading

## Parts (Modules):
1. Client - Client for Client-Server architecture
2. Server - Server for Client-Server architecture
3. Client P2P - Client for Peer-to-Peer architecture
4. Network Base - Base library including utils functions, etc...

## Usage
Before using remember to build the project!

#### Warning: There isn't any kind of encryption or security

### Client-Server
To use the client-server architecture run the ```Server.exe```, it will start on port 1337 by default. Then start **twice** the ```Client.exe``` and provide nicknames. Clients will connect to the server and now you can exchange messages by simply typing them to terminal. You can join as many clients you want.

### Peer-to-peer (P2P)
To start, open the ```ClientP2P.exe``` app. Then provide your client's port (<ins>must be unique for each client</ins>). Leave the "Enter port to start connection" field blank. Next, open the ```ClientP2P.exe``` again, provide **different** port and pass your previous client's port to "Enter port to start connection" field and start. You should now be connected. There is no support for nicknames in P2P, but you can freely send messages to each other. You can join as many clients as you wish and they will all be synced.

#### How does syncing work?
Since P2P is decentrialized, there is no simple way of obtaining all users. When the new user connects to any of users already connected to the network it asks for its' list of connected clients. Then it tries to connect to them. This process continues until it connects to all users in network.