#ifndef CLASSROOM_HPP
#define CLASSROOM_HPP

#include <string>
#include <vector>
#include "Lecture.hpp"
using namespace std;

class Classroom {
private:
    string name;
    string building;
    int capacity;
    vector<Lecture> schedule;

public:
    Classroom();
    Classroom(string n, string b, int c);
    
    string getName();
    string getBuilding();
    int getCapacity();
    vector<Lecture>& getSchedule();
    int getLectureCount();
    
    void initSchedule();
    bool markLectureOff(int index);
    bool bookLecture(int index, string name);
    bool deleteLecture(int index);
    bool rescheduleLecture(int oldIndex, int newIndex);
    void showSchedule();
    void showAvailable();
    bool hasAvailable();
    Lecture* getLecture(int index);
    int findLectureByTime(string time);
};

#endif