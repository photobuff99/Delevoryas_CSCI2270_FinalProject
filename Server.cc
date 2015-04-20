#include <iostream>
#include <fstream>
#include "Utilities.h"
#include <string>
#include <fstream>
#include "HashTable.h"
#include <vector>

#define TOPICS "topics.txt"

using std::cout; using std::cin;
using std::string; using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::to_string;

struct Topic
{
    string name;
    string original_post;
    int num_responses;
    vector<string> responses;

    Topic(string n, string op, int nr, vector<string> rs) : name(n),original_post(op),
                                                            num_responses(nr),
                                                            responses(rs){}
    
};

void HandleClient(int clientfd);
void WriteMessage(string filename, string message);
string GetMessages(string filename);
void FillTable(HashTable<Topic> & table);
void CreateTopicFile(Topic topic, long table_size);

int main(int argc, char*argv[]) {
    int servfd, clientfd;
    socklen_t sin_size;
    struct sockaddr_storage their_addr;

    // Get a socket
    servfd = Socket(NULL, SERV_PORT, AF_UNSPEC, SOCK_STREAM, 0);
    if (servfd == -1) {
        cerr << "Error getting file descriptor" << endl;
        exit(1);
    }

    // Start listening on socket
    if (Listen(servfd, 10) == -1) {
        cerr << "Error listening" << endl;
        exit(1);
    }

    // Initiate Hashtable: constructed with topic names and messages inside
    HashTable<Topic> topics = HashTable<Topic>(100);
    ifstream in(TOPICS);
    string line, op, n;
    vector<string> rs;
    while (getline(in,n)) {
        getline(in,line);
        op = line.substr(4);
        while (in.peek() == ' ') {
            getline(in, line);
            rs.push_back(line.substr(4));
        }
        CreateTopicFile(Topic(n,op,rs.size(),rs), 100);
        topics.insert(n, Topic(n,op,rs.size(),rs));
        rs.clear();
    }

    in.close();

    // Accept clients
    while (1) {
        sin_size = sizeof their_addr;
        clientfd = accept(servfd, (struct sockaddr *)&their_addr, &sin_size);
        if (clientfd == -1) {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        if (!fork()) { // child process
            close(servfd);
            HandleClient(clientfd);
            close(clientfd);
            exit(0);
        }
        close(clientfd);
    }

    close(servfd);
    return 0;

}

void HandleClient(int clientfd)
{
    //SendMessage(clientfd, "Hello, client!");
    //SendMessage(clientfd, msg);
    string recvd = RecvMessage(clientfd, 256);
    cout << "server: received " << recvd << endl;
    if (recvd == GET) {
        WriteMessage(, recvd);
        
    }
}

void WriteMessage(string filename, string message)
{
    ofstream out (filename, std::ios::app);
    if (out.is_open()) {
        out << message;
        out << endl;
        out.close();
    }
}

string GetMessages(string filename)
{
    std::ifstream in (filename);
    std::string messages, line;
    while (getline(in,line)) {
        messages += line;
        messages += "\n";
    }
    return messages;
}

void FillTable(HashTable<Topic> & table)
{
    ifstream in (TOPICS);
    string line;

    while (getline(in,line)) {
        continue;
    }

    in.close();
}

void CreateTopicFile(Topic topic, long table_size)
{
    long hash = SimpleHash(topic.name,table_size);
    string filename = "f" + to_string(hash);
    ofstream out (filename);
    if (out.is_open()){
        out << topic.original_post << '\n';
        for (int i = 0; i < topic.responses.size(); ++i) {
            out << topic.responses[i] << '\n';
        }
        out.close();
    }
}
