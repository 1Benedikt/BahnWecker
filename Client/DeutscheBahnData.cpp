#include "DeutscheBahnData.h"


struct string {
    char* ptr;
    size_t len;
};

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

bool CheckForAtrribute(pugi::xml_node a)
{
    std::cout << a.attribute("id").value() << "\n";
    pugi::xml_attribute attr = a.attribute("id");
    if (attr)
    {
        std::cout << "h\n";
        return strcmp(attr.value(), "8640899555193381858-2211281440-7") == 0;
        
    }
    return false;
}

/*int main(int argc, char* argv[])
{
   
   /* curl_global_init(CURL_GLOBAL_ALL);
    std::string ClientId = "DB-Client-Id: 99cc64b2f46f9ac154580c291446e583";
    std::string DbApiKey = "DB-Api-Key: dd2b2e4c468d70225fd0e94bbb1ead8d";
    CURL* req = curl_easy_init();
    struct string response;
    init_string(&response);
    CURLcode res;
    struct curl_slist* headers = NULL;
    std::ofstream file("DeutscheBahn.xml", std::ofstream::out);

    if (req)
    {
        curl_easy_setopt(req, CURLOPT_CUSTOMREQUEST, "GET");
       
        curl_easy_setopt(req, CURLOPT_URL, "https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/8000181" );
        
        curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, writefunc);

        curl_easy_setopt(req, CURLOPT_WRITEDATA, &response);

        headers = curl_slist_append(headers, ClientId.c_str());

        headers = curl_slist_append(headers, DbApiKey.c_str());

        headers = curl_slist_append(headers, "accept: application/xml");

        curl_easy_setopt(req, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(req);
    
    }
   

    file << response.ptr;
    file.close();
    *//*
    pugi::xml_document doc;
    pugi::xml_document docPlan;
    
    pugi::xml_parse_result res = doc.load_file("DeutscheBahn.xml");
    pugi::xml_parse_result resPlan = docPlan.load_file("DeutscheBahnPlan.xml");
  
    std::string id;
    std::string ppth;
   
    if (res && resPlan)
    {
        pugi::xml_node nodePlan = docPlan.child("timetable").child("s");

        for (pugi::xml_node cNode = doc.child("timetable").child("s"); cNode; cNode = cNode.next_sibling())
        {
            id = cNode.attribute("id").value();

            for (pugi::xml_node cNodePlan = nodePlan; cNodePlan; cNodePlan = cNodePlan.next_sibling())
            {
                if (id == cNodePlan.attribute("id").value())
                {
                    if (cNodePlan.child("ar"))
                    {
                        std::cout << id << "  " << cNodePlan.child("ar").attribute("ppth").value() << "\n";
                    }
                    else
                    {
                        std::cout << id << "  " << cNodePlan.child("dp").attribute("ppth").value() << "\n";
                    }
                }
            }
            
        }
        
    }
    else
    {
        std::cout << "[-] " << res.description() << "\n";
        std::cout << "[-] " << resPlan.description() << "\n";
    }


    
   
   // curl_easy_cleanup(req);
    return 0;
}*/
/*
int main()
{
   

    pugi::xml_document doc;

    if (doc.load_file("DeutscheBahn.xml"))
    {
        std::cout << "Load result: " << doc.child("timetable").child("s").child("ar").attribute("ppth").value() << "\n";
        std::cout << "Load result: " << doc.child("timetable").attribute("station").value();
    }

}*/


namespace DB
{
    
    TrainInformation::TrainInformation(std::string D, std::string O, std::string AT, std::string DT, std::string CT, std::string ID, std::string n)
    {
        if (D == "")
        {
            Destination = "NO DESTINATION FOUND";
        }
        else {
            Destination = D;
        }

        if (O == "")
        {
            Origin = "NO ORIGIN FOUND";
        }
        else
        {
            Origin = O;
        }

        ArrivalTime = (AT == "") ? "NO TIME FOUND" : AT;

        DepartureTime = (DT == "") ? "NO TIME FOUND" : DT;      //May change back to If-Statements, "if" is more readable but longer and ternary is shorter but not really readable
        ChangedTime = (CT == "") ? "NO TIME FOUND" : DT;      //May change back to If-Statements, "if" is more readable but longer and ternary is shorter but not really readable
        TrainID = (ID == "") ? "NO TIME FOUND" : ID;      //May change back to If-Statements, "if" is more readable but longer and ternary is shorter but not really readable
        TrainNumber = (n == "") ? "NO TIME FOUND" : n;      //May change back to If-Statements, "if" is more readable but longer and ternary is shorter but not really readable

    }

    TrainInformation::TrainInformation(){}

    void TrainInformation::setChangedTime(std::string newValue)
    {
        ChangedTime = newValue;
    }

    std::vector<TrainInformation>* GetListOfTrains()
    {
        int i = 0;
        pugi::xml_document ArrivingTrains;
        pugi::xml_parse_result ArrivingResult = ArrivingTrains.load_file("Requests/DeutscheBahnRequestPlan.txt");
        std::vector<TrainInformation>* ReturnTrains = new std::vector<TrainInformation>;
        pugi::xml_document ChangeTrains;
        pugi::xml_parse_result ChangeResult = ChangeTrains.load_file("Requests/DeutscheBahnRequestChanges.txt");
        pugi::xml_node FirstNode = ChangeTrains.child("timetable").child("s");
        pugi::xml_node tempNode;
        pugi::xml_attribute tempAttribute;
        std::string tempString;
        
        if (ArrivingResult && ChangeResult)
        {
            pugi::xml_node FirstStop = ArrivingTrains.child("timetable").child("s");

            for (pugi::xml_node CurrentNode = FirstStop; CurrentNode; CurrentNode = CurrentNode.next_sibling())
            {
                ReturnTrains->push_back(DB::TrainInformation(XMLHelper::GetEndStation(CurrentNode),
                                                             XMLHelper::GetOriginalStation(CurrentNode),
                                                             XMLHelper::GetArrivalTime(CurrentNode),
                                                             "",
                                                             "",
                                                             XMLHelper::GetID(CurrentNode),
                                                             XMLHelper::GetTrainNumber(CurrentNode)));
                ReturnTrains->at(i).setChangedTime(XMLHelper::FindDelay(FirstNode, ReturnTrains->at(i).TrainID));
              //   tempAttribute = FirstNode.find_child_by_attribute("id", ReturnTrains->at(i).TrainID.c_str());
                
    
               ++i;
            }

        }
        else
        {
            std::cout << "[-] " << ArrivingResult.description() << "\n";
            return nullptr;
        }
       
        return ReturnTrains;
    }
};
