#include "XMLExtractor.h"


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

	std::string GetTrainNumber(pugi::xml_node TrainData)
	{
		std::string TrainNumber;

		TrainNumber = TrainData.child("tl").attribute("n").value();
		return TrainNumber;
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

	std::string FindDelay(pugi::xml_node TrainData, std::string ID)
	{
		
		std::string ReturnDelay = "";
	
		for (pugi::xml_node currentNode = TrainData; currentNode; currentNode = currentNode.next_sibling())
		{
			if (currentNode.attribute("id").value() == ID)
			{
				std::cout << "ID: " << currentNode.attribute("id").value() << "  " << ID << "\n"; 
				
				if (currentNode.child("dp").attribute("ct"))
				{
					ReturnDelay = currentNode.child("dp").attribute("ct").value();
					ReturnDelay = ReturnDelay.substr(6, 9);
					ReturnDelay.insert(2, ":");
					return ReturnDelay;
				}
				else if(currentNode.child("ar").attribute("ct"))
				{
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


	std::string CreateXMLFile(const char* EVA, const char* n, const char* UsualTime)
	{
		pugi::xml_document doc;
		std::stringstream stream;
		
		doc.append_child("TrainList");
		doc.child("TrainList").append_child("Train");
		doc.child("TrainList").child("Train").append_attribute("EVA");
		doc.child("TrainList").child("Train").attribute("EVA").set_value(EVA);

		doc.child("TrainList").child("Train").append_attribute("n");
		doc.child("TrainList").child("Train").attribute("n").set_value(n);
		doc.child("TrainList").child("Train").append_attribute("UsualTime");
		doc.child("TrainList").child("Train").attribute("UsualTime").set_value(UsualTime);
		
		doc.print(stream);
		std::cout << stream.str() << "\n";
		return stream.str();
	}

	bool IsUserInformationFileValid(std::string File)
	{

		pugi::xml_document UserInformationFile;
		pugi::xml_parse_result UserInformationFileParseResult = UserInformationFile.load_buffer(File.c_str(), File.size());

		if (UserInformationFileParseResult)
		{
			pugi::xml_node Node = UserInformationFile.child("TrainsTable");
			if (!Node) return false;

			Node = Node.child("Train");
			if (!Node) return false;

			pugi::xml_attribute Attribut = Node.attribute("n");
			if (!Attribut) return false;
			try
			{
				std::stoi(Attribut.value());
			}
			catch (std::invalid_argument& e)
			{
				std::cout << e.what() << "\n";
				std::cout << Attribut.value() << " ist keine Zahl\n";
				return false;
			}

			Attribut = Node.attribute("EVA");
			if (!Attribut) return false;
			try
			{
				std::stoi(Attribut.value());
			}
			catch (std::invalid_argument& e)
			{
				std::cout << e.what() << "\n";
				std::cout << Attribut.value() << " ist keine Zahl\n";
				return false;
			}

			Attribut = Node.attribute("UsualTime");
			if (!Attribut) return false;
			try
			{
				std::stoi(Attribut.value());
			}
			catch (std::invalid_argument& e)
			{
				std::cout << e.what() << "\n";
				std::cout << Attribut.value() << " ist keine Zahl\n";
				return false;
			}

			Attribut = Node.attribute("WakeUpTime");
			if (!Attribut) return false;
			try
			{
				std::stoi(Attribut.value());
			}
			catch (std::invalid_argument& e)
			{
				std::cout << e.what() << "\n";
				std::cout << Attribut.value() << " ist keine Zahl\n";
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}

	}


	std::string DateToDBDate(struct tm tm)
	{
		std::string Date = std::to_string(tm.tm_year - 100);
		if ((tm.tm_mon + 1) < 10)
		{
			Date.append("0");
		}
		Date.append(std::to_string(tm.tm_mon + 1));
		if (tm.tm_mday < 10)
		{
			Date.append("0");
		}
		Date.append(std::to_string(tm.tm_mday));
		return Date;
	}


};