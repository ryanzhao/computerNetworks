#include"misc.h"
#include"lib/syscallWrap.h"
#include<sys/time.h>
#include<sys/select.h>
int setReadTimer(int fd, int sec) {
    int maxfdp1 = fd+1;
    fd_set readSet;
    //------------------------------------------------------------------
    // this set zero operation must be done to prevent strange behaviour
    //------------------------------------------------------------------
    FD_ZERO(&readSet);
    FD_SET(fd, &readSet);
    struct timeval waittime;
    waittime.tv_sec=sec;
    waittime.tv_usec=0;
    return select(maxfdp1,&readSet, NULL, NULL, &waittime);
}

void sigchld_listen_handler(int sig) {
    //----------------------------------------------------------------
    // read children's exit status in case they become zombies forever
    //----------------------------------------------------------------
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if(VOMIT) {
        cout<<"Child with pid: "<<pid<<" terminated"<<endl;
    }
}

void sigint_listen_handler(int sig) {
    cout<<"\nInterrupt received, will terminate ..."<<endl;
    exit(0);
}
