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
    rescheduledTo = "";
}

Lecture::Lecture(string t, string s, bool isOff)
{
    time = t;
    subject = s;
    status = isOff ? Constants::STATUS_OFF : Constants::STATUS_ACTIVE;
    bookedBy = "";
    rescheduledTo = "";
}

string Lecture::getTime() { return time; }
string Lecture::getSubject() { return subject; }
int Lecture::getStatus() { return status; }
string Lecture::getBookedBy() { return bookedBy; }
string Lecture::getRescheduledTo() { return rescheduledTo; }

string Lecture::getStatusString()
{
    if (status == Constants::STATUS_OFF) return "OFF";
    if (status == Constants::STATUS_BOOKED) return "BOOKED";
    if (status == Constants::STATUS_RESCHEDULED) return "RESCHEDULED";
    return "ACTIVE";
}

void Lecture::setTime(string t) { time = t; }
void Lecture::setSubject(string s) { subject = s; }
void Lecture::setStatus(int s) { status = s; }
void Lecture::setBookedBy(string name) { bookedBy = name; }
void Lecture::setRescheduledTo(string t) { rescheduledTo = t; }

bool Lecture::markOff()
{
    // Cannot mark break/free slots as OFF (they're already OFF)
    if (isBreak() || isFreeSlot()) return false;
    
    // Only ACTIVE lectures can be marked OFF
    if (status == Constants::STATUS_ACTIVE)
    {
        status = Constants::STATUS_OFF;
        return true;
    }
    return false;
}

bool Lecture::book(string name)
{
    // Only OFF lectures (that are not breaks) can be booked
    if (status == Constants::STATUS_OFF && !isBreak())
    {
        status = Constants::STATUS_BOOKED;
        bookedBy = name;
        return true;
    }
    return false;
}

bool Lecture::rescheduleTo(string newTime)
{
    // Only ACTIVE lectures can be rescheduled
    if (status == Constants::STATUS_ACTIVE)
    {
        status = Constants::STATUS_RESCHEDULED;
        rescheduledTo = newTime;
        return true;
    }
    return false;
}

bool Lecture::isAvailable()
{
    // OFF and not booked and not a break
    return (status == Constants::STATUS_OFF && bookedBy.empty() && !isBreak());
}

bool Lecture::isBreak()
{
    return subject == "Lunch Break";
}

bool Lecture::isFreeSlot()
{
    return subject == "Free Slot";
}

void Lecture::reset()
{
    status = Constants::STATUS_ACTIVE;
    bookedBy = "";
    rescheduledTo = "";
}