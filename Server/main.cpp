#include "Requests.h"
#include "pugi/pugixml.hpp"
#include "XMLExtractor.h"
#include <iostream>
#include <time.h>
#include <string>
#include <unistd.h>
#include "pugi/pugixml.hpp"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <thread>
#include <pthread.h>
#include <condition_variable>
#include <mutex>
#include "Server.h"
#include <signal.h>
#include "Wecker.h"


bool setUp(){

	std::ofstream InformationFile;
	InformationFile.open("TrainInformation.xml", std::ios::out | std::ios::trunc);

	if(InformationFile.is_open())
	{
		std::cout << "[+] Informationsdatei wurde erfolgreich erstellt\n";
	} else
	{
		std::cout << "[-] Informationsdatei konnte nicht erstellt werden\n";
		return false;
	}
}





extern std::mutex tLock;
extern std::condition_variable tCondition;
extern bool tSetupFinished ;
extern bool isTrainDelayed;

struct UserInfo
{
	std::string UsualTime;
	std::string EVA;
	std::string TrainNumber;
	std::string WakeUpTime;
};

bool GetUserInfo(UserInfo& uInfo)
{
	pugi::xml_document UserInformationFile;
	pugi::xml_parse_result iResult = UserInformationFile.load_file("TestPut");

	if(!iResult)
	{
		std::cout << "[-] Konnte UserInfo datei nicht öffnen " << iResult.description() << "\n";
		return false;
	}
	else
	{
		uInfo.TrainNumber = UserInformationFile.child("TrainList").child("Train").attribute("n").value();
		uInfo.EVA = UserInformationFile.child("TrainList").child("Train").attribute("EVA").value();
		uInfo.UsualTime = UserInformationFile.child("TrainList").child("Train").attribute("UsualTime").value();
		uInfo.WakeUpTime = UserInformationFile.child("TrainList").child("Train").attribute("WakeUpTime").value();

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

bool hasFileChanged = false; 

void SigUsr(int sig)
{
	std::cout << "[+] SigUsr wurde aufgerufen\n";
	hasFileChanged = true;
}

void setLightGreen()
{
	system("raspi-gpio set 17 dh");
	system("raspi-gpio set 27 dl");
	system("raspi-gpio set 18 dh");
}

void setLightRed()
{
	system("raspi-gpio set 17 dl");
	system("raspi-gpio set 27 dh");
	system("raspi-gpio set 18 dh");
}

int main()
{
	system("raspi-gpio set 17 op");
	system("raspi-gpio set 27 op");
	system("raspi-gpio set 18 op");

	setLightGreen();	
	Wecker::InitWecker();
//	std::string Weck;
//	std::cin >> Weck;
	
	signal(SIGUSR1, SigUsr);
	tSetupFinished = false;
	time_t T = time(NULL);
	struct tm tm = *localtime(&T);
	std::cout << &tCondition << "  " << &tLock << "  " << &tSetupFinished << "\n";
	Server::HTTPServer HAServer{};
	struct UserInfo uInfo;

 	std::cout << "Created\n";

	if(!XMLHelper::IsUserInformationFileValid())
	{
		std::cout << "[-] Nicht richtig formatiert";
		std::unique_lock<std::mutex> lock(tLock);
		if(!tSetupFinished)
		{
			tCondition.wait(lock);
		}

		std::cout << "[+] Setup is finished now\n";
		if(tSetupFinished) std::cout << "AIWUDKHJANOUIZKFLAHEBAUKWFGHZU";
	}
	std::cout << "[+] Nach Verspaetung suchen?\n";
	getchar();


	if(GetUserInfo(uInfo))
	{

	}
	else
	{
		std::cout << "[-] Konnte UserInfo nicht bekommen\n";
		 std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	//Wecker::SetWeckzeit(uInfo.WakeUpTime);
	Wecker::SetWeckzeit("1625");
	Wecker::GetTimerExpiration();
	Wecker::AddDelayTime("0005");
	Wecker::GetTimerExpiration();
	getchar();

	while(true)
	{
		if(hasFileChanged)
		{
			GetUserInfo(uInfo);
			hasFileChanged = false;
		}

		Requests::RequestDataFromServer(uInfo.EVA.c_str(), XMLHelper::DateToDBDate(tm).c_str(), uInfo.UsualTime.c_str(), Requests::RequestType::PLANNED, tm);
		Requests::RequestDataFromServer(uInfo.EVA.c_str(), XMLHelper::DateToDBDate(tm).c_str(), uInfo.UsualTime.c_str(), Requests::RequestType::CHANGES, tm);
		XMLHelper::FindDelayForTrain();
		if(isTrainDelayed)
		{
			Wecker::SetWeckzeit("1607");
			setLightRed();
		}
		else
		{
			setLightGreen();
		}
		getchar();
	}
	
	
	


	getchar();
	//XMLHelper::FindDelayForTrain();
	
}

