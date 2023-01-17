#pragma once


#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <string>
#include <string.h>
#include <time.h>

namespace Requests{


	struct String{
		char* ptr;
		size_t len;
	};

	enum class ReqeustStatus
	{
		FAILED,
		SUCCESS,
		NOT_SEND	
	};


	enum class RequestType
	{
		CHANGES,
		PLANNED
	};

	
	

	void init_string(struct String);

	//size_t writefunc(void*, size_t, size_t, struct String*);
	

	bool RequestDataFromServer(const char*,const char*,const char*, RequestType, struct tm tm);

	bool isSystemTimeCorrect(); //Noch Implementieren

}


