#define WIN32_LEAN_AND_MEAN

#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <stdexcept>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

const char *handleRequest(string request);

int main(void) {
	// Documentation obtained from https://learn.microsoft.com/en-us/windows/win32/winsock/
	cout << "Initializing winsock" << endl;
	WSADATA wsadata;
	int ret = 0;

	ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != 0) throw runtime_error("WSADATA startup failed " + to_string(ret));

	cout << "Initializing socket" << endl;

	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (ret != 0) {
		WSACleanup();
		throw runtime_error("getaddrinfo failed " + to_string(ret));
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		long error_no = WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		throw runtime_error("ERROR at socket(): " + to_string(error_no));
	}

	cout << "Binding a socket" << endl;

	ret = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR) {
		long error_no = WSAGetLastError();
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		throw runtime_error("Bind failed with error " + to_string(error_no));
	}

	cout << "Listening to the socket" << endl;

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		long error_no = WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		throw runtime_error("Listen failed with error " + to_string(error_no));
	}

	cout << "Accepting a connection" << endl;

	SOCKET ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		long error_no = WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		throw runtime_error("Accept failed: " + to_string(error_no));
	}

	cout << "Receiving and sending Data" << endl;
	char recvbuf[DEFAULT_BUFFLEN];
	int sendret = 0;
	int recvbuflen = DEFAULT_BUFFLEN;

	do {
		ret = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (ret > 0) {
			cout << "Bytes recieved: " << ret << endl;
			cout << "The message was: " << recvbuf << endl;
			const char *sendbuf = handleRequest(recvbuf);
			sendret = send(ClientSocket, sendbuf, ret, 0);
			if (sendret == SOCKET_ERROR) {
				long error_no = WSAGetLastError();
				closesocket(ClientSocket);
				WSACleanup();
				throw runtime_error("Send failed" + to_string(error_no));
			}
			cout << "Bytes sent: " << sendret << endl;

		} else if (ret == 0)
			cout << "Closing connection..." << endl;
		else {
			long error_no = WSAGetLastError();
			closesocket(ClientSocket);
			WSACleanup();
			throw runtime_error("Recieve failed " + to_string(error_no));
		}
	} while (ret > 0);

	cout << "Disconnecting Socket" << endl;

	ret = shutdown(ClientSocket, SD_SEND);
	if (ret == SOCKET_ERROR) {
		long error_no = WSAGetLastError();
		closesocket(ClientSocket);
		WSACleanup();
		throw runtime_error("shutdown failed: " + to_string(error_no));
	}

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

const char *handleRequest(string request) {
	// Decode the request
	if (request.substr(0, 3) == "GET") {
		cout << "This was a GET request" << endl;
		return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
	} else {
		cout << "Some other request";
		return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
	}
}
