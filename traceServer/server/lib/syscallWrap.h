//-----------------------------------------------------------------------------
// As suggested by [1], it is good practice to have check return value of sock-
// related call. If those were done each time a call such as 'socket', 'bind' 
// made; code would look ugly. These set of functions are to take care of error
// checking. They follow the same function prototypes, except for the name. And 
// ONLY the first letter of the functions are changed to Capital letter. 
//      e.g. int socket(int, int, int); 
//           int Socket(int, int, int);
// 
//------------
// Reference: 
//------------
//  [1]: W. Richard Stevens, Unix network programming, Vol 1, 1998
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: 2 ~ 3 weeks before Wed,Oct 02th 2013 04:14:48 PM EDT
// Last Modified: Thu,Oct 10th 2013 10:11:09 PM EDT
//-----------------------------------------------------------------------------
#ifndef SYSCALL_WRAP_H_
#define SYSCALL_WRAP_H_
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>
#include<unistd.h>
#include<cstdio>

// avoid typing the long 'struct sockaddr'
typedef struct sockaddr SA;
// print error message and quit the program, follow from Stevens
void sys_err(const char*);
// socket creation
int Socket(int family, int type, int protocol);
// bind "unamed" socket with address
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
// listen on a particular socket
int Listen(int socket, int backlog);
// accept a connection and return connected fd
int Accept(int socket, struct sockaddr *address, socklen_t *address_len);
// close a file descriptor
int Close(int fildes);
//-------------------------------------------------
// portable and reliable version of signal handling
//-------------------------------------------------
// for signal handlers  
typedef void    Sigfunc(int);  
// wrapper for signal function
Sigfunc * Signal(int signo, Sigfunc *func);
//--------------------------------------------------
// readn and writen read/write n bytes of data to fd
// since read/write interruptable, thses two functions
// will buffer what's been read and continue
//--------------------------------------------------
ssize_t Read(int fd, void *vpter, size_t n);
ssize_t Write(int fd, const void *vptr, size_t n);
// fdopen() open a file by file descriptor
FILE* Fdopen(int fd, const char *mode);
//---------------------------------------------------------------------
// Fgets() takes care of error, return 1 for normal, 0 for end-of-file,
// -1 for error other than EINTR, when EINTR, restart
//---------------------------------------------------------------------
int Fgets(char* str, int size, FILE* stream);
// Dup2() with error checking
int Dup2(int fildes, int fildes2);
#endif
