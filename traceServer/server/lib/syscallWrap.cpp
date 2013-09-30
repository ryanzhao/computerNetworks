#include"syscallWrap.h"
#include<cstdio>
#include<cstdlib>
#include<unistd.h>

void sys_err(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int Socket(int family, int type, int protocol) {
    int n;
    if( (n=socket(family, type, protocol)) < 0) {
        sys_err("call to socket() failed");
    }
    return n;
}

int Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
    int n;
    if( (n=bind(socket, address, address_len)) < 0) {
        sys_err("call to bind() failed");
    }
    return n;
}

int Listen(int socket, int backlog) {
    int n;
    if( (n=listen(socket, backlog)) < 0) {
        sys_err("call to listen() failed");
    }
    return n;
}

int Accept(int socket, struct sockaddr *address, socklen_t *address_len) {
    int n;
    if( (n=accept(socket, address, address_len)) < 0) {
        sys_err("call to accept() failed");
    } 
    return n;
}

int Close(int fildes) {
    int n;
    if( (n=close(fildes)) < 0) {
        sys_err("call to close() failed");
    }
    return n;
}

Sigfunc * signal(int signo, Sigfunc *func) {
    // Reliable version of signal(), using POSIX sigaction(). 
    struct sigaction    act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef  SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return(oact.sa_handler);
}

Sigfunc *Signal(int signo, Sigfunc *func) {
    Sigfunc *ret;
    if( (ret=signal(signo, func))==SIG_ERR) {
        sys_err("call to signal() failed");
    }
    return ret;
}
