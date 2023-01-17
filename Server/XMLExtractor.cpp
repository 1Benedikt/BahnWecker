#include "XMLExtractor.h"
std::string USERINFORMATIONPATH = "/home/bent/Desktop/GitProjekt/NeuBahn/build/TestPut";
std::string DBREQUESTSPLANPATH = "/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests";
std::string DBREQUESTSCHANGESPATH = "/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests";
bool isTrainDelayed;
namespace XMLHelper
{
	/// <summary>
	/// Returns the Endstation of a Timetablestop
	/// </summary>
	/// <param name="TrainData"> The node must be of a Timetable-Node ("s")</param>
	/// <returns>Returns Endstation as a String, if not successfull it returns a nullptr</returns>
	std::string GetEndStation(pugi::xml_node TrainData)
	{
		std::string EndStation;
		pugi::xml_node Depature;
		 if (TrainData.child("dp"))
		{
			Depature = TrainData.child("dp");
		 }
		 else
		 {
			 std::cout << "[-] " << "Error no dt found [XMLHelper.cpp]";
			 return "";
 		 }

		 EndStation = Depature.attribute("ppth").value();
		 size_t LastPos = EndStation.find_last_of("|");
		 return EndStation.substr(LastPos +1, EndStation.size() -1);
	}

	std::string GetOriginalStation(pugi::xml_node TrainData)
	{
		
		std::string OriginalStation;
		pugi::xml_node Arrival;
		if (TrainData.child("ar"))
		{
			Arrival = TrainData.child("ar");
		}
		else
		{
			std::cout << "[-] " << "Error no dt found [XMLHelper.cpp]";
			return "";
		}

	    OriginalStation = Arrival.attribute("ppth").value();
		size_t FirstPos = OriginalStation.find_first_of("|");
		return OriginalStation.substr(0, FirstPos);

	}

	std::string GetArrivalTime(pugi::xml_node TrainData)
	{
	
		std::string Time;
		pugi::xml_node Departure;
		if (TrainData.child("dp"))
		{
			Departure = TrainData.child("dp");
		}
		else
		{
			std::cout << "[-] " << "Error no dp found [XMLHelper.cpp]";
			return "";
		}

		Time = Departure.attribute("pt").value();
		Time = Time.substr(6, 9);
		Time.insert(2, ":");
		return Time;

	}

	std::string GetDelay(pugi::xml_node TrainData)
	{
		std::string DelayTime;
		pugi::xml_node DelayNode;
	
		if (TrainData.child("dp"))
		{
			DelayNode = TrainData.child("dp");
		}
		else
		{
			std::cout << "[-] " << "Error no Delay found [XMLHelper.cpp]";
			return "";
		}

		DelayTime = DelayNode.attribute("ct").value(); 
		return DelayTime;

	}

	std::string GetID(pugi::xml_node TrainData)
	{
		std::string ID;
		

		ID = TrainData.attribute("id").value();
	
		return ID;
	}

	std::string FindDelay(pugi::xml_node TrainData, std::string ID, bool Formatted)
	{
		
		std::string ReturnDelay = "";
	
		for (pugi::xml_node currentNode = TrainData; currentNode; currentNode = currentNode.next_sibling())
		{
			if (currentNode.attribute("id").value() == ID)
			{
				std::cout << "ID: " << currentNode.attribute("id").value() << "  " << ID << "\n"; 
				
				if (currentNode.child("dp").attribute("ct"))
				{
					if(!Formatted) return currentNode.child("dp").attribute("ct").value();
					ReturnDelay = currentNode.child("dp").attribute("ct").value();
					ReturnDelay = ReturnDelay.substr(6, 9);
					ReturnDelay.insert(2, ":");
					return ReturnDelay;
				}
				else if(currentNode.child("ar").attribute("ct"))
				{
					if(!Formatted) return currentNode.child("dp").attribute("ct").value();
					ReturnDelay = currentNode.child("ar").attribute("ct").value();
					ReturnDelay = ReturnDelay.substr(6, 9);
					ReturnDelay.insert(2, ":");
					return ReturnDelay;
				}
				else
				{
					ReturnDelay = "[-] XMLHelper.cpp: ERROR NO AR OR DP CT\n";
				}
				
			}
		}
		return ReturnDelay;
	}

    std::string FindNode(pugi::xml_node TrainData, std::string ID)
    {
        for(pugi::xml_node CurrentNode = TrainData; CurrentNode; CurrentNode = CurrentNode.next_sibling())
        {
            if(CurrentNode.child("tl").attribute("n").value() == ID )
            {
                return CurrentNode.child("tl").attribute("n").value();
            }
        }
        return "";
    }

	bool FindDelayForTrain()
	{
		isTrainDelayed = false;
		time_t T = time(NULL);
		struct tm tm = *localtime(&T);
		bool TrainFound = false;
		std::string TrainNumber;
		std::string EVA;
		std::string UsualTime;
		std::string ID;
		std::cout << "[+] FInd delay\n";
		pugi::xml_document UserTraininformationFile;
		pugi::xml_parse_result UserTraininformationFileResult = UserTraininformationFile.load_file(USERINFORMATIONPATH.c_str());

		if(!UserTraininformationFileResult)
		{
			std::cout << "[-] Datei mit Nutzerinformationen konnte nicht geöffnet werden: " << UserTraininformationFileResult.description() << "\n";
			return false;
		}

		TrainNumber = UserTraininformationFile.child("TrainList").child("Train").attribute("n").value();
		EVA = UserTraininformationFile.child("TrainList").child("Train").attribute("EVA").value();
		UsualTime = UserTraininformationFile.child("TrainList").child("Train").attribute("UsualTime").value();
		std::cout << "[+] USUAL TIME: " << UsualTime << "  " << TrainNumber << "  "<<  EVA << "\n";

		pugi::xml_document PlanFile;
		pugi::xml_parse_result PlanFileResult = PlanFile.load_file(std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestPlan").append("-").append(DateToDBDate(tm)).append("-").append(UsualTime).append(".txt").c_str()); 
		std::cout << std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestPlan").append("-").append(DateToDBDate(tm)).append("-").append(UsualTime).append(".txt").c_str() << "\n";
		if(!PlanFileResult)
		{
			std::cout << "[-] Datei mit Fahrzeiten konnte nicht geöffnet werden: " << PlanFileResult.description() << "\n";
			return false;
		}
		pugi::xml_document ChangeFile;
		pugi::xml_parse_result ChangeFileResult = ChangeFile.load_file(std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestChanges").append("-").append(DateToDBDate(tm)).append("-").append(UsualTime).append(".txt").c_str());

		if(!ChangeFileResult)
		{
			std::cout << "[-] Datei mit Verspaetungen konnte nicht geöffnet werden: " << ChangeFileResult.description() << "\n";
			return false;
		}

		
		pugi::xml_node ChangeFileNode = ChangeFile.child("timetable").child("s");
		pugi::xml_node PlanFileNode = PlanFile.child("timetable").child("s");

		for(pugi::xml_node node = PlanFileNode; node; node = node.next_sibling())
		{
		
			if(node.child("tl").attribute("n").value() == TrainNumber)
			{
				ID = GetID(node);
				std::cout << "[+] Trainnumber gefunden\n";
				if(node.child("dp").attribute("pt").value() != FindDelay(ChangeFileNode, ID, false)){
					 std::cout << "[-] Zug verspätet\n";
					 isTrainDelayed = true;
				}
				TrainFound = true;
				break;
			}
			else
			{
				std::cout << "[-] Trainnumber nicht gefunden\n";
			}
		}

		if(!TrainFound)
		{
			return AdvancedFindDelayForTrain(UsualTime, TrainNumber, EVA);
		}

		std::cout << FindDelay(ChangeFileNode, ID, true) << "\n";
		return true;
		
	}

	bool AdvancedFindDelayForTrain(std::string UsualTime, std::string TrainNumber, std::string EVA)
	{
		
		time_t T = time(NULL);
		struct tm tm = *localtime(&T);
		std::string ID;
		
		pugi::xml_document ChangeFile;
			pugi::xml_parse_result ChangeFileResult = ChangeFile.load_file(std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestChanges").append("-").append(DateToDBDate(tm)).append("-").append(UsualTime).append(".txt").c_str());
		//	std::cout << "AAA" << std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestChanges").append("-").append(UsualTime).append(".txt").c_str() << "AAA\n";
			if(!ChangeFileResult)
			{
				std::cout << "[-] Advanced Datei mit Verspaetungen konnte nicht geöffnet werden: " << ChangeFileResult.description() << "\n";
				
			}
		for(int i = 0; i < 24; ++i)
		{
			pugi::xml_document PlanFile;
			pugi::xml_parse_result PlanFileResult = PlanFile.load_file(std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestPlan").append("-").append(DateToDBDate(tm)).append("-").append(std::to_string(i)).append(".txt").c_str()); 
			//std::cout << "AAA" << std::string("/home/bent/Desktop/GitProjekt/NeuBahn/src/Requests").append("/DeutscheBahnRequestPlan").append("-").append("230101").append("-").append(std::to_string(i)).append(".txt").c_str() << "AAA\n";
			if(!PlanFileResult)
			{
				std::cout << "[-] Advanced Datei mit Fahrzeiten konnte nicht geöffnet werden: " << PlanFileResult.description() << "\n";
				continue;
			}

			
			
			pugi::xml_node ChangeFileNode = ChangeFile.child("timetable").child("s");
			pugi::xml_node PlanFileNode = PlanFile.child("timetable").child("s");
	
			for(pugi::xml_node node = PlanFileNode; node; node = node.next_sibling())
			{
			
				if(node.child("tl").attribute("n").value() == TrainNumber)
				{
					ID = GetID(node);
					std::cout << "[+] Trainnumber gefunden Advanced\n";
						std::cout << FindDelay(ChangeFileNode, ID, true) << "\n";
					return true;
				}
			}
		}
		
		return false;
	}

		bool IsUserInformationFileValid()
		{
			
			pugi::xml_document UserInformationFile;
			pugi::xml_parse_result UserInformationFileParseResult = UserInformationFile.load_file("TestPut");

			if(!UserInformationFileParseResult)
			{
				std::cout << UserInformationFileParseResult.description() << "\n";
				std::cout << "[-] Datei konnte nicht geoeffnet werden\n";
				return false;
			} else
			{
				pugi::xml_node Node = UserInformationFile.child("TrainList");
				if(!Node) return false;			

				Node = Node.child("Train");
				if(!Node) return false;		

				pugi::xml_attribute Attribut = Node.attribute("n");
				if(!Attribut) return false;		
				try
				{
					std::stoi(Attribut.value());
				} catch (std::invalid_argument &e)
				 {
   					std::cout << e.what() << "\n";
					std::cout << Attribut.value() << " ist keine Zahl\n";
					return false;
				}

				Attribut = Node.attribute("EVA");
				if(!Attribut) return false;	
				try
				{
					std::stoi(Attribut.value());
				} catch (std::invalid_argument &e)
				 {
   					std::cout << e.what() << "\n";
					std::cout << Attribut.value() << " ist keine Zahl\n";
					return false;
				}

				Attribut = Node.attribute("UsualTime");
				if(!Attribut) return false;	
				try
				{
					std::stoi(Attribut.value());
				} catch (std::invalid_argument &e)
				 {
   					std::cout << e.what() << "\n";
					std::cout << Attribut.value() << " ist keine Zahl\n";
					return false;
				}	

				Attribut = Node.attribute("WakeUpTime");
				if(!Attribut) return false;	
				try
				{
					std::stoi(Attribut.value());
				} catch (std::invalid_argument &e)
				 {
   					std::cout << e.what() << "\n";
					std::cout << Attribut.value() << " ist keine Zahl\n";
					return false;
				}		

				return true;
			
			}

		}

		
		std::string DateToDBDate(struct tm tm)
		{
			std::string Date = std::to_string(tm.tm_year - 100);
			if((tm.tm_mon + 1) < 10)
			{
				Date.append("0");
			}
			Date.append(std::to_string(tm.tm_mon + 1));
				if(tm.tm_mday < 10)
			{
				Date.append("0");
			}
			Date.append(std::to_string(tm.tm_mday));
			return Date;
		}

};
