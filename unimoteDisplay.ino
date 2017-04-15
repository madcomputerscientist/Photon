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

    digitalWrite(D2, backlight);
    
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
        if (currentMode == 2)
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
    // TV Remote mode
    else if (mode == 1)
    {
        TvRemoteMode(button);
    }
    // Cable Remote mode
    else if (mode == 2)
    {
        CableRemoteMode(button);
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

void CableRemoteMode(int button)
{
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

void setMode(int mode)
{
    currentMode = mode;
    
    if (mode == 0)
    {
        setText("Clock", "");
        showClock();
    }
    else if (mode == 1)
    {
        setText("TV Remote", "VU-VD-CU-CD-Mode");
    }
    else if (mode == 2)
    {
        setText("Cable Remote", "TV-CB-CC-In-Mode");
    }
}

void IrBlasterRequest(String remote, String command)
{
    IPAddress ip(192, 168, 1, 103);
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