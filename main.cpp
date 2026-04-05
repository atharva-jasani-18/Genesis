#include <iostream>
#include <string>
#include "Core/ClassroomSystem.hpp"
#include "Utils/Constants.hpp"
using namespace std;


// ===================== SFML MODE =====================
#ifdef USE_SFML
#include <SFML/Graphics.hpp>

sf::Font font;
ClassroomSystem sys;

int screen = 0, prevScreen = 0;
int selRoom = -1, selLec = -1, selReq = -1, selAvail = -1;
string input1 = "", input2 = "", input3 = "";
string msgTitle = "", msgText = "";
int activeInput = 0;

vector<string> availableSlots;
vector<int> availableIndices;

void drawBtn(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, sf::Color c, bool hov) {
    sf::RectangleShape btn(sf::Vector2f(bw, h));
    btn.setPosition(x, y);
    btn.setFillColor(hov ? sf::Color((int)(c.r*0.8), (int)(c.g*0.8), (int)(c.b*0.8)) : c);
    w.draw(btn);
    
    sf::Text t(txt, font, 13);
    t.setPosition(x + (bw - t.getGlobalBounds().width)/2, y + (h-16)/2);
    t.setFillColor(sf::Color::White);
    w.draw(t);
}

void drawInput(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, string placeholder, bool active, bool pwd = false) {
    sf::RectangleShape box(sf::Vector2f(bw, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(active ? sf::Color(52, 152, 219) : sf::Color(150, 150, 150));
    w.draw(box);
    
    string display = txt.empty() ? placeholder : (pwd ? string(txt.size(), '*') : txt);
    sf::Text t(display, font, 13);
    t.setPosition(x + 8, y + (h-16)/2);
    t.setFillColor(txt.empty() ? sf::Color(150, 150, 150) : sf::Color::Black);
    w.draw(t);
}

void drawList(sf::RenderWindow& w, float x, float y, float bw, float h, vector<string>& items, int sel) {
    sf::RectangleShape bg(sf::Vector2f(bw, h));
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color::White);
    bg.setOutlineThickness(1);
    bg.setOutlineColor(sf::Color(180, 180, 180));
    w.draw(bg);
    
    float ih = 30;
    for (int i = 0; i < (int)items.size() && i * ih < h - 5; i++) {
        sf::RectangleShape item(sf::Vector2f(bw - 6, ih - 2));
        item.setPosition(x + 3, y + 3 + i * ih);
        item.setFillColor(i == sel ? sf::Color(52, 152, 219) : sf::Color(245, 245, 245));
        w.draw(item);
        
        sf::Text t(items[i], font, 11);
        t.setPosition(x + 8, y + 7 + i * ih);
        t.setFillColor(i == sel ? sf::Color::White : sf::Color::Black);
        w.draw(t);
    }
    
    if (items.empty()) {
        sf::Text t("No items", font, 11);
        t.setPosition(x + 10, y + 10);
        t.setFillColor(sf::Color(150, 150, 150));
        w.draw(t);
    }
}

void drawLabel(sf::RenderWindow& w, float x, float y, string txt, int size = 13) {
    sf::Text t(txt, font, size);
    t.setPosition(x, y);
    t.setFillColor(sf::Color::Black);
    w.draw(t);
}

void showMsg(string title, string text) {
    msgTitle = title;
    msgText = text;
    prevScreen = screen;
    screen = 9;
}

void clearInputs() {
    input1 = "";
    input2 = "";
    input3 = "";
    activeInput = 0;
}

void resetSelections() {
    selRoom = -1;
    selLec = -1;
    selReq = -1;
    selAvail = -1;
}

vector<string> getRoomList() {
    vector<string> list;
    for (int i = 0; i < sys.classroomCount(); i++)
    {
        Classroom* r = sys.getClassroom(i);
        if (r != NULL)
            list.push_back(to_string(i) + ". " + r->getName() + " (" + r->getBuilding() + ")");
    }
    return list;
}

vector<string> getScheduleList() {
    vector<string> list;
    if (selRoom >= 0 && selRoom < sys.classroomCount()) {
        Classroom* room = sys.getClassroom(selRoom);
        if (room != NULL) {
            vector<Lecture>& s = room->getSchedule();
            for (int i = 0; i < (int)s.size(); i++) {
                string line = to_string(i) + ". " + s[i].getTime() + " | " + s[i].getSubject() + " [" + s[i].getStatusString() + "]";
                list.push_back(line);
            }
        }
    }
    return list;
}

void updateAvailableList() {
    availableSlots.clear();
    availableIndices.clear();
    
    if (selRoom >= 0 && selRoom < sys.classroomCount()) {
        Classroom* room = sys.getClassroom(selRoom);
        if (room != NULL) {
            vector<Lecture>& s = room->getSchedule();
            for (int i = 0; i < (int)s.size(); i++) {
                if (s[i].isAvailable()) {
                    availableSlots.push_back(to_string(i) + ". " + s[i].getTime() + " | " + s[i].getSubject());
                    availableIndices.push_back(i);
                }
            }
        }
    }
}

vector<string> getRequestList() {
    vector<string> list;
    for (int i = 0; i < sys.requestCount(); i++) {
        Request* r = sys.getRequest(i);
        if (r != NULL)
            list.push_back(to_string(i) + ". " + r->getRequester() + " | " + r->getRoom() + " | " + r->getTime() + " [" + r->getStatusString() + "]");
    }
    return list;
}

void handleTextInput(sf::Event& e) {
    string* target = NULL;
    if (activeInput == 1) target = &input1;
    else if (activeInput == 2) target = &input2;
    else if (activeInput == 3) target = &input3;
    
    if (target == NULL) return;
    
    if (e.text.unicode == 8 && target->size() > 0)
        target->pop_back();
    else if (e.text.unicode >= 32 && e.text.unicode < 127 && target->size() < 30)
        *target += (char)e.text.unicode;
}

int getClickIndex(float mouseY, float listY, int itemCount) {
    int idx = (int)((mouseY - listY - 3) / 30);
    if (idx >= 0 && idx < itemCount) return idx;
    return -1;
}

// Find lecture index by time string
int findLectureByTime(int roomIdx, string timeStr) {
    if (roomIdx < 0 || roomIdx >= sys.classroomCount()) return -1;
    
    Classroom* room = sys.getClassroom(roomIdx);
    if (room == NULL) return -1;
    
    vector<Lecture>& schedule = room->getSchedule();
    for (int i = 0; i < (int)schedule.size(); i++) {
        if (schedule[i].getTime() == timeStr)
            return i;
    }
    return -1;
}

void runSFML() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "Classroom Management System");
    window.setFramerateLimit(60);
    
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
            if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
                cout << "Font error!\n";
                return;
            }
    
    while (window.isOpen()) {
        sf::Event e;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float mx = mousePos.x, my = mousePos.y;
        
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            
            if (e.type == sf::Event::TextEntered && activeInput > 0)
                handleTextInput(e);
            
            if (e.type == sf::Event::MouseButtonReleased) {
                // ========== SCREEN 0: Main Menu ==========
                if (screen == 0) {
                    if (mx > 350 && mx < 550) {
                        if (my > 180 && my < 225) { screen = 1; clearInputs(); }
                        if (my > 240 && my < 285) { screen = 3; resetSelections(); }
                        if (my > 300 && my < 345) {
                            showMsg("System Status", "Classrooms: " + to_string(sys.classroomCount()) + 
                                "\nRequests: " + to_string(sys.requestCount()) + 
                                "\nPending: " + to_string(sys.pendingCount()));
                        }
                        if (my > 360 && my < 405) window.close();
                    }
                }
                // ========== SCREEN 1: Admin Login ==========
                else if (screen == 1) {
                    if (mx > 340 && mx < 560 && my > 235 && my < 270) activeInput = 1;
                    if (mx > 340 && mx < 440 && my > 290 && my < 325) {
                        if (sys.login(input1)) { screen = 2; clearInputs(); resetSelections(); }
                        else showMsg("Error", "Wrong password!");
                    }
                    if (mx > 460 && mx < 560 && my > 290 && my < 325) { screen = 0; clearInputs(); }
                }
                // ========== SCREEN 2: Admin Panel ==========
                else if (screen == 2) {
                    if (mx > 20 && mx < 160) {
                        if (my > 80 && my < 115) { screen = 4; resetSelections(); }
                        if (my > 125 && my < 160) { screen = 5; resetSelections(); }
                        if (my > 170 && my < 205) { screen = 7; clearInputs(); }
                        if (my > 215 && my < 250) {
                            if (selRoom >= 0 && selRoom < sys.classroomCount()) {
                                sys.deleteClassroom(selRoom);
                                resetSelections();
                                showMsg("Success", "Room deleted!");
                            }
                            else showMsg("Error", "Select a room first!");
                        }
                        if (my > 260 && my < 295) {
                            if (selRoom >= 0 && selLec >= 0) {
                                if (sys.markOff(selRoom, selLec)) showMsg("Success", "Marked OFF!");
                                else showMsg("Error", "Cannot mark OFF!");
                            }
                            else showMsg("Error", "Select room & lecture!");
                        }
                        if (my > 305 && my < 340) {
                            if (selRoom >= 0 && selLec >= 0) {
                                if (sys.deleteLecture(selRoom, selLec)) {
                                    selLec = -1;
                                    showMsg("Success", "Lecture deleted!");
                                }
                                else showMsg("Error", "Cannot delete!");
                            }
                            else showMsg("Error", "Select room & lecture!");
                        }
                        if (my > 350 && my < 385) { screen = 8; clearInputs(); selLec = -1; selAvail = -1; }
                        if (my > 395 && my < 430) { screen = 6; selReq = -1; }
                        if (my > 440 && my < 475) { sys.logout(); screen = 0; resetSelections(); }
                    }
                    
                    if (mx > 180 && mx < 400 && my > 100 && my < 450) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) { selRoom = idx; selLec = -1; }
                    }
                    
                    if (mx > 420 && mx < 880 && my > 100 && my < 450 && selRoom >= 0) {
                        Classroom* room = sys.getClassroom(selRoom);
                        if (room != NULL) {
                            int idx = getClickIndex(my, 100, room->getLectureCount());
                            if (idx >= 0) selLec = idx;
                        }
                    }
                }
                // ========== SCREEN 3: User Panel ==========
                else if (screen == 3) {
                    if (mx > 350 && mx < 550) {
                        if (my > 140 && my < 180) { screen = 4; resetSelections(); }
                        if (my > 195 && my < 235) { screen = 5; resetSelections(); }
                        if (my > 250 && my < 290) { screen = 11; resetSelections(); }
                        if (my > 305 && my < 345) { screen = 10; resetSelections(); clearInputs(); }
                        if (my > 360 && my < 400) screen = 0;
                    }
                }
                // ========== SCREEN 4: View Classrooms ==========
                else if (screen == 4) {
                    if (mx > 750 && mx < 870 && my > 520 && my < 555)
                        screen = sys.isAdmin() ? 2 : 3;
                    
                    if (mx > 50 && mx < 400 && my > 100 && my < 480) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) selRoom = idx;
                    }
                }
                // ========== SCREEN 5: View Schedule ==========
                else if (screen == 5) {
                    if (mx > 750 && mx < 870 && my > 520 && my < 555)
                        screen = sys.isAdmin() ? 2 : 3;
                    
                    if (mx > 30 && mx < 250 && my > 100 && my < 480) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) { selRoom = idx; selLec = -1; }
                    }
                    
                    if (mx > 270 && mx < 870 && my > 100 && my < 480 && selRoom >= 0) {
                        Classroom* room = sys.getClassroom(selRoom);
                        if (room != NULL) {
                            int idx = getClickIndex(my, 100, room->getLectureCount());
                            if (idx >= 0) selLec = idx;
                        }
                    }
                }
                // ========== SCREEN 6: Requests ==========
                else if (screen == 6) {
                    if (mx > 50 && mx < 170 && my > 450 && my < 485) {
                        if (selReq >= 0 && selReq < sys.requestCount()) {
                            if (sys.approveRequest(selReq)) { selReq = -1; showMsg("Success", "Approved!"); }
                            else showMsg("Error", "Cannot approve!");
                        }
                        else showMsg("Error", "Select a request!");
                    }
                    if (mx > 190 && mx < 310 && my > 450 && my < 485) {
                        if (selReq >= 0 && selReq < sys.requestCount()) {
                            if (sys.rejectRequest(selReq)) { selReq = -1; showMsg("Success", "Rejected!"); }
                            else showMsg("Error", "Cannot reject!");
                        }
                        else showMsg("Error", "Select a request!");
                    }
                    if (mx > 750 && mx < 870 && my > 450 && my < 485) { screen = 2; selReq = -1; }
                    
                    if (mx > 50 && mx < 600 && my > 100 && my < 430) {
                        int idx = getClickIndex(my, 100, sys.requestCount());
                        if (idx >= 0) selReq = idx;
                    }
                }
                // ========== SCREEN 7: Add Classroom ==========
                else if (screen == 7) {
                    if (mx > 300 && mx < 600 && my > 150 && my < 185) activeInput = 1;
                    if (mx > 300 && mx < 600 && my > 210 && my < 245) activeInput = 2;
                    if (mx > 300 && mx < 600 && my > 270 && my < 305) activeInput = 3;
                    
                    if (mx > 300 && mx < 420 && my > 330 && my < 365) {
                        if (!input1.empty()) {
                            int cap = 50;
                            if (!input3.empty()) {
                                try { cap = stoi(input3); } catch (...) { cap = 50; }
                            }
                            sys.addClassroom(input1, input2.empty() ? "Main" : input2, cap);
                            clearInputs();
                            showMsg("Success", "Classroom added!");
                        }
                        else showMsg("Error", "Enter room name!");
                    }
                    if (mx > 440 && mx < 560 && my > 330 && my < 365) { screen = 2; clearInputs(); }
                }
                // ========== SCREEN 8: Reschedule ==========
                else if (screen == 8) {
                    // Room list
                    if (mx > 30 && mx < 220 && my > 100 && my < 350) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) { 
                            selRoom = idx; 
                            selLec = -1; 
                            selAvail = -1;
                            updateAvailableList();
                        }
                    }
                    
                    // Lecture list (source)
                    if (mx > 240 && mx < 480 && my > 100 && my < 350 && selRoom >= 0) {
                        Classroom* room = sys.getClassroom(selRoom);
                        if (room != NULL) {
                            int idx = getClickIndex(my, 100, room->getLectureCount());
                            if (idx >= 0) selLec = idx;
                        }
                    }
                    
                    // Available slots list (target)
                    if (mx > 500 && mx < 740 && my > 100 && my < 350 && selRoom >= 0) {
                        updateAvailableList();
                        int idx = getClickIndex(my, 100, (int)availableSlots.size());
                        if (idx >= 0 && idx < (int)availableIndices.size()) {
                            selAvail = idx;
                        }
                    }
                    
                    // Reschedule button
                    if (mx > 500 && mx < 620 && my > 380 && my < 415) {
                        if (selRoom < 0)
                            showMsg("Error", "Select a room!");
                        else if (selLec < 0)
                            showMsg("Error", "Select source lecture!");
                        else if (selAvail < 0 || selAvail >= (int)availableIndices.size())
                            showMsg("Error", "Select target slot!");
                        else {
                            Classroom* room = sys.getClassroom(selRoom);
                            if (room != NULL && selLec < room->getLectureCount()) {
                                Lecture& lec = room->getSchedule()[selLec];
                                
                                if (lec.getSubject() == "Lunch Break")
                                    showMsg("Error", "Cannot reschedule Lunch Break!");
                                else if (lec.getSubject() == "Free Slot")
                                    showMsg("Error", "Cannot reschedule Free Slot!");
                                else if (lec.getStatus() != 0)
                                    showMsg("Error", "Only ACTIVE lectures can be rescheduled!");
                                else {
                                    int targetIdx = availableIndices[selAvail];
                                    bool success = sys.reschedule(selRoom, selLec, targetIdx);
                                    if (success) {
                                        showMsg("Success", "Lecture rescheduled!");
                                        selLec = -1;
                                        selAvail = -1;
                                        updateAvailableList();
                                    }
                                    else
                                        showMsg("Error", "Reschedule failed!");
                                }
                            }
                        }
                    }
                    
                    // Back button
                    if (mx > 640 && mx < 760 && my > 380 && my < 415) {
                        screen = 2;
                        clearInputs();
                        resetSelections();
                    }
                }
                // ========== SCREEN 9: Message ==========
                else if (screen == 9) {
                    if (mx > 380 && mx < 520 && my > 340 && my < 375) screen = prevScreen;
                }
                // ========== SCREEN 10: User Request ==========
                else if (screen == 10) {
                    if (mx > 30 && mx < 250 && my > 100 && my < 420) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) {
                            selRoom = idx;
                            selAvail = -1;
                            selLec = -1;
                            updateAvailableList();
                        }
                    }
                    
                    if (mx > 270 && mx < 560 && my > 100 && my < 420 && selRoom >= 0) {
                        updateAvailableList();
                        int idx = getClickIndex(my, 100, (int)availableSlots.size());
                        if (idx >= 0 && idx < (int)availableIndices.size()) {
                            selAvail = idx;
                            selLec = availableIndices[idx];
                        }
                    }
                    
                    if (mx > 590 && mx < 860 && my > 300 && my < 335) activeInput = 1;
                    if (mx > 590 && mx < 860 && my > 360 && my < 395) activeInput = 2;
                    
                    if (mx > 590 && mx < 720 && my > 420 && my < 455) {
                        if (input1.empty())
                            showMsg("Error", "Enter your name!");
                        else if (selRoom < 0)
                            showMsg("Error", "Select a classroom!");
                        else if (selLec < 0)
                            showMsg("Error", "Select an available slot!");
                        else {
                            if (sys.facultyRequest(input1, selRoom, selLec, input2.empty() ? "Booking" : input2)) {
                                clearInputs();
                                resetSelections();
                                showMsg("Success", "Request sent!");
                            }
                            else
                                showMsg("Error", "Slot not available!");
                        }
                    }
                    
                    if (mx > 740 && mx < 860 && my > 420 && my < 455) {
                        screen = 3;
                        clearInputs();
                        resetSelections();
                    }
                }
                // ========== SCREEN 11: View Available ==========
                else if (screen == 11) {
                    if (mx > 750 && mx < 870 && my > 520 && my < 555) screen = 3;
                    
                    if (mx > 30 && mx < 250 && my > 100 && my < 480) {
                        int idx = getClickIndex(my, 100, sys.classroomCount());
                        if (idx >= 0) {
                            selRoom = idx;
                            updateAvailableList();
                        }
                    }
                }
            }
        }
        
        // ==================== DRAWING ====================
        window.clear(sf::Color(240, 240, 240));
        
        sf::RectangleShape titleBar(sf::Vector2f(900, 50));
        titleBar.setFillColor(sf::Color(44, 62, 80));
        window.draw(titleBar);
        
        sf::Text title("Classroom Management System", font, 20);
        title.setPosition(300, 12);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        window.draw(title);
        
        // ========== SCREEN 0: Main Menu ==========
        if (screen == 0) {
            drawLabel(window, 260, 70, "9AM-8PM | Lectures 10AM-5PM | Pass: admin123", 11);
            drawBtn(window, 350, 180, 200, 45, "Admin Login", sf::Color(52, 152, 219), false);
            drawBtn(window, 350, 240, 200, 45, "User Menu", sf::Color(46, 204, 113), false);
            drawBtn(window, 350, 300, 200, 45, "System Status", sf::Color(155, 89, 182), false);
            drawBtn(window, 350, 360, 200, 45, "Exit", sf::Color(231, 76, 60), false);
        }
        // ========== SCREEN 1: Admin Login ==========
        else if (screen == 1) {
            drawLabel(window, 340, 200, "Enter Admin Password:");
            drawInput(window, 340, 235, 220, 35, input1, "Password", activeInput == 1, true);
            drawBtn(window, 340, 290, 100, 35, "Login", sf::Color(46, 204, 113), false);
            drawBtn(window, 460, 290, 100, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 2: Admin Panel ==========
        else if (screen == 2) {
            drawBtn(window, 20, 80, 140, 35, "View Rooms", sf::Color(52, 152, 219), false);
            drawBtn(window, 20, 125, 140, 35, "View Schedule", sf::Color(52, 152, 219), false);
            drawBtn(window, 20, 170, 140, 35, "Add Room", sf::Color(46, 204, 113), false);
            drawBtn(window, 20, 215, 140, 35, "Delete Room", sf::Color(231, 76, 60), false);
            drawBtn(window, 20, 260, 140, 35, "Mark OFF", sf::Color(230, 126, 34), false);
            drawBtn(window, 20, 305, 140, 35, "Delete Lecture", sf::Color(192, 57, 43), false);
            drawBtn(window, 20, 350, 140, 35, "Reschedule", sf::Color(155, 89, 182), false);
            drawBtn(window, 20, 395, 140, 35, "Requests(" + to_string(sys.pendingCount()) + ")", sf::Color(52, 73, 94), false);
            drawBtn(window, 20, 440, 140, 35, "Logout", sf::Color(149, 165, 166), false);
            
            drawLabel(window, 180, 80, "Classrooms:");
            vector<string> rooms = getRoomList();
            drawList(window, 180, 100, 220, 350, rooms, selRoom);
            
            drawLabel(window, 420, 80, "Schedule:");
            vector<string> lecs = getScheduleList();
            drawList(window, 420, 100, 460, 350, lecs, selLec);
            
            drawLabel(window, 20, 490, "Select room & lecture, then click action button.", 11);
        }
        // ========== SCREEN 3: User Panel ==========
        else if (screen == 3) {
            drawBtn(window, 350, 140, 200, 40, "View Classrooms", sf::Color(52, 152, 219), false);
            drawBtn(window, 350, 195, 200, 40, "View Schedule", sf::Color(52, 152, 219), false);
            drawBtn(window, 350, 250, 200, 40, "View Available", sf::Color(46, 204, 113), false);
            drawBtn(window, 350, 305, 200, 40, "Request Booking", sf::Color(155, 89, 182), false);
            drawBtn(window, 350, 360, 200, 40, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 4: View Classrooms ==========
        else if (screen == 4) {
            drawLabel(window, 50, 75, "All Classrooms:", 16);
            vector<string> rooms = getRoomList();
            drawList(window, 50, 100, 350, 380, rooms, selRoom);
            
            if (selRoom >= 0 && selRoom < sys.classroomCount()) {
                Classroom* r = sys.getClassroom(selRoom);
                if (r != NULL) {
                    drawLabel(window, 450, 100, "Details:", 14);
                    drawLabel(window, 450, 130, "Name: " + r->getName());
                    drawLabel(window, 450, 155, "Building: " + r->getBuilding());
                    drawLabel(window, 450, 180, "Capacity: " + to_string(r->getCapacity()));
                    drawLabel(window, 450, 205, "Lectures: " + to_string(r->getLectureCount()));
                }
            }
            
            drawBtn(window, 750, 520, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 5: View Schedule ==========
        else if (screen == 5) {
            drawLabel(window, 30, 75, "Rooms:", 14);
            vector<string> rooms = getRoomList();
            drawList(window, 30, 100, 220, 380, rooms, selRoom);
            
            drawLabel(window, 270, 75, "Schedule:", 14);
            vector<string> lecs = getScheduleList();
            drawList(window, 270, 100, 600, 380, lecs, selLec);
            
            drawBtn(window, 750, 520, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 6: Requests ==========
        else if (screen == 6) {
            drawLabel(window, 50, 75, "All Requests:", 16);
            vector<string> reqs = getRequestList();
            drawList(window, 50, 100, 550, 330, reqs, selReq);
            
            drawBtn(window, 50, 450, 120, 35, "Approve", sf::Color(46, 204, 113), false);
            drawBtn(window, 190, 450, 120, 35, "Reject", sf::Color(231, 76, 60), false);
            drawBtn(window, 750, 450, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 7: Add Classroom ==========
        else if (screen == 7) {
            drawLabel(window, 300, 100, "Add New Classroom", 18);
            
            drawLabel(window, 300, 135, "Room Name:");
            drawInput(window, 300, 150, 300, 35, input1, "e.g. Room 301", activeInput == 1);
            
            drawLabel(window, 300, 195, "Building:");
            drawInput(window, 300, 210, 300, 35, input2, "e.g. Building A", activeInput == 2);
            
            drawLabel(window, 300, 255, "Capacity:");
            drawInput(window, 300, 270, 300, 35, input3, "e.g. 50", activeInput == 3);
            
            drawBtn(window, 300, 330, 120, 35, "Add", sf::Color(46, 204, 113), false);
            drawBtn(window, 440, 330, 120, 35, "Cancel", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 8: Reschedule ==========
        else if (screen == 8) {
            drawLabel(window, 30, 75, "1. Select Room:", 14);
            vector<string> rooms = getRoomList();
            drawList(window, 30, 100, 190, 250, rooms, selRoom);
            
            drawLabel(window, 240, 75, "2. Select Lecture (ACTIVE):", 14);
            vector<string> lecs = getScheduleList();
            drawList(window, 240, 100, 240, 250, lecs, selLec);
            
            drawLabel(window, 500, 75, "3. Select Target Slot:", 14);
            updateAvailableList();
            drawList(window, 500, 100, 240, 250, availableSlots, selAvail);
            
            if (availableSlots.empty() && selRoom >= 0)
                drawLabel(window, 510, 110, "No available slots!", 11);
            
            // Show selection info
            if (selRoom >= 0 && selLec >= 0) {
                Classroom* room = sys.getClassroom(selRoom);
                if (room != NULL && selLec < room->getLectureCount()) {
                    Lecture& lec = room->getSchedule()[selLec];
                    drawLabel(window, 30, 360, "Source: " + lec.getSubject() + " (" + lec.getTime() + ") [" + lec.getStatusString() + "]", 12);
                }
            }
            
            if (selAvail >= 0 && selAvail < (int)availableSlots.size()) {
                drawLabel(window, 30, 385, "Target: " + availableSlots[selAvail], 12);
            }
            
            drawBtn(window, 500, 380, 120, 35, "Reschedule", sf::Color(155, 89, 182), false);
            drawBtn(window, 640, 380, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 9: Message ==========
        else if (screen == 9) {
            sf::RectangleShape overlay(sf::Vector2f(900, 600));
            overlay.setFillColor(sf::Color(0, 0, 0, 120));
            window.draw(overlay);
            
            sf::RectangleShape box(sf::Vector2f(340, 170));
            box.setPosition(280, 200);
            box.setFillColor(sf::Color::White);
            box.setOutlineThickness(3);
            box.setOutlineColor(sf::Color(52, 152, 219));
            window.draw(box);
            
            sf::Text t(msgTitle, font, 18);
            t.setPosition(300, 220);
            t.setFillColor(sf::Color(44, 62, 80));
            t.setStyle(sf::Text::Bold);
            window.draw(t);
            
            sf::Text msg(msgText, font, 13);
            msg.setPosition(300, 260);
            msg.setFillColor(sf::Color(60, 60, 60));
            window.draw(msg);
            
            drawBtn(window, 380, 340, 140, 35, "OK", sf::Color(52, 152, 219), false);
        }
        // ========== SCREEN 10: User Request ==========
        else if (screen == 10) {
            drawLabel(window, 30, 75, "1. Select Room:", 14);
            vector<string> rooms = getRoomList();
            drawList(window, 30, 100, 220, 320, rooms, selRoom);
            
            drawLabel(window, 270, 75, "2. Select Available Slot:", 14);
            updateAvailableList();
            drawList(window, 270, 100, 290, 320, availableSlots, selAvail);
            
            if (availableSlots.empty() && selRoom >= 0)
                drawLabel(window, 280, 110, "No slots available!", 11);
            
            drawLabel(window, 590, 280, "3. Your Name:");
            drawInput(window, 590, 300, 270, 35, input1, "Enter your name", activeInput == 1);
            
            drawLabel(window, 590, 340, "4. Purpose:");
            drawInput(window, 590, 360, 270, 35, input2, "e.g. Extra class", activeInput == 2);
            
            drawBtn(window, 590, 420, 130, 35, "Submit", sf::Color(46, 204, 113), false);
            drawBtn(window, 740, 420, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // ========== SCREEN 11: View Available ==========
        else if (screen == 11) {
            drawLabel(window, 30, 75, "Select Room:", 14);
            vector<string> rooms = getRoomList();
            drawList(window, 30, 100, 220, 380, rooms, selRoom);
            
            drawLabel(window, 270, 75, "Available Slots:", 14);
            updateAvailableList();
            drawList(window, 270, 100, 400, 380, availableSlots, -1);
            
            if (availableSlots.empty() && selRoom >= 0)
                drawLabel(window, 280, 110, "No slots available in this room!", 12);
            
            drawBtn(window, 750, 520, 120, 35, "Back", sf::Color(149, 165, 166), false);
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