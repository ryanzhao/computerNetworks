//----------------------------------------------------------------------------
// Check "syscallWrap.h" for details
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: the same day as "syscallWrap.h"
// Modified: Fri,Oct 11th 2013 11:29:11 AM EDT
//           Let Fgets() clear 'str' buffer each time it is called
// Last Modified: Fri,Oct 11th 2013 11:30:08 AM EDT
//----------------------------------------------------------------------------
#include"syscallWrap.h"
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<errno.h>

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

//----------------------------------------------------
// directly copied from Unix network programming Pg 78
//----------------------------------------------------
ssize_t Read(int fd, void *vptr, size_t n) {
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = (char*) vptr;
    nleft=n;
    // main loop to read
    while(nleft>0) {
        if( (nread = read(fd, ptr, nleft)) <0) {
            if (errno==EINTR) {
                // a way to tell to restart
                nread = 0;
            } else {
                return -1;
            }
        } else if (nread==0) {      // EOF
            break;
        }

        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

//----------------------------------------------------
// directly copied from Unix network programming Pg 78
//----------------------------------------------------
ssize_t Write(int fd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    char * ptr = (char*) vptr;
    nleft = n;
    // the main loop
    while( nleft > 0) {
        if( (nwritten = write(fd, ptr, nleft)) <=0) {
            if (errno = EINTR) {
                nwritten = 0;   // need to call write again
            }
            else {
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}
FILE* Fdopen(int fd, const char *mode) {
    FILE* f_fd=fdopen(fd, mode);
    if(f_fd==NULL) {
        sys_err("call to fdopen() failed");
    } else {
        return f_fd;
    }
}

int Fgets(char* str, int size, FILE* stream) {
    // clear str buffer
    memset(str,0,size);
    // clear errno
    int olderrno=errno;
    errno = 0;
    while(fgets(str,size,stream)==NULL) {
        if(errno == 0) {
            errno = olderrno;
            return 0;
        }
        if(errno == EINTR) {
            printf("fgets() get interrupted, restarting\n");
            continue;
        } else {
            sys_err("call to fgets() has error other than EINTR");
            return -1;
        }
    }
    return 1;
}

int Dup2(int fildes, int fildes2) {
    int status;
    status = dup2(fildes, fildes2);
    if(status == -1) {
        sys_err("call to dup2() failed");
    }
    return status;
}
