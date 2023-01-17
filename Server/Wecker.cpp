#include "Wecker.h"

void TimerHandler(int sig, siginfo_t *si, void* uc)
{
	std::cout << "TIMER FERtiG";
}


bool Wecker::InitWecker()
{
    struct sigevent SigEv = {0};
	SigEv.sigev_notify = SIGEV_SIGNAL;
	SigEv.sigev_signo = SIGRTMIN;

	timer_t tempID;
	if(timer_create(CLOCK_REALTIME, &SigEv, &tempID ) != 0)
	{
		std::cout << "[-] Fehler timer konnte nicht erstellt werden\n";
		return false;
	}
	SetTimerID(tempID);

	struct sigaction SigAct = {0};
	SigAct.sa_flags = SA_SIGINFO;
	SigAct.sa_sigaction = TimerHandler;

	if(sigemptyset(&SigAct.sa_mask) != 0)
	{
		std::cout << "[-] Signal für Timer konnte nicht erstellt werden\n";
		return false;
	}

	if(sigaction(SIGRTMIN, &SigAct, NULL) != 0)
	{
		std::cout << "[-] Signal für Timer konnte nicht erstellt werden\n";
		return false;
	}

	return true;
    
}

int ZeitDifferenz(int AktuelleZeitInMinuten, int WeckzeitInMinuten)
{
	int ReturnZeit;
	if(WeckzeitInMinuten < AktuelleZeitInMinuten)
        {
        	ReturnZeit = (1440 - AktuelleZeitInMinuten ) + WeckzeitInMinuten;         
        }
        else
		{
        	ReturnZeit = WeckzeitInMinuten - AktuelleZeitInMinuten;
        }

		return ReturnZeit;
}

bool Wecker::SetWeckzeit(std::string Zeit)
{

	time_t T = time(NULL);
	struct tm tm = *localtime(&T);

	int currentH = tm.tm_hour;
	int currentM = tm.tm_min;
    
	int h = stoi(Zeit.substr(0, 2));
	int m = stoi(Zeit.substr(2, 2));

	if(h > 23)
	{
		std::cout << "[-] Fehler Stunde ist größer als 24: h = " << h << "\n";
		return false;
	}

	if(m > 60)
	{
		std::cout << "[-] Fehler Minute ist größer als 60: m = " << m << "\n";
		return false;
	}

	int WeckzeitInMinuten = h * 60 + m;
	int AktuelleZeitInMinuten = currentH * 60 + currentM;
	int WarteZeitInMinuten;
        
		if(WeckzeitInMinuten < AktuelleZeitInMinuten)
        {
        	WarteZeitInMinuten = (1440 - AktuelleZeitInMinuten ) + WeckzeitInMinuten;         
        }
        else
		{
        	WarteZeitInMinuten = WeckzeitInMinuten - AktuelleZeitInMinuten;
        }
          
	std::cout << "[+] WarteZeit ist " << WarteZeitInMinuten << "\n";

	struct itimerspec WeckZeit;
	WeckZeit.it_interval.tv_sec = 0;
	WeckZeit.it_interval.tv_nsec = 0;
	WeckZeit.it_value.tv_sec = WarteZeitInMinuten * 60;
	WeckZeit.it_value.tv_nsec = 0;

	if(timer_settime(GetTimerID(), 0, &WeckZeit, NULL) != 0)
	{
		std::cout << "[+] Fehler Timerzeit konnte nichteingestellt werden\n";
		return false;
	}
	return true;
}

bool Wecker::AddDelayTime(std::string Zeit)
{
	time_t T = time(NULL);
	struct tm tm = *localtime(&T);

	int currentH = tm.tm_hour;
	int currentM = tm.tm_min;
    
	int h = stoi(Zeit.substr(0, 2));
	int m = stoi(Zeit.substr(2, 2));

	if(h > 23)
	{
		std::cout << "[-] Fehler Stunde ist größer als 24: h = " << h << "\n";
		return false;
	}

	if(m > 60)
	{
		std::cout << "[-] Fehler Minute ist größer als 60: m = " << m << "\n";
		return false;
	}

	int AktuelleZeitInMinuten = currentH * 60 + currentM;
	int Delay = ZeitDifferenz(AktuelleZeitInMinuten, h*60 + m);
	std::cout << "{+] DELAY " << Delay << "\n";

	struct itimerspec DelayZeit;
	timer_gettime(GetTimerID(), &DelayZeit);
	DelayZeit.it_value.tv_sec += h*60*60 + m*60;

	if(timer_settime(GetTimerID(), 0, &DelayZeit, NULL) != 0)
	{
		std::cout << "[+] Fehler Timerzeit konnte nichteingestellt werden\n";
		return false;
	}

	return true;
}


bool Wecker::GetTimerExpiration()
{
	itimerspec ExpirationZeit;
	timer_gettime(GetTimerID(), &ExpirationZeit);

	std::cout << ExpirationZeit.it_value.tv_sec << "  " << ExpirationZeit.it_value.tv_nsec << "\n";
	return true;
}
