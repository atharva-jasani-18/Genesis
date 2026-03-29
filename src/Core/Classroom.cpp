#include "Core/Classroom.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

Classroom::Classroom() {
    name = "";
    building = "Main";
    capacity = 50;
}

Classroom::Classroom(string n, string b, int c) {
    name = n;
    building = b;
    capacity = c;
    initSchedule();
}

string Classroom::getName() { return name; }
string Classroom::getBuilding() { return building; }
int Classroom::getCapacity() { return capacity; }
vector<Lecture>& Classroom::getSchedule() { return schedule; }
int Classroom::getLectureCount() { return schedule.size(); }

void Classroom::initSchedule() {
    schedule.clear();
    for (int i = 0; i < Constants::TOTAL_SLOTS; i++) {
        bool isOff = (i == 0 || i == 3 || i >= 8);  // Free slots and lunch break
        schedule.push_back(Lecture(Constants::TIME_SLOTS[i], Constants::DEFAULT_SUBJECTS[i], isOff));
    }
}

bool Classroom::markLectureOff(int index) {
    if (index >= 0 && index < schedule.size())
        return schedule[index].markOff();
    return false;
}

bool Classroom::bookLecture(int index, string name) {
    if (index >= 0 && index < schedule.size())
        return schedule[index].book(name);
    return false;
}

bool Classroom::deleteLecture(int index) {
    if (index >= 0 && index < schedule.size()) {
        // Convert to free slot instead of removing (keeps schedule size)
        schedule[index].setSubject("Free Slot");
        schedule[index].setStatus(Constants::STATUS_OFF);
        schedule[index].setBookedBy("");
        return true;
    }
    return false;
}

bool Classroom::rescheduleLecture(int oldIndex, int newIndex) {
    if (oldIndex < 0 || oldIndex >= schedule.size() || 
        newIndex < 0 || newIndex >= schedule.size())
        return false;
    
    Lecture& oldLec = schedule[oldIndex];
    Lecture& newLec = schedule[newIndex];
    
    // Validate: old lecture must be ACTIVE and not booked
    if (oldLec.getStatus() != Constants::STATUS_ACTIVE || 
        !oldLec.getBookedBy().empty())
        return false;
    
    // Validate: new slot must be FREE (Free Slot and OFF)
    if (!newLec.isFreeSlot() || newLec.getStatus() != Constants::STATUS_OFF)
        return false;
    
    // Swap subjects and statuses
    string subjectToMove = oldLec.getSubject();
    
    oldLec.setSubject("Free Slot");
    oldLec.setStatus(Constants::STATUS_OFF);
    oldLec.setBookedBy("");
    oldLec.setRescheduledTo("");
    
    newLec.setSubject(subjectToMove);
    newLec.setStatus(Constants::STATUS_ACTIVE);
    newLec.setBookedBy("");
    newLec.setRescheduledTo("");
    
    return true;
}

void Classroom::showSchedule() {
    cout << "\n=== Schedule: " << name << " ===\n";
    for (int i = 0; i < schedule.size(); i++) {
        cout << i << ". " << schedule[i].getTime() << " | "
             << schedule[i].getSubject() << " [" 
             << schedule[i].getStatusString() << "]";
        if (!schedule[i].getBookedBy().empty())
            cout << " - " << schedule[i].getBookedBy();
        if (!schedule[i].getRescheduledTo().empty())
            cout << " -> " << schedule[i].getRescheduledTo();
        cout << endl;
    }
}

void Classroom::showAvailable() {
    cout << "\n=== Available Slots in " << name << " ===\n";
    bool found = false;
    for (int i = 0; i < schedule.size(); i++) {
        if (schedule[i].isAvailable()) {
            cout << i << ". " << schedule[i].getTime() << " - " 
                 << schedule[i].getSubject() << endl;
            found = true;
        }
    }
    if (!found) cout << "*** No slots available! ***\n";
}

bool Classroom::hasAvailable() {
    for (int i = 0; i < schedule.size(); i++)
        if (schedule[i].isAvailable()) return true;
    return false;
}

Lecture* Classroom::getLecture(int index) {
    if (index >= 0 && index < schedule.size())
        return &schedule[index];
    return NULL;
}

int Classroom::findLectureByTime(string time) {
    for (int i = 0; i < schedule.size(); i++) {
        if (schedule[i].getTime() == time)
            return i;
    }
    return -1;
}