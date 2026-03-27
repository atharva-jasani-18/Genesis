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
int screen = 0, selRoom = -1, selLec = -1;
string input = "", msgTitle = "", msgText = "";

void drawBtn(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, sf::Color c, bool hov)
{
    sf::RectangleShape btn(sf::Vector2f(bw, h));
    btn.setPosition(x, y);
    btn.setFillColor(hov ? sf::Color(c.r*0.8, c.g*0.8, c.b*0.8) : c);
    w.draw(btn);
    
    sf::Text t(txt, font, 14);
    t.setPosition(x + (bw - t.getGlobalBounds().width)/2, y + (h-18)/2);
    t.setFillColor(sf::Color::White);
    w.draw(t);
}

void drawInput(sf::RenderWindow& w, float x, float y, float bw, float h, string txt, bool pwd)
{
    sf::RectangleShape box(sf::Vector2f(bw, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(100, 100, 100));
    w.draw(box);
    
    sf::Text t(pwd ? string(txt.size(), '*') : txt, font, 14);
    t.setPosition(x + 8, y + (h-18)/2);
    t.setFillColor(sf::Color::Black);
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
    
    float ih = 32;
    for (int i = 0; i < items.size() && i * ih < h - 5; i++)
    {
        sf::RectangleShape item(sf::Vector2f(bw - 6, ih - 2));
        item.setPosition(x + 3, y + 3 + i * ih);
        item.setFillColor(i == sel ? sf::Color(52, 152, 219) : sf::Color(240, 240, 240));
        w.draw(item);
        
        sf::Text t(items[i], font, 11);
        t.setPosition(x + 8, y + 8 + i * ih);
        t.setFillColor(i == sel ? sf::Color::White : sf::Color::Black);
        w.draw(t);
    }
}

void showMsg(string title, string text) { msgTitle = title; msgText = text; screen = 9; }

void runSFML()
{
    sf::RenderWindow window(sf::VideoMode(850, 550), "Classroom Management System");
    window.setFramerateLimit(60);
    
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
            { cout << "Font error!\n"; return; }
    
    while (window.isOpen())
    {
        sf::Event e;
        sf::Vector2i m = sf::Mouse::getPosition(window);
        float mx = m.x, my = m.y;
        
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
            
            if (e.type == sf::Event::TextEntered && screen == 1)
            {
                if (e.text.unicode == 8 && input.size() > 0) input.pop_back();
                else if (e.text.unicode >= 32 && e.text.unicode < 127 && input.size() < 20)
                    input += (char)e.text.unicode;
            }
            
            if (e.type == sf::Event::MouseButtonReleased)
            {
                // Main Menu
                if (screen == 0)
                {
                    if (mx > 320 && mx < 530 && my > 180 && my < 225) screen = 1;
                    if (mx > 320 && mx < 530 && my > 240 && my < 285) screen = 3;
                    if (mx > 320 && mx < 530 && my > 300 && my < 345) window.close();
                }
                // Login
                else if (screen == 1)
                {
                    if (mx > 320 && mx < 420 && my > 280 && my < 315)
                    {
                        if (sys.login(input)) { screen = 2; input = ""; }
                        else showMsg("Error", "Wrong password!");
                    }
                    if (mx > 440 && mx < 540 && my > 280 && my < 315) { screen = 0; input = ""; }
                }
                // Admin Panel
                else if (screen == 2)
                {
                    if (mx > 30 && mx < 180 && my > 90 && my < 130) screen = 4;
                    if (mx > 30 && mx < 180 && my > 140 && my < 180) screen = 5;
                    if (mx > 30 && mx < 180 && my > 190 && my < 230)
                    {
                        if (selRoom >= 0 && selLec >= 0)
                        {
                            if (sys.markOff(selRoom, selLec)) showMsg("OK", "Marked OFF!");
                            else showMsg("Error", "Cannot mark OFF!");
                        }
                        else showMsg("Error", "Select room & lecture!");
                    }
                    if (mx > 30 && mx < 180 && my > 240 && my < 280)
                    {
                        if (selRoom >= 0 && selLec >= 0)
                        {
                            if (sys.deleteLecture(selRoom, selLec)) { selLec = -1; showMsg("OK", "Deleted!"); }
                            else showMsg("Error", "Cannot delete!");
                        }
                        else showMsg("Error", "Select room & lecture!");
                    }
                    if (mx > 30 && mx < 180 && my > 290 && my < 330)
                    {
                        if (selRoom >= 0)
                        {
                            sys.deleteClassroom(selRoom);
                            selRoom = -1; selLec = -1;
                            showMsg("OK", "Classroom deleted!");
                        }
                        else showMsg("Error", "Select classroom!");
                    }
                    if (mx > 30 && mx < 180 && my > 340 && my < 380) screen = 6;
                    if (mx > 30 && mx < 180 && my > 390 && my < 430) { sys.logout(); screen = 0; }
                    
                    // Room list click
                    if (mx > 200 && mx < 420) { int idx = (my - 93) / 32; 
                        if (idx >= 0 && idx < sys.classroomCount()) { selRoom = idx; selLec = -1; } }
                    // Lecture list click
                    if (mx > 440 && mx < 820 && selRoom >= 0) { int idx = (my - 93) / 32;
                        Classroom* r = sys.getClassroom(selRoom);
                        if (r && idx >= 0 && idx < r->getLectureCount()) selLec = idx; }
                }
                // User Panel
                else if (screen == 3)
                {
                    if (mx > 320 && mx < 530 && my > 140 && my < 180) screen = 4;
                    if (mx > 320 && mx < 530 && my > 195 && my < 235) screen = 5;
                    if (mx > 320 && mx < 530 && my > 250 && my < 290)
                    {
                        if (selRoom >= 0 && selLec >= 0)
                        {
                            if (sys.facultyRequest("User", selRoom, selLec, "Booking"))
                                showMsg("OK", "Request sent!");
                            else showMsg("Error", "Slot not available!");
                        }
                        else showMsg("Error", "Select room & lecture!");
                    }
                    if (mx > 320 && mx < 530 && my > 305 && my < 345) screen = 0;
                }
                // View Classrooms
                else if (screen == 4)
                {
                    if (mx > 700 && mx < 820 && my > 480 && my < 515) screen = sys.isAdmin() ? 2 : 3;
                    if (mx > 50 && mx < 350) { int idx = (my - 93) / 32;
                        if (idx >= 0 && idx < sys.classroomCount()) selRoom = idx; }
                }
                // View Schedule
                else if (screen == 5)
                {
                    if (mx > 700 && mx < 820 && my > 480 && my < 515) screen = sys.isAdmin() ? 2 : 3;
                    if (mx > 50 && mx < 280) { int idx = (my - 93) / 32;
                        if (idx >= 0 && idx < sys.classroomCount()) { selRoom = idx; selLec = -1; } }
                    if (mx > 300 && mx < 820 && selRoom >= 0) { int idx = (my - 93) / 32;
                        Classroom* r = sys.getClassroom(selRoom);
                        if (r && idx >= 0 && idx < r->getLectureCount()) selLec = idx; }
                }
                // Requests
                else if (screen == 6)
                {
                    if (mx > 50 && mx < 170 && my > 420 && my < 455)
                    {
                        if (selLec >= 0 && sys.approveRequest(selLec)) { selLec = -1; showMsg("OK", "Approved!"); }
                        else showMsg("Error", "Select request!");
                    }
                    if (mx > 190 && mx < 310 && my > 420 && my < 455)
                    {
                        if (selLec >= 0 && sys.rejectRequest(selLec)) { selLec = -1; showMsg("OK", "Rejected!"); }
                        else showMsg("Error", "Select request!");
                    }
                    if (mx > 700 && mx < 820 && my > 420 && my < 455) { screen = 2; selLec = -1; }
                    if (mx > 50 && mx < 500) { int idx = (my - 93) / 32;
                        if (idx >= 0 && idx < sys.requestCount()) selLec = idx; }
                }
                // Message
                else if (screen == 9)
                {
                    if (mx > 360 && mx < 490 && my > 320 && my < 355) screen = sys.isAdmin() ? 2 : 0;
                }
            }
        }
        
        window.clear(sf::Color(236, 240, 241));
        
        sf::Text title("Classroom Management System", font, 22);
        title.setPosition(260, 15);
        title.setFillColor(sf::Color(44, 62, 80));
        title.setStyle(sf::Text::Bold);
        window.draw(title);
        
        if (screen == 0)
        {
            sf::Text sub("9AM-8PM | Lectures 10AM-5PM | Pass: admin123", font, 11);
            sub.setPosition(280, 50);
            sub.setFillColor(sf::Color(100, 100, 100));
            window.draw(sub);
            
            drawBtn(window, 320, 180, 210, 45, "Admin Login", sf::Color(52, 152, 219), my > 180 && my < 225);
            drawBtn(window, 320, 240, 210, 45, "User Menu", sf::Color(46, 204, 113), my > 240 && my < 285);
            drawBtn(window, 320, 300, 210, 45, "Exit", sf::Color(231, 76, 60), my > 300 && my < 345);
        }
        else if (screen == 1)
        {
            sf::Text lbl("Password:", font, 14);
            lbl.setPosition(320, 210);
            lbl.setFillColor(sf::Color::Black);
            window.draw(lbl);
            
            drawInput(window, 320, 235, 220, 32, input, true);
            drawBtn(window, 320, 280, 100, 35, "Login", sf::Color(46, 204, 113), false);
            drawBtn(window, 440, 280, 100, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 2)
        {
            drawBtn(window, 30, 90, 150, 40, "View Rooms", sf::Color(52, 152, 219), false);
            drawBtn(window, 30, 140, 150, 40, "View Schedule", sf::Color(52, 152, 219), false);
            drawBtn(window, 30, 190, 150, 40, "Mark OFF", sf::Color(230, 126, 34), false);
            drawBtn(window, 30, 240, 150, 40, "Delete Lecture", sf::Color(231, 76, 60), false);
            drawBtn(window, 30, 290, 150, 40, "Delete Room", sf::Color(192, 57, 43), false);
            drawBtn(window, 30, 340, 150, 40, "Requests", sf::Color(155, 89, 182), false);
            drawBtn(window, 30, 390, 150, 40, "Logout", sf::Color(149, 165, 166), false);
            
            vector<string> rooms;
            for (int i = 0; i < sys.classroomCount(); i++) rooms.push_back(sys.getClassroom(i)->getName());
            drawList(window, 200, 90, 220, 350, rooms, selRoom);
            
            vector<string> lecs;
            if (selRoom >= 0)
            {
                vector<Lecture>& s = sys.getClassroom(selRoom)->getSchedule();
                for (int i = 0; i < s.size(); i++)
                    lecs.push_back(s[i].getTime() + " " + s[i].getSubject() + " [" + s[i].getStatusString() + "]");
            }
            drawList(window, 440, 90, 380, 350, lecs, selLec);
            
            sf::Text info("Pending: " + to_string(sys.pendingCount()), font, 12);
            info.setPosition(30, 450);
            info.setFillColor(sf::Color::Black);
            window.draw(info);
        }
        else if (screen == 3)
        {
            drawBtn(window, 320, 140, 210, 40, "View Classrooms", sf::Color(52, 152, 219), false);
            drawBtn(window, 320, 195, 210, 40, "View Schedule", sf::Color(52, 152, 219), false);
            drawBtn(window, 320, 250, 210, 40, "Request Booking", sf::Color(46, 204, 113), false);
            drawBtn(window, 320, 305, 210, 40, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 4)
        {
            sf::Text lbl("All Classrooms", font, 16);
            lbl.setPosition(50, 65);
            lbl.setFillColor(sf::Color::Black);
            window.draw(lbl);
            
            vector<string> rooms;
            for (int i = 0; i < sys.classroomCount(); i++)
            {
                Classroom* r = sys.getClassroom(i);
                rooms.push_back(r->getName() + " (" + r->getBuilding() + ") Cap:" + to_string(r->getCapacity()));
            }
            drawList(window, 50, 90, 300, 380, rooms, selRoom);
            drawBtn(window, 700, 480, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 5)
        {
            sf::Text l1("Rooms", font, 13); l1.setPosition(50, 70); l1.setFillColor(sf::Color::Black); window.draw(l1);
            sf::Text l2("Schedule (Click to select)", font, 13); l2.setPosition(300, 70); l2.setFillColor(sf::Color::Black); window.draw(l2);
            
            vector<string> rooms;
            for (int i = 0; i < sys.classroomCount(); i++) rooms.push_back(sys.getClassroom(i)->getName());
            drawList(window, 50, 90, 230, 380, rooms, selRoom);
            
            vector<string> lecs;
            if (selRoom >= 0)
            {
                vector<Lecture>& s = sys.getClassroom(selRoom)->getSchedule();
                for (int i = 0; i < s.size(); i++)
                    lecs.push_back(s[i].getTime() + " | " + s[i].getSubject() + " [" + s[i].getStatusString() + "]");
            }
            drawList(window, 300, 90, 520, 380, lecs, selLec);
            drawBtn(window, 700, 480, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 6)
        {
            sf::Text lbl("All Requests", font, 16);
            lbl.setPosition(50, 65);
            lbl.setFillColor(sf::Color::Black);
            window.draw(lbl);
            
            vector<string> reqs;
            for (int i = 0; i < sys.requestCount(); i++)
            {
                Request& r = sys.getRequests()[i];
                reqs.push_back(r.getRequester() + " | " + r.getRoom() + " | " + r.getTime() + " [" + r.getStatusString() + "]");
            }
            drawList(window, 50, 90, 500, 310, reqs, selLec);
            
            drawBtn(window, 50, 420, 120, 35, "Approve", sf::Color(46, 204, 113), false);
            drawBtn(window, 190, 420, 120, 35, "Reject", sf::Color(231, 76, 60), false);
            drawBtn(window, 700, 420, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 9)
        {
            sf::RectangleShape ov(sf::Vector2f(850, 550));
            ov.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(ov);
            
            sf::RectangleShape box(sf::Vector2f(300, 150));
            box.setPosition(275, 190);
            box.setFillColor(sf::Color::White);
            box.setOutlineThickness(2);
            box.setOutlineColor(sf::Color(52, 152, 219));
            window.draw(box);
            
            sf::Text t(msgTitle, font, 16);
            t.setPosition(290, 210);
            t.setFillColor(sf::Color::Black);
            t.setStyle(sf::Text::Bold);
            window.draw(t);
            
            sf::Text m(msgText, font, 13);
            m.setPosition(290, 250);
            m.setFillColor(sf::Color(60, 60, 60));
            window.draw(m);
            
            drawBtn(window, 360, 320, 130, 35, "OK", sf::Color(52, 152, 219), false);
        }
        
        window.display();
    }
}
#endif

// ===================== CONSOLE MODE =====================
void consoleMenu(ClassroomSystem& sys)
{
    int choice;
    while (true)
    {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1. Admin Login\n2. User Menu\n3. Status\n0. Exit\n> ";
        cin >> choice;
        
        if (choice == 0) break;
        if (choice == 3) { sys.showStatus(); continue; }
        
        if (choice == 1)
        {
            string pass;
            cout << "Password: ";
            cin >> pass;
            if (!sys.login(pass)) { cout << "Wrong!\n"; continue; }
            
            while (true)
            {
                cout << "\n===== ADMIN =====\n";
                cout << "1. Add Room\n2. Delete Room\n3. View Rooms\n4. Schedule\n";
                cout << "5. Mark OFF\n6. Delete Lecture\n7. Reschedule\n8. Requests\n";
                cout << "9. Approve\n10. Reject\n0. Logout\n> ";
                cin >> choice;
                
                if (choice == 0) { sys.logout(); break; }
                if (choice == 1)
                {
                    string n, b; int c;
                    cout << "Name: "; cin.ignore(); getline(cin, n);
                    cout << "Building: "; getline(cin, b);
                    cout << "Capacity: "; cin >> c;
                    sys.addClassroom(n, b, c);
                }
                else if (choice == 2)
                {
                    sys.showClassrooms();
                    cout << "Index: "; cin >> choice;
                    sys.deleteClassroom(choice - 1);
                }
                else if (choice == 3) sys.showClassrooms();
                else if (choice == 4)
                {
                    sys.showClassrooms();
                    cout << "Room: "; cin >> choice;
                    sys.showSchedule(choice - 1);
                }
                else if (choice == 5)
                {
                    int r, l;
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showSchedule(r - 1); cout << "Lecture: "; cin >> l;
                    sys.markOff(r - 1, l);
                }
                else if (choice == 6)
                {
                    int r, l;
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showSchedule(r - 1); cout << "Lecture: "; cin >> l;
                    sys.deleteLecture(r - 1, l);
                }
                else if (choice == 7)
                {
                    int r, l; string t;
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showSchedule(r - 1); cout << "Lecture: "; cin >> l;
                    cout << "New time: "; cin >> t;
                    sys.reschedule(r - 1, l, t);
                }
                else if (choice == 8) sys.showRequests();
                else if (choice == 9)
                {
                    sys.showRequests(); cout << "Index: "; cin >> choice;
                    sys.approveRequest(choice);
                }
                else if (choice == 10)
                {
                    sys.showRequests(); cout << "Index: "; cin >> choice;
                    sys.rejectRequest(choice);
                }
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                cout << "\n===== USER =====\n";
                cout << "1. View Rooms\n2. Schedule\n3. Available\n4. Request\n0. Back\n> ";
                cin >> choice;
                
                if (choice == 0) break;
                if (choice == 1) sys.showClassrooms();
                else if (choice == 2)
                {
                    sys.showClassrooms(); cout << "Room: "; cin >> choice;
                    sys.showSchedule(choice - 1);
                }
                else if (choice == 3)
                {
                    sys.showClassrooms(); cout << "Room: "; cin >> choice;
                    sys.showAvailable(choice - 1);
                }
                else if (choice == 4)
                {
                    int r, l; string n, p;
                    cout << "Name: "; cin.ignore(); getline(cin, n);
                    sys.showClassrooms(); cout << "Room: "; cin >> r;
                    sys.showAvailable(r - 1); cout << "Lecture: "; cin >> l;
                    cout << "Purpose: "; cin.ignore(); getline(cin, p);
                    if (sys.facultyRequest(n, r - 1, l, p)) cout << "Sent!\n";
                    else cout << "Failed!\n";
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