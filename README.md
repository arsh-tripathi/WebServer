# WebServer

A HTTP Web server made in C++

## Video demonstrating the use of the program 

https://github.com/arsh-tripathi/WebServer/assets/110033985/7eee78e8-a4e4-460a-8795-a602d28630bc


## Installation 

You require the following things to run this program. As of right now, this program is only working on windows and is designed for a windows system.

1. A C++ compiler (preferably gcc)
2. Appropriate headers ("winsock")

Download the main.cpp and compile the program using the make.bat file or simply type the command

```bash
g++ main.cpp -o server -lws2_32
```

## Running the program 

To run the program simply execute the command

```bash
./server.exe
```

This will start a server at the default port (27015) and you open a browser and connect to the port at http://127.0.0.1:27015 or http://localhost:27015

Then the server will display an default index.html files it can find. You can specify the files after the address as well to get specific files.

To terminate, simply press Ctrl+C, and the program will end gracefully.

## Features

I have implemented basic handling for GET PUT and DELETE requests. There is also the possiblility of implementing other requests, but they are specific to your end purpose, and this program is just for a general server.

If you have any suggestions for improvement kindly shoot me an email at: arsht2004@gmail.com 


