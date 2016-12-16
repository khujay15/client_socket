#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
typedef uint32_t in_addr_t;

using namespace std;


int main(int argc, char *argv[])
{
    int port;
    string ip_to_connet;
    in_addr_t conv_addr;
    if(argc >2)
    {
        ip_to_connet=(char*)inet_addr(argv[1]);
    }
    else
    {
        cout<<"enter ip address: ";
        cin>>ip_to_connet;

        cout<<"enter port number: ";
        cin>>port;
        conv_addr=inet_addr(ip_to_connet.c_str());
    }
    int sd=socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        cout<<errno<<endl;
        exit(0);
    }


    sockaddr_in addr;   //socket address
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr=conv_addr;   //IP address in hex
    addr.sin_port=htons(port);    //port number

    if( connect(sd,(sockaddr*)&addr,sizeof(addr)) == -1 )  // if error, return -1
    {
        cout<< strerror(errno) <<endl;
    }
    //char http_request[]= "GET / HTTP/1.1\r\nHost: test.gilgil.net\r\n\r\n";

    if(fcntl(sd,F_SETFL,O_NONBLOCK) == -1)
    {
        cout<<"error: non block"<<endl;
        exit(0);
    }


    cin.ignore();
    char http_request[256];
    cout<<"enter request: ";
    cin.getline(http_request,256);

    if( send(sd,http_request,strlen(http_request),0) == -1 )
    {
        cout<< strerror(errno)<<endl;
        exit(0);
    }

    while(1)
    {
    static const int BUFFER_SIZE=256;
    char recv_buf[BUFFER_SIZE];
    ssize_t res=recv(sd,recv_buf,BUFFER_SIZE-1,0);
    if(res == -1 || res ==0 )
    {
        cout<< strerror(errno) <<endl;
        break;
    }
    recv_buf[res]='\0';
    cout << recv_buf << endl;

    //cin.ignore();
    string request;
    cout<<"enter request: ";
    cin.getline((char*)request.c_str(),256);
    if(request.c_str() =="quit")
        break;

    if(send(sd,(char*)request.c_str(),strlen(request.c_str()),0) == -1 )
    {
        cout<< strerror(errno)<<endl;
        exit(0);
    }
    }
    close(sd);


    return 0;
}
