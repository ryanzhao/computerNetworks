#include"sockWrap.h"
#include<cstdio>
#include<cstdlib>

void sys_err(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int Socket(int family, int type, int protocol) {
    int n;
    if( (n=socket(family, type, protocol)) < 0) {
        sys_err("can't create socket");
    }
    return n;
}

int Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
    int n;
    if( (n=bind(socket, address, address_len)) < 0) {
        sys_err("can't bind socket");
    }
    return n;
}

int Listen(int socket, int backlog) {
    int n;
    if( (n=listen(socket, backlog)) < 0) {
        sys_err("call to listen failed");
    }
    return n;
}

int Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
    int n;
    if( (n=accept(socket, address, address_len)) < 0) {
        sys_err("call to accept failed");
    } 
    return n;
}
