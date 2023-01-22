#include "Requests.h"

//https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
namespace Requests{

struct string {
    char* ptr;
    size_t len;
};
//https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
void init_string(struct string* s) {
    s->len = 0;
    s->ptr = (char*)malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}
//https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
 size_t writefunc(void* ptr, size_t size, size_t nmemb, void* b)
{
	struct string* s = (struct string*) b;	
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);

    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}


bool RequestDataFromServer(const char EVA[8], const char Date[7], const char Hour[3], RequestType ReqType, struct tm tm)
{

    std::string ClientId = "API_KEY_EINFÜGEN";
    std::string DbApiKey = "API_KEY_EINFÜGEN";

    CURL* req = curl_easy_init();
    struct string response;
    init_string(&response);
    CURLcode res;
    struct curl_slist* headers = NULL;
    long httpCode = 0;

    curl_easy_setopt(req, CURLOPT_CUSTOMREQUEST, "GET");
   
    if(ReqType == RequestType::PLANNED)
    {
     curl_easy_setopt(req, CURLOPT_URL, std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/plan/").append(EVA).append("/").append(Date).append("/").append(Hour).c_str());
   //  std::cout << "[+] " << std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/plan/").append(EVA).append("/").append(Date).append("/").append(Hour).c_str() << "\n";
    } 
    else if(ReqType == RequestType::CHANGES)
    {
	curl_easy_setopt(req, CURLOPT_URL, std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/").append(EVA).c_str());
	//std::cout << "[+] " << std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/").append(EVA).c_str() << "\n";
    }else
    {
    	return false;
    }

    curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(req, CURLOPT_WRITEDATA, &response);

    headers = curl_slist_append(headers, ClientId.c_str());
    headers = curl_slist_append(headers, DbApiKey.c_str());
    headers = curl_slist_append(headers, "accept: application/xml");
    curl_easy_setopt(req, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(req);
   // std::cout << "[+] Message has been sent\n";

    curl_easy_getinfo(req, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_ABORTED_BY_CALLBACK && httpCode == 200)
    {
    	  
            std::ofstream DeutscheBahnRequestFile;
            if (ReqType == RequestType::PLANNED)
            {
                DeutscheBahnRequestFile.open(std::string( "../src/Requests/DeutscheBahnRequestPlan").append("-").append(Date).append("-").append(Hour).append(".txt").c_str(), std::ios::trunc); 
                std::cout << std::string( "../src/Requests/DeutscheBahnRequestPlan").append("-").append(Date).append("-").append(Hour).append(".txt").c_str() << "\n";
            }
            else if (ReqType == RequestType::CHANGES)
            {
                DeutscheBahnRequestFile.open(std::string("../src/Requests/DeutscheBahnRequestChanges").append("-").append(Date).append("-").append(Hour).append(".txt").c_str(), std::ios::trunc);
		
            }

            if (DeutscheBahnRequestFile.is_open()) std::cout << "Datei geoeffnet\n";
            DeutscheBahnRequestFile << response.ptr;
	       // std::cout << response.ptr << "\n";

        //    std::cout << response.ptr;
        DeutscheBahnRequestFile.close();
        delete[] response.ptr;
        return true;
    }
    else
    {
        std::cout << "[-] Error with response: Response code is " << httpCode << "\n";
        delete[] response.ptr;
        return false;
    }


}



}
