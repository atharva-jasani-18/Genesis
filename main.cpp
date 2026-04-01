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
int screen = 0, selRoom = -1, selLec = -1, oldLec = -1, newLec = -1, rescheduleStep = 0;
string input = "", msgTitle = "", msgText = "";

void drawBtn(sf::RenderWindow& w, float x, float y, float ww, float h, string txt, sf::Color c, bool hov)
{
    sf::RectangleShape btn(sf::Vector2f(ww, h));
    btn.setPosition(x, y);
    btn.setFillColor(hov ? sf::Color(c.r*0.8, c.g*0.8, c.b*0.8) : c);
    w.draw(btn);
    
    sf::Text t(txt, font, 14);
    t.setPosition(x + (ww - t.getGlobalBounds().width)/2, y + (h-18)/2);
    t.setFillColor(sf::Color::White);
    w.draw(t);
}

void drawInput(sf::RenderWindow& w, float x, float y, float ww, float h, string txt, bool pwd)
{
    sf::RectangleShape box(sf::Vector2f(ww, h));
    box.setPosition(x, y);
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(200, 200, 200));
    w.draw(box);
    
    sf::Text t(pwd ? string(txt.size(), '*') : txt, font, 14);
    t.setPosition(x + 8, y + (h-18)/2);
    t.setFillColor(sf::Color::Black);
    w.draw(t);
}

void drawList(sf::RenderWindow& w, float x, float y, float ww, float h, vector<string>& items, int sel, int highlight1 = -1, int highlight2 = -1)
{
    sf::RectangleShape bg(sf::Vector2f(ww, h));
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color::White);
    bg.setOutlineThickness(1);
    bg.setOutlineColor(sf::Color(180, 180, 180));
    w.draw(bg);
    
    float ih = 32;
    for (int i = 0; i < items.size() && i * ih < h - 5; i++)
    {
        sf::RectangleShape item(sf::Vector2f(ww - 6, ih - 2));
        item.setPosition(x + 3, y + 3 + i * ih);
        
        sf::Color col;
        if (i == highlight1) col = sf::Color(52, 152, 219); // Blue for old
        else if (i == highlight2) col = sf::Color(46, 204, 113); // Green for new
        else if (i == sel) col = sf::Color(230, 126, 34); // Orange for selected
        else col = sf::Color(240, 240, 240);
        
        item.setFillColor(col);
        w.draw(item);
        
        sf::Text t(items[i], font, 11);
        t.setPosition(x + 8, y + 8 + i * ih);
        t.setFillColor(i == highlight1 || i == highlight2 ? sf::Color::White : sf::Color::Black);
        w.draw(t);
    }
}

void showMsg(string title, string text) { msgTitle = title; msgText = text; screen = 9; }

void runSFML()
{
    sf::RenderWindow window(sf::VideoMode(900, 550), "Classroom Management System");
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
                    if (mx > 30 && mx < 180 && my > 440 && my < 480) screen = 7; // Reschedule
                    
                    // Room list click
                    if (mx > 200 && mx < 420) 
                    { 
                        int idx = (my - 93) / 32; 
                        if (idx >= 0 && idx < sys.classroomCount()) 
                        { 
                            selRoom = idx; 
                            selLec = -1; 
                            oldLec = -1;
                            newLec = -1;
                            rescheduleStep = 0;
                        } 
                    }
                    // Lecture list click
                    if (mx > 440 && mx < 820 && selRoom >= 0) 
                    { 
                        int idx = (my - 93) / 32;
                        Classroom* r = sys.getClassroom(selRoom);
                        if (r && idx >= 0 && idx < r->getLectureCount()) 
                        {
                            if (screen == 7) // Reschedule screen
                            {
                                if (rescheduleStep == 0)
                                {
                                    oldLec = idx;
                                    rescheduleStep = 1;
                                }
                                else if (rescheduleStep == 1)
                                {
                                    newLec = idx;
                                    // Try reschedule
                                    if (sys.reschedule(selRoom, oldLec, newLec))
                                    {
                                        showMsg("OK", "Rescheduled!");
                                        oldLec = -1;
                                        newLec = -1;
                                        rescheduleStep = 0;
                                    }
                                    else
                                    {
                                        showMsg("Error", "Cannot reschedule!\nOld must be ACTIVE\nNew must be Free Slot");
                                        newLec = -1;
                                        rescheduleStep = 0;
                                    }
                                }
                            }
                            else
                            {
                                selLec = idx;
                            }
                        }
                    }
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
                // Reschedule Screen
                else if (screen == 7)
                {
                    if (mx > 700 && mx < 820 && my > 480 && my < 515) 
                    { 
                        screen = 2; 
                        oldLec = -1; 
                        newLec = -1; 
                        rescheduleStep = 0;
                    }
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
            
            drawBtn(window, 320, 180, 210, 45, "Admin Login", sf::Color(52, 152, 219), mx > 180 && my < 225);
            drawBtn(window, 320, 240, 210, 45, "User Menu", sf::Color(46, 204, 113), mx > 240 && my < 285);
            drawBtn(window, 320, 300, 210, 45, "Exit", sf::Color(231, 76, 60), mx > 300 && my < 345);
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
            drawBtn(window, 30, 440, 150, 40, "Reschedule", sf::Color(155, 89, 182), false);
            
            vector<string> rooms;
            for (int i = 0; i < sys.classroomCount(); i++) 
                rooms.push_back(sys.getClassroom(i)->getName());
            drawList(window, 200, 90, 220, 350, rooms, selRoom);
            
            vector<string> lecs;
            if (selRoom >= 0)
            {
                Classroom* r = sys.getClassroom(selRoom);
                for (int i = 0; i < r->getLectureCount(); i++)
                {
                    Lecture* l = r->getLecture(i);
                    string status = l->getStatusString();
                    lecs.push_back(l->getTime() + " " + l->getSubject() + " [" + status + "]");
                }
            }
            drawList(window, 440, 90, 380, 350, lecs, selLec);
            
            sf::Text info("Pending: " + to_string(sys.pendingCount()), font, 12);
            info.setPosition(30, 500);
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
                Classroom* r = sys.getClassroom(selRoom);
                for (int i = 0; i < r->getLectureCount(); i++)
                {
                    Lecture* l = r->getLecture(i);
                    lecs.push_back(l->getTime() + " | " + l->getSubject() + " [" + l->getStatusString() + "]");
                }
            }
            drawList(window, 300, 90, 520, 380, lecs, selLec);
            drawBtn(window, 700, 480, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        // Screen 6: Requests
        else if (screen == 6)
        {
            sf::Text lbl("All Requests", font, 16);
            lbl.setPosition(50, 65);
            lbl.setFillColor(sf::Color::Black);
            window.draw(lbl);
    
            vector<string> reqs;
            for (int i = 0; i < sys.requestCount(); i++)
            {
                Request* r = sys.getRequest(i);
                if (r != NULL)
                {
                reqs.push_back(r->getRequester() + " | " + r->getRoom() + " | " + r->getTime() + " [" + r->getStatusString() + "]");
                }
            }
            drawList(window, 50, 90, 550, 310, reqs, selReq);
    
            drawBtn(window, 50, 420, 120, 35, "Approve", sf::Color(46, 204, 113), false);
            drawBtn(window, 190, 420, 120, 35, "Reject", sf::Color(231, 76, 60), false);
            drawBtn(window, 700, 420, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 7) // Reschedule
        {
            sf::Text lbl1("Rooms", font, 13); lbl1.setPosition(50, 70); lbl1.setFillColor(sf::Color::Black); window.draw(lbl1);
            sf::Text lbl2("Schedule (Select lecture to move)", font, 13); lbl2.setPosition(200, 70); lbl2.setFillColor(sf::Color::Black); window.draw(lbl2);
            sf::Text lbl3("Free Slots (Select destination)", font, 13); lbl3.setPosition(500, 70); lbl3.setFillColor(sf::Color::Black); window.draw(lbl3);
            
            // Room list
            vector<string> rooms;
            for (int i = 0; i < sys.classroomCount(); i++) 
                rooms.push_back(sys.getClassroom(i)->getName());
            drawList(window, 50, 90, 130, 350, rooms, selRoom);
            
            // Schedule list
            vector<string> scheduleList;
            if (selRoom >= 0)
            {
                Classroom* r = sys.getClassroom(selRoom);
                for (int i = 0; i < r->getLectureCount(); i++)
                {
                    Lecture* l = r->getLecture(i);
                    scheduleList.push_back(l->getTime() + " " + l->getSubject() + " [" + l->getStatusString() + "]");
                }
            }
            drawList(window, 200, 90, 280, 350, scheduleList, selLec, oldLec, newLec);
            
            // Free slots list
            vector<string> freeSlots;
            if (selRoom >= 0)
            {
                Classroom* r = sys.getClassroom(selRoom);
                for (int i = 0; i < r->getLectureCount(); i++)
                {
                    Lecture* l = r->getLecture(i);
                    if (l->isFreeSlot() && l->getStatus() == Constants::STATUS_OFF)
                    {
                        freeSlots.push_back(l->getTime() + " - Free");
                    }
                }
            }
            drawList(window, 500, 90, 200, 350, freeSlots, -1, -1, newLec);
            
            // Instructions
            sf::Text instr;
            instr.setFont(font);
            if (rescheduleStep == 0)
                instr.setString("Step 1: Select lecture to move (blue)");
            else
                instr.setString("Step 2: Select free slot (green)");
            instr.setCharacterSize(12);
            instr.setFillColor(sf::Color(60, 60, 60));
            instr.setPosition(200, 460);
            window.draw(instr);
            
            drawBtn(window, 700, 480, 120, 35, "Back", sf::Color(149, 165, 166), false);
        }
        else if (screen == 9)
        {
            sf::RectangleShape ov(sf::Vector2f(900, 550));
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