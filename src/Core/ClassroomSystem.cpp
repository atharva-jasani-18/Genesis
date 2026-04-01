#include "Core/ClassroomSystem.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

ClassroomSystem::ClassroomSystem() {
    adminLoggedIn = false;
    addClassroom("Classroom No. 1", "Academic Complex", 40);
    addClassroom("Classroom No. 2", "Academic Complex", 45);
    addClassroom("Classroom No. 3", "Academic Complex", 50);
}

bool ClassroomSystem::login(string password) {
    if (password == Constants::ADMIN_PASSWORD) {
        adminLoggedIn = true;
        return true;
    }
    return false;
}
void ClassroomSystem::logout() { 
    adminLoggedIn = false; 
}
bool ClassroomSystem::isAdmin() { 
    return adminLoggedIn;
}
void ClassroomSystem::addClassroom(string name, string building, int capacity) {
    classrooms.push_back(Classroom(name, building, capacity));
    cout << "Classroom Added Successfully!" << endl;
}
bool ClassroomSystem::deleteClassroom(int index) {
    if (index >= 0 && index < classrooms.size()) {
        classrooms.erase(classrooms.begin() + index);
        cout << "Classroom Deleted Successfully!" << endl;
        return true;
    }
    else {
        cout<<"Invalid Choice!"<<endl;
    }
    return false;
}
void ClassroomSystem::showClassrooms() {
    cout << "\n=== All Classrooms ===\n";
    if (classrooms.empty()) { 
        cout << "No classrooms!\n"; 
        return; 
    }
    for (int i = 0; i < classrooms.size(); i++) {
        cout << (i + 1) << ". " << classrooms[i].getName() 
             << " (" << classrooms[i].getBuilding() << ")"
             << " - Cap: " << classrooms[i].getCapacity() << "\n";
    }
}

bool ClassroomSystem::approveRequest(int index)
{
    if (index >= 0 && index < requests.size() && requests[index].isPending()) {
        requests[index].approve();
        // Book the lecture
        string room = requests[index].getRoom();
        string time = requests[index].getTime();
        for (int i = 0; i < classrooms.size(); i++) {
            if (classrooms[i].getName() == room) {
                int idx = classrooms[i].findLectureByTime(time);
                if (idx != -1) {
                    classrooms[i].bookLecture(idx, requests[index].getRequester());
                }
                break;
            }
        }
        return true;
    }
    return false;
}

bool ClassroomSystem::rejectRequest(int index) {
    if (index >= 0 && index < requests.size())
        return requests[index].reject();
    return false;
}

void ClassroomSystem::showRequests() {
    cout << "\n=== Requests ===\n";
    if (requests.empty()) { 
        cout << "No requests.\n"; 
        return; }
    for (int i = 0; i < requests.size(); i++) {
        cout << i << ". ";
        requests[i].display();
    }
}

bool ClassroomSystem::markOff(int roomIdx, int lectureIdx) {
    if (roomIdx >= 0 && roomIdx < classrooms.size()) {
        return classrooms[roomIdx].markLectureOff(lectureIdx);
    }
    return false;
}

bool ClassroomSystem::reschedule(int roomIdx, int oldIdx, int newIdx) {
    if (roomIdx >= 0 && roomIdx < classrooms.size())
        return classrooms[roomIdx].rescheduleLecture(oldIdx, newIdx);
    return false;
}

bool ClassroomSystem::deleteLecture(int roomIdx, int lectureIdx) {
    if (roomIdx >= 0 && roomIdx < classrooms.size())
        return classrooms[roomIdx].deleteLecture(lectureIdx);
    return false;
}

bool ClassroomSystem::facultyRequest(string name, int roomIdx, int lectureIdx, string purpose) {
    if (roomIdx < 0 || roomIdx >= classrooms.size()) return false;
    Classroom* room = getClassroom(roomIdx);
    if (!room) return false;
    
    Lecture* lec = room->getLecture(lectureIdx);
    if (!lec || !lec->isAvailable()) return false;
    
    requests.push_back(Request(name, room->getName(), lec->getTime(), purpose));
    return true;
}

bool ClassroomSystem::clubRequest(string name, int roomIdx, string time, string purpose) {
    if (roomIdx < 0 || roomIdx >= classrooms.size()) return false;
    Classroom* room = getClassroom(roomIdx);
    if (!room) return false;
    
    // Check if time slot exists and is available
    int idx = room->findLectureByTime(time);
    if (idx == -1) return false;
    
    Lecture* lec = room->getLecture(idx);
    if (!lec || !lec->isAvailable()) return false;
    
    requests.push_back(Request(name, room->getName(), time, purpose));
    return true;
}

Classroom* ClassroomSystem::getClassroom(int index) {
    if (index >= 0 && index < classrooms.size())
        return &classrooms[index];
    return NULL;
}

int ClassroomSystem::classroomCount() { return classrooms.size(); }
int ClassroomSystem::requestCount() { return requests.size(); }

int ClassroomSystem::pendingCount() {
    int count = 0;
    for (int i = 0; i < requests.size(); i++)
        if (requests[i].isPending()) count++;
    return count;
}

void ClassroomSystem::showSchedule(int index) {
    if (index >= 0 && index < classrooms.size())
        classrooms[index].showSchedule();
}

void ClassroomSystem::showAvailable(int index) {
    if (index >= 0 && index < classrooms.size())
        classrooms[index].showAvailable();
}

void ClassroomSystem::showStatus() {
    cout << "\n=== System Status ===\n";
    cout << "Classrooms: " << classroomCount() << "\n";
    cout << "Requests: " << requestCount() << "\n";
    cout << "Pending: " << pendingCount() << "\n";
    cout << "Admin: " << (adminLoggedIn ? "Yes" : "No") << "\n";
}

Request* ClassroomSystem::getRequest(int index) {
    if (index >= 0 && index < (int)requests.size())
        return &requests[index];
    return NULL;
}