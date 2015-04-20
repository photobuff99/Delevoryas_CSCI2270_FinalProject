#include "Utilities.h"
#include <iostream>

using std::cout; using std::cin; using std::endl; using std::cerr; using std::string;

int     Socket(const char *ip_addr, const char *port ,int domain, int type, int protocol)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sockfd, s;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = domain;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    if (ip_addr == NULL)
        hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(ip_addr, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1)
            continue;

        if (ip_addr == NULL)
            if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
                break;      /* Success finding server socket */

        if (ip_addr != NULL)
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
                break;      /* Success finding client socket */

        close(sockfd);
    }

    if (rp == NULL) {
        if (ip_addr == NULL)
            fprintf(stderr, "Could not bind\n");
        else
            fprintf(stderr, "Could not connect\n");
        return -1;
    }

    freeaddrinfo(result);

    return sockfd;
}

int Listen(int sockfd, int queue_size)
{
    if (listen(sockfd, queue_size) == -1)
        return -1;
    else return 0;
}

// returns number of characters sent
int SendMessage(int sockfd, string message)
{
    return send(sockfd, message.c_str(), message.length(), 0);
}

// returns message received, max_msg_size must be <= 1024
string RecvMessage(int sockfd, int max_size)
{
    const int buffer_size = (max_size > 1024) ? 1024 : max_size;
    char buffer[buffer_size];
    memset(buffer, 0, sizeof buffer);

    recv(sockfd, buffer, buffer_size-1, 0);
    string msg(buffer);
    return msg;
}

int ParseMessage(string message)
{
    int fs = message.find(" ");
    int ss = message.find(" ",fs+1);
    string type = message.substr(0,fs);
    string resource = message.substr(fs+1,ss-fs);
    if (resource == "/topics.txt")
        return 1;
    else return 0;
}

void RequestTopics(int sockfd, string user)
{
    string request;
    //char CR = 13, LF = 10;
    request += "GET /topics.txt HTTP/1.0\n";
    request += user + '\n' + '\n';
    if (SendMessage(sockfd, request) < request.length())
        std::cerr << "Error in sending message" << endl;
}
