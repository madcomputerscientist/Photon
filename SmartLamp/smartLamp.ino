// This #include statement was automatically added by the Particle IDE.
#include <PubNub.h>

char channelSmartLamp[] = "SMARTLAMP";

void setup()
{
    pinMode(D0, INPUT_PULLDOWN);
    pinMode(D1, INPUT_PULLDOWN);
    pinMode(D2, INPUT_PULLDOWN);
    pinMode(D4, INPUT_PULLDOWN);
    pinMode(D7, INPUT_PULLDOWN);

    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    
    LampOff();
    
    Particle.subscribe(channelSmartLamp, smartLampEventHandler);
}

void smartLampEventHandler(const char *event, const char *data)
{
    String command = String(data);
    
    if (command == "Off")
    {
        LampOff();
    }
    else if (command == "On")
    {
        LampOn();
    }
    else if (command == "White")
    {
        LampWhite();
    }
    else if (command == "Yellow")
    {
        LampYellow();
    }
    else if (command == "Blink")
    {
        LampBlink();
    }
}

void loop()
{
    if (ButtonPressed(D0))
    {
        LampOff();
    }
    else if (ButtonPressed(D1))
    {
        LampOn();
    }
    else if (ButtonPressed(D2))
    {
        LampWhite();
    }
    else if (ButtonPressed(D4))
    {
        LampYellow();
    }
    else if (ButtonPressed(D7))
    {
        LampBlink();
    }
}

void LampOn()
{
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
}

void LampOff()
{
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
}

void LampWhite()
{
    digitalWrite(D5, HIGH);
    digitalWrite(D6, LOW);
}

void LampYellow()
{
    digitalWrite(D5, LOW);
    digitalWrite(D6, HIGH);
}

void LampBlink()
{
    int index;
    
    for (index = 0; index < 3; index++)
    {
        LampOn();
        delay(100);
        LampWhite();
        delay(100);
        LampYellow();
        delay(100);
        LampOff();
        delay(100);
    }
}

bool ButtonPressed(int button)
{
    bool buttonPressed = false;

    if (digitalRead(button) == HIGH)
    {
        delay(50);
        
        if (digitalRead(button) == LOW)
        {
            buttonPressed = true;
        }
    }

    return buttonPressed;
}