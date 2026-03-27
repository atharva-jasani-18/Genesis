#include "Core/Lecture.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

Lecture::Lecture()
{
    time = "";
    subject = "";
    status = Constants::STATUS_ACTIVE;
    bookedBy = "";
}

Lecture::Lecture(string t, string s, bool isOff)
{
    time = t;
    subject = s;
    status = isOff ? Constants::STATUS_OFF : Constants::STATUS_ACTIVE;
    bookedBy = "";
}

string Lecture::getTime() { return time; }
string Lecture::getSubject() { return subject; }
int Lecture::getStatus() { return status; }
string Lecture::getBookedBy() { return bookedBy; }

string Lecture::getStatusString()
{
    if (status == Constants::STATUS_OFF) return "OFF";
    if (status == Constants::STATUS_BOOKED) return "BOOKED";
    return "ACTIVE";
}

void Lecture::setStatus(int s) { status = s; }
void Lecture::setBookedBy(string name) { bookedBy = name; }

bool Lecture::markOff()
{
    if (isBreak()) return false;
    if (status == Constants::STATUS_ACTIVE)
    {
        status = Constants::STATUS_OFF;
        return true;
    }
    return false;
}

bool Lecture::book(string name)
{
    if (status == Constants::STATUS_OFF)
    {
        status = Constants::STATUS_BOOKED;
        bookedBy = name;
        return true;
    }
    return false;
}

bool Lecture::isAvailable()
{
    return status == Constants::STATUS_OFF && bookedBy.empty();
}

bool Lecture::isBreak()
{
    return subject == "Lunch Break" || subject == "Free Slot";
}