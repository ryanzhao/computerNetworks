//----------------------------------------------------------------------------
// Some of the helper functions such as signal handlers, automatic timing-out,
// and debug (VOMIT) macros, etc.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Wed,Oct 02th 2013 03:45:43 PM EDT
// Modified: Sun,Oct 13th 2013 04:30:53 PM EDT
//           Add userIp() function
// Last Modified: Sun,Oct 13th 2013 04:31:43 PM EDT
//----------------------------------------------------------------------------
#include"misc.h"
#include"lib/syscallWrap.h"
#include<sys/time.h>
#include<sys/select.h>
// this is for userIp() function
char str[INET_ADDRSTRLEN];
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
    // decrease concurrent number of users
    extern int numUsers;
    numUsers--;
}

void sigint_listen_handler(int sig) {
    extern eventsLog servLog;
    servLog.logIt("Server receive Interrupt signal, shutting down...\n");
    cout<<"\nInterrupt received, will terminate ..."<<endl;
    exit(0);
}

char* userIp(struct sockaddr_in cliaddr) {
    memset(str,0,sizeof(str));
    inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));
    return str;
}
