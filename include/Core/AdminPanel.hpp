#ifndef ADMIN_PANEL_HPP
#define ADMIN_PANEL_HPP

#include <string>
using namespace std;

class AdminPanel
{
public:
    virtual void addClassroom(string name, string building, int capacity) = 0;
    virtual bool deleteClassroom(int index) = 0;
    virtual void showClassrooms() = 0;
    virtual bool approveRequest(int index) = 0;
    virtual bool rejectRequest(int index) = 0;
    virtual void showRequests() = 0;
    virtual ~AdminPanel() {}
};

#endif