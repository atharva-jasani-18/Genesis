#ifndef LECTURE_HPP
#define LECTURE_HPP

#include <string>
using namespace std;

class Lecture
{
private:
    string time;
    string subject;
    int status;
    string bookedBy;

public:
    Lecture();
    Lecture(string t, string s, bool isOff);
    
    string getTime();
    string getSubject();
    int getStatus();
    string getBookedBy();
    string getStatusString();
    
    void setStatus(int s);
    void setBookedBy(string name);
    
    bool markOff();
    bool book(string name);
    bool isAvailable();
    bool isBreak();
};

#endif