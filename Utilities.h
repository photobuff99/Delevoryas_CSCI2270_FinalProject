#include <iostream>
extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
}

#define SERV_NODE "localhost"
#define SERV_PORT "7700"

#define GET "GET"
#define POST "POST"

struct Request;

            /* Socket Connection    */
// Wrapper function to replace getaddrinfo() and socket()
// Param: www.example.com/IP for client, NULL for server, Port num, v4/v6, TCP/UDP, 0
int     Socket(const char *ip_addr, const char *port,
               int domain, int type, int protocol);

// Start listening on the given socket file descriptor
int     Listen(int sockfd, int queue_size);


            /* Socket Communication */

// returns number of characters sent
int SendMessage(int sockfd, std::string message);

// returns message received, max_size <= 1024
std::string RecvMessage(int sockfd, int max_size);

// Parse message as HTTP Request, returning the formatted header to be used to execute command
Request ParseMessage(std::string message);

/*
 * 1. = send topics to user
 *
 *
 */

// Send Topic Request
//void RequestTopics(int sockfd, std::string user);

            /* String Parsing Functions */

struct Request
{
    std::string type; // GET or POST for this project
    std::string resource_location; // To GET, or to POST to
    std::string user_id; // email, username, etc (optional)

    Request(){};
    Request(std::string t, std::string rl) : type(t), resource_location(rl){}
    Request(std::string t, std::string rl, std::string id) : type(t), resource_location(rl), user_id(id){}
    inline std::string ToString()
        { return type+" "+resource_location+"\n"+user_id+"\n\n"; }
};

            /* Hash Functions */
long SimpleHash(std::string key, long table_size);
