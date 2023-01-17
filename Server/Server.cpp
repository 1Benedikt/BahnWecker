#include "Server.h"

std::mutex tLock;
bool tSetupFinished;
std::condition_variable tCondition;

namespace Server
{

	bool Send(std::string Request)
	{

	}

	HTTPRequest::HTTPRequest(char* Message, int len)
	{
		//std::cout << Message;
		Length = len;
		char space = '\x20';
		long i =  (long) (std::find(Message, Message + len, space) - Message);
		std::string str( Message, len );
		//std::cout << "STR string" << "\n";
		long j =(int) (std::find( 1 + i + Message, Message + len, space) - Message);
		std::cout << j - i - 1 << "\n";
		Path.append((char*)(Message + i +1), j -i - 1 ); //
		//std::cout << "PATH STRING" << "\n";
		if(i == 3)
		{
			if(strncmp(Message, "GET", 3) == 0)
			{
				std::cout << "[+] GET" << "\n";
				Method = METHOD::GET;
			} else if(strncmp(Message, "PUT", 3) == 0)
			{
				int zaehl = 0;
				Method = METHOD::PUT;
			
				int c = str.find("Content-Length:");
				int k = str.find("\r", c);
				c += 17;
					for(int j = (k - c); j >= 0; --j)
				{
				    //std::cout << Message[c -1 + j];
				    //std::cout << ((Message[c -1 + j] - 0x30)) * pow(10, (k - c) - j )<< "\n";
					ContentLength +=  ((Message[c -1 + j] - 0x30)) * pow(10, (k - c) - j );
				}
	
				std::string temp(Message + (len - ContentLength - 1 ), ContentLength + 1);
				Content = temp;
				//std::cout << Content;

			} 
			else
			{
				Method = METHOD::UNSUPPORTED;
			}
		} else
		{
			Method = METHOD::UNSUPPORTED;
		}

		//std::cout << "REQUESt FERTIG" << "\n";
		
	}

	HTTPResponse::HTTPResponse(HTTPRequest Request)
	{
		//std::cout << "RESPONSE" << "\n";
		if(Request.Method == METHOD::GET)
		{
			std::stringstream RequestBuffer;
			std::ifstream GetFile;
			GetFile.open(Request.Path, std::ios::in);
			if(GetFile.is_open())
			{
				RequestBuffer << GetFile.rdbuf();
				Content = RequestBuffer.str();
				std::cout << Content;
				GetFile.seekg(0, std::ios::end);
				ContentLength = GetFile.tellg();
				GetFile.close();
				ContentType = Server::CONTENTTYPE::XML;
				Status = OK;
			} else
			{
				std::cout << "File konnte nicht geöffnet werden\n";
			}
		} else if(Request.Method == METHOD::PUT)
		{
			std::ofstream PutFile;
			
			PutFile.open(Request.Path, std::ios::out | std::ios::trunc);
			if(PutFile.is_open())
			{
				if(Request.Path == "TestPut")
				{
					std::cout << "[+] Lock tSetupFinished\n";
					std::unique_lock<std::mutex> lock(tLock);
					tSetupFinished = false;
				}
				PutFile << Request.Content;
				PutFile.close();
				if(XMLHelper::IsUserInformationFileValid())
				{
					std::cout << "[+] Benutzerinformationen wurden erfolgreich geändert\n";
					std::unique_lock<std::mutex> lock(tLock);
					tSetupFinished = true;
					tCondition.notify_one();
					kill(getpid(), SIGUSR1);
				}
				Status = CREATED;
				ContentLocation = Request.Path;
				
			}
		}
		//std::cout << "RESPONSEBUFFER SCHREIBEN" << "\n";
		ResponseBuffer = "HTTP/1.1 ";
		ResponseBuffer.append(std::to_string(Status));
		if(Status == OK)
		{
			ResponseBuffer.append(" OK\r\n");
		}else if(Status == CREATED)
		{
			ResponseBuffer.append(" Created\r\n");
		}else if(Status == NOTFOUND)
		{
			ResponseBuffer.append(" Not Found\r\n\r\n");
		}

		if(Request.Method == METHOD::GET && Status != NOTFOUND)
		{
			ResponseBuffer.append("Content-Length: ").append(std::to_string(ContentLength)).append("\r\n\r\n");
			ResponseBuffer.append(Content);
		}else if(Request.Method == METHOD::PUT && Status != NOTFOUND)
		{
			ResponseBuffer.append("Content-Location: ").append(ContentLocation).append("\r\n\r\n");
		}

		//std::cout << '\n';
		//std::cout << ResponseBuffer << "\n ENDE";

		
	}


	

	void ClientHandler(void* HServer)
	{
		HTTPServer* Server = (HTTPServer*) HServer; 
		char response[512];
		int i = 0;
		while(i = recv(Server->SocketToClient, response, 512, 0), i > 0)
		{
			//std::cout << i << "\n";
			HTTPRequest Request(response, i );
			HTTPResponse Response(Request);
			send(Server->SocketToClient, Response.ResponseBuffer.data(), Response.ResponseBuffer.size(), 0);
		}
		std::cout << "[-] Client hat die Verbindung unterbrochen\n";
		close(Server->SocketToClient);

	}

	int HTTPServer::Start()
	{
		std::cout << "[+] Server started\n";
		//Deklaration von Sockets und dem Thread für den Client
		SOCKET ConnectSocket;
		struct sockaddr_in Server, Client;

		//Initialisierung vom Socket und von den sockaddr
		ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
 
		if(ServerSocket == -1)
		{
			std::cout << "[-] Socket konnte nicht erstellt werden\n";
		}


		Server.sin_family = AF_INET;
		Server.sin_addr.s_addr = INADDR_ANY;
		Server.sin_port = htons(8888);

		//Socket binden
		if(bind(ServerSocket, (struct sockaddr*) &Server, sizeof(Server)) < 0)
		{
			std::cout << "[-} Socket konnte nicht gebindet werden\n";
		}

		std::cout << "[+] Socket erfolgreich gebunden\n";
		
		//Server "aktivieren"
		listen(ServerSocket, 1);

		std::cout << "[+] Server lauscht jetzt für eingehende Verbindungen \n";

		int ClientSize = sizeof(struct sockaddr_in);
				
		//Verbindung vom Client akzeptieren
		while(true)
		{
			SocketToClient =  accept(ServerSocket, (struct sockaddr*)&Client, (socklen_t*)&ClientSize);
			std::cout << "[+] Verbindung erstellt\n";


			ClientThread = new std::thread(ClientHandler, (void*)this);
	
			std::cout << "[+] Socket wurde Thread zugewiesen\n";
			ClientThread->join();
		}
		//ÜBErARBEITEN START WIRD SOLANGE DIE MAIN BLOCKIEREN BIS THREAD GEJOINT IST

		return EXIT_SUCCESS;

	}

	HTTPServer::HTTPServer()
	{	
		ServerThread = new std::thread(&HTTPServer::Start, this);
		std::cout << "[+] Thread for Server created";
		
	}

	HTTPServer::~HTTPServer()
	{
		std::cout << "[+] Destruktor wurde aufgerufen\n";
		ClientThread->join();
		std::cout << "[+] Client Thread ist gejoint\n";
		ServerThread->join();
		std::cout << "[+] Destruktor ist fertig\n";
	}

}
