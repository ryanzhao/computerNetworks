#include"progArgs.h"
#include<cstring>
#include<cstdlib>
#include<string>
#include<fstream>
rate::rate(int32_t r, int32_t s) {
    req = r;
    sec = s;
}

ostream& operator << (ostream & output, const rate& r) {
    output << r.req << "/" << r.sec;
    return output;
}

progArgs::progArgs(int argc, char** argv) {
    // set up default values first
    port = 1216;
    reqPerSec = new rate(4,60);
    maxUsers=2;
    strictDest=false;

    //--------------------------------
    // let's do input argument parsing
    //--------------------------------
    
    // put argv into 1-dim array
    int len_input=0;
    for(int i=1;i<argc;i++) {
        len_input+=strlen(argv[i])+1;
    }
    char* input;
    input = new char[len_input];
    strcpy(input,argv[1]);
    for(int i=2;i<argc;i++){
        strcat(input,"-");
        strcat(input,argv[i]);
    }
    // do parsing
    char* pch;
    const char* delim="-=,";
    pch = strtok(input,delim);
    while(pch!=NULL) {
        // check to see if there's port number
        if( (strcmp(pch,"PORT")==0) || 
            (strcmp(pch,"port")==0) || 
            (strcmp(pch,"p")==0)) {
            pch = strtok(NULL,delim);
            port = atoi(pch);
        }
        // check rate
        else if( (strcmp(pch,"RATE") == 0) ||
                   (strcmp(pch,"rate") == 0) ||
                   (strcmp(pch,"r") == 0)) {
            pch = strtok(NULL,delim);
            reqPerSec->setReq(atoi(pch));
            pch = strtok(NULL,delim);
            reqPerSec->setSec(atoi(pch));
        } 
        // check max_users
        else if( (strcmp(pch, "MAX_USERS") == 0) ||
                   (strcmp(pch, "max_users") == 0) ||
                   (strcmp(pch, "u") == 0)) {
            pch = strtok(NULL,delim);
            maxUsers = atoi(pch);
        }
        // check strict_dest
        else if( (strcmp(pch, "STRICT_DEST") == 0) ||
                   (strcmp(pch, "strict_dest") == 0) ||
                   (strcmp(pch, "s") == 0)) {
            pch = strtok(NULL,delim);
            if(atoi(pch) == 1) {
                strictDest = true;
            } else if(atoi(pch) == 0) {
                strictDest = false;
            } else {
                invalidInput();
            }
        }
        else {
            invalidInput();
        }
        pch = strtok(NULL,delim);
    }
}

progArgs::~progArgs() {
    delete reqPerSec;
}

void progArgs::usage() {
    ifstream infile("usage.txt");
    if( infile.is_open()) {
        string line;
        while( infile.good()) {
            getline(infile,line);
            cout<<line<<endl;
        }
        infile.close();
    } else {
        cerr<<"File 'usage.txt' open failed"<<endl;
        exit(EXIT_FAILURE);
    }
}

void progArgs::invalidInput() {
    cerr<<"Invalid Argument Input"<<endl;
    usage();
    exit(EXIT_FAILURE);
}
