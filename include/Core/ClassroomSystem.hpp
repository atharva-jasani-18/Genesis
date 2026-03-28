#ifndef CLASSROOM_SYSTEM_HPP
#define CLASSROOM_SYSTEM_HPP

#include <vector>
#include <string>
#include "AdminPanel.hpp"
#include "Classroom.hpp"
#include "Request.hpp"
using namespace std;

class ClassroomSystem : public AdminPanel
{
private:
    vector<Classroom> classrooms;
    vector<Request> requests;
    bool adminLoggedIn;

public:
    ClassroomSystem();
    
    bool login(string password);
    void logout();
    bool isAdmin();
    
    void addClassroom(string name, string building, int capacity);
    bool deleteClassroom(int index);
    void showClassrooms();
    bool approveRequest(int index);
    bool rejectRequest(int index);
    void showRequests();
    
    bool markOff(int roomIdx, int lectureIdx);
    bool reschedule(int roomIdx, int oldIdx, int newIdx);
    bool deleteLecture(int roomIdx, int lectureIdx);
    bool facultyRequest(string name, int roomIdx, int lectureIdx, string purpose);
    bool clubRequest(string name, int roomIdx, string time, string purpose);
    
    Classroom* getClassroom(int index);
    int classroomCount();
    int requestCount();
    int pendingCount();
    
    void showSchedule(int index);
    void showAvailable(int index);
    void showStatus();
};

#endif