// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

// This #include statement was automatically added by the Particle IDE.
#include "InternetButton/InternetButton.h"

/* Here's a nice combination of features that I like to use.
Note the use of the allButtons function. */

InternetButton b = InternetButton();

bool motionMode = false;
bool tvRemoteMode = false;
bool racerMode = false;
bool appleTvMode = false;
bool speakerMode = false;
int currentModeIndex = 0;
int lastDirection = 0;
int buttonDown = A0;
int buttonLeft = A1;
int buttonRight = D1;
int buttonUp = D2;
int button1 = D4;
int button2 = D5;
int button3 = D6;
int button4 = D7;

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

void setup()
{
    // Tell b to get everything ready to go
    // Use b.begin(1); if you have the original SparkButton, which does not have a buzzer or a plastic enclosure
    // to use, just add a '1' between the parentheses in the code above.
    b.begin();

    pinMode(buttonDown, INPUT_PULLDOWN);
    pinMode(buttonLeft, INPUT_PULLDOWN);
    pinMode(buttonRight, INPUT_PULLDOWN);
    pinMode(buttonUp, INPUT_PULLDOWN);
    pinMode(button1, INPUT_PULLDOWN);
    pinMode(button2, INPUT_PULLDOWN);
    pinMode(button3, INPUT_PULLDOWN);
    pinMode(button4, INPUT_PULLDOWN);
    
    //Spark.function("", )
    SetMode(currentModeIndex);

    Serial.begin(9600);
}

void loop()
{
    //Particle.publish("1 " + String(tinkerDigitalRead(button1)), NULL, 60, PRIVATE);
    //Particle.publish("2 " + String(tinkerDigitalRead(button2)), NULL, 60, PRIVATE);
    //Particle.publish("3 " + String(tinkerDigitalRead(button3)), NULL, 60, PRIVATE);
    //Particle.publish("4 " + String(tinkerDigitalRead(button4)), NULL, 60, PRIVATE);

/*
    Particle.publish("down " + String(tinkerDigitalRead(buttonDown)), NULL, 60, PRIVATE);
    Particle.publish("left " + String(tinkerDigitalRead(buttonLeft)), NULL, 60, PRIVATE);
    Particle.publish("right " + String(tinkerDigitalRead(buttonRight)), NULL, 60, PRIVATE);
    Particle.publish("up " + String(tinkerDigitalRead(buttonUp)), NULL, 60, PRIVATE);
*/

    b.allLedsOff();
    
    if (motionMode == true)
    {
        MotionMode();
    }
    else if (racerMode == true)
    {
        RacerMode();
    }
    else if (tvRemoteMode == true)
    {
        TvRemoteMode();
    }
    else if (appleTvMode == true)
    {
        AppleTvMode();
    }
    else if (speakerMode == true)
    {
        SpeakerMode();
    }

    if (digitalRead(button4) == HIGH)
    {
        currentModeIndex += 1;
        if (currentModeIndex > 3) currentModeIndex = 0;
        SetMode(currentModeIndex);
        
        Particle.publish("Set Mode " + String(currentModeIndex), NULL, 60, PRIVATE);
        delay(500);
    }
    
/*    
    if (digitalRead(button3) == HIGH)
    {
        Particle.publish("3", NULL, 60, PRIVATE);
        b.rainbow(5);
        b.playSong("E5,8,G5,8,E6,8,C6,8,D6,8,G6,8");
    }

    if (digitalRead(button4) == HIGH)
    {
        Particle.publish("Call Me", NULL, 60, PRIVATE);
        b.rainbow(5);
    }
*/
}

int ledToggle(String command)
{
}

void SetMode(int modeIndex)
{
    switch (modeIndex)
    {
        case 0:
            motionMode = false;
            tvRemoteMode = false;
            racerMode = true;
            appleTvMode = false;
            speakerMode = false;
            
            b.allLedsOn(0, 255, 0);
            break;
        case 1:
            motionMode = false;
            tvRemoteMode = true;
            racerMode = false;
            appleTvMode = false;
            speakerMode = false;

            b.ledOn(12, 0, 255, 0); // Green
            b.ledOn(1, 0, 0, 255); // Blue
            b.ledOn(2, 0, 255, 0); // Green
            b.ledOn(3, 0, 0, 255); // Blue
            b.ledOn(4, 0, 255, 0); // Green
            b.ledOn(5, 0, 0, 255); // Blue
            b.ledOn(6, 0, 255, 0); // Green
            b.ledOn(7, 0, 0, 255); // Blue
            b.ledOn(8, 0, 255, 0); // Green
            b.ledOn(9, 0, 0, 255); // Blue
            b.ledOn(10, 0, 255, 0); // Green
            b.ledOn(11, 0, 0, 255); // Blue
            break;
        case 2:
            motionMode = false;
            tvRemoteMode = false;
            racerMode = false;
            appleTvMode = true;
            speakerMode = false;
            
            b.allLedsOn(255, 255, 255);
            break;
        case 3:
            motionMode = false;
            tvRemoteMode = false;
            racerMode = false;
            appleTvMode = false;
            speakerMode = true;
            
            b.allLedsOn(255, 255, 0);
            break;
    }
}

void RacerMode()
{
    if (digitalRead(button1) == HIGH)
    {
        b.allLedsOn(255, 0, 0); // All Red
        Particle.publish("SMARTRACER_MOVE", "brake");
        delay(500);
    }

    if (digitalRead(buttonDown) == HIGH)
    {
        b.ledOn(6, 0, 0, 255); // Blue
        Particle.publish("SMARTRACER_MOVE", "back");
        delay(500);
    }
    else if (digitalRead(buttonDown) == LOW)
    {
        b.ledOn(6, 0, 0, 0);
    }

    if (digitalRead(buttonLeft) == HIGH)
    {
        b.ledOn(9, 255, 0, 255); // Magenta
        Particle.publish("SMARTRACER_MOVE", "left");
        delay(500);
    }
    else if (digitalRead(buttonLeft) == LOW)
    {
        b.ledOn(9, 0, 0, 0);
    }

    if (digitalRead(buttonRight) == HIGH)
    {
        b.ledOn(3, 0, 255, 0); // Green
        Particle.publish("SMARTRACER_MOVE", "right");
        delay(500);
    }
    else if (digitalRead(buttonRight) == LOW)
    {
        b.ledOn(3, 0, 0, 0);
    }

    if (digitalRead(buttonUp) == HIGH)
    {
        b.ledOn(12, 255, 0, 0); // Red
        Particle.publish("SMARTRACER_MOVE", "forward");
        delay(500);
    }
    else if (digitalRead(buttonUp) == LOW)
    {
        b.ledOn(12, 0, 0, 0);
    }
}

void IrBlasterRequest(String remote, String command)
{
    IPAddress ip(192, 168, 1, 112);
    request.ip = ip;
    request.port = 80;

    request.path = "/irremote.php?remote=" + remote + "&key=" + command;
    http.get(request, response, headers);
    Particle.publish("TV Remote Request", request.path);
    Particle.publish("TV Remote Response", String(response.status));
}

void TvRemoteMode()
{
    if (digitalRead(button1) == HIGH)
    {
        b.allLedsOn(0, 255, 0); // All Green
        IrBlasterRequest("hometv", "KEY_POWER");
        delay(500);
    }

    if (digitalRead(button2) == HIGH)
    {
        b.allLedsOn(0, 0, 255); // All Blue
        IrBlasterRequest("twc", "KEY_POWER");
        delay(500);
    }

    // Toggle CC
    if (digitalRead(button3) == HIGH)
    {
        b.allLedsOn(0, 0, 255); // All Blue
        IrBlasterRequest("twc", "KEY_SETUP");
        delay(500);
        //IrBlasterRequest("twc", "KEY_DOWN");
        //delay(500);
        IrBlasterRequest("twc", "KEY_SELECT");
        delay(500);
        IrBlasterRequest("twc", "KEY_DOWN");
        delay(500);
        IrBlasterRequest("twc", "KEY_SELECT");
        delay(1000);
        IrBlasterRequest("twc", "KEY_EXIT");
        delay(500);
    }

    if (digitalRead(buttonDown) == HIGH)
    {
        b.ledOn(6, 0, 255, 0); // Green
        IrBlasterRequest("hometv", "KEY_VOLUMEDOWN");
        delay(10);
    }

    if (digitalRead(buttonLeft) == HIGH)
    {
        b.ledOn(9, 0, 0, 255); // Blue
        IrBlasterRequest("twc", "KEY_CHANNELDOWN");
        delay(10);
    }

    if (digitalRead(buttonRight) == HIGH)
    {
        b.ledOn(3, 0, 0, 255); // Blue
        IrBlasterRequest("twc", "KEY_CHANNELUP");
        delay(10);
    }

    if (digitalRead(buttonUp) == HIGH)
    {
        b.ledOn(12, 0, 255, 0); // Green
        IrBlasterRequest("hometv", "KEY_VOLUMEUP");
        delay(10);
    }
}

void AppleTvMode()
{
    if (digitalRead(button1) == HIGH)
    {
        b.allLedsOn(0, 255, 0); // All Green
        IrBlasterRequest("appletv", "KEY_OK");
        delay(10);
    }

    if (digitalRead(button2) == HIGH)
    {
        b.allLedsOn(255, 0, 0); // All Red
        IrBlasterRequest("appletv", "KEY_MENU");
        delay(10);
    }

    if (digitalRead(button3) == HIGH)
    {
        b.allLedsOn(0, 0, 255); // All Blue
        IrBlasterRequest("appletv", "KEY_PLAY");
        delay(10);
    }

    if (digitalRead(buttonDown) == HIGH)
    {
        b.ledOn(6, 0, 255, 0); // Green
        IrBlasterRequest("appletv", "KEY_DOWN");
        delay(10);
    }

    if (digitalRead(buttonLeft) == HIGH)
    {
        b.ledOn(9, 0, 0, 255); // Blue
        IrBlasterRequest("appletv", "KEY_LEFT");
        delay(10);
    }

    if (digitalRead(buttonRight) == HIGH)
    {
        b.ledOn(3, 0, 0, 255); // Blue
        IrBlasterRequest("appletv", "KEY_RIGHT");
        delay(10);
    }

    if (digitalRead(buttonUp) == HIGH)
    {
        b.ledOn(12, 0, 255, 0); // Green
        IrBlasterRequest("appletv", "KEY_UP");
        delay(10);
    }
}

void SpeakerRequest(String command)
{
    IPAddress ip(192, 168, 1, 110);
    request.ip = ip;
    request.port = 80;

    request.path = "/volume.php?direction=" + command;
    http.get(request, response, headers);
    Particle.publish("Speaker Remote Request", request.path);
    Particle.publish("Speaker Remote Response", String(response.status));
}

void SpeakerMode()
{
    if (digitalRead(button1) == HIGH)
    {
        b.allLedsOn(0, 255, 0); // All Green
        IrBlasterRequest("appletv", "KEY_OK");
        delay(10);
    }

    if (digitalRead(button2) == HIGH)
    {
        b.allLedsOn(255, 0, 0); // All Red
        IrBlasterRequest("appletv", "KEY_MENU");
        delay(10);
    }

    if (digitalRead(button3) == HIGH)
    {
        b.allLedsOn(0, 0, 255); // All Blue
        IrBlasterRequest("appletv", "KEY_PLAY");
        delay(10);
    }

    if (digitalRead(buttonDown) == HIGH)
    {
        b.ledOn(6, 0, 255, 0); // Green
        SpeakerRequest("down");
        delay(10);
    }

    if (digitalRead(buttonLeft) == HIGH)
    {
        b.ledOn(9, 0, 0, 255); // Blue
        IrBlasterRequest("hometv", "KEY_VOLUMEDOWN");
        delay(10);
    }

    if (digitalRead(buttonRight) == HIGH)
    {
        b.ledOn(3, 0, 0, 255); // Blue
        IrBlasterRequest("hometv", "KEY_VOLUMEUP");
        delay(10);
    }

    if (digitalRead(buttonUp) == HIGH)
    {
        b.ledOn(12, 0, 255, 0); // Green
        SpeakerRequest("up");
        delay(10);
    }
}

void MotionMode()
{
    /*
    int xValue = b.readX();
    int yValue = b.readY();
    int zValue = b.readZ();

    Particle.publish("xyz " + String(xValue) + " " + String(yValue) + " " + String(zValue), NULL, 60, PRIVATE);
    b.allLedsOn(abs(xValue), abs(yValue), abs(zValue));
    */

    // Want to figure out which LED is the lowest?
    // We've hidden the necessary trigonometry in this function.
    int ledPos = b.lowestLed();

    // Turn the LEDs off so they don't all end up on
    b.allLedsOff();

    // Now turn that LED on
    b.ledOn(ledPos, 255, 0, 255);

    if (lastDirection != ledPos)
    {
        Particle.publish("direction " + String(ledPos), NULL, 60, PRIVATE);
    }
    
    lastDirection = ledPos;

    // Wait a mo'
    delay(100);
}