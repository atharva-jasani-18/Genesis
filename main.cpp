#include <iostream>
#include <string>
#include "Core/ClassroomSystem.hpp"
#include "Utils/Constants.hpp"
using namespace std;


// ===================== SFML MODE =====================
#ifdef USE_SFML
#include <SFML/Graphics.hpp>

sf::Font font;
ClassroomSystem* sysPtr = NULL;

int screen = 0, prevScreen = 0;
int selRoom = -1, selLec = -1, selReq = -1, selAvail = -1;
string input1 = "", input2 = "", input3 = "";
string msgTitle = "", msgText = "";
int activeInput = 0;
vector<string> availSlots;
vector<int> availIdx;

void drawBtn(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, sf::Color c)
{
    sf::RectangleShape b(sf::Vector2f(bw, h));
    b.setPosition(x, y);
    b.setFillColor(c);
    w.draw(b);
    sf::Text t;
    t.setFont(font);
    t.setString(txt);
    t.setCharacterSize(13);
    t.setFillColor(sf::Color::White);
    t.setPosition(x + (bw - t.getGlobalBounds().width) / 2, y + (h - 16) / 2);
    w.draw(t);
}

void drawInput(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, string ph, bool act, bool pwd = false)
{
    sf::RectangleShape b(sf::Vector2f(bw, h));
    b.setPosition(x, y);
    b.setFillColor(sf::Color::White);
    b.setOutlineThickness(2);
    b.setOutlineColor(act ? sf::Color(52, 152, 219) : sf::Color(150, 150, 150));
    w.draw(b);
    sf::Text t;
    t.setFont(font);
    t.setString(txt.empty() ? ph : (pwd ? string(txt.size(), '*') : txt));
    t.setCharacterSize(13);
    t.setFillColor(txt.empty() ? sf::Color(150, 150, 150) : sf::Color::Black);
    t.setPosition(x + 8, y + (h - 16) / 2);
    w.draw(t);
}

void drawList(sf::RenderWindow& w, float x, float y, float bw, float h, vector<string>& items, int sel)
{
    sf::RectangleShape bg(sf::Vector2f(bw, h));
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color::White);
    bg.setOutlineThickness(1);
    bg.setOutlineColor(sf::Color(180, 180, 180));
    w.draw(bg);
    for (int i = 0; i < (int)items.size() && (i + 1) * 26 < h; i++)
    {
        sf::RectangleShape it(sf::Vector2f(bw - 6, 24));
        it.setPosition(x + 3, y + 3 + i * 26);
        it.setFillColor(i == sel ? sf::Color(52, 152, 219) : sf::Color(245, 245, 245));
        w.draw(it);
        sf::Text t;
        t.setFont(font);
        t.setString(items[i]);
        t.setCharacterSize(11);
        t.setFillColor(i == sel ? sf::Color::White : sf::Color::Black);
        t.setPosition(x + 6, y + 6 + i * 26);
        w.draw(t);
    }
}

void drawLabel(sf::RenderWindow& w, float x, float y, string txt, int sz = 13)
{
    sf::Text t;
    t.setFont(font);
    t.setString(txt);
    t.setCharacterSize(sz);
    t.setFillColor(sf::Color::Black);
    t.setPosition(x, y);
    w.draw(t);
}

void showMsg(string t, string m) { msgTitle = t; msgText = m; prevScreen = screen; screen = 9; }
void clearInputs() { input1 = input2 = input3 = ""; activeInput = 0; }
void resetSel() { selRoom = selLec = selReq = selAvail = -1; }

int clickIdx(float mouseY, float listY, int cnt)
{
    int i = (int)((mouseY - listY - 3) / 26);
    return (i >= 0 && i < cnt) ? i : -1;
}

vector<string> getRooms()
{
    vector<string> list;
    for (int i = 0; i < sysPtr->classroomCount(); i++)
    {
        Classroom* r = sysPtr->getClassroom(i);
        if (r) list.push_back(to_string(i + 1) + ". " + r->getName() + " (" + r->getBuilding() + ")");
    }
    return list;
}

vector<string> getSchedule()
{
    vector<string> list;
    if (selRoom < 0 || selRoom >= sysPtr->classroomCount()) return list;
    Classroom* room = sysPtr->getClassroom(selRoom);
    if (!room) return list;
    vector<Lecture>& sc = room->getSchedule();
    for (int i = 0; i < (int)sc.size(); i++)
    {
        string line = to_string(i) + ". " + sc[i].getTime() + " - " + sc[i].getSubject() + " [" + sc[i].getStatusString() + "]";
        list.push_back(line);
    }
    return list;
}

void updateAvail()
{
    availSlots.clear();
    availIdx.clear();
    if (selRoom < 0 || selRoom >= sysPtr->classroomCount()) return;
    Classroom* room = sysPtr->getClassroom(selRoom);
    if (!room) return;
    vector<Lecture>& sc = room->getSchedule();
    for (int i = 0; i < (int)sc.size(); i++)
    {
        if (sc[i].isAvailable())
        {
            availSlots.push_back(to_string(i) + ". " + sc[i].getTime() + " - " + sc[i].getSubject());
            availIdx.push_back(i);
        }
    }
}

vector<string> getReqs()
{
    vector<string> list;
    for (int i = 0; i < sysPtr->requestCount(); i++)
    {
        Request* r = sysPtr->getRequest(i);
        if (r) list.push_back(to_string(i) + ". " + r->getRequester() + " | " + r->getRoom() + " | " + r->getTime() + " [" + r->getStatusString() + "]");
    }
    return list;
}

void handleText(sf::Event& e)
{
    string* t = NULL;
    if (activeInput == 1) t = &input1;
    else if (activeInput == 2) t = &input2;
    else if (activeInput == 3) t = &input3;
    if (!t) return;
    if (e.text.unicode == 8 && t->size() > 0) t->pop_back();
    else if (e.text.unicode >= 32 && e.text.unicode < 127 && t->size() < 30) *t += (char)e.text.unicode;
}

void fixDefaultSchedules()
{
    string times[] = {"09:00-10:00", "10:00-11:00", "11:00-12:00", "12:00-13:00", "13:00-14:00", "14:00-15:00", "15:00-16:00", "16:00-17:00", "17:00-18:00", "18:00-19:00", "19:00-20:00"};
    string subs[] = {"Free Slot", "Mathematics", "Physics", "Lunch Break", "Chemistry", "English", "Computer", "Biology", "Free Slot", "Free Slot", "Free Slot"};

    for (int i = 0; i < sysPtr->classroomCount(); i++)
    {
        Classroom* r = sysPtr->getClassroom(i);
        if (!r) continue;
        vector<Lecture>& sc = r->getSchedule();

        bool needsFix = false;
        if (sc.size() > 0 && sc[1].getSubject().empty()) needsFix = true;
        if (sc.size() > 0 && sc[1].getSubject() == "") needsFix = true;
        if (sc.size() == 0) needsFix = true;

        if (needsFix)
        {
            sc.clear();
            for (int j = 0; j < 11; j++)
            {
                bool isOff = (j == 0 || j == 3 || j >= 8);
                sc.push_back(Lecture(times[j], subs[j], isOff));
            }
        }
    }
}

void runSFML()
{
    sf::RenderWindow window(sf::VideoMode(950, 600), "Classroom Management System");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
            if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf"))
            {
                cout << "Font error!\n";
                return;
            }

    ClassroomSystem localSys;
    sysPtr = &localSys;
    fixDefaultSchedules();

    while (window.isOpen())
    {
        sf::Event e;
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        float mx = mp.x, my = mp.y;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::TextEntered && activeInput > 0) handleText(e);

            if (e.type == sf::Event::MouseButtonReleased)
            {
                // SCREEN 0: Main
                if (screen == 0)
                {
                    if (mx > 375 && mx < 575)
                    {
                        if (my > 180 && my < 225) { screen = 1; clearInputs(); }
                        if (my > 245 && my < 290) { screen = 3; resetSel(); }
                        if (my > 310 && my < 355) showMsg("Status", "Rooms: " + to_string(sysPtr->classroomCount()) + "\nPending: " + to_string(sysPtr->pendingCount()));
                        if (my > 375 && my < 420) window.close();
                    }
                }
                // SCREEN 1: Login
                else if (screen == 1)
                {
                    if (mx > 365 && mx < 585 && my > 240 && my < 275) activeInput = 1;
                    if (mx > 365 && mx < 465 && my > 295 && my < 330)
                    {
                        if (sysPtr->login(input1)) { screen = 2; clearInputs(); resetSel(); }
                        else showMsg("Error", "Wrong password!");
                    }
                    if (mx > 485 && mx < 585 && my > 295 && my < 330) { screen = 0; clearInputs(); }
                }
                // SCREEN 2: Admin
                else if (screen == 2)
                {
                    if (mx > 20 && mx < 160)
                    {
                        if (my > 80 && my < 115) { screen = 4; resetSel(); }
                        if (my > 125 && my < 160) { screen = 5; resetSel(); }
                        if (my > 170 && my < 205) { screen = 7; clearInputs(); }
                        if (my > 215 && my < 250)
                        {
                            if (selRoom >= 0 && selRoom < sysPtr->classroomCount())
                            { sysPtr->deleteClassroom(selRoom); resetSel(); showMsg("OK", "Deleted!"); }
                            else showMsg("Error", "Select room!");
                        }
                        if (my > 260 && my < 295)
                        {
                            if (selRoom >= 0 && selLec >= 0)
                            { if (sysPtr->markOff(selRoom, selLec)) showMsg("OK", "Marked OFF!"); else showMsg("Error", "Cannot mark OFF!"); }
                            else showMsg("Error", "Select room & lecture!");
                        }
                        if (my > 305 && my < 340)
                        {
                            if (selRoom >= 0 && selLec >= 0)
                            { if (sysPtr->deleteLecture(selRoom, selLec)) { selLec = -1; showMsg("OK", "Deleted!"); } else showMsg("Error", "Cannot delete!"); }
                            else showMsg("Error", "Select room & lecture!");
                        }
                        if (my > 350 && my < 385) { screen = 8; selLec = selAvail = -1; }
                        if (my > 395 && my < 430) { screen = 6; selReq = -1; }
                        if (my > 440 && my < 475) { sysPtr->logout(); screen = 0; resetSel(); }
                    }
                    if (mx > 180 && mx < 380 && my > 100 && my < 470)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) { selRoom = i; selLec = -1; } }
                    if (mx > 400 && mx < 920 && my > 100 && my < 470 && selRoom >= 0)
                    { Classroom* r = sysPtr->getClassroom(selRoom); if (r) { int i = clickIdx(my, 100, r->getLectureCount()); if (i >= 0) selLec = i; } }
                }
                // SCREEN 3: User
                else if (screen == 3)
                {
                    if (mx > 375 && mx < 575)
                    {
                        if (my > 150 && my < 190) { screen = 4; resetSel(); }
                        if (my > 205 && my < 245) { screen = 5; resetSel(); }
                        if (my > 260 && my < 300) { screen = 11; resetSel(); }
                        if (my > 315 && my < 355) { screen = 10; resetSel(); clearInputs(); }
                        if (my > 370 && my < 410) screen = 0;
                    }
                }
                // SCREEN 4: View Rooms
                else if (screen == 4)
                {
                    if (mx > 800 && mx < 920 && my > 530 && my < 565) screen = sysPtr->isAdmin() ? 2 : 3;
                    if (mx > 50 && mx < 400 && my > 100 && my < 490)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) selRoom = i; }
                }
                // SCREEN 5: Schedule
                else if (screen == 5)
                {
                    if (mx > 800 && mx < 920 && my > 530 && my < 565) screen = sysPtr->isAdmin() ? 2 : 3;
                    if (mx > 30 && mx < 230 && my > 100 && my < 490)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) { selRoom = i; selLec = -1; } }
                    if (mx > 260 && mx < 920 && my > 100 && my < 490 && selRoom >= 0)
                    { Classroom* r = sysPtr->getClassroom(selRoom); if (r) { int i = clickIdx(my, 100, r->getLectureCount()); if (i >= 0) selLec = i; } }
                }
                // SCREEN 6: Requests
                else if (screen == 6)
                {
                    if (mx > 50 && mx < 170 && my > 450 && my < 485)
                    { if (selReq >= 0 && selReq < sysPtr->requestCount()) { if (sysPtr->approveRequest(selReq)) { selReq = -1; showMsg("OK", "Approved!"); } } else showMsg("Error", "Select request!"); }
                    if (mx > 190 && mx < 310 && my > 450 && my < 485)
                    { if (selReq >= 0 && selReq < sysPtr->requestCount()) { if (sysPtr->rejectRequest(selReq)) { selReq = -1; showMsg("OK", "Rejected!"); } } else showMsg("Error", "Select request!"); }
                    if (mx > 800 && mx < 920 && my > 450 && my < 485) { screen = 2; selReq = -1; }
                    if (mx > 50 && mx < 600 && my > 100 && my < 430)
                    { int i = clickIdx(my, 100, sysPtr->requestCount()); if (i >= 0) selReq = i; }
                }
                // SCREEN 7: Add Room
                else if (screen == 7)
                {
                    if (mx > 325 && mx < 625 && my > 160 && my < 195) activeInput = 1;
                    if (mx > 325 && mx < 625 && my > 220 && my < 255) activeInput = 2;
                    if (mx > 325 && mx < 625 && my > 280 && my < 315) activeInput = 3;
                    if (mx > 325 && mx < 455 && my > 340 && my < 375)
                    { if (!input1.empty()) { int c = 50; try { c = stoi(input3); } catch (...) {} sysPtr->addClassroom(input1, input2.empty() ? "Main" : input2, c); clearInputs(); showMsg("OK", "Added!"); } else showMsg("Error", "Enter name!"); }
                    if (mx > 475 && mx < 605 && my > 340 && my < 375) { screen = 2; clearInputs(); }
                }
                // SCREEN 8: Reschedule
                else if (screen == 8)
                {
                    if (mx > 30 && mx < 200 && my > 100 && my < 360)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) { selRoom = i; selLec = selAvail = -1; updateAvail(); } }
                    if (mx > 220 && mx < 500 && my > 100 && my < 360 && selRoom >= 0)
                    { Classroom* r = sysPtr->getClassroom(selRoom); if (r) { int i = clickIdx(my, 100, r->getLectureCount()); if (i >= 0) selLec = i; } }
                    if (mx > 520 && mx < 800 && my > 100 && my < 360 && selRoom >= 0)
                    { updateAvail(); int i = clickIdx(my, 100, (int)availSlots.size()); if (i >= 0) selAvail = i; }
                    if (mx > 520 && mx < 650 && my > 390 && my < 425)
                    {
                        if (selRoom < 0) showMsg("Error", "Select room!");
                        else if (selLec < 0) showMsg("Error", "Select lecture!");
                        else if (selAvail < 0 || selAvail >= (int)availIdx.size()) showMsg("Error", "Select target!");
                        else
                        {
                            Classroom* r = sysPtr->getClassroom(selRoom);
                            if (r && selLec < r->getLectureCount())
                            {
                                Lecture& l = r->getSchedule()[selLec];
                                if (l.isBreak() || l.getSubject() == "Free Slot") showMsg("Error", "Cannot reschedule this!");
                                else if (l.getStatus() != 0) showMsg("Error", "Only ACTIVE can be rescheduled!");
                                else
                                {
                                    if (sysPtr->reschedule(selRoom, selLec, availIdx[selAvail]))
                                    { selLec = selAvail = -1; updateAvail(); showMsg("OK", "Rescheduled!"); }
                                    else showMsg("Error", "Failed!");
                                }
                            }
                        }
                    }
                    if (mx > 670 && mx < 800 && my > 390 && my < 425) { screen = 2; resetSel(); }
                }
                // SCREEN 9: Message
                else if (screen == 9)
                {
                    if (mx > 400 && mx < 550 && my > 340 && my < 375) screen = prevScreen;
                }
                // SCREEN 10: User Request
                else if (screen == 10)
                {
                    if (mx > 30 && mx < 230 && my > 100 && my < 420)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) { selRoom = i; selAvail = selLec = -1; updateAvail(); } }
                    if (mx > 260 && mx < 520 && my > 100 && my < 420 && selRoom >= 0)
                    { updateAvail(); int i = clickIdx(my, 100, (int)availSlots.size()); if (i >= 0) { selAvail = i; selLec = availIdx[i]; } }
                    if (mx > 560 && mx < 860 && my > 180 && my < 215) activeInput = 1;
                    if (mx > 560 && mx < 860 && my > 250 && my < 285) activeInput = 2;
                    if (mx > 560 && mx < 690 && my > 320 && my < 355)
                    {
                        if (input1.empty()) showMsg("Error", "Enter name!");
                        else if (selLec < 0) showMsg("Error", "Select slot!");
                        else
                        {
                            if (sysPtr->facultyRequest(input1, selRoom, selLec, input2.empty() ? "Booking" : input2))
                            { clearInputs(); resetSel(); showMsg("OK", "Request sent!"); }
                            else showMsg("Error", "Slot not available!");
                        }
                    }
                    if (mx > 710 && mx < 840 && my > 320 && my < 355) { screen = 3; clearInputs(); resetSel(); }
                }
                // SCREEN 11: Available
                else if (screen == 11)
                {
                    if (mx > 800 && mx < 920 && my > 530 && my < 565) screen = 3;
                    if (mx > 30 && mx < 230 && my > 100 && my < 490)
                    { int i = clickIdx(my, 100, sysPtr->classroomCount()); if (i >= 0) { selRoom = i; updateAvail(); } }
                }
            }
        }

        // DRAWING
        window.clear(sf::Color(240, 240, 240));
        sf::RectangleShape bar(sf::Vector2f(950, 50));
        bar.setFillColor(sf::Color(44, 62, 80));
        window.draw(bar);
        sf::Text tit;
        tit.setFont(font);
        tit.setString("Classroom Management System");
        tit.setCharacterSize(20);
        tit.setFillColor(sf::Color::White);
        tit.setPosition(320, 12);
        window.draw(tit);

        if (screen == 0)
        {
            drawLabel(window, 280, 80, "9AM-8PM | Lectures 10AM-5PM | Pass: admin123", 11);
            drawBtn(window, 375, 180, 200, 45, "Admin Login", sf::Color(52, 152, 219));
            drawBtn(window, 375, 245, 200, 45, "User Menu", sf::Color(46, 204, 113));
            drawBtn(window, 375, 310, 200, 45, "Status", sf::Color(155, 89, 182));
            drawBtn(window, 375, 375, 200, 45, "Exit", sf::Color(231, 76, 60));
        }
        else if (screen == 1)
        {
            drawLabel(window, 365, 210, "Password:");
            drawInput(window, 365, 240, 220, 35, input1, "Enter password", activeInput == 1, true);
            drawBtn(window, 365, 295, 100, 35, "Login", sf::Color(46, 204, 113));
            drawBtn(window, 485, 295, 100, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 2)
        {
            drawBtn(window, 20, 80, 140, 35, "View Rooms", sf::Color(52, 152, 219));
            drawBtn(window, 20, 125, 140, 35, "View Schedule", sf::Color(52, 152, 219));
            drawBtn(window, 20, 170, 140, 35, "Add Room", sf::Color(46, 204, 113));
            drawBtn(window, 20, 215, 140, 35, "Delete Room", sf::Color(231, 76, 60));
            drawBtn(window, 20, 260, 140, 35, "Mark OFF", sf::Color(230, 126, 34));
            drawBtn(window, 20, 305, 140, 35, "Delete Lecture", sf::Color(192, 57, 43));
            drawBtn(window, 20, 350, 140, 35, "Reschedule", sf::Color(155, 89, 182));
            drawBtn(window, 20, 395, 140, 35, "Requests(" + to_string(sysPtr->pendingCount()) + ")", sf::Color(52, 73, 94));
            drawBtn(window, 20, 440, 140, 35, "Logout", sf::Color(149, 165, 166));
            drawLabel(window, 180, 78, "Classrooms:");
            vector<string> r = getRooms();
            drawList(window, 180, 100, 200, 370, r, selRoom);
            drawLabel(window, 400, 78, "Schedule:");
            vector<string> s = getSchedule();
            drawList(window, 400, 100, 520, 370, s, selLec);
        }
        else if (screen == 3)
        {
            drawBtn(window, 375, 150, 200, 40, "View Rooms", sf::Color(52, 152, 219));
            drawBtn(window, 375, 205, 200, 40, "View Schedule", sf::Color(52, 152, 219));
            drawBtn(window, 375, 260, 200, 40, "View Available", sf::Color(46, 204, 113));
            drawBtn(window, 375, 315, 200, 40, "Request Booking", sf::Color(155, 89, 182));
            drawBtn(window, 375, 370, 200, 40, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 4)
        {
            drawLabel(window, 50, 78, "All Classrooms:");
            vector<string> r = getRooms();
            drawList(window, 50, 100, 350, 390, r, selRoom);
            if (selRoom >= 0 && selRoom < sysPtr->classroomCount())
            {
                Classroom* rm = sysPtr->getClassroom(selRoom);
                if (rm)
                {
                    drawLabel(window, 450, 110, "Name: " + rm->getName());
                    drawLabel(window, 450, 135, "Building: " + rm->getBuilding());
                    drawLabel(window, 450, 160, "Capacity: " + to_string(rm->getCapacity()));
                }
            }
            drawBtn(window, 800, 530, 120, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 5)
        {
            drawLabel(window, 30, 78, "Rooms:");
            vector<string> r = getRooms();
            drawList(window, 30, 100, 200, 400, r, selRoom);
            drawLabel(window, 260, 78, "Schedule:");
            vector<string> s = getSchedule();
            drawList(window, 260, 100, 660, 400, s, selLec);
            drawBtn(window, 800, 530, 120, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 6)
        {
            drawLabel(window, 50, 78, "Requests:");
            vector<string> r = getReqs();
            drawList(window, 50, 100, 550, 330, r, selReq);
            drawBtn(window, 50, 450, 120, 35, "Approve", sf::Color(46, 204, 113));
            drawBtn(window, 190, 450, 120, 35, "Reject", sf::Color(231, 76, 60));
            drawBtn(window, 800, 450, 120, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 7)
        {
            drawLabel(window, 325, 110, "Add Classroom", 16);
            drawLabel(window, 325, 145, "Name:");
            drawInput(window, 325, 160, 300, 35, input1, "Room name", activeInput == 1);
            drawLabel(window, 325, 205, "Building:");
            drawInput(window, 325, 220, 300, 35, input2, "Building", activeInput == 2);
            drawLabel(window, 325, 265, "Capacity:");
            drawInput(window, 325, 280, 300, 35, input3, "50", activeInput == 3);
            drawBtn(window, 325, 340, 130, 35, "Add", sf::Color(46, 204, 113));
            drawBtn(window, 475, 340, 130, 35, "Cancel", sf::Color(149, 165, 166));
        }
        else if (screen == 8)
        {
            drawLabel(window, 30, 78, "1. Room:");
            vector<string> r = getRooms();
            drawList(window, 30, 100, 170, 260, r, selRoom);
            drawLabel(window, 220, 78, "2. Lecture (ACTIVE):");
            vector<string> s = getSchedule();
            drawList(window, 220, 100, 280, 260, s, selLec);
            drawLabel(window, 520, 78, "3. Target Slot:");
            updateAvail();
            drawList(window, 520, 100, 280, 260, availSlots, selAvail);
            drawBtn(window, 520, 390, 130, 35, "Reschedule", sf::Color(155, 89, 182));
            drawBtn(window, 670, 390, 130, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 9)
        {
            sf::RectangleShape ov(sf::Vector2f(950, 600));
            ov.setFillColor(sf::Color(0, 0, 0, 120));
            window.draw(ov);
            sf::RectangleShape bx(sf::Vector2f(350, 180));
            bx.setPosition(300, 200);
            bx.setFillColor(sf::Color::White);
            bx.setOutlineThickness(2);
            bx.setOutlineColor(sf::Color(52, 152, 219));
            window.draw(bx);
            drawLabel(window, 320, 220, msgTitle, 16);
            drawLabel(window, 320, 260, msgText, 13);
            drawBtn(window, 400, 340, 150, 35, "OK", sf::Color(52, 152, 219));
        }
        else if (screen == 10)
        {
            drawLabel(window, 30, 78, "1. Room:");
            vector<string> r = getRooms();
            drawList(window, 30, 100, 200, 320, r, selRoom);
            drawLabel(window, 260, 78, "2. Available:");
            updateAvail();
            drawList(window, 260, 100, 260, 320, availSlots, selAvail);
            drawLabel(window, 560, 160, "3. Your Name:");
            drawInput(window, 560, 180, 300, 35, input1, "Name", activeInput == 1);
            drawLabel(window, 560, 230, "4. Purpose:");
            drawInput(window, 560, 250, 300, 35, input2, "Purpose", activeInput == 2);
            drawBtn(window, 560, 320, 130, 35, "Submit", sf::Color(46, 204, 113));
            drawBtn(window, 710, 320, 130, 35, "Back", sf::Color(149, 165, 166));
        }
        else if (screen == 11)
        {
            drawLabel(window, 30, 78, "Room:");
            vector<string> r = getRooms();
            drawList(window, 30, 100, 200, 400, r, selRoom);
            drawLabel(window, 260, 78, "Available Slots:");
            updateAvail();
            drawList(window, 260, 100, 400, 400, availSlots, -1);
            if (availSlots.empty() && selRoom >= 0) drawLabel(window, 270, 110, "No slots available!", 11);
            drawBtn(window, 800, 530, 120, 35, "Back", sf::Color(149, 165, 166));
        }

        window.display();
    }
}
#endif

// ===================== CONSOLE MODE =====================
void consoleMenu(ClassroomSystem& sys) {
    int choice;
    while (true) {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1. Admin Login\n2. User Menu\n3. Status\n0. Exit\n> ";
        cin >> choice;
        if (choice == 0) break; // Exit
        if (choice == 3) { // Status
            sys.showStatus();
            continue;
        }
        if (choice == 1) { // Admin Login
            string pass;
            cout << "Enter Admin Password: ";
            cin >> pass;
            if (!sys.login(pass)) {
                cout << "Wrong Password!\n";
                continue; 
            }
            while (true) {
                cout << "\n===== ADMIN =====\n";
                cout << "1. Add Classroom\n2. Delete Classroom\n3. View Classrooms\n";
                cout << "4. View Schedule\n5. Mark OFF\n6. Delete Lecture\n";
                cout << "7. Reschedule\n8. Requests\n";
                cout << "9. Approve\n10. Reject\n0. Logout\n> ";
                cin >> choice;
                
                if (choice == 0) { // Logout
                    sys.logout(); 
                    break; 
                }
                if (choice == 1) { // Add Room
                    string n, b; int c;
                    cout << "Name: "; 
                    cin.ignore(); 
                    getline(cin, n);
                    cout << "Building: "; 
                    getline(cin, b);
                    cout << "Capacity: "; 
                    cin >> c;
                    sys.addClassroom(n, b, c);
                }
                else if (choice == 2) { // Delete Room
                    sys.showClassrooms();
                    cout << "Enter Classroom Index to Delete: "; cin >> choice;
                    sys.deleteClassroom(choice - 1);
                }
                else if (choice == 3) { // View Room
                    sys.showClassrooms();
                }
                else if (choice == 4) { // View Schedule
                    sys.showClassrooms();
                    cout << "Enter Classroom Index: "; cin >> choice;
                    sys.showSchedule(choice - 1);
                }
                else if (choice == 5) { // Mark OFF
                    int r, l;
                    sys.showClassrooms(); cout << "Enter Classroom Index: "; 
                    cin >> r;
                    sys.showSchedule(r - 1); cout << "Enter Lecture Index to Mark OFF: "; 
                    cin >> l;
                    if (sys.markOff(r - 1, l)) cout << "Marked OFF!\n";
                    else cout << "Failed! Already OFF or break\n";
                }
                else if (choice == 6) { // Delete Lecture
                    int r, l;
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showSchedule(r - 1); cout << "Lecture: "; cin >> l;
                    if (sys.deleteLecture(r - 1, l)) cout << "Deleted!\n";
                    else cout << "Failed!\n";
                }
                else if (choice == 7) { // Reschedule
                    int r, oldL, newL;
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showSchedule(r - 1);
                    cout << "Enter Lecture Index to move: "; cin >> oldL;
                    sys.showSchedule(r - 1); // Show index again
                    cout << "Destination free slot: "; cin >> newL;
                    
                    if (sys.reschedule(r - 1, oldL, newL))
                        cout << "Lecture Rescheduled Successfully!\n";
                    else{
                        cout << "Failed! Check:\n";
                        cout << "- Old lecture must be ACTIVE\n";
                        cout << "- New slot must be Free Slot (OFF)\n";
                    }
                }
                else if (choice == 8) sys.showRequests(); // Requests
                else if (choice == 9) { // Approve
                    sys.showRequests(); cout << "Index: "; cin >> choice;
                    if (sys.approveRequest(choice)) cout << "Request Approved!\n";
                    else cout << "Failed!\n";
                }
                else if (choice == 10) { // Reject
                    sys.showRequests(); cout << "Index: "; cin >> choice;
                    if (sys.rejectRequest(choice)) cout << "Rejected!\n";
                    else cout << "Failed!\n";
                }
            }
        }
        else if (choice == 2) {
            while (true) {
                cout << "\n===== USER =====\n";
                cout << "1. View Classrooms\n2. View Schedule\n3. View Available Slots\n";
                cout << "4. Request to Book\n0. Back\n> ";
                cin >> choice;
                
                if (choice == 0) break;
                if (choice == 1) sys.showClassrooms(); // View Classroom
                else if (choice == 2) { // View Schedule
                    sys.showClassrooms(); cout << "Room: "; cin >> choice; 
                    sys.showSchedule(choice - 1);
                }
                else if (choice == 3) { // View Available Slots
                    sys.showClassrooms(); cout << "Room: "; cin >> choice;
                    sys.showAvailable(choice - 1);
                }
                else if (choice == 4) { // Request to Book
                    int r, l; string n, p;
                    cout << "Name: "; cin.ignore(); getline(cin, n);
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showAvailable(r - 1); cout << "Lecture: "; cin >> l;
                    cout << "Purpose: "; cin.ignore(); getline(cin, p);
                    if (sys.facultyRequest(n, r - 1, l, p)) cout << "Request Sent Successfully!\n";
                    else cout << "Failed! Slot not available.\n";
                }
            }
        }
    }
}

int main()
{
    cout << "========================================\n";
    cout << "   CLASSROOM MANAGEMENT SYSTEM\n";
    cout << "   9AM-8PM | Lectures 10AM-5PM\n";
    cout << "   Password: " << Constants::ADMIN_PASSWORD << "\n";
    cout << "========================================\n";

#ifdef USE_SFML
    runSFML();
#else
    ClassroomSystem sys;
    consoleMenu(sys);
#endif
    
    return 0;
}