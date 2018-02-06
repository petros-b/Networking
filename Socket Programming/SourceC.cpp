#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

using namespace std;

int main()
{
	string ipAdd = "127.0.0.1"; // ip address of the server
	int port; // listening to port num on server 
	cout << "Welcome, Please Enter Port Number: ";
	cin >> port;

	while(port !=54000){
		cout <<endl<<"Error: Invalid port Num" << endl << "Please Enter Valid Port Number (54000):";
		cin >>port;
	}	
	
					  
	//create socket

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		//cerr << "can't create socket, Err #" << connres.perror() << endl;
		return 0;
	}

	//fill in a hint structure

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdd.c_str(), &hint.sin_addr);

	//connect to server

	int connres = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connres == -1) {
		cerr << "can't connect to server, err #" << endl;

		close(sock);
		return 0;
	}
	//dowhile loop to send/receive data 

	char buf[4096];

	string userInput;

	do {
		//prompt user for some text
		cout << ">";

		getline(cin, userInput);
		if (userInput.size() > 0)
		{
			
			//send the text 
				int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
				if (sendResult != -1)
				{
					memset(buf,0, 4096);
					int bytesRec = recv(sock, buf, 4096, 0);
					if (bytesRec > 0)
					{
						cout<< string(buf, 0, bytesRec) << endl;
					}
				}
			
		}
		else if (userInput.size() == 0) 
		{
			;
		}
		//wait for response
		//echo response

	} while (userInput!="QUIT");
	cout << "Client Signed Out" << endl;
	close(sock);
	//gracefully close everything

}