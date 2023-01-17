#pragma once

#include <string>
#include <curl/curl.h>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>

//Benutze momentan libcurl werde aber wahrscheinlich langfristig auf WinSock setzen

namespace Network
{

	enum class RequestStatus
	{
		FAILED,
		SUCCESS,
		NOT_SEND
	};

	enum class RequestType
	{
		Changes,
		Planned
	};

	bool RequestDataFromServer(char EVA[8], char Date[7], char Hour[3], RequestType );


	//Beíde Funktionen geklaut von https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string

	size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s);

	void init_string(struct string* s);

	struct string {
		char* ptr;
		size_t len;
	};
}