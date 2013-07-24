//  For the purpose and use of each of the following functions
//  please check "utils.h"
#include "utils.h"

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
    {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
    
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void usage()
{
    printf("Usage, using Short Unix style\n");
    printf("-p <port number>\n");
    printf("-u <maximum number of users allowed>\n");
    printf("-r <number of requests> <number of seconds\n");
    printf("-s <strict destination, 0 or 1>\n");
}

void clientquit(int new_fd)
{
    char msg[BUFSIZE];
    bzero(msg, BUFSIZE);
    sprintf(msg,"Client requires disconnection\n");
    writelog(msg);
    
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    printf("bye\n");
    dup2(oldstdout, STDOUT_FILENO);
    close(oldstdout);
    bzero(msg, BUFSIZE);
    sprintf(msg,"Client closed connection\n");
    writelog(msg);}

void printhelp(int new_fd)
{
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    printf("Do <help> to see this message\n");
    printf("Do <traceroute me> to traceroute your client machine \n");
    printf("Do <traceroute [destination] (can be ip or hostname) to traceroute destination\n");
    printf("Do <traceroute [filename] to do a batch version of traceroute, but is limited to rate control\n");
    printf("Do <quit> to quit\n");
    dup2(oldstdout, STDOUT_FILENO);
    close(oldstdout);
}

void invalidargs(int new_fd)
{
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    printf("You gave an unknown command, please input again\n");
    dup2(oldstdout, STDOUT_FILENO);
    close(oldstdout);
    printhelp(new_fd);
}

void traceme(int new_fd, char* client_address, raterecord* rr)
{
    char *cmd= (char*) malloc(sizeof(char)*BUFSIZ);
    memset(cmd, 0, sizeof(char)*strlen(cmd));
    sprintf(cmd, "traceroute %s\n", client_address);
    //redirect output to client
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    if (ratelimit(rr)==OVERRATE)
    {
        printf("You are only allowed to do traceroutes %d times within %d seconds\n", rr->maxreq, rr->interval);
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Client requests over what is supposed to\n");
        writelog(msg);
    }
    else
    {
        printf("-------------------------------------------------------------------\n");
        system(cmd);
        printf("###################################################################\n");
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
    }
}

//traceroute based on file input
void tracefile(int new_fd, char* fname, raterecord* rr)
{
    char cmd[MAXLINELEN];
    memset(cmd, 0, MAXLINELEN);
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    FILE *fp=fopen(fname, "r");
    while (fgets(cmd, MAXLINELEN, fp)!=NULL)
    {
        if (ratelimit(rr)==OVERRATE)
        {
            printf("You are only allowed to do traceroutes %d times within %d seconds\n", rr->maxreq, rr->interval);
            dup2(oldstdout, STDOUT_FILENO);
            close(oldstdout);
            char msg[BUFSIZE];
            bzero(msg, BUFSIZE);
            sprintf(msg, "Client requests over what is supposed to\n");
            writelog(msg);
            return;
        }
        else
        {
            printf("-------------------------------------------------------------------\n");
            system(cmd);
            printf("###################################################################\n");
            memset(cmd, 0, MAXLINELEN);
        }
    }
    dup2(oldstdout, STDOUT_FILENO);
    close(oldstdout);
    fclose(fp);

}

//traceroute with -s src_ip option
void tracesrcip(int new_fd, char* src_ip, char* client_address, int strict, char* client_input, raterecord* rr)
{
    // if strict =1  we need to check src_ip
    if (strict==1)
    {
        // client_ip != src_ip
        if (strcmp(src_ip, client_address)!=0)
        {
            int oldstdout   =   dup(STDOUT_FILENO);
            if(dup2(new_fd, STDOUT_FILENO)==-1)
            {
                char msg[BUFSIZE];
                bzero(msg, BUFSIZE);
                sprintf(msg, "Dup2 error: %s\n", strerror(errno));
                writelog(msg);
                exit(errno);
            }
            printf("We only allow source ip the same as your ip!\n");
            dup2(oldstdout, STDOUT_FILENO);
            close(oldstdout);
            return;
        }
    }
    //we have to drop stupid /r/n for using ftp
    char* cmd=(char*)malloc(sizeof(char)*strlen(client_input));
    memset(cmd, 0, sizeof(char)*strlen(cmd));
    int idx=0;
    while (client_input[idx]!='\r')
    {
        cmd[idx]=client_input[idx];
        idx+=1;
    }
    cmd[idx]='\n';
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    if (ratelimit(rr)==OVERRATE)
    {
        printf("You are only allowed to do traceroutes %d times within %d seconds\n", rr->maxreq, rr->interval);
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Client requests over what is supposed to\n");
        writelog(msg);
    }
    else
    {
        printf("-------------------------------------------------------------------\n");
        system(cmd);
        printf("###################################################################\n");
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
    }
}

//a regular traceroute
void tracenormal(int new_fd, char* client_input, raterecord* rr)
{
    //we have to drop stupid /r/n for using ftp
    char* cmd=(char*)malloc(sizeof(char)*strlen(client_input));
    memset(cmd, 0, strlen(cmd));
    int idx=0;
    while (client_input[idx]!='\r')
    {
        cmd[idx]=client_input[idx];
        idx+=1;
    }
    cmd[idx]='\n';
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    if (ratelimit(rr)==OVERRATE)
    {
        printf("You are only allowed to do traceroutes %d times within %d seconds\n", rr->maxreq, rr->interval);
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Client requests over what is supposed to\n");
        writelog(msg);
    }
    else
    {
        printf("-------------------------------------------------------------------\n");
        system(cmd);
        printf("###################################################################\n");
        dup2(oldstdout, STDOUT_FILENO);
        close(oldstdout);
        free(cmd);
    }
}


int gettasktype(char* client_input, char* fname, char* src_ip)
{
    //input is quit
    if(strncmp(client_input, "quit", 4)==0)
    {
        return QUIT;
    }
    
    //input is help
    if (strncmp(client_input, "help", 4)==0)
    {
        return HELP;
    }
    
    //don't want to mess up with client_input when using strtok
    char* tmp;
    char* input=NULL;
    input   =   (char*)malloc(sizeof(char)*strlen(client_input));
    memset(input, 0, sizeof(char)*strlen(input));
    memcpy(input, client_input, strlen(client_input));
    char* delim="\r\n ";
    tmp=strtok(input, delim);
    
    //input unknown
    if (strncmp(tmp, "traceroute", 10)!=0)
    {
        free(input);
        return UNKN;
    }
    
    // check parameters after "traceroute"
    // we only allow "me" or "filename" to
    // appear right after "traceroute".
    // -s could be anyway
    int counter=1;
    tmp=strtok(NULL, delim);
    while (tmp!=NULL)
    {
        if (counter==1)
        {
            // check traceroute me
            if (strncmp(tmp, "me", 2)==0)
            {
                free(input);
                return TRACME;
            }
            //check file
            FILE* fp;
            if ((fp=fopen(tmp, "r"))!=NULL)
            {
                strcpy(fname, tmp);
                free(input);
                fclose(fp);
                return TRACFL;
            }
        }
        // check -s options
        if (strncmp(tmp, "-s", 2)==0)
        {
            tmp=strtok(NULL, delim);
            strcpy(src_ip, tmp);
            free(input);
            return TRACSC;
        }
        tmp=strtok(NULL, delim);
        counter++;
    }
    
    free(input);
    return TRACNM;    
}

void setdfltvals()
{
    strcpy(port, "1216");
    users       =   2;
    requests    =   4;
    interval    =   60;
    strict      =   0;
    backlog     =   10;
    rcv_timeout.tv_sec = 10;
	rcv_timeout.tv_usec = 0;
}

raterecord* rrinit()
{
    raterecord* rr=(raterecord*)malloc(sizeof(raterecord));
    memset(rr, 0, sizeof(raterecord));
    
    rr->maxreq=requests;
    rr->interval=interval;
    rr->reqidx=0;
    rr->reqlist=(int*)malloc(sizeof(int)*(requests+1));
    memset(rr->reqlist,-1 , sizeof(int)*(requests+1));
    
    return rr;
}

void freerecord(raterecord* rr)
{
    free(rr->reqlist);
    rr->reqlist=NULL;
    free(rr);
    rr=NULL;
}

int ratelimit(raterecord* rr)
{
    if (rr->reqidx<rr->maxreq)
    {
        rr->reqlist[rr->reqidx]=(int)time(NULL);
        rr->reqidx++;
        return RATEOK;
    }
    else
    {
        int curtime=(int)time(NULL);
        //not exceed rate limits
        if ((curtime-rr->reqlist[0])>=rr->interval)
        {
            //write to the final buffer first
            rr->reqlist[rr->maxreq]=curtime;
            int i;
            for (i=0; i<rr->maxreq; i++)
            {
                rr->reqlist[i]=rr->reqlist[i+1];
            }
            rr->reqidx++;
            return RATEOK;
        }
        else
        {
            return OVERRATE;
        }
    }
}

void limit_handler(int sig)
{
	pid_t cpid;
	cpid = wait(NULL);
    signal(SIGCHLD, limit_handler);
    users++;
}

void limit_notify(int new_fd, int allowable_users)
{
	int oldstdout = dup(STDOUT_FILENO);
	if (dup2(new_fd, STDOUT_FILENO) == -1)
    {
		char msg[BUFSIZE];
		bzero(msg, BUFSIZE);
		sprintf(msg, "Dup2 error: %s\n", strerror(errno));
		writelog(msg);
		exit(errno);
	}
	printf("Disconnected for server has reached maximum user: %d\n", allowable_users);
	dup2(oldstdout, STDOUT_FILENO);
	close(oldstdout);
    char msg[BUFSIZE];
    bzero(msg, BUFSIZE);
    sprintf(msg, "One of the client was rejected because of reaching limit users\n");
    writelog(msg);
}

void tomessage(int new_fd)
{
    int oldstdout   =   dup(STDOUT_FILENO);
    if(dup2(new_fd, STDOUT_FILENO)==-1)
    {
        char msg[BUFSIZE];
        bzero(msg, BUFSIZE);
        sprintf(msg, "Dup2 error: %s\n", strerror(errno));
        writelog(msg);
        exit(errno);
    }
    printf("Inactive for %ld seconds, Server closed connection\n", rcv_timeout.tv_sec);
    dup2(oldstdout, STDOUT_FILENO);
    close(oldstdout);
    char msg[BUFSIZE];
    bzero(msg, BUFSIZE);
    sprintf(msg, "Client automatic timeout, server closes connection\n");
    writelog(msg);
    close(new_fd);
}
