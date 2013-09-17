//----------------------------------------------------------------------------
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: before Tue,Sep 17th 2013 09:39:19 AM EDT
// Last Modified: Tue,Sep 17th 2013 10:13:43 AM EDT
//----------------------------------------------------------------------------
#include"progArgs.h"
#include"eventsLog.h"
#include"lib/sockWrap.h"
#include<cstdlib>
#include<iostream>
#include<cstring>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<unistd.h>

using namespace std;
#define PORT 6666
#define LISTENQ 10

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
    // accept
    clilen = sizeof(cliaddr);
    // clilen is value-result parameters
    connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
    close(listenfd);
    write(connfd,"hell0 world\n", 15);
    close(connfd);

    // progArgs pa(argc,argv);
    // pa.checkArgs();
    // cout<<pa<<endl;
    // eventsLog el;
    // el.logIt("what the fuck");
    return 0;
}
