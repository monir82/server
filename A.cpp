#include<bits/stdc++.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
int masterfd,pos;
map<string,int>node;


string trim(const string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void *ManageClient(void * client){
	int *c=(int*) client;
	int clnt=(*c);
	char buffer[1024];
	bzero(buffer,sizeof(buffer));
	read(clnt,buffer,sizeof(buffer));
	string username=buffer;

	node.insert({username,clnt});
	cout<<"New connection from "<<username<<endl;
	ssize_t byte;
	while(1){
		byte=read(clnt,buffer,sizeof(buffer));
		if(byte==-1){
			perror("Read failed\n");
			exit(1);
		}
		if(byte==0){
			close((clnt));
			node.erase(username);
			cout<<"Client Disconnected\n"<<endl;
			break;
		}
		json request;
		printf("%s\n", buffer);
		request=json::parse(buffer);
		string sender,reciever,massage;
		sender=request["sender"];
		reciever=request["reciever"];
		massage=request["massage"];
		string response;
		response+="{\"sender\":\""+sender+"\",\"Msg\":\""+massage+"\"}";
		if(node.find(reciever)!=node.end()){
			cout<<node[reciever]<<endl;
			send(node[reciever],response.c_str(),byte,0);
			bzero(buffer,sizeof(buffer));
		}
	}
	return NULL;
}

void *server(void *arg){
	masterfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server,client;

	memset(&server,0,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl("35.224.16.169");
	server.sin_port=htons(4545);
	int enable = 1;
	setsockopt(masterfd, SOL_SOCKET, SO_REUSEADDR,&enable, sizeof(enable));
	if(bind(masterfd,(struct sockaddr*)&server,sizeof(server))<0)
		perror("[-] Bind failed...\n");
	else{
		cout<<"[+] Bind successfull...\n";
		if(listen(masterfd,100)<0)
			perror("[-] Listen failed...\n");
		else{
			cout<<"[+] Listenning...\n";
			socklen_t clilen=sizeof(client);
			
			while(1){
				int client=accept(masterfd,(struct sockaddr*) &client,&clilen);
				pthread_t CLIENT;
				pthread_create(&CLIENT,NULL,ManageClient,(void*)&client);
			}
		}
	}

	return arg;
}
int main(){
	pthread_t t1;
	int ret=pthread_create(&t1,NULL,&server,NULL);
	if(ret!=0)
		perror("Thread create");
	pthread_exit(NULL);
}
