#include "Core/ClassroomSystem.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

ClassroomSystem::ClassroomSystem()
{
    adminLoggedIn = false;
    addClassroom("Room 101", "Building A", 40);
    addClassroom("Room 102", "Building A", 35);
    addClassroom("Lab 201", "Building B", 30);
}

bool ClassroomSystem::login(string password)
{
    if (password == Constants::ADMIN_PASSWORD)
    {
        adminLoggedIn = true;
        return true;
    }
    return false;
}

void ClassroomSystem::logout() { adminLoggedIn = false; }
bool ClassroomSystem::isAdmin() { return adminLoggedIn; }

void ClassroomSystem::addClassroom(string name, string building, int capacity)
{
    classrooms.push_back(Classroom(name, building, capacity));
}

bool ClassroomSystem::deleteClassroom(int index)
{
    if (index >= 0 && index < classrooms.size())
    {
        classrooms.erase(classrooms.begin() + index);
        return true;
    }
    return false;
}

void ClassroomSystem::showClassrooms()
{
    cout << "\n=== All Classrooms ===\n";
    if (classrooms.empty()) { cout << "No classrooms.\n"; return; }
    for (int i = 0; i < classrooms.size(); i++)
    {
        cout << (i + 1) << ". " << classrooms[i].getName() 
             << " (" << classrooms[i].getBuilding() << ")"
             << " - Cap: " << classrooms[i].getCapacity() << "\n";
    }
}

bool ClassroomSystem::approveRequest(int index)
{
    if (index >= 0 && index < requests.size() && requests[index].isPending())
    {
        requests[index].approve();
        // Book the lecture
        string room = requests[index].getRoom();
        string time = requests[index].getTime();
        for (int i = 0; i < classrooms.size(); i++)
        {
            if (classrooms[i].getName() == room)
            {
                vector<Lecture>& sched = classrooms[i].getSchedule();
                for (int j = 0; j < sched.size(); j++)
                {
                    if (sched[j].getTime() == time)
                    {
                        sched[j].book(requests[index].getRequester());
                        break;
                    }
                }
                break;
            }
        }
        return true;
    }
    return false;
}

bool ClassroomSystem::rejectRequest(int index)
{
    if (index >= 0 && index < requests.size())
        return requests[index].reject();
    return false;
}

void ClassroomSystem::showRequests()
{
    cout << "\n=== Requests ===\n";
    if (requests.empty()) { cout << "No requests.\n"; return; }
    for (int i = 0; i < requests.size(); i++)
    {
        cout << i << ". ";
        requests[i].display();
    }
}

bool ClassroomSystem::markOff(int roomIdx, int lectureIdx)
{
    if (roomIdx >= 0 && roomIdx < classrooms.size())
        return classrooms[roomIdx].markLectureOff(lectureIdx);
    return false;
}

bool ClassroomSystem::reschedule(int roomIdx, int lectureIdx, string newTime)
{
    if (roomIdx >= 0 && roomIdx < classrooms.size())
    {
        vector<Lecture>& sched = classrooms[roomIdx].getSchedule();
        if (lectureIdx >= 0 && lectureIdx < sched.size())
        {
            sched[lectureIdx].setStatus(Constants::STATUS_OFF);
            return true;
        }
    }
    return false;
}

bool ClassroomSystem::deleteLecture(int roomIdx, int lectureIdx)
{
    if (roomIdx >= 0 && roomIdx < classrooms.size())
        return classrooms[roomIdx].deleteLecture(lectureIdx);
    return false;
}

bool ClassroomSystem::facultyRequest(string name, int roomIdx, int lectureIdx, string purpose)
{
    if (roomIdx < 0 || roomIdx >= classrooms.size()) return false;
    vector<Lecture>& sched = classrooms[roomIdx].getSchedule();
    if (lectureIdx < 0 || lectureIdx >= sched.size()) return false;
    if (!sched[lectureIdx].isAvailable()) return false;
    
    requests.push_back(Request(name, classrooms[roomIdx].getName(), 
                               sched[lectureIdx].getTime(), purpose));
    return true;
}

bool ClassroomSystem::clubRequest(string name, int roomIdx, string time, string purpose)
{
    if (roomIdx < 0 || roomIdx >= classrooms.size()) return false;
    requests.push_back(Request(name, classrooms[roomIdx].getName(), time, purpose));
    return true;
}

vector<Classroom>& ClassroomSystem::getClassrooms() { return classrooms; }
vector<Request>& ClassroomSystem::getRequests() { return requests; }

Classroom* ClassroomSystem::getClassroom(int index)
{
    if (index >= 0 && index < classrooms.size())
        return &classrooms[index];
    return NULL;
}

int ClassroomSystem::classroomCount() { return classrooms.size(); }
int ClassroomSystem::requestCount() { return requests.size(); }

int ClassroomSystem::pendingCount()
{
    int count = 0;
    for (int i = 0; i < requests.size(); i++)
        if (requests[i].isPending()) count++;
    return count;
}

void ClassroomSystem::showSchedule(int index)
{
    if (index >= 0 && index < classrooms.size())
        classrooms[index].showSchedule();
}

void ClassroomSystem::showAvailable(int index)
{
    if (index >= 0 && index < classrooms.size())
        classrooms[index].showAvailable();
}

void ClassroomSystem::showStatus()
{
    cout << "\n=== System Status ===\n";
    cout << "Classrooms: " << classroomCount() << "\n";
    cout << "Requests: " << requestCount() << "\n";
    cout << "Pending: " << pendingCount() << "\n";
    cout << "Admin: " << (adminLoggedIn ? "Yes" : "No") << "\n";
}