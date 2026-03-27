#ifndef CLASSROOM_SYSTEM_HPP
#define CLASSROOM_SYSTEM_HPP

#include <vector>
#include <string>
#include "AdminPanel.hpp"
#include "Classroom.hpp"
#include "Request.hpp"
using namespace std;

// INHERITANCE & POLYMORPHISM
class ClassroomSystem : public AdminPanel
{
private:
    vector<Classroom> classrooms;
    vector<Request> requests;
    bool adminLoggedIn;

public:
    ClassroomSystem();
    
    // Auth
    bool login(string password);
    void logout();
    bool isAdmin();
    
    // AdminPanel implementation
    void addClassroom(string name, string building, int capacity);
    bool deleteClassroom(int index);
    void showClassrooms();
    bool approveRequest(int index);
    bool rejectRequest(int index);
    void showRequests();
    
    // Operations
    bool markOff(int roomIdx, int lectureIdx);
    bool reschedule(int roomIdx, int lectureIdx, string newTime);
    bool deleteLecture(int roomIdx, int lectureIdx);
    bool facultyRequest(string name, int roomIdx, int lectureIdx, string purpose);
    bool clubRequest(string name, int roomIdx, string time, string purpose);
    
    // Getters
    vector<Classroom>& getClassrooms();
    vector<Request>& getRequests();
    Classroom* getClassroom(int index);
    int classroomCount();
    int requestCount();
    int pendingCount();
    
    void showSchedule(int index);
    void showAvailable(int index);
    void showStatus();
};

#endif