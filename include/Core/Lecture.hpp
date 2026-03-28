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
    string rescheduledTo;

public:
    Lecture();
    Lecture(string t, string s, bool isOff);
    
    string getTime();
    string getSubject();
    int getStatus();
    string getBookedBy();
    string getRescheduledTo();
    string getStatusString();
    
    void setTime(string t);
    void setSubject(string s);
    void setStatus(int s);
    void setBookedBy(string name);
    void setRescheduledTo(string time);
    
    bool markOff();
    bool book(string name);
    bool rescheduleTo(string newTime);
    bool isAvailable();
    bool isBreak();
    bool isFreeSlot();
    void reset();
};

#endif