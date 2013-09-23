//----------------------------------------------------------------------------
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: before Tue,Sep 17th 2013 09:39:19 AM EDT
// Last Modified: Tue,Sep 17th 2013 10:50:11 PM EDT
//----------------------------------------------------------------------------
#include"progArgs.h"
#include"eventsLog.h"
#include"lib/syscallWrap.h"
#include<cstdlib>
#include<iostream>
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>

using namespace std;
#define PORT 6666
#define LISTENQ 10
#define VOMIT 1

int main(int argc, char** argv) {
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    // socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    // prepare servaddr for bind
    memset(&servaddr, sizeof(servaddr), 0);
    servaddr.sin_family = AF_INET;
    // bind to any address available
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    // bind
    Bind(listenfd, (SA *) & servaddr, sizeof(servaddr));
    // listen
    Listen(listenfd, LISTENQ);
    // seek chance to accept from client
    while(1) {
        // accept
        clilen = sizeof(cliaddr);
        // clilen is value-result parameters
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
        if(VOMIT) {
            cout<<"get connection from: "<<endl;
        }
        // set automatic timeout if no input were found
        int maxfdp1 = connfd+1;
        fd_set rset;
        FD_SET(connfd, &rset);
        struct timeval waittime;
        waittime.tv_sec=5;
        waittime.tv_usec=0;
        if( select(maxfdp1,&rset, NULL, NULL, &waittime) ==0) {
            const char* output="time out\n";
            write(connfd,output, strlen(output));
            Close(listenfd);
        }
        else {
            const char* output="some input\n";
            write(connfd,output, strlen(output));
            Close(listenfd);
        }
    }
    Close(connfd);

    // progArgs pa(argc,argv);
    // pa.checkArgs();
    // cout<<pa<<endl;
    // eventsLog el;
    // el.logIt("what the fuck");
    return 0;
}
