#include "iDrive.h"

IDrive::IDrive(int busSpeed) {
    _busSpeed = busSpeed;

    initMessage.id = 0x273;
    initMessage.len = 8;
    initMessage.data[0] = 0x1d;
    initMessage.data[1] = 0xe1;
    initMessage.data[2] = 0x0;
    initMessage.data[3] = 0xF0;
    initMessage.data[4] = 0xFF;
    initMessage.data[5] = 0x7F;
    initMessage.data[6] = 0xDE;
    initMessage.data[7] = 0x4;
    
    
    pollMessage.id = 0x501;
    pollMessage.len = 8;
    pollMessage.data[0] = 0x1;
    pollMessage.data[1] = 0x0;
    pollMessage.data[2] = 0x0;
    pollMessage.data[3] = 0x0;
    pollMessage.data[4] = 0x0;
    pollMessage.data[5] = 0x0;
    pollMessage.data[6] = 0x0;
    pollMessage.data[7] = 0x0;
}

void IDrive::setButtonCallback(void (*callback)(ButtonRef, ButtonState)) {
    this->buttonCallback = callback;
}

void IDrive::setRotaryCallback(void (*callback)(signed char)) {
    this->rotaryCallback = callback;
}

void IDrive::setLightBrightness(char brightness, bool fade) {
    if (!running)
        return;
        
    if (!fade)
        setBrightness(brightness);

    targetLightBrightness = brightness;
}

void IDrive::lightsOn(bool fade) {
    if (!running)
        return;
        
    if (!fade)
        setBrightness(253);
    
    targetLightBrightness = 253;
}

void IDrive::lightsOff(bool fade) {
    if (!running)
        return;
        
    if (!fade)
        setBrightness(254);

    targetLightBrightness = 254;
}

void IDrive::setBrightness(char brightness) {
    CANMessage message;
    message.id = 0x202;
    message.len = 2;
    message.data[0] = brightness;
    message.data[1] = 0x0;

    CANChannel can(CAN_D1_D2);
    can.transmit(message);
    lightBrightness = brightness;
}

void IDrive::setDebug(bool enabled) {
    if (debug)
        Serial.begin(9600);
    debug = enabled;
}

bool IDrive::begin() {
    if (buttonCallback && rotaryCallback) {
        rotaryPosition = 255;
        CANChannel can(CAN_D1_D2);
        can.begin(_busSpeed);
        can.transmit(initMessage);
        running = true;
    }
    
    return running;
}

void IDrive::end() {
    CANChannel can(CAN_D1_D2);
    can.end();
    running = false;
}

void IDrive::loop() {
    if (running) {
        CANChannel can(CAN_D1_D2);
        
        if (millis() > (lastPoll+500)) {
            lastPoll = millis();
            can.transmit(pollMessage);
        }
    
        if(can.available() > 0) {
            CANMessage message;
            if(can.receive(message)) {
                if (message.id == 0x267 && message.data[1] == 0xfd) { // Buttons
                    ButtonState *button;
                    ButtonRef buttonRef;
                    
                    switch (message.data[4]) {
                        case 0xc0 :
                            switch (message.data[5]) {
                                case 0x1 :
                                    button = &menuButton;
                                    buttonRef = menu;
                                    break;
                                case 0x2 :
                                    button = &backButton;
                                    buttonRef = back;
                                    break;
                                case 0x4 :
                                    button = &optionButton;
                                    buttonRef = option;
                                    break;
                                case 0x8 :
                                    button = &radioButton;
                                    buttonRef = radio;
                                    break;
                                case 0x10 :
                                    button = &mediaButton;
                                    buttonRef = media;
                                    break;
                                case 0x20 :
                                    button = &navButton;
                                    buttonRef = nav;
                                    break;
                                case 0x40 :
                                    button = &telButton;
                                    buttonRef = tel;
                                    break;
                            }
                            break;
                        case 0xde :
                            button = &rotaryTopButton;
                            buttonRef = knob;
                            break;
                        case 0xdd :
                            button = &directionButton;
                            buttonRef = knobDirection;
                            break;
                    }
                    
                    switch (message.data[3]) {
                        case 0x0 :
                            if (*button != Released) {
                                *button = Released;
                                buttonCallback(buttonRef, Released);
                            }
                            break;
                        case 0x1 :
                            if (*button != Pressed) {
                                *button = Pressed;
                                buttonCallback(buttonRef, Pressed);
                            }
                            break;
                        case 0x2 :
                            if (*button != Held) {
                                *button = Held;
                                buttonCallback(buttonRef, Held);
                            }
                            break;
                        case 0x11 :
                            if (*button != Up) {
                                *button = Up;
                                buttonCallback(buttonRef, Up);
                            }
                            break;
                        case 0x12 :
                            if (*button != UpHeld) {
                                *button = UpHeld;
                                buttonCallback(buttonRef, UpHeld);
                            }
                            break;
                        case 0x21 :
                            if (*button != Right) {
                                *button = Right;
                                buttonCallback(buttonRef, Right);
                            }
                            break;
                        case 0x22 :
                            if (*button != RightHeld) {
                                *button = RightHeld;
                                buttonCallback(buttonRef, RightHeld);
                            }
                            break;
                        case 0x41 :
                            if (*button != Down) {
                                *button = Down;
                                buttonCallback(buttonRef, Down);
                            }
                            break;
                        case 0x42 :
                            if (*button != DownHeld) {
                                *button = DownHeld;
                                buttonCallback(buttonRef, DownHeld);
                            }
                            break;
                        case 0x81 :
                            if (*button != Left) {
                                *button = Left;
                                buttonCallback(buttonRef, Left);
                            }
                            break;
                        case 0x82 :
                            if (*button != LeftHeld) {
                                *button = LeftHeld;
                                buttonCallback(buttonRef, LeftHeld);
                            }
                            break;
                    }
                    
                } else if (message.id == 0x264 && message.data[1] == 0xfd) {

                    if (message.data[3] > rotaryPosition) {
                        int diff = message.data[3] - rotaryPosition;
                        
                        if (diff < 100)
                            rotaryCallback(diff);
                        else
                            rotaryCallback((message.data[3] - 256) - rotaryPosition);
                            
                    } else {
                        
                        int diff = message.data[3] - rotaryPosition;
                        
                        if (diff > -100)
                            rotaryCallback(diff);
                        else
                            rotaryCallback((256 - rotaryPosition) + message.data[3]);
                    }
                    
                    rotaryPosition = message.data[3];
                    
                } 
                
                if (debug) {
                    Serial.printf("Id : 0x%02x ", message.id);
                    for (int i = 0; i < message.len; i++)
                        Serial.printf(" 0x%02x ", message.data[i]);
                    Serial.println("");
                }
                
                if (message.id == 0x267 && message.data[1] == 0xea) { // This seems to be a rotary error and can be fixed by sending initMessage again.
                    if (debug)
                        Serial.println("RESENDING INIT");
                    can.transmit(initMessage);
                }
            }
        }
        
        if (lightBrightness != targetLightBrightness && millis() > (lastBrightnessChange + millsBetweenBrightnessUpdates)) {
            lastBrightnessChange = millis();
            
            if ((targetLightBrightness > lightBrightness || lightBrightness >= 254) && targetLightBrightness != 254)
                setBrightness(lightBrightness+1);
            else
                setBrightness(lightBrightness-1);
        }
            
        if(can.isEnabled() != canEnabled) {
            canEnabled = can.isEnabled();
            
            if (debug) {
                if (canEnabled)
                    Serial.println("CAN enabled");
                else
                    Serial.println("CAN not enabled");
            }
        }
        
        if (can.errorStatus() != canError) {
            canError = can.errorStatus();
            if (debug) {
                if(can.errorStatus() == CAN_NO_ERROR) {
                    Serial.println("no_error");
                }
                if(can.errorStatus() == CAN_ERROR_PASSIVE) {
                    Serial.println("error_passive");
                }
                if(can.errorStatus() == CAN_BUS_OFF) {
                    Serial.println("bus_off");
                }
            }
        }
    }
}