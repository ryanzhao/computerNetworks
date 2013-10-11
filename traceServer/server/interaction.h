//----------------------------------------------------------------------------
// This class take client input, and take corresponding action based on client
// input.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Morning of Thu,Oct 10th 2013 EDT
// Modified: Thu,Oct 10th 2013 11:18:09 PM EDT 
//           add client addr as a private member, for traceMe()
// Last Modified: Thu,Oct 10th 2013 11:36:46 PM EDT
//----------------------------------------------------------------------------
#ifndef _INTERACTION_H_
#define _INTERACTION_H_
#include"lib/syscallWrap.h"
class interaction {
    public:
        //-----------------------
        // const value definition
        //-----------------------
        // buffer size
        static const int BUFSIZE = 1024;
        // return code for parseInput() function
        static const int FNAME = 0;
        static const int IPHOST = 1;       // trace ip or host
        static const int ME = 2;
        static const int INVLD = -1;       // invalid input
        static const int QUIT = -2; 
        static const int HELP = -3;
    private:
        //-------------
        // class member
        //-------------
        // connected file descriptor, used to communicate with client
        int connfd;
        // input from client
        char inBuff[BUFSIZE];
        // ip or host or filename tmp buffer
        char ipHostFname[BUFSIZE];
        // client addr
        struct sockaddr_in *clientAddr;
        //-----------------------
        // private function calls
        //-----------------------
        //quit
        void quit();
        // print help
        void help();
        // invalid input
        void invalid();
        // trace ip/host in a file
        void traceFname();
        // trace ip or host
        void traceIpHost(const char* ipHost);
        // trace me
        void traceMe();
        // parse input
        int parseInput();
    public:
        interaction(int fd, struct sockaddr_in *cliaddr);
        // return inbuffer pointer
        char* getInBuff() { return inBuff;}
        // take action according to input
        void takeAction();
};
#endif 