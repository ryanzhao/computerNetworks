//  Traceroute server utility, daemon on default port at 12160
//  The base's code is from Beej's guide of a server.c example on
//  Chapter 6, and we extended upon that
//  Route map 1~4 finished by Kang Zhao, 2012/09/21
//  Route map 5~6 finished by Weiran Zhao, by 2012/09/24
//  Route map 7 finished by Kang Zhao and Weiran Zhao by 2012/09/25


//---------------------------references-------------------------------------
//  1.  Beej's guide to network programming:
//  http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
//  2.  How to do command-line parsing using c:
//  http://www.codingunit.com/c-tutorial-command-line-parameter-parsing

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>

#include "utils.h"
#include "keeplog.h"

int main(int argc, char *argv[])
{
    //server starts up
    char msg[BUFSIZE];
    bzero(msg, BUFSIZE);
    sprintf(msg, "Server starts up\n");
    writelog(msg);
    
    //-------------------------------------
    //socket related variables
    //-------------------------------------    
    // listen on sock_fd, new connection on new_fd
	int sockfd, new_fd;  
	struct addrinfo hints, *servinfo, *p;
    // connector's address information
	struct sockaddr_storage their_addr; 
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char client_address[INET6_ADDRSTRLEN];
	int rv;    
    
    //get commnad line options
    setdfltvals();
    int allowable_users=users;
    while ((argc > 1) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
			case 'p':
                memcpy(port, argv[2], strlen(argv[2])+1);
                argc-=2;
                argv+=2;
				break;
                
			case 'u':
                users   =   atoi(argv[2]);
                argc-=2;
                argv+=2;
				break;
                
            case 'r':
                requests    =   atoi(argv[2]);
                interval    =   atoi(argv[3]);
                argc-=3;
                argv+=3;
                break;
                
            case 's':
                strict  =   atoi(argv[2]);
                argc-=2;
                argv+=2;
                break;
                
			default:
                bzero(msg,BUFSIZE);
                sprintf(msg, "Wrong arguments: %s\n",argv[1]);
                writelog(msg);
				usage();
                exit(1);
		}
	}
    //log basic argument
    bzero(msg, BUFSIZE);
    sprintf(msg, "Listening port: %s\n", port);
    writelog(msg);    
    bzero(msg, BUFSIZE);
    sprintf(msg, "Rate limiting: %d requests per %d seconds\n", requests,interval);
    writelog(msg);
    bzero(msg, BUFSIZE);
    sprintf(msg, "Maximum number of users: %d\n", users);
    writelog(msg);
    bzero(msg, BUFSIZE);
    sprintf(msg, "Strict Destination (false/true=0/1): %d\n", strict);
    writelog(msg);
    
    //set up address info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    // use my IP
	hints.ai_flags = AI_PASSIVE; 
    
	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        bzero(msg, BUFSIZE);
        sprintf(msg, "Get addrinfo error: %s\n", gai_strerror(rv));
		writelog(msg);
		exit(1);
	}
    
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
    {
		if ((sockfd = socket(p->ai_family,
                             p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            bzero(msg, BUFSIZE);
            sprintf(msg, "Socket error: %s\n", strerror(errno));
			writelog(msg);
			continue;
		}
		//setsockopt: Handy debugging trick that lets us rerun the
        //server immediately after we kill it;otherwise we have to
        //wait about 20 secs.
        //Eliminates "ERROR on binding: Address already in use" error.
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
        {
			bzero(msg, BUFSIZE);
            sprintf(msg, "Set socket options error: %s\n", strerror(errno));
			writelog(msg);
			exit(1);
		}
        
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
			close(sockfd);
            bzero(msg, BUFSIZE);
            sprintf(msg, "Bind error: %s\n", strerror(errno));
			writelog(msg);
			continue;
		}
        
		break;
	}
    
	if (p == NULL)
    {
		bzero(msg, BUFSIZE);
        sprintf(msg, "Server failed to bind\n");
        writelog(msg);
		exit(1);
	}
    
	freeaddrinfo(servinfo);
    
	if (listen(sockfd, backlog) == -1)
    {
		bzero(msg, BUFSIZE);
        sprintf(msg, "Listening error: %s\n", strerror(errno));
        writelog(msg);
		exit(1);
	}
    
    // reap all dead processes
	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
		bzero(msg, BUFSIZE);
        sprintf(msg, "Sigaction error: %s\n", strerror(errno));
        writelog(msg);
		exit(1);
	}
    
    bzero(msg, BUFSIZE);
    sprintf(msg, "Start waiting for connections\n");
    writelog(msg);
    
    //-------------------------------------
    //sending receiving variables
    //-------------------------------------
    //# bytes server received
	int status;    
    // client's input
	char client_input[BUFSIZE];
    // name of possible file for traceroute <file>
    char fname[BUFSIZE];
    // used to store source ip
    char src_ip[INET6_ADDRSTRLEN];
    
	while(1)
    {
        // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
        {
			bzero(msg, BUFSIZE);
            sprintf(msg, "Accept error: %s\n", strerror(errno));
			writelog(msg);
			continue;
		}
        
        //time-out
		if((setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(struct timeval)))==-1)
        {
			bzero(msg, BUFSIZE);
			sprintf(msg, "setsockopt SO_RCVTIMEO error: %s\n", strerror(errno));
			writelog(msg);
		}
        
        //get address from their representation
		inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  client_address,
                  sizeof client_address);
        
        bzero(msg,BUFSIZE );
        sprintf(msg, "Got connection from %s at port %d\n", client_address, ntohs(((struct sockaddr_in*)&their_addr)->sin_port));
        writelog(msg);
        
        if (users>0)
        {
            users--;
            pid_t childpid;
            if((childpid=fork())==-1)
            {
                bzero(msg, BUFSIZE);
                sprintf(msg, "Fork error: %s\n", strerror(errno));
                writelog(msg);
                exit(errno);
            }
            
            //taking care of child
            if(childpid==0)
            {
                //child doesn't listen
                close(sockfd);
                //variable for rate control
                raterecord* rr=rrinit();
                while (1)
                {
                    bzero(client_input, BUFSIZE);
                    if((status = (int)(recv(new_fd,client_input,BUFSIZE,0)))== -1)
                    {
                        tomessage(new_fd);
                        exit(0);
                    }
                    
                    //check if client closes itself
                    if (status==0)
                    {
                        bzero(msg, BUFSIZE);
                        sprintf(msg, "Client closed connections\n");
                        writelog(msg);
                        close(new_fd);
                        exit(0);
                    }
                    
                    bzero(msg, BUFSIZE);
                    sprintf(msg, "Received %d bytes: %s", status, client_input);
                    writelog(msg);
                    
                    //deals with different user input
                    switch (gettasktype(client_input, fname, src_ip))
                    {
                        case QUIT:
                            clientquit(new_fd);
                            close(new_fd);
                            exit(0);
                            break;
                            
                        case HELP:
                            printhelp(new_fd);
                            continue;
                            break;
                            
                        case TRACME:
                            traceme(new_fd, client_address,rr);
                            continue;
                            break;
                            
                        case TRACFL:
                            tracefile(new_fd,fname,rr);
                            continue;
                            break;
                            
                        case TRACSC:
                            tracesrcip(new_fd,src_ip, client_address, strict, client_input,rr);
                            continue;
                            break;
                            
                        case TRACNM:
                            tracenormal(new_fd,client_input,rr);
                            continue;
                            break;
                            
                        default:
                            invalidargs(new_fd);
                            continue;
                            break;
                    }   //for switch
                }       //end while(1) for receiving
                //free rate control variables
                freerecord(rr);
            }
            else
            {
                signal(SIGCHLD, limit_handler);
            }
        }
        else
        {
            //Maximum user limit reached
			limit_notify(new_fd, allowable_users);
			close(new_fd);
        }
	}
	return 0;
}

