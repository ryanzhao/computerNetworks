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
//-----------------------------------------------------------------------------
#include<sys/types.h>
#include<sys/socket.h>

// avoid typing the long 'struct sockaddr'
typedef struct sockaddr SA;
// print error message and quit the program, follow from Stevens
void sys_err(const char*);
int Socket(int family, int type, int protocol);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *address, socklen_t *address_len);

