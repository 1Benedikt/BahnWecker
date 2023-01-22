#include "DeutscheBahnData.h"



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
