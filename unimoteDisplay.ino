// This #include statement was automatically added by the Particle IDE.
#include <PubNub.h>
#include "application.h"

// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkJson.h>

#include <LiquidCrystal_I2C_Spark.h>

LiquidCrystal_I2C *lcd;

HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

int lastSecond = 0;
int backlight = LOW;
int currentMode = 0;

char pubKeySmartLamp[] = "pub-c-0bed295e-a3e2-451a-882c-76cda83f73c9";
char subKeySmartLamp[] = "sub-c-3477209c-225a-11e7-894d-0619f8945a4f";
char channelSmartLamp[] = "Smart Lamp";

char pubKeyAlexaPi[] = "pub-c-c986ff40-0e29-415b-a1a6-1dcf79095d51";
char subKeyAlexaPi[] = "sub-c-2d8fef94-dad3-11e6-8da7-0619f8945a4f";
char channelAlexaPi[] = "Alexa Pi";

void setup(void)
{
    Time.zone(-4);
    
    pinMode(D2, INPUT_PULLDOWN);
    pinMode(D3, INPUT_PULLDOWN);
    pinMode(D4, INPUT_PULLDOWN);
    pinMode(D5, INPUT_PULLDOWN);
    pinMode(D6, INPUT_PULLDOWN);
    pinMode(D7, INPUT_PULLDOWN);
    pinMode(A0, INPUT_PULLDOWN);
    pinMode(A1, INPUT_PULLDOWN);

    //digitalWrite(D2, backlight);
    
    Serial.begin(9600);
    lcd = new LiquidCrystal_I2C(0x3F, 16, 2);
    lcd->init();
    lcd->backlight();
    lcd->clear();
}

void loop(void)
{
    if (currentMode == 0)
    {
        showClock();
    }
    
    if (ButtonPressed(D2))
    {
        modeAction(currentMode, D2);
    }

    if (ButtonPressed(D3))
    {
        modeAction(currentMode, D3);
    }

    if (ButtonPressed(D4))
    {
        modeAction(currentMode, D4);
    }

    if (ButtonPressed(D5))
    {
        modeAction(currentMode, D5);
    }

    if (ButtonPressed(D6))
    {
        modeAction(currentMode, D6);
    }

    if (ButtonPressed(D7))
    {
        modeAction(currentMode, D7);
    }

    if (ButtonPressed(A0))
    {
        modeAction(currentMode, A0);
    }

    if (ButtonPressed(A1))
    {
        if (currentMode == 4)
        {
            currentMode = 0;
        }
        else
        {
            currentMode = currentMode + 1;
        }
        
        setMode(currentMode);
    }
}

void setMode(int mode)
{
    currentMode = mode;
    
    if (mode == 0)
    {
        PubNub.begin(pubKeyAlexaPi, subKeyAlexaPi);
        setText("Clock", "");
        showClock();
    }
    else if (mode == 1)
    {
        setText("Cable", "");
    }
    else if (mode == 2)
    {
        setText("Cable Playback", "");
    }
    else if (mode == 3)
    {
        PubNub.begin(pubKeySmartLamp, subKeySmartLamp);
        setText("Smart Lamp", "");
    }
    else if (mode == 4)
    {
        setText("Apple TV", "");
    }
}

void modeAction(int mode, int button)
{
    // Clock mode
    if (mode == 0)
    {
        TvRemoteMode(button);

        // Switch backlight
        /*
        if (button == D3)
        {
            if (backlight == LOW)
            {
                backlight = HIGH;
            }
            else
            {
                backlight = LOW;
            }
            
            digitalWrite(D2, backlight);
        }
        */
    }
    // Cable Remote mode
    else if (mode == 1)
    {
        CableRemoteMode(button);
    }
    // Cable Playback mode
    else if (mode == 2)
    {
        CablePlaybackMode(button);
    }
    // Light mode
    else if (mode == 3)
    {
        LightMode(button);
    }
    else if (mode == 4)
    {
        AppleTvMode(button);
    }
}

void showClock()
{
    if (Time.second() != lastSecond)
    {
        //Serial.print(Time.timeStr());
        
        String hour = (Time.hour() < 10 ? "    0" : "    ") + String(Time.hour());
        String minute = (Time.minute() < 10 ? ":0": ":") + String(Time.minute());
        String second = (Time.second() < 10 ? ":0": ":") + String(Time.second());
        
        //Particle.publish("SWEARABLE", hour);
        //Particle.publish("SWEARABLE", minute);
        //Particle.publish("SWEARABLE", second);
        
        setText(getCurrentDate(), hour + minute + second);
        //lcd->setCursor(0, 1);
        //lcd->print(hour);
        //lcd->print(minute);
        //lcd->print(second);
        
        lastSecond = Time.second();
        
        //Particle.publish("SWEARABLE", String(lastSecond));
    }
}

void AppleTvMode(int button)
{
    if (button == D3)
    {
        setText("Apple TV", "Up");
        IrBlasterRequest("appletv", "KEY_UP");
        delay(10);
    }

    if (button == D4)
    {
        setText("Apple TV", "Down");
        IrBlasterRequest("appletv", "KEY_DOWN");
        delay(10);
    }

    if (button == D5)
    {
        setText("Apple TV", "Right");
        IrBlasterRequest("appletv", "KEY_RIGHT");
        delay(10);
    }

    if (button == D2)
    {
        setText("Apple TV", "Left");
        IrBlasterRequest("appletv", "KEY_LEFT");
        delay(10);
    }

    if (button == D6)
    {
        setText("Apple TV", "Select");
        IrBlasterRequest("appletv", "KEY_OK");
        delay(10);
    }

    if (button == D7)
    {
        setText("Apple TV", "Menu");
        IrBlasterRequest("appletv", "KEY_MENU");
        delay(10);
    }
    
    if (button == A0)
    {
        setText("TV", "Input");
        IrBlasterRequest("hometv", "KEY_CYCLEWINDOWS");
        delay(10);
    }
}

void CableRemoteMode(int button)
{
    if (button == D3)
    {
        setText("Cable", "Up");
        IrBlasterRequest("twc", "KEY_UP");
        delay(10);
    }

    if (button == D4)
    {
        setText("Cable", "Down");
        IrBlasterRequest("twc", "KEY_DOWN");
        delay(10);
    }

    if (button == D5)
    {
        setText("Cable", "Right");
        IrBlasterRequest("twc", "KEY_RIGHT");
        delay(10);
    }

    if (button == D2)
    {
        setText("Cable", "Left");
        IrBlasterRequest("twc", "KEY_LEFT");
        delay(10);
    }

    if (button == D6)
    {
        setText("Cable", "Select");
        IrBlasterRequest("twc", "KEY_SELECT");
        delay(10);
    }

    if (button == D7)
    {
        setText("Cable", "Exit");
        IrBlasterRequest("twc", "KEY_EXIT");
        delay(10);
    }
    
    if (button == A0)
    {
        setText("Cable", "Menu");
        IrBlasterRequest("twc", "KEY_MENU");
        delay(10);
    }
}

void CablePlaybackMode(int button)
{
    if (button == D3)
    {
        setText("Cable Playback", "Forward");
        IrBlasterRequest("twc", "KEY_FASTFORWARD");
        delay(10);
    }

    if (button == D4)
    {
        setText("Cable Playback", "Rewind");
        IrBlasterRequest("twc", "KEY_REWIND");
        delay(10);
    }

    if (button == D5)
    {
        setText("Cable Playback", "Fast Forward");
        IrBlasterRequest("twc", "KEY_FASTFORWARD");
        delay(10);
        IrBlasterRequest("twc", "KEY_FASTFORWARD");
        delay(10);
        IrBlasterRequest("twc", "KEY_FASTFORWARD");
        delay(10);
        IrBlasterRequest("twc", "KEY_FASTFORWARD");
        delay(10);
    }

    if (button == D2)
    {
        setText("Cable Playback", "Fast Rewind");
        IrBlasterRequest("twc", "KEY_REWIND");
        delay(10);
        IrBlasterRequest("twc", "KEY_REWIND");
        delay(10);
        IrBlasterRequest("twc", "KEY_REWIND");
        delay(10);
        IrBlasterRequest("twc", "KEY_REWIND");
        delay(10);
    }

    if (button == D6)
    {
        setText("Cable Playback", "Play");
        IrBlasterRequest("twc", "KEY_PLAY");
        delay(10);
    }

    if (button == D7)
    {
        setText("Cable Playback", "Stop");
        IrBlasterRequest("twc", "KEY_STOP");
        delay(10);
    }
    
    if (button == A0)
    {
        setText("Cable Playback", "Record");
        IrBlasterRequest("twc", "KEY_RECORD");
        delay(10);
    }
}

void TvRemoteMode(int button)
{
    if (button == D3)
    {
        setText("TV", "Volume Up");
        IrBlasterRequest("hometv", "KEY_VOLUMEUP");
        delay(10);
    }

    if (button == D4)
    {
        setText("TV", "Volume Down");
        IrBlasterRequest("hometv", "KEY_VOLUMEDOWN");
        delay(10);
    }

    if (button == D5)
    {
        setText("Cable", "Channel Up");
        IrBlasterRequest("twc", "KEY_CHANNELUP");
        delay(10);
    }

    if (button == D2)
    {
        setText("Cable", "Channel Down");
        IrBlasterRequest("twc", "KEY_CHANNELDOWN");
        delay(10);
    }

    if (button == D6)
    {
        setText("TV", "Power");
        IrBlasterRequest("hometv", "KEY_POWER");
        delay(10);
    }

    if (button == D7)
    {
        setText("Cable", "Power");
        IrBlasterRequest("twc", "KEY_POWER");
        delay(10);
    }
    
    // Toggle CC
    if (button == A0)
    {
        setText("Cable", "Toggle Closed Caption");
        IrBlasterRequest("twc", "KEY_SETUP");                                                                                                                                                                                                                                                                                    
        delay(500);
        IrBlasterRequest("twc", "KEY_SELECT");
        delay(500);
        IrBlasterRequest("twc", "KEY_DOWN");
        delay(500);
        IrBlasterRequest("twc", "KEY_SELECT");
        delay(1000);
        IrBlasterRequest("twc", "KEY_EXIT");
        delay(500);
    }
}

void LightMode(int button)
{
    String command;
    
    if (button == D3)
    {
        command = "On";
    }

    if (button == D4)
    {
        command = "Off";
    }

    if (button == D5)
    {
        command = "White";
    }

    if (button == D2)
    {
        command = "Yellow";
    }

    if (button == D6)
    {
        command = "Blink";
    }    

    setText("Smart Lamp", command);

    TCPClient *client;
    String msg = "{\"command\":\"" + command + "\"}";
    client = PubNub.publish(channelSmartLamp, msg);
    client->stop();
    delay(200);
}

void IrBlasterRequest(String remote, String command)
{
    IPAddress ip(192, 168, 86, 27);
    request.ip = ip;
    request.port = 80;

    request.path = "/irremote.php?remote=" + remote + "&key=" + command;
    http.get(request, response, headers);
    Particle.publish("TV Remote Request", request.path);
    Particle.publish("TV Remote Response", String(response.status));
}

bool ButtonPressed(int button)
{
    bool buttonPressed = false;
    
    /*
    if (digitalRead(button) == HIGH)
    {
        buttonPressed = true;
    }
    */

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

String getCurrentDate()
{
    int weekdayValue = Time.weekday();
    int monthValue = Time.month();
    int day = Time.day();
    int year = Time.year();
    String weekday, month;
    
    if (weekdayValue == 1)
    {
        weekday = "Sun";
    } 
    else if (weekdayValue == 2)
    {
        weekday = "Mon";
    }
    else if (weekdayValue == 3)
    {
        weekday = "Tue";
    }
    else if (weekdayValue == 4)
    {
        weekday = "Wed";
    }
    else if (weekdayValue == 5)
    {
        weekday = "Thu";
    }
    else if (weekdayValue == 6)
    {
        weekday = "Fri";
    }
    else if (weekdayValue == 7)
    {
        weekday = "Sat";
    }
    
    if (monthValue == 1)
    {
        month = "Jan";
    }
    else if (monthValue == 2)
    {
        month = "Feb";
    }
    else if (monthValue == 3)
    {
        month = "Mar";
    }
    else if (monthValue == 4)
    {
        month = "Apr";
    }
    else if (monthValue == 5)
    {
        month = "May";
    }
    else if (monthValue == 6)
    {
        month = "Jun";
    }
    else if (monthValue == 7)
    {
        month = "Jul";
    }
    else if (monthValue == 8)
    {
        month = "Aug";
    }
    else if (monthValue == 9)
    {
        month = "Sep";
    }
    else if (monthValue == 10)
    {
        month = "Oct";
    }
    else if (monthValue == 11)
    {
        month = "Nov";
    }
    else if (monthValue == 12)
    {
        month = "Dec";
    }
    
    return weekday + " " + month + " " + String(day) + " " + String(year);
}

void setText(String line1, String line2)
{
    // Clear the screen.
    lcd->clear();

    // Move the cursor to the start of the first line and print the text.
    lcd->setCursor(0,0);
    lcd->print(line1);

    // Move the cursor to the start of the second line and print the text.
    lcd->setCursor(0,1);
    lcd->print(line2);
}

void clearText()
{
    lcd->clear();
}