#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
using namespace std;

namespace Constants
{
    const string ADMIN_PASSWORD = "admin123";
    
    const int STATUS_ACTIVE = 0;
    const int STATUS_OFF = 1;
    const int STATUS_BOOKED = 2;
    const int STATUS_RESCHEDULED = 3;
    
    const int REQUEST_PENDING = 0;
    const int REQUEST_APPROVED = 1;
    const int REQUEST_REJECTED = 2;
    
    const int TOTAL_SLOTS = 11;
    
    const string TIME_SLOTS[TOTAL_SLOTS] = {
        "09:00-10:00", "10:00-11:00", "11:00-12:00", "12:00-13:00",
        "13:00-14:00", "14:00-15:00", "15:00-16:00", "16:00-17:00",
        "17:00-18:00", "18:00-19:00", "19:00-20:00"
    };
    
    const string DEFAULT_SUBJECTS[TOTAL_SLOTS] = {
        "Free Slot", "Mathematics", "Physics", "Lunch Break",
        "Chemistry", "English", "Computer", "Biology",
        "Free Slot", "Free Slot", "Free Slot"
    };
}

#endif