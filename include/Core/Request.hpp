#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
using namespace std;

class Request {
private:
    string requester;
    string room;
    string time;
    string purpose;
    int status;

public:
    Request();
    Request(string req, string rm, string t, string p);
    
    string getRequester();
    string getRoom();
    string getTime();
    string getPurpose();
    int getStatus();
    string getStatusString();
    
    bool approve();
    bool reject();
    bool isPending();
    void display();
};

#endif