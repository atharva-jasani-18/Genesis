#include "Core/Request.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

Request::Request() {
    requester = "";
    room = "";
    time = "";
    purpose = "";
    status = Constants::REQUEST_PENDING;
}

Request::Request(string req, string rm, string t, string p) {
    requester = req;
    room = rm;
    time = t;
    purpose = p;
    status = Constants::REQUEST_PENDING;
}

string Request::getRequester() { return requester; }
string Request::getRoom() { return room; }
string Request::getTime() { return time; }
string Request::getPurpose() { return purpose; }
int Request::getStatus() { return status; }

string Request::getStatusString() {
    if (status == Constants::REQUEST_APPROVED) return "APPROVED";
    if (status == Constants::REQUEST_REJECTED) return "REJECTED";
    return "PENDING";
}

bool Request::approve() {
    if (status == Constants::REQUEST_PENDING)
    {
        status = Constants::REQUEST_APPROVED;
        return true;
    }
    return false;
}

bool Request::reject() {
    if (status == Constants::REQUEST_PENDING) {
        status = Constants::REQUEST_REJECTED;
        return true;
    }
    return false;
}

bool Request::isPending() {
    return status == Constants::REQUEST_PENDING;
}

void Request::display() {
    cout << requester << " | " << room << " | " << time 
         << " | " << purpose << " [" << getStatusString() << "]\n";
}