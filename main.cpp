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
#include <pthread.h>

typedef uint32_t in_addr_t;

using namespace std;

int sd;
static const int BUFFER_SIZE=256;
bool stop=false;

void* receving(void* data)
{
    char recv_buf[BUFFER_SIZE];
    ssize_t res;

    while(1)
    {
        res=recv(sd,recv_buf,BUFFER_SIZE-1,0);
        if(res == -1 || res ==0 )
        {
            //cout<< strerror(errno) <<endl;
            continue;
        }
        else if(stop)
        {
          break;
        }
        recv_buf[res]='\0';
        cout<<endl;
        cout << recv_buf << endl;
    }
}


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
    sd=socket(AF_INET, SOCK_STREAM, 0);
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
    cout<<endl;
    cout<<"enter request(To braodcast, @broadcast: ";
    cin.getline(http_request,256);


    if( send(sd,http_request,strlen(http_request),0) == -1 )
    {
        cout<< strerror(errno)<<endl;
        exit(0);
    }

    pthread_t p_thread;
    int thr_id;
    thr_id=pthread_create(&p_thread,NULL,receving,NULL);

    while(1)
    {
        char http[256];
        cin>>http;

        if(http=="quit")
        {
            stop=true;
            break;
        }

        if(send(sd,http,strlen(http),0) == -1 )
        {
            cout<< strerror(errno)<<endl;
            exit(0);
        }
    }

    close(sd);
    pthread_detach(p_thread);

    return 0;
}
