#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace sf;

// Color palette for better visual design
namespace Colors {
    const Color PRIMARY(41, 128, 185);        // Blue
    const Color PRIMARY_DARK(52, 73, 94);     // Dark blue-gray
    const Color PRIMARY_LIGHT(174, 214, 241); // Light blue
    const Color SUCCESS(46, 204, 113);        // Green
    const Color DANGER(231, 76, 60);          // Red
    const Color WARNING(241, 196, 15);        // Yellow
    const Color BACKGROUND(236, 240, 241);    // Light gray
    const Color TEXT_DARK(44, 62, 80);        // Dark gray
    const Color TEXT_LIGHT(236, 240, 241);    // Light gray
    const Color CARD_BG(255, 255, 255, 240);  // Semi-transparent white
    const Color SHADOW(0, 0, 0, 50);          // Shadow
}

// Structure Definitions
struct Ticket {
    int ticketID;
    string Name, Age, NIC, Contact, BookedClass, BookedTName, BookedTID,
        BookedDest, BookedSource, BookedDate, DeptTime, ArrTime, BookedSeatNum;
    float Price;
    Ticket* next;
};

struct Train {
    string TName, TID, TDest, TSource, TDate, arriveTime, departTime, TClasses;
    int TSeats;
};

// Enhanced Button Class with modern styling
class Button {
private:
    RectangleShape shape;
    RectangleShape shadow;
    Text text;
    int id;
    bool isHovered;
    bool isPressed;
    Clock pressClock;
    bool wasPressed;
    Color normalColor;
    Color hoverColor;
    Color pressColor;

public:
    Button(Vector2f size, Vector2f position, const string& btnText, Font& font, int btnId,
        Color normal = Colors::PRIMARY, Color hover = Colors::PRIMARY_DARK, Color press = Colors::PRIMARY_LIGHT) {

        // Shadow setup
        shadow.setSize(size);
        shadow.setPosition(position.x + 3, position.y + 3);
        shadow.setFillColor(Colors::SHADOW);

        // Main button setup
        shape.setSize(size);
        shape.setPosition(position);
        normalColor = normal;
        hoverColor = hover;
        pressColor = press;
        shape.setFillColor(normalColor);

        // Rounded corners effect (simulated with outline)
        shape.setOutlineThickness(0);

        text.setFont(font);
        text.setString(btnText);
        text.setCharacterSize(20);
        text.setFillColor(Colors::TEXT_LIGHT);
        text.setStyle(Text::Bold);

        FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(position.x + size.x / 2.0f,
            position.y + size.y / 2.0f);

        id = btnId;
        isHovered = false;
        isPressed = false;
        wasPressed = false;
    }

    void update(Vector2f mousePos) {
        isHovered = shape.getGlobalBounds().contains(mousePos);

        bool currentlyPressed = isHovered && Mouse::isButtonPressed(Mouse::Left);

        if (currentlyPressed && !wasPressed) {
            isPressed = true;
            pressClock.restart();
        }
        else if (!currentlyPressed) {
            isPressed = false;
        }

        wasPressed = currentlyPressed;

        // Enhanced visual feedback with smooth transitions
        if (isPressed) {
            shape.setFillColor(pressColor);
            text.setFillColor(Colors::TEXT_DARK);
        }
        else if (isHovered) {
            shape.setFillColor(hoverColor);
            text.setFillColor(Colors::TEXT_LIGHT);
        }
        else {
            shape.setFillColor(normalColor);
            text.setFillColor(Colors::TEXT_LIGHT);
        }
    }

    bool isClicked(Vector2f mousePos, const Event& event) {
        return (
            event.type == Event::MouseButtonReleased &&
            event.mouseButton.button == Mouse::Left &&
            shape.getGlobalBounds().contains(mousePos)
            );
    }

    void draw(RenderWindow& window) {
        window.draw(shadow);
        window.draw(shape);
        window.draw(text);
    }

    int getId() const { return id; }
    bool getHovered() const { return isHovered; }
};

// Enhanced TextBox Class with modern styling
class TextBox {
private:
    RectangleShape box;
    RectangleShape shadow;
    Text text;
    Text placeholder;
    string content;
    string placeholderText;
    int maxLength;
    bool isSelected;
    Clock cursorClock;
    bool showCursor;

public:
    TextBox(Vector2f size, Vector2f position, Font& font, int maxLen, string placeholderTxt = "") {
        // Shadow setup
        shadow.setSize(size);
        shadow.setPosition(position.x + 2, position.y + 2);
        shadow.setFillColor(Colors::SHADOW);

        // Main box setup
        box.setSize(size);
        box.setPosition(position);
        box.setFillColor(Colors::CARD_BG);
        box.setOutlineThickness(2);
        box.setOutlineColor(Color(189, 195, 199)); // Light border

        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(Colors::TEXT_DARK);
        text.setPosition(position.x + 10, position.y + 8);

        // Placeholder setup
        placeholder.setFont(font);
        placeholder.setString(placeholderTxt);
        placeholder.setCharacterSize(18);
        placeholder.setFillColor(Color(149, 165, 166)); // Gray placeholder
        placeholder.setPosition(position.x + 10, position.y + 8);

        maxLength = maxLen;
        content = "";
        placeholderText = placeholderTxt;
        isSelected = false;
        showCursor = false;
    }

    void handleEvent(Event& event, Vector2f mousePos) {
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            bool wasSelected = isSelected;
            isSelected = box.getGlobalBounds().contains(mousePos);

            if (isSelected != wasSelected) {
                box.setOutlineColor(isSelected ? Colors::PRIMARY : Color(189, 195, 199));
                if (isSelected) {
                    cursorClock.restart();
                    showCursor = true;
                }
            }
        }

        if (isSelected && event.type == Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!content.empty()) {
                    content.pop_back();
                }
            }
            else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                if (content.length() < maxLength) {
                    content += static_cast<char>(event.text.unicode);
                }
            }
        }
    }

    void update() {
        if (isSelected) {
            if (cursorClock.getElapsedTime().asSeconds() > 0.5) {
                showCursor = !showCursor;
                cursorClock.restart();
            }
        }
        else {
            showCursor = false;
        }
    }

    void draw(RenderWindow& window) {
        window.draw(shadow);
        window.draw(box);

        if (content.empty() && !isSelected && !placeholderText.empty()) {
            window.draw(placeholder);
        }
        else {
            string displayText = content;
            if (isSelected && showCursor) {
                displayText += "|";
            }
            text.setString(displayText);
            window.draw(text);
        }
    }

    string getContent() const { return content; }
    void setContent(string newContent) {
        content = newContent;
        if (content.length() > maxLength) {
            content = content.substr(0, maxLength);
        }
    }
    bool isEmpty() const { return content.empty(); }
    bool isFocused() const { return isSelected; }
    void clearContent() { content = ""; }
};

// Railway Booking System Class with enhanced GUI
class RailwaySystem {
private:
    RenderWindow window;
    Font font;
    Texture bgTexture;
    Sprite background;

    Ticket* head;
    Ticket* tail;
    vector<Train> trains;

    int currentScreen;
    string errorMessage;
    Clock errorClock;
    bool showError;

    // Booking information
    string passengerName, passengerAge, passengerNIC, passengerContact;
    string selectedTrainID, selectedClass, selectedSeat;
    float ticketPrice;
    int currentTicketID;

    // UI Elements
    vector<Button> buttons;
    vector<TextBox> textBoxes;
    vector<Text> infoTexts;

public:
    RailwaySystem() : window(VideoMode(1200, 900), "Railway Booking System"),
        currentScreen(0), head(nullptr), tail(nullptr), showError(false) {

        // Try to load font
        if (!font.loadFromFile("arial.ttf")) {
            if (!font.loadFromFile("C:/Windows/Fonts/seguiemj.ttf") &&  // <- Replaced with Segoe UI Emoji
                !font.loadFromFile("C:/Windows/Fonts/arial.ttf") &&
                !font.loadFromFile("C:/Windows/Fonts/seguiemj.ttf") && // <- Absolute path fallback
                !font.loadFromFile("/System/Library/Fonts/Arial.ttf") &&
                !font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                cerr << "Warning: Could not load font file. Using default font." << endl;
            }
        }


        // Create gradient background
        Image img;
        img.create(1200, 900);
        for (int y = 0; y < 900; y++) {
            float factor = static_cast<float>(y) / 900.0f;
            Uint8 r = static_cast<Uint8>(52 + (236 - 52) * factor);
            Uint8 g = static_cast<Uint8>(73 + (240 - 73) * factor);
            Uint8 b = static_cast<Uint8>(94 + (241 - 94) * factor);

            for (int x = 0; x < 1200; x++) {
                img.setPixel(x, y, Color(r, g, b));
            }
        }
        bgTexture.loadFromImage(img);
        background.setTexture(bgTexture);

        initializeTrains();
        setupScreenUI();
        srand(time(0));
    }

    void initializeTrains() {
        trains = {
            {"Shalimar Express", "SHE753", "Lahore", "Karachi", "24-Dec-2020",
             "20:00 (8:00 PM)", "16:40 (4:40 PM)", "Economy, AC Lower, AC Business", 30},
            {"Karakoram Express", "KKE694", "Faisalabad", "Karachi", "26-Dec-2020",
             "08:00 (8:00 AM)", "04:40 (4:40 AM)", "Economy, AC Lower, AC Business", 30},
            {"Green Line Express", "GLE400", "Islamabad", "Karachi", "05-Jan-2021",
             "12:00 (12:00 PM)", "10:00 (10:00 AM)", "Economy, AC Lower, AC Business", 30},
            {"Tezgam Express", "TEZ123", "Faisalabad", "Karachi", "01-Jan-2021",
             "15:00 (03:00 PM)", "9:00 (09:00 AM)", "Economy, AC Lower, AC Business", 30},
            {"Karachi Express", "KHE123", "Lahore", "Karachi", "31-Dec-2020",
             "12:00 (12:00 AM)", "12:00 (09:00 PM)", "Economy, AC Lower, AC Business", 30}
        };
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            bool textBoxHandled = false;
            for (auto& tb : textBoxes) {
                tb.handleEvent(event, mousePos);
                if (tb.isFocused() && event.type == Event::TextEntered) {
                    textBoxHandled = true;
                }
            }

            if (!textBoxHandled) {
                for (auto& btn : buttons) {
                    btn.update(mousePos);
                }

                for (auto& btn : buttons) {
                    if (btn.isClicked(mousePos, event)) {
                        handleButtonClick(btn.getId());
                        break;
                    }
                }
            }
        }
    }

    void handleButtonClick(int buttonId) {
        int oldScreen = currentScreen;

        switch (currentScreen) {
        case 0: // Main menu
            if (buttonId == 1) currentScreen = 1;
            else if (buttonId == 2) currentScreen = 2;
            else if (buttonId == 3) currentScreen = 3;
            else if (buttonId == 4) window.close();
            break;

        case 1: // Booking screen
            if (buttonId == 1) {
                if (validateBookingForm()) {
                    passengerName = textBoxes[0].getContent() + " " + textBoxes[1].getContent();
                    passengerAge = textBoxes[2].getContent();
                    passengerNIC = textBoxes[3].getContent();
                    passengerContact = textBoxes[4].getContent();

                    currentTicketID = rand() % 9000 + 1000;
                    createTicket();
                    showErrorMessage("Ticket booked successfully! ID: " + to_string(currentTicketID));
                    currentScreen = 0;
                }
            }
            else if (buttonId == 2) {
                currentScreen = 0;
            }
            break;

        case 2: // View ticket
            if (buttonId == 1) {
                if (!textBoxes[0].isEmpty()) {
                    try {
                        int searchID = stoi(textBoxes[0].getContent());
                        displayTicketInfo(searchID);
                    }
                    catch (const std::exception& e) {
                        showErrorMessage("Invalid ticket ID format!");
                    }
                }
                else {
                    showErrorMessage("Please enter a ticket ID!");
                }
            }
            else if (buttonId == 2) {
                currentScreen = 0;
            }
            break;

        case 3: // Admin login
            if (buttonId == 1) {
                string username = textBoxes[0].getContent();
                string password = textBoxes[1].getContent();

                if ((username == "admin" && password == "admin123") ||
                    (username == "ABDULLAH" && password == "ABDI123") ||
                    (username == "USMAN" && password == "US123")) {
                    currentScreen = 4;
                    showErrorMessage("Login successful!");
                }
                else {
                    showErrorMessage("Invalid credentials!");
                }
            }
            else if (buttonId == 2) {
                currentScreen = 0;
            }
            break;

        case 4: // Admin panel
            if (buttonId == 1) {
                currentScreen = 6;
            }
            else if (buttonId == 2) {
                currentScreen = 7;
            }
            else if (buttonId == 3) {
                currentScreen = 0;
            }
            break;

        case 6: // View all bookings
            if (buttonId == 1) {
                currentScreen = 4;
            }
            break;

        case 7: // Add trains
            if (buttonId == 1) {
                if (validateTrainForm()) {
                    Train newTrain;
                    newTrain.TName = textBoxes[0].getContent();
                    newTrain.TID = textBoxes[1].getContent();
                    newTrain.TSource = textBoxes[2].getContent();
                    newTrain.TDest = textBoxes[3].getContent();
                    newTrain.TDate = textBoxes[4].getContent();
                    newTrain.departTime = textBoxes[5].getContent();
                    newTrain.arriveTime = textBoxes[6].getContent();
                    newTrain.TClasses = textBoxes[7].getContent();
                    try {
                        newTrain.TSeats = stoi(textBoxes[8].getContent());
                        trains.push_back(newTrain);
                        showErrorMessage("Train added successfully!");
                        currentScreen = 4;
                    }
                    catch (const std::exception& e) {
                        showErrorMessage("Invalid number of seats!");
                    }
                }
            }
            else if (buttonId == 2) {
                currentScreen = 4;
            }
            break;
        }

        if (oldScreen != currentScreen) {
            setupScreenUI();
        }
    }

    void setupScreenUI() {
        buttons.clear();
        textBoxes.clear();
        infoTexts.clear();

        switch (currentScreen) {
        case 0: setupMainMenuUI(); break;
        case 1: setupBookingScreenUI(); break;
        case 2: setupViewTicketScreenUI(); break;
        case 3: setupAdminLoginScreenUI(); break;
        case 4: setupAdminPanelUI(); break;
        case 6: setupAllBookingsUI(); break;
        case 7: setupAddTrainScreenUI(); break;
        }
    }

    void setupMainMenuUI() {
        buttons.push_back(Button(Vector2f(280, 60), Vector2f(460, 300), " Book Ticket", font, 1, Colors::PRIMARY, Colors::PRIMARY_DARK, Colors::PRIMARY_LIGHT));
        buttons.push_back(Button(Vector2f(280, 60), Vector2f(460, 390), " View Ticket", font, 2, Colors::SUCCESS, Color(39, 174, 96), Color(125, 206, 160)));
        buttons.push_back(Button(Vector2f(280, 60), Vector2f(460, 480), " Admin Login", font, 3, Colors::WARNING, Color(243, 156, 18), Color(245, 176, 65)));
        buttons.push_back(Button(Vector2f(280, 60), Vector2f(460, 570), " Exit", font, 4, Colors::DANGER, Color(192, 57, 43), Color(231, 76, 60)));
    }

    void setupBookingScreenUI() {
        textBoxes.push_back(TextBox(Vector2f(300, 45), Vector2f(400, 200), font, 20, "Enter first name"));
        textBoxes.push_back(TextBox(Vector2f(300, 45), Vector2f(400, 260), font, 20, "Enter last name"));
        textBoxes.push_back(TextBox(Vector2f(200, 45), Vector2f(400, 320), font, 3, "Age"));
        textBoxes.push_back(TextBox(Vector2f(300, 45), Vector2f(400, 380), font, 13, "XXXXX-XXXXXXX-X"));
        textBoxes.push_back(TextBox(Vector2f(300, 45), Vector2f(400, 440), font, 11, "03XX-XXXXXXX"));

        buttons.push_back(Button(Vector2f(180, 50), Vector2f(420, 520), " Continue", font, 1, Colors::SUCCESS, Color(39, 174, 96), Color(125, 206, 160)));
        buttons.push_back(Button(Vector2f(180, 50), Vector2f(620, 520), " Cancel", font, 2, Colors::DANGER, Color(192, 57, 43), Color(231, 76, 60)));
    }

    void setupViewTicketScreenUI() {
        textBoxes.push_back(TextBox(Vector2f(350, 45), Vector2f(425, 250), font, 10, "Enter Ticket ID"));

        buttons.push_back(Button(Vector2f(180, 50), Vector2f(450, 330), " Search", font, 1, Colors::PRIMARY, Colors::PRIMARY_DARK, Colors::PRIMARY_LIGHT));
        buttons.push_back(Button(Vector2f(180, 50), Vector2f(650, 330), " Back", font, 2, Colors::WARNING, Color(243, 156, 18), Color(245, 176, 65)));
    }

    void setupAdminLoginScreenUI() {
        textBoxes.push_back(TextBox(Vector2f(350, 45), Vector2f(425, 280), font, 20, "Username"));
        textBoxes.push_back(TextBox(Vector2f(350, 45), Vector2f(425, 360), font, 20, "Password"));

        buttons.push_back(Button(Vector2f(180, 50), Vector2f(450, 460), " Login", font, 1, Colors::PRIMARY, Colors::PRIMARY_DARK, Colors::PRIMARY_LIGHT));
        buttons.push_back(Button(Vector2f(180, 50), Vector2f(650, 460), " Back", font, 2, Colors::WARNING, Color(243, 156, 18), Color(245, 176, 65)));
    }

    void setupAdminPanelUI() {
        buttons.push_back(Button(Vector2f(320, 60), Vector2f(440, 250), " View All Bookings", font, 1, Colors::PRIMARY, Colors::PRIMARY_DARK, Colors::PRIMARY_LIGHT));
        buttons.push_back(Button(Vector2f(320, 60), Vector2f(440, 340), " Add Trains", font, 2, Colors::SUCCESS, Color(39, 174, 96), Color(125, 206, 160)));
        buttons.push_back(Button(Vector2f(320, 60), Vector2f(440, 430), " Logout", font, 3, Colors::DANGER, Color(192, 57, 43), Color(231, 76, 60)));
    }

    void setupAllBookingsUI() {
        Ticket* temp = head;
        int yPos = 200;
        int index = 1;

        while (temp != nullptr && yPos < 700) {
            string ticketInfo = to_string(index) + ". ID: " + to_string(temp->ticketID) +
                " | " + temp->Name + " | " + temp->BookedTName;
            infoTexts.push_back(Text(ticketInfo, font, 20));
            infoTexts.back().setPosition(100, yPos);
            infoTexts.back().setFillColor(Colors::TEXT_DARK);
            infoTexts.back().setStyle(Text::Bold);
            yPos += 35;
            index++;
            temp = temp->next;
        }

        buttons.push_back(Button(Vector2f(200, 50), Vector2f(500, 750), "← Back", font, 1, Colors::WARNING, Color(243, 156, 18), Color(245, 176, 65)));
    }

    void setupAddTrainScreenUI() {
        // Left column
        textBoxes.push_back(TextBox(Vector2f(280, 40), Vector2f(200, 150), font, 50, "Train Name"));
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(200, 210), font, 10, "Train ID"));
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(200, 270), font, 20, "Source City"));
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(200, 330), font, 20, "Destination"));
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(200, 390), font, 15, "DD-MMM-YYYY"));

        // Right column
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(700, 150), font, 20, "HH:MM"));
        textBoxes.push_back(TextBox(Vector2f(220, 40), Vector2f(700, 210), font, 20, "HH:MM"));
        textBoxes.push_back(TextBox(Vector2f(280, 40), Vector2f(700, 270), font, 50, "Economy, AC Lower"));
        textBoxes.push_back(TextBox(Vector2f(150, 40), Vector2f(700, 330), font, 4, "Number"));

        buttons.push_back(Button(Vector2f(200, 50), Vector2f(400, 480), "✓ Add Train", font, 1, Colors::SUCCESS, Color(39, 174, 96), Color(125, 206, 160)));
        buttons.push_back(Button(Vector2f(200, 50), Vector2f(620, 480), "✗ Cancel", font, 2, Colors::DANGER, Color(192, 57, 43), Color(231, 76, 60)));
    }

    bool validateBookingForm() {
        for (const auto& tb : textBoxes) {
            if (tb.isEmpty()) {
                showErrorMessage("Please fill all fields!");
                return false;
            }
        }
        return true;
    }

    bool validateTrainForm() {
        for (const auto& tb : textBoxes) {
            if (tb.isEmpty()) {
                showErrorMessage("Please fill all fields!");
                return false;
            }
        }
        return true;
    }

    void update() {
        for (auto& tb : textBoxes) {
            tb.update();
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        for (auto& btn : buttons) {
            btn.update(mousePos);
        }

        if (showError && errorClock.getElapsedTime().asSeconds() > 4) {
            showError = false;
        }
    }
    void render() {
        window.clear();
        window.draw(background);

        // Draw header card for each screen
        drawHeaderCard();

        switch (currentScreen) {
        case 0: renderMainMenu(); break;
        case 1: renderBookingScreen(); break;
        case 2: renderViewTicketScreen(); break;
        case 3: renderAdminLoginScreen(); break;
        case 4: renderAdminPanel(); break;
        case 6: renderAllBookings(); break;
        case 7: renderAddTrainScreen(); break;
        }

        for (auto& tb : textBoxes) {
            tb.draw(window);
        }
        for (auto& btn : buttons) {
            btn.draw(window);
        }
        for (auto& text : infoTexts) {
            window.draw(text);
        }

        if (showError) {
            renderErrorMessage();
        }

        window.display();
    }

    void drawHeaderCard() {
        RectangleShape headerCard(Vector2f(1000, 120));
        headerCard.setPosition(100, 20);
        headerCard.setFillColor(Colors::CARD_BG);

        RectangleShape headerShadow(Vector2f(1000, 120));
        headerShadow.setPosition(103, 23);
        headerShadow.setFillColor(Colors::SHADOW);

        window.draw(headerShadow);
        window.draw(headerCard);
    }

    void renderMainMenu() {
        displayText("RAILWAY BOOKING SYSTEM", 600, 50, 42, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Developed by:", 600, 180, 22, Colors::TEXT_DARK, Text::Regular, true);
        displayText("Muhammad Abdullah Khan & Muhammad Usman Nazir", 600, 210, 18, Colors::TEXT_DARK, Text::Regular, true);
    }

    void renderBookingScreen() {
        displayText("Book Your Ticket", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);

        displayText("First Name:", 120, 210, 20, Colors::TEXT_DARK, Text::Bold);
        displayText("Last Name:", 120, 270, 20, Colors::TEXT_DARK, Text::Bold);
        displayText("Age:", 120, 330, 20, Colors::TEXT_DARK, Text::Bold);
        displayText("CNIC Number:", 120, 390, 20, Colors::TEXT_DARK, Text::Bold);
        displayText("Contact Number:", 120, 450, 20, Colors::TEXT_DARK, Text::Bold);
    }

    void renderViewTicketScreen() {
        displayText("Search Your Ticket", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Enter your Ticket ID to view details:", 120, 210, 22, Colors::TEXT_DARK, Text::Bold);
    }

    void renderAdminLoginScreen() {
        displayText("Admin Portal", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Secure Login Required", 600, 180, 20, Colors::TEXT_DARK, Text::Italic, true);

        displayText("Username:", 120, 290, 20, Colors::TEXT_DARK, Text::Bold);
        displayText("Password:", 120, 370, 20, Colors::TEXT_DARK, Text::Bold);
    }

    void renderAdminPanel() {
        displayText("Admin Dashboard", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Welcome to the Admin Panel", 600, 180, 20, Colors::TEXT_DARK, Text::Regular, true);

        // Add a subtle info card
        RectangleShape infoCard(Vector2f(900, 80));
        infoCard.setPosition(150, 220);
        infoCard.setFillColor(Colors::CARD_BG);

        RectangleShape infoShadow(Vector2f(900, 80));
        infoShadow.setPosition(153, 223);
        infoShadow.setFillColor(Colors::SHADOW);

        window.draw(infoShadow);
        window.draw(infoCard);

        displayText("Manage your railway system efficiently", 600, 250, 18, Colors::TEXT_DARK, Text::Regular, true);
        displayText("Select an option from the menu below", 600, 275, 16, Colors::TEXT_DARK, Text::Regular, true);
    }

    void renderAllBookings() {
        displayText("All Bookings", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Complete booking records", 600, 180, 18, Colors::TEXT_DARK, Text::Regular, true);

        // Create content area with background
        RectangleShape contentCard(Vector2f(1000, 400));
        contentCard.setPosition(100, 220);
        contentCard.setFillColor(Colors::CARD_BG);

        RectangleShape contentShadow(Vector2f(1000, 400));
        contentShadow.setPosition(103, 223);
        contentShadow.setFillColor(Colors::SHADOW);

        window.draw(contentShadow);
        window.draw(contentCard);

        if (infoTexts.empty()) {
            displayText("No bookings found", 600, 400, 24, Colors::TEXT_DARK, Text::Regular, true);
            displayText("Bookings will appear here once tickets are booked", 600, 430, 16, Colors::TEXT_DARK, Text::Italic, true);
        }
    }

    void renderAddTrainScreen() {
        displayText("Add New Train", 600, 50, 36, Colors::TEXT_DARK, Text::Bold, true);
        displayText("Enter train details below", 600, 180, 18, Colors::TEXT_DARK, Text::Regular, true);

        // Create form background
        RectangleShape formCard(Vector2f(1000, 450));
        formCard.setPosition(100, 220);
        formCard.setFillColor(Colors::CARD_BG);

        RectangleShape formShadow(Vector2f(1000, 450));
        formShadow.setPosition(103, 223);
        formShadow.setFillColor(Colors::SHADOW);

        window.draw(formShadow);
        window.draw(formCard);

        // Left column labels with consistent styling
        displayText("Train Name:", 150, 280, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Train ID:", 150, 330, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Source:", 150, 380, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Destination:", 150, 430, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Date:", 150, 480, 18, Colors::TEXT_DARK, Text::Bold);

        // Right column labels with consistent styling
        displayText("Departure Time:", 550, 280, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Arrival Time:", 550, 330, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Classes:", 550, 380, 18, Colors::TEXT_DARK, Text::Bold);
        displayText("Seats:", 550, 430, 18, Colors::TEXT_DARK, Text::Bold);

        // Add helper text
        displayText("All fields are required", 600, 530, 14, Colors::TEXT_DARK, Text::Italic, true);
    }
    void renderErrorMessage() {
        // Enhanced error message with better styling
        RectangleShape errorBox(Vector2f(600, 60));
        errorBox.setFillColor(Color(220, 53, 69, 230)); // Bootstrap danger color with transparency
        errorBox.setOutlineThickness(2);
        errorBox.setOutlineColor(Color(176, 42, 55)); // Darker red border
        errorBox.setPosition(300, 500);

        // Add shadow for depth
        RectangleShape errorShadow(Vector2f(600, 60));
        errorShadow.setPosition(303, 503);
        errorShadow.setFillColor(Color(0, 0, 0, 100));

        window.draw(errorShadow);
        window.draw(errorBox);

        // Error icon and text
        displayText("!", 320, 515, 20, Color::White, Text::Bold);

        Text errorText(errorMessage, font, 18);
        errorText.setFillColor(Color::White);
        errorText.setStyle(Text::Bold);
        errorText.setPosition(350, 515);

        window.draw(errorText);
    }

    void displayText(string text, float x, float y, int size = 30, Color color = Color::White,
        Uint32 style = Text::Regular, bool centered = false) {
        Text displayText;
        displayText.setFont(font);
        displayText.setString(text);
        displayText.setCharacterSize(size);
        displayText.setFillColor(color);
        displayText.setStyle(style);

        if (centered) {
            FloatRect textBounds = displayText.getLocalBounds();
            displayText.setPosition(x - textBounds.width / 2, y);
        }
        else {
            displayText.setPosition(x, y);
        }

        window.draw(displayText);
    }

    void showErrorMessage(string message) {
        errorMessage = message;
        showError = true;
        errorClock.restart();
    }

    void displayTicketInfo(int ticketID) {
        infoTexts.clear();

        Ticket* temp = head;
        bool found = false;
        while (temp != nullptr) {
            if (temp->ticketID == ticketID) {
                // Create ticket display card
                RectangleShape ticketCard(Vector2f(900, 400));
                ticketCard.setPosition(150, 250);
                ticketCard.setFillColor(Colors::CARD_BG);

                RectangleShape ticketShadow(Vector2f(900, 400));
                ticketShadow.setPosition(153, 253);
                ticketShadow.setFillColor(Colors::SHADOW);

                window.draw(ticketShadow);
                window.draw(ticketCard);

                vector<string> ticketDetails = {
                    "Ticket ID: " + to_string(temp->ticketID),
                    "Name: " + temp->Name,
                    "Age: " + temp->Age,
                    "NIC: " + temp->NIC,
                    "Contact: " + temp->Contact,
                    "Train ID: " + temp->BookedTID,
                    "Train Name: " + temp->BookedTName,
                    "Class: " + temp->BookedClass,
                    "Seat: " + temp->BookedSeatNum,
                    "Date: " + temp->BookedDate,
                    "From: " + temp->BookedSource,
                    "To: " + temp->BookedDest,
                    "Departure: " + temp->DeptTime,
                    "Arrival: " + temp->ArrTime,
                    "Price: Rs. " + to_string(temp->Price)
                };

                for (size_t i = 0; i < ticketDetails.size(); i++) {
                    Text ticketText(ticketDetails[i], font, 16);
                    ticketText.setPosition(180, 270 + i * 24);
                    ticketText.setFillColor(Colors::TEXT_DARK);
                    ticketText.setStyle(Text::Regular);
                    infoTexts.push_back(ticketText);
                }

                found = true;
                break;
            }
            temp = temp->next;
        }

        if (!found) {
            showErrorMessage("Ticket not found! Please check your Ticket ID.");
        }
    }

    void createTicket() {
        Ticket* newTicket = new Ticket();
        newTicket->ticketID = currentTicketID;
        newTicket->Name = passengerName;
        newTicket->Age = passengerAge;
        newTicket->NIC = passengerNIC;
        newTicket->Contact = passengerContact;
        newTicket->BookedClass = "Economy";
        newTicket->BookedTName = "Sample Train";
        newTicket->BookedTID = "T123";
        newTicket->BookedDest = "Lahore";
        newTicket->BookedSource = "Karachi";
        newTicket->BookedDate = "01-Jan-2023";
        newTicket->DeptTime = "10:00 AM";
        newTicket->ArrTime = "06:00 PM";
        newTicket->BookedSeatNum = "A" + to_string(rand() % 50 + 1);
        newTicket->Price = 2500.0f;
        newTicket->next = nullptr;

        if (head == nullptr) {
            head = newTicket;
            tail = newTicket;
        }
        else {
            tail->next = newTicket;
            tail = newTicket;
        }
    }

    ~RailwaySystem() {
        // Clean up linked list
        while (head != nullptr) {
            Ticket* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main() {
    try {
        RailwaySystem system;
        system.run(); // Use the run() method instead of manual loop
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}