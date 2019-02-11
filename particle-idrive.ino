#include "iDrive.h"

//  CAN bus speed in HZ.
//  iDrive controllers are typicall 100khz or 500khz
IDrive iDrive(500000);

void buttonCallback(IDrive::ButtonRef button, IDrive::ButtonState state) {

    switch(button) {
        case IDrive::media :
            Serial.print("Media ");
            break;
        case IDrive::radio :
            Serial.print("Radio ");
            break;
        case IDrive::menu :
            Serial.print("Menu ");
            break;
        case IDrive::tel :
            Serial.print("Tel ");
            break;
        case IDrive::nav :
            Serial.print("Nav ");
            break;
        case IDrive::back :
            Serial.print("Back ");
            break;
        case IDrive::option :
            Serial.print("Option ");
            break;
        case IDrive::knob :
            Serial.print("Knob ");
            break;
        case IDrive::knobDirection :
            Serial.print("Knob Direction ");
    }
    
    if (state == IDrive::Released)
        Serial.println("released");
    else if (state == IDrive::Pressed)
        Serial.println("pressed");
    else if (state == IDrive::Held)
        Serial.println("held");
    else if (state == IDrive::Up)
        Serial.println("up");
    else if (state == IDrive::Down)
        Serial.println("down");
    else if (state == IDrive::Left)
        Serial.println("left");
    else if (state == IDrive::Right)
        Serial.println("right");
    else if (state == IDrive::UpHeld)
        Serial.println("up held");
    else if (state == IDrive::DownHeld)
        Serial.println("down held");
    else if (state == IDrive::LeftHeld)
        Serial.println("left held");
    else if (state == IDrive::RightHeld)
        Serial.println("right held");
}

void rotaryCallback(signed char moveAmount) {
    Serial.printlnf("Rotary moved %d", moveAmount);
}

void setup() {

    Serial.begin(9600);

    // Wait for Serial to connect
    while(!Serial.isConnected()) Particle.process();
    
    Serial.println("Welcome");
    
    // Debug prints all can bus messages to the screen.
    // Off by default.
    iDrive.setDebug(false);
    
    // Add the callack for button presses
    iDrive.setButtonCallback(&buttonCallback);
    
    // Add the callback for rotary movement
    iDrive.setRotaryCallback(&rotaryCallback);
    
    // Returns false if the button callback
    // or rotary callback is not set
    iDrive.begin();
}

void loop() {
    
    if (Serial.available()) {
        char data = Serial.read();
        
        switch (data) {
            case 'q':
                // Fade the lights on
                iDrive.lightsOn(true);
                break;
            case 'w':
                // Fade the lights off
                iDrive.lightsOff(true);
                break;
            case 'a':
                // Set the lights on
                iDrive.lightsOn(false);
                break;
            case 's':
                // Set the lights off
                iDrive.lightsOff(false);
                break;
            case 'd':
                // Toggle debug
                iDrive.setDebug(!iDrive.isDebug());
                Serial.printlnf("Debug %s", iDrive.isDebug() ? "on" : "off");
                break;
            case 'z':
                Serial.println("Begining");
                iDrive.begin();
                break;
            case 'x':
                Serial.println("Ending");
                iDrive.end();
                break;
        }
    }
    
    // Checks for CANBus messages and raises events
    iDrive.loop();
}