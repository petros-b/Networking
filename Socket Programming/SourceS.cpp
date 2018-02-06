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

struct entry {
	string first;
	string last;
	string num;
	int index = 0;
};


int main() {
	//create socket
	int s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == -1) {
		cerr << "Can't Initialize Socket! Quitting..." << endl;
		return 0;
	}
	//bind socket to ipadress and port
	struct sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.s_addr = INADDR_ANY; //this might be an issue

	bind(s, (struct sockaddr*)&hint, sizeof(hint));

	//tell the socket is for listening
	listen(s, SOMAXCONN);

	//wait for connection
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	int clientSocket = accept(s, (struct sockaddr*)&client, &clientSize);

	if (clientSocket == 0) {
		cerr << "Can't Initialize Client! Quitting..." << endl;
		return 0;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	memset(host,0, NI_MAXHOST);
	memset(service,0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << "Connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port " << ntohs(client.sin_port) << endl;
	}
	//close listening socket
	close(s);
	//while loop: accept and echo message back to client
	char buf[4096];

	entry add_book[20]; // address 
	
	fstream file;
	int bcount = 0;
	string file_line;
	int beg=0, end = 0;
	string sub = "";
	int i = 0;

	

	while (true) {
		
		memset(buf, 0,4096);

		//wait for client data
		int bytesRecv = recv(clientSocket, buf, 4096, 0); // COMMAND THAT TAKES USER INPUT
		file.open("book.txt"); //open address book 
		if (!file) { // exception handling on book 
			file.close();

		//std:ofstream file2("book.txt");
			//file2.close();
			file.open("book.txt");
		}
		else { cout << "file created and found"; }
		

		while (getline(file, file_line)) { //read in entries 
			end = 0;
			beg = 0;
			end = file_line.find(" ");
			sub = file_line.substr(beg, end - beg);
			stringstream(sub) >> add_book[bcount].index;
			beg = end;

			end = file_line.find(" ", beg + 1);
			sub = file_line.substr(beg + 1, end - beg);
			stringstream(sub) >> add_book[bcount].first;
			beg = end;

			end = file_line.find(" ", beg + 1);
			sub = file_line.substr(beg + 1, end - beg);
			stringstream(sub) >> add_book[bcount].last;
			beg = end;

			end = file_line.find(" ", beg + 1);
			sub = file_line.substr(beg + 1, end - beg);
			stringstream(sub) >> add_book[bcount].num;
			beg = end;

			bcount++;

		}

		if (bytesRecv == -1) {
			cerr << "Error in read(). Quitting..." << endl;
			break;
		}
		else if (bytesRecv == 0) {
			cout << "Client disconnected... " << endl;
			break;
		}
		else {

			string convert = string(buf, 0, bytesRecv);
			beg = 0; end = 0;
			bcount = 0;
			end = convert.find(" ");
			sub = convert.substr(beg, end);

			beg = end + 1;

			//add function

			if (sub == "ADD") {
				i = 0;
				while (add_book[bcount].index != 0 && bcount != 20) { //traverse to end of list on address book 
					bcount++;
				}
				add_book[bcount].index = 1000 + bcount;
				end = convert.find(" ", beg);
				sub = convert.substr(beg, end - beg);
				stringstream(sub) >> add_book[bcount].first;
				beg = end + 1;
				end = convert.find(" ", beg);
				sub = convert.substr(beg, end - beg);
				stringstream(sub) >> add_book[bcount].last;
				beg = end + 1;
				end = convert.find(" ", beg);
				sub = convert.substr(beg, end - beg);
				stringstream(sub) >> add_book[bcount].num;
				beg = end + 1;

				sub = "200 OK \nThe new Record ID is " + add_book[bcount].index;
				memset(buf, 0, 4096);
				for (int k = 0; k < sub.length(); k++) {
					buf[k] = sub[k];
					i++;

				}
			}
			//delete function
			else if (sub == "DELETE") {
				i = 0;
				end = convert.find(" ", beg);
				sub = convert.substr(beg, end - beg);


				while (add_book[bcount].index != stoi(sub) && bcount != 20) { //traverse to end of list on address book 
					bcount++;
				}
				add_book[bcount].index = 0; // delete info from addr book 
				memset(buf, 0, 4096);
				//traverse to end of list on address book

					
				sub = "200 OK \nRecord Deleted \n";
				for (int k = 0; k < sub.length(); k++) {
					buf[i] = sub[k];
					i++;

				}

			}
			else if (sub == "LIST") {
				i = 0;
				memset(buf, 0, 4096);
				while (bcount < 20) { //traverse to end of list on address book

					if (add_book[bcount].index != 0) {

						sub = add_book[bcount].index + " " + add_book[bcount].first + " " + " " + add_book[bcount].last + " " + add_book[bcount].num + "\n";

						for (int k = 0; k < sub.length(); k++) {
							buf[i] = sub[k];
							i++;

						}

					}

					bcount++;
				}
			}
			else if (sub == "SHUTDOWN") {
				i=0;
				memset(buf, 0, 4096);
				close(clientSocket);
				close(s);
				break;
				sub = "200 OK \nServer Shutdown \n";
				for (int k = 0; k < sub.length(); k++) {
					buf[i] = sub[k];
					i++;

				}
				send(clientSocket, buf, i + 1, 0);
			}
			else{
				i=0;
				memset(buf, 0, 4096);
				sub = "Error: Invalid Command \n";
				for (int k = 0; k < sub.length(); k++) {
					buf[i] = sub[k];
					i++;

				}
			}
			
		
			//list function
			//shutdown
			//quit

			if (sub != "QUIT" && sub != "SHUTDOWN") {
				bcount = 0;

			std:ofstream file2("book.txt");
				while (bcount < 20) { //read in entries 
					if (add_book[bcount].index != 0) {

						file2 << add_book[bcount].index << " " << add_book[bcount].first << " " << add_book[bcount].last << " " << add_book[bcount].num << "\n";
						add_book[bcount].index = 0;
					}

					bcount++;

				}
				bcount = 0;
				file.close();
				file2.close();
				send(clientSocket, buf, i + 1, 0); //COMMAND THAT SHOOTS CLIENT MESSAGE BACK OUT echo message back to client
			}
			else if(sub=="QUIT"){//tell winsock the socket is for listening
				close(clientSocket);
				cout << "Client Disconnected \n";
				memset(buf, 0, 4096);
				int s = socket(AF_INET, SOCK_STREAM, 0);
				sockaddr_in hint;
				hint.sin_family = AF_INET;
				hint.sin_port = htons(54000);
				hint.sin_addr.s_addr = INADDR_ANY;


				bind(s, (sockaddr*)&hint, sizeof(hint));

				//tell winsock the socket is for listening
				listen(s, SOMAXCONN);

				//wait for connection
				sockaddr_in client;
				socklen_t clientSize = sizeof(client);
				int clientSocket = accept(s, (sockaddr*)&client, &clientSize);


			}
		}

		}
		return 0;
	}