#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <signal.h>
#include "XMLExtractor.h"

typedef int SOCKET;

//Für Server einen extra Benutzer erstellen und oder einstellen das nur auf gewisse Datein zugegriffen werden kann.

namespace Server
{

	enum STATUS
	{
		OK = 200,
		CREATED = 201,
		NOTFOUND = 404

	};

	enum class METHOD
	{
		GET,
		PUT,
		UNSUPPORTED
	};

	enum class CONTENTTYPE
	{
		XML,
		TEXT
	};

	struct HTTPRequest
	{
		HTTPRequest(char*, int);

		METHOD Method;
		std::string Path;
		int Version;
		int Length;
		std::string Content;
		int ContentLength = 0;
	};

	struct HTTPResponse
	{
		public:
		HTTPResponse(HTTPRequest);
		int Status;
		int ContentLength;
		std::string Content;
		CONTENTTYPE ContentType;
		std::string ContentLocation;
		std::string ResponseBuffer;

	};
	void ClientHandler(void*);
	class HTTPServer
	{
		public:
		SOCKET SocketToClient;
		std::thread* ClientThread;
		std::thread* ServerThread;
		SOCKET ServerSocket;

		
		int Start();
		HTTPServer();
		bool Send(std::string);
		~HTTPServer();
		
	};
	

}
