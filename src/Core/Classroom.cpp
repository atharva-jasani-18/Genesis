#include "Core/Classroom.hpp"
#include "Utils/Constants.hpp"
#include <iostream>
using namespace std;

Classroom::Classroom()
{
    name = "";
    building = "Main";
    capacity = 50;
}

Classroom::Classroom(string n, string b, int c)
{
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

void Classroom::initSchedule()
{
    schedule.clear();
    for (int i = 0; i < Constants::TOTAL_SLOTS; i++)
    {
        bool isOff = (i == 0 || i == 3 || i >= 8);  // Free slots and lunch
        schedule.push_back(Lecture(Constants::TIME_SLOTS[i], Constants::DEFAULT_SUBJECTS[i], isOff));
    }
}

bool Classroom::markLectureOff(int index)
{
    if (index >= 0 && index < schedule.size())
        return schedule[index].markOff();
    return false;
}

bool Classroom::bookLecture(int index, string name)
{
    if (index >= 0 && index < schedule.size())
        return schedule[index].book(name);
    return false;
}

bool Classroom::deleteLecture(int index)
{
    if (index >= 0 && index < schedule.size())
    {
        schedule.erase(schedule.begin() + index);
        return true;
    }
    return false;
}

void Classroom::showSchedule()
{
    cout << "\n=== Schedule: " << name << " ===\n";
    for (int i = 0; i < schedule.size(); i++)
    {
        cout << i << ". " << schedule[i].getTime() << " | "
             << schedule[i].getSubject() << " [" 
             << schedule[i].getStatusString() << "]";
        if (!schedule[i].getBookedBy().empty())
            cout << " - " << schedule[i].getBookedBy();
        cout << endl;
    }
}

void Classroom::showAvailable()
{
    cout << "\n=== Available in " << name << " ===\n";
    bool found = false;
    for (int i = 0; i < schedule.size(); i++)
    {
        if (schedule[i].isAvailable())
        {
            cout << i << ". " << schedule[i].getTime() << " - " 
                 << schedule[i].getSubject() << endl;
            found = true;
        }
    }
    if (!found) cout << "No slots available!\n";
}

bool Classroom::hasAvailable()
{
    for (int i = 0; i < schedule.size(); i++)
        if (schedule[i].isAvailable()) return true;
    return false;
}