#include"progArgs.h"
#include<cstdlib>
#include<iostream>
#include<cstring>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>

using namespace std;

int main(int argc, char** argv) {
    //int status;
    //struct addrinfo hints;
    //struct addrinfo *servinfo, *p;
    //char ipstr[INET6_ADDRSTRLEN];
    //memset(&hints, 0, sizeof(hints));

    //hints.ai_family = AF_UNSPEC;
    //hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE;

    //if((status = getaddrinfo(NULL,"1234",&hints, &servinfo))!=0) {
    //    cerr<<"getaddrinfo error"<<endl;
    //    exit(1);
    //}
    //for(p = servinfo; p != NULL; p = p->ai_next) {
    //    char* ipver = new char[5];
    //    void* addr;
    //    if(p->ai_family == AF_INET) {
    //        struct sockaddr_in *ipv4 = (struct sockaddr_in*) p->ai_addr;
    //        addr = &(ipv4->sin_addr);
    //        strcpy(ipver, "IPV4");
    //    } else {
    //        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
    //        addr = &(ipv6->sin6_addr);
    //        strcpy(ipver, "IPV6");
    //    }

    //    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
    //    cout<<ipver<<": "<<ipstr<<endl;
    //}
    //freeaddrinfo(servinfo);
    progArgs pa=progArgs(argc,argv);
    return 0;
}
