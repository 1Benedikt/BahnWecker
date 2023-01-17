#include "NetworkRequests.h"


namespace Network {

   

    void init_string(struct string* s) {
        s->len = 0;
        s->ptr = (char*)malloc(s->len + 1);
        if (s->ptr == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }
        s->ptr[0] = '\0';
    }

    size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s)
    {
        size_t new_len = s->len + size * nmemb;
        s->ptr = (char*)realloc(s->ptr, new_len + 1);
        if (s->ptr == NULL) {
            fprintf(stderr, "realloc() failed\n");
            exit(EXIT_FAILURE);
        }
        memcpy(s->ptr + s->len, ptr, size * nmemb);
        s->ptr[new_len] = '\0';
        s->len = new_len;

        return size * nmemb;
    }

    bool RequestDataFromServer(char EVA[8], char Date[7], char Hour[3], RequestType RequestT )
    {

     //   std::cout << std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/plan/").append(EVA).append("/").append(Date).append("/").append(Hour) << "\n";
        std::string ClientId = "DB-Client-Id: 99cc64b2f46f9ac154580c291446e583";
        std::string DbApiKey = "DB-Api-Key: dd2b2e4c468d70225fd0e94bbb1ead8d";

        CURL* req = curl_easy_init();
        struct string response;
        init_string(&response);
        CURLcode res;
        struct curl_slist* headers = NULL;
        long httpCode = 0;

        curl_easy_setopt(req, CURLOPT_CUSTOMREQUEST, "GET");

        if (RequestT == RequestType::Planned)
        {
            curl_easy_setopt(req, CURLOPT_URL, std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/plan/").append(EVA).append("/").append(Date).append("/").append(Hour).c_str());
        } 
        else if (RequestT == RequestType::Changes)
        {
            curl_easy_setopt(req, CURLOPT_URL, std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/").append(EVA).c_str());
            std::cout << "\n[+] CHANGES: " << std::string("https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/").append(EVA) << "\n";
        }

        curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(req, CURLOPT_WRITEDATA, &response);

        headers = curl_slist_append(headers, ClientId.c_str());
        headers = curl_slist_append(headers, DbApiKey.c_str());
        headers = curl_slist_append(headers, "accept: application/xml");
        curl_easy_setopt(req, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(req);
        
        curl_easy_getinfo(req, CURLINFO_RESPONSE_CODE, &httpCode);
        if (res != CURLE_ABORTED_BY_CALLBACK && httpCode == 200 )
        {
            std::cout << "ERROR CODE ---- " << res << "------\n";
            std::ofstream DeutscheBahnRequestFile;
            if (RequestT == RequestType::Planned)
            {
                DeutscheBahnRequestFile.open("Requests/DeutscheBahnRequestPlan.txt", std::ios::trunc);
            }
            else if (RequestT == RequestType::Changes)
            {
                DeutscheBahnRequestFile.open("Requests/DeutscheBahnRequestChanges.txt", std::ios::trunc);
            }

            if (DeutscheBahnRequestFile.is_open()) std::cout << "Datei geoeffnet\n";
            DeutscheBahnRequestFile << response.ptr;
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
