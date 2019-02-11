#ifndef __IDRIVE_H_
#define __IDRIVE_H_

#include "application.h"

class IDrive {
    
    public:
        typedef enum {
            Released = 0,
            Pressed = 1,
            Held  = 2,
            Up = 3,
            Down = 4,
            Left = 5,
            Right = 6,
            UpHeld = 7,
            DownHeld = 8,
            LeftHeld = 9,
            RightHeld = 10
        } ButtonState;
        
        typedef enum {
            media = 0,
            radio = 1,
            menu = 2,
            tel = 3,
            nav = 4,
            back = 5,
            option = 6,
            knob = 7,
            knobDirection = 8
        } ButtonRef;

    
    public:
        IDrive(int busSpeed);
        void setButtonCallback(void (*callback)(ButtonRef, ButtonState));
        void setRotaryCallback(void (*callback)(signed char));
        void setLightBrightness(char brightness, bool fade);
        void lightsOn(bool fade);
        void lightsOff(bool fade);
        void setDebug(bool enable);
        bool isDebug() { return debug; };
        bool begin();
        void end();
        void loop();
    
    private:
        int _busSpeed;

        CANMessage initMessage;
        CANMessage pollMessage;

        bool canEnabled = false;
        int canError = 0;
        bool debug = false;
        bool running = false;

        ButtonState mediaButton = Released;
        ButtonState radioButton = Released;
        ButtonState menuButton = Released;
        ButtonState telButton = Released;
        ButtonState navButton = Released;
        ButtonState backButton = Released;
        ButtonState optionButton = Released;
        
        ButtonState rotaryTopButton = Released;
        ButtonState directionButton = Released;
        
        // 253 == full brightness
        // 254 == Off
        char lightBrightness = 254;
        char targetLightBrightness = 254;
        unsigned long lastBrightnessChange;
        const int millsBetweenBrightnessUpdates = 2;
        void setBrightness(char brightness);
        
        unsigned char rotaryPosition = 255;
        
        unsigned long lastPoll;
        
        void (*buttonCallback)(ButtonRef, ButtonState);
        void (*rotaryCallback)(signed char);
};

#endif  // __iDRIVE_H_