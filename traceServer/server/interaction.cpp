//----------------------------------------------------------------------------
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Thu,Oct 10th 2013 11:15:36 PM EDT
// Modified: Thu,Oct 10th 2013 11:18:09 PM EDT 
//           add client addr as a private member, for traceMe()
// Modified: Fri,Oct 11th 2013 04:38:15 PM EDT
//           add rate limiting functionality
//           rate limitor is mainly handled by traceIpHost();
// Last Modified: Fri,Oct 11th 2013 08:42:41 PM EDT
//----------------------------------------------------------------------------
#include"interaction.h"
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>
#include"misc.h"
interaction::interaction(int fd, struct sockaddr_in *cliaddr, rateLimiting* rl){
    rLimitor = rl;
    connfd = fd;
    clientAddr = cliaddr;
    memset(inBuff,0, BUFSIZE);
    memset(ipHostFname,0, BUFSIZE);
}
void interaction::takeAction() {
    int status;
    switch((status=parseInput())) {
        case QUIT:
            quit();
            break;
        case HELP:
            help();
            break;
        case ME:
            traceMe();
            break;
        case FNAME:
            traceFname();
            break;
        case IPHOST:
            traceIpHost(ipHostFname);
            break;
        case INVLD:
            invalid();
            break;
        default:
            break;
    }
}
int interaction::parseInput() {
    char* pch;
    const char* delim=" ";
    // delete trailing \r\n added from telnet
    if(strlen(inBuff)>=2) {
        memset(inBuff+(strlen(inBuff)-2), 0, 2);
    }
    //--------------
    // start parsing
    //--------------
    pch=strtok(inBuff,delim);
    if(pch!=NULL) {
        // input is 'quit'
        if(strcmp(pch,"quit")==0) {
            // make sure there's not any words after 'quit'
            pch=strtok(NULL,delim);
            if(pch!=NULL) {
                return INVLD;
            } else {
                return QUIT;
            }
        } // input is 'quit'
        // input is 'help'
        if(strcmp(pch,"help")==0) {
            // make sure there's not any words after 'help'
            pch=strtok(NULL,delim);
            if(pch!=NULL) {
                return INVLD;
            } else {
                return HELP;
            }
        } // input is 'help'
        // input is 'traceroute'
        if(strcmp(pch,"traceroute")==0) {
            // need to next word
            pch=strtok(NULL,delim);
            // if nothing followed 'traceroute'
            if(pch==NULL) {
                return INVLD;
            }
            // if is 'traceroute me'
            if(strcmp(pch,"me")==0) {
                pch=strtok(NULL,delim);
                // make sure nothing followed 'me'
                if(pch!=NULL) {
                    return INVLD;
                } else {
                    return ME;
                }
            } else { // 'traceroute' followed by 'filename' or 'ip/host'
                memset(ipHostFname,0,BUFSIZE);
                strcpy(ipHostFname, pch);
                // if that is a filename
                if(access(ipHostFname,R_OK)==0) {
                    return FNAME;
                } else { // otherwise, treat as ip/host name
                    return IPHOST;
                }
                //------------------------------------------------
                // the rest following 'traceroute' will be checked
                // by traceroute utility on server when called
                //------------------------------------------------
            } // traceroute 'me' or not
        } // input is 'traceroute'
        return INVLD;
    }
    return INVLD;
}

void interaction::quit() {
    dprintf(connfd, "QUIT Request from client received, quitting...\n");
    // log this event
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr->sin_addr), str,sizeof(str));
    extern eventsLog servLog;
    servLog.logIt("User from IP: %s (with pid %d)"
            "closes connection from server\n", str, getpid());
    Close(connfd);
    exit(0);
}

void interaction::help() {
    // I put necessary help info in a text file to shorten code
    FILE* fhelp = fopen("text/help.txt","r");
    if(fhelp==NULL) {
        cout<<"file: ./text/help.txt open failed"<<endl;
    }
    // read till the end of file
    while(Fgets(ipHostFname, BUFSIZE, fhelp)==1) {
        dprintf(connfd, "%s",ipHostFname);
    }
}

void interaction::traceIpHost(const char* ipHost) {
    // check if we over limit
    if(rLimitor->isOverLimit()) {
        // notify user
        dprintf(connfd, "*********************************************\n");
        dprintf(connfd, "Making too many requests than you can\n");
        dprintf(connfd, "%s\n",rLimitor->currentLimit());
        dprintf(connfd, "*********************************************\n");
        // log it
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr->sin_addr), str,sizeof(str));
        extern eventsLog servLog;
        servLog.logIt("User from IP: %s (with pid %d)"
                "making too many requests\n", str, getpid());
        return;
    }
    pid_t child_pid;
    if((child_pid=fork())<0) {
        sys_err("call to fork() failed");
    } else if(child_pid==0) { // in child
        // Close stuff from parents
        // redirect stderr and stdout to connfd
        Dup2(connfd, STDOUT_FILENO);
        Dup2(connfd, STDERR_FILENO);
        cout<<"------------------------------------------"
            <<"----------------------------------"<<endl;
        // call execlp 'NULL' must be provided
        execlp("traceroute","traceroute",ipHost, NULL);
    } else {// in parent
        if(VOMIT) {
            cout<<"client interaction process: "<<getpid()
                <<" forked a child with pid: "<<child_pid<<endl;
        }
        // block until child finish
        waitpid(child_pid, NULL, 0);
    }
}

void interaction:: invalid() {
    dprintf(connfd,"INVALID INPUT\n");
    help();
}

void interaction:: traceMe() {
    memset(ipHostFname, 0, BUFSIZE);
    inet_ntop(AF_INET, &(clientAddr->sin_addr), ipHostFname,BUFSIZE);
    traceIpHost(ipHostFname);
}

void interaction::traceFname() {
    FILE* fFname = fopen(ipHostFname,"r");
    if(fFname==NULL) {
        sys_err("call to fopen() failed");
    }
    // read till the end of file
    char* pch;
    const char* delim=" ";
    while(Fgets(ipHostFname, BUFSIZE, fFname)==1) {
        //---------------------------------
        // remove '\n' at the end of string
        //---------------------------------
        ipHostFname[strlen(ipHostFname)-1]=0;
        pch = strtok(ipHostFname,delim);
        //--------------------------------------------------------
        //This magic number 'ipHostFname+strlen(ipHostFname)+1' is
        //the address of strings followed 'traceroute'
        //--------------------------------------------------------
        char* magic = ipHostFname+strlen(ipHostFname)+1;
        if(strcmp(pch,"traceroute")==0) {
            // check if traceroute 'me'
            if(strcmp(magic,"me")==0) {
                traceMe();
            } else {
                traceIpHost(magic);
            }
        } else {
            dprintf(connfd, "INVALID INPUT in file: %s %s", ipHostFname, magic);
        }
    }
    fclose(fFname);
}
