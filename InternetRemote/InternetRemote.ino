// This #include statement was automatically added by the Particle IDE.
#include "SparkIntervalTimer.h"

// This #include statement was automatically added by the Particle IDE.
#include "SimpleRingBuffer.h"

// This #include statement was automatically added by the Particle IDE.
#include "GOFi2cOLED.h"
//#include "Wire.h"

// This #include statement was automatically added by the Particle IDE.
#include <PubNub.h>

// This #include statement was automatically added by the Particle IDE.
#include <InternetButton.h>

#include <math.h>

#define MICROPHONE_PIN A0
#define AUDIO_BUFFER_MAX 8192

#define SINGLE_PACKET_MIN 512
#define SINGLE_PACKET_MAX 1024

#define TCP_SERVER_PORT 1234
#define SERIAL_DEBUG_ON true

#define AUDIO_TIMING_VAL 62 /* 16kHz */

uint8_t txBuffer[SINGLE_PACKET_MAX + 1];
SimpleRingBuffer audio_buffer;

unsigned long lastRead = micros();
unsigned long lastSend = millis();
char myIpAddress[24];

TCPClient audioClient;
TCPClient checkClient;
TCPServer audioServer = TCPServer(TCP_SERVER_PORT);

IntervalTimer readMicTimer;

int _sendBufferLength = 0;
unsigned int lastPublished = 0;
bool _isRecording = false;

volatile int counter = 0;

unsigned int lastLog = 0;
unsigned int lastClientCheck = 0;


InternetButton b = InternetButton();
GOFi2cOLED oled;

bool flashlightOn = false;
bool notificationAvailable = false;
bool listeningOn = false;
int currentModeIndex = 0;
int lastDirection;
int lastSecond = 0;

int button1 = A1;

char pubKeyInternetRemote[] = "pub-c-00a7863c-157e-41cd-9949-cf7c4f7b0485";
char subKeyInternetRemote[] = "sub-c-dd38910c-4037-11e7-b6a4-02ee2ddab7fe";
char channelInternetRemote[] = "Internet Remote";

char channelNotification[] = "Notification";
char channelListen[] = "Listen";

void setup()
{
    Time.zone(-4);

    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D
    oled.display(); // show splashscreen
    delay(2000);
    oled.clearDisplay();

    b.begin();

    pinMode(button1, INPUT_PULLDOWN);

    setMode(currentModeIndex);

    //setupPubNub();
    setupNotification();
    
    setupMicrophone();
}

void setupMicrophone()
{
    Particle.subscribe(channelListen, listenEventHandler);

    #if SERIAL_DEBUG_ON
    Serial.begin(115200);
    #endif
    
    setADCSampleTime(ADC_SampleTime_3Cycles);

    pinMode(MICROPHONE_PIN, INPUT);

    //int mySampleRate = AUDIO_TIMING_VAL;

    Particle.variable("ipAddress", myIpAddress, STRING);
    //Particle.variable("\", &mySampleRate, INT);
    //Particle.publish("sample_rate", " my sample rate is: " + String(AUDIO_TIMING_VAL));

    IPAddress myIp = WiFi.localIP();
    sprintf(myIpAddress, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);

    audio_buffer.init(AUDIO_BUFFER_MAX);

    // // send a chunk of audio every 1/2 second
    // sendAudioTimer.begin(sendAudio, 1000, hmSec);

    audioServer.begin();

    lastRead = micros();

    readMicTimer.begin(readMic, AUDIO_TIMING_VAL, uSec);
}

void listenEventHandler(const char *event, const char *data)
{
    if ((String(data) == "start" && !listeningOn) || (String(data) == "stop" && listeningOn))
    {
        toggleListening();
    }
}

void setupNotification()
{
    Particle.subscribe(channelNotification, notificationEventHandler);
}

void notificationEventHandler(const char *event, const char *data)
{
    setText("Message", data);
    notificationAvailable = true;
}

/*
void setupPubNub()
{
    Serial.begin(9600);
    PubNub.begin(pubKeyInternetRemote, subKeyInternetRemote);
    
    TCPClient *client;
    char msg[64] = "{\"photon\":\"on\"}";
    client = PubNub.publish(channelNotification, msg);
    client->stop();
    delay(200);
}

void checkNotification()
{
    TCPClient *client;
    client = PubNub.subscribe(channelNotification);
    while (client->connected())
    {
        char c = client->read();
        //setText("Alert", String(c));
        //Particle.publish(c, NULL, 60, PRIVATE);
        //Serial.println(c);
    }
    client->stop();
    delay(200);
}
*/

void loop()
{
    if (listeningOn)
    {
        getAudio();
    }
    
    if (!notificationAvailable && !listeningOn)
    {
        modeAction(currentModeIndex);
    }

    if (b.buttonOn(4))
    {
        Particle.publish("Button Pressed", "Left", 60, PRIVATE);
        
        currentModeIndex += 1;
        if (currentModeIndex > 2) currentModeIndex = 0;
        setMode(currentModeIndex);
    }

    if (b.buttonOn(1))
    {
        Particle.publish("Button Pressed", "Up", 60, PRIVATE);

        toggleFlashlight();
    }

    if (b.buttonOn(2))
    {
        Particle.publish("Button Pressed", "Right", 60, PRIVATE);
        
        b.rainbow(5);
        b.playSong("E5,8,G5,8,E6,8,C6,8,D6,8,G6,8");
        b.allLedsOff();
    }
    
    if (buttonPressed(button1))
    {
        Particle.publish("Button Pressed", "A", 60, PRIVATE);

        if (!notificationAvailable)
        {
            invokeQuery();
        }

        notificationAvailable = false;
    }

    if (b.buttonOn(3))
    {
        Particle.publish("Button Pressed", "Down", 60, PRIVATE);

        invokeQuery();
    }
}

void modeAction(int modeIndex)
{
    switch (modeIndex)
    {
        case 0:
            showClock();
            break;
        case 1:
            motionMode();
            break;
        case 2:
            directionMode();
            break;
    }
}

void setMode(int modeIndex)
{
    switch (modeIndex)
    {
        case 0:
            b.allLedsOff();
            break;
        case 1:
            setText("Motion", "");
            b.allLedsOn(0, 0, 255); // Green
            delay(500);
            break;
        case 2:
            setText("Direction", "");
            b.allLedsOn(0, 255, 0); // Green
            delay(500);
            break;
    }
}

void invokeQuery()
{
/*
    TCPClient *client;
    String msg = "{\"command\":\"query\"}";
    client = PubNub.publish(channelInternetRemote, msg);
    client->stop();
    delay(200);
*/
    b.rainbow(5);
    Particle.publish("Listen", "initiate", 60, PRIVATE);
    //delay(500);
}

void toggleListening()
{
    if (!listeningOn)
    {
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
        listeningOn = true;
        setText("Listening", "On");
    }
    else
    {
        b.allLedsOff();
        listeningOn = false;
        setText("Listening", "Off");
    }
}

void toggleFlashlight()
{
    if (!flashlightOn)
    {
        b.allLedsOn(255, 255, 255);
        flashlightOn = true;
        setText("Flashlight", "On");
    }
    else
    {
        b.allLedsOff();
        flashlightOn = false;
        setText("Flashlight", "Off");
    }
    
    delay(1000);
}

void motionMode()
{
    int xValue = b.readX();
    int yValue = b.readY();
    int zValue = b.readZ();

    b.allLedsOn(abs(xValue), abs(yValue), abs(zValue));
    Particle.publish("xyz " + String(xValue) + " " + String(yValue) + " " + String(zValue), NULL, 60, PRIVATE);
    
    String motionData = "x:" + String(xValue) + "\r\n" + "y:" + String(yValue) + "\r\n" + "z:" + String(zValue);
    setText("Motion", motionData);

    // Wait a mo'
    delay(50);
}

void directionMode()
{
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
        setText("Direction", String(ledPos), 3);
    }
    
    lastDirection = ledPos;

    // Wait a mo'
    delay(100);
}

void showClock()
{
    if (Time.second() != lastSecond)
    {
        String hour = (Time.hour() < 10 ? " 0" : " ") + String(Time.hour());
        String minute = (Time.minute() < 10 ? ":0": ":") + String(Time.minute());
        String second = (Time.second() < 10 ? ":0": ":") + String(Time.second());
        
        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setTextColor(WHITE);
        oled.setCursor(0, 0);
        oled.println(getCurrentDate());
        oled.println(hour + minute + second);
        oled.display();

        lastSecond = Time.second();
    }
}

String getCurrentDate()
{
    int weekdayValue = Time.weekday();
    int monthValue = Time.month();
    int day = Time.day();
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
    
    return weekday + " " + month + " " + String(day);
}

bool buttonPressed(int button)
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

void setText(String line1, String line2)
{
    int bodySize;
    
    if (line2.length() > 30)
    {
        bodySize = 1;
    }
    else
    {
        bodySize = 2;
    }

    setText(line1, line2, bodySize);
}

void setText(String line1, String line2, int line2Size)
{
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.println(line1);
    oled.setTextSize(line2Size);
    oled.println(line2);
    oled.display();
}

void getAudio()
{
    unsigned int now = millis();

    if ((now - lastClientCheck) > 100) {
        lastClientCheck = now;

        checkClient = audioServer.available();
        if (checkClient.connected()) {
            audioClient = checkClient;
        }
    }

    #if SERIAL_DEBUG_ON
    if ((now - lastLog) > 1000) {
        lastLog = now;

        Serial.println("counter was " + String(counter));
        counter = 0;
    }
    #endif

    sendEvery(100);
}

void readMic(void)
{
    //read audio
    uint16_t value = analogRead(MICROPHONE_PIN);

    value = map(value, 0, 4095, 0, 255);
    audio_buffer.put(value);

    counter++;
}

void sendEvery(int delay)
{
    // if it's been longer than 100ms since our last broadcast, then broadcast.
    if ((millis() - lastSend) >= delay) {
        sendAudio();
        lastSend = millis();
    }
}

void sendAudio(void)
{
    if (!audioClient.connected()) {
        return;
    }
    int count = 0;
    int storedSoundBytes = audio_buffer.getSize();

    // don't read out more than the max of our ring buffer
    // remember, we're also recording while we're sending
    while (count < storedSoundBytes) {

        if (audio_buffer.getSize() < SINGLE_PACKET_MIN) {
            break;
        }

        // read out max packet size at a time

        // for loop should be faster, since we can check our buffer size just once?
        int size = min(audio_buffer.getSize(), SINGLE_PACKET_MAX);
        int c = 0;
        for(int c = 0; c < size; c++) {
            txBuffer[c] = audio_buffer.get();
        }
        count += size;

        audioClient.write(txBuffer, size);
    }
}

/*
void write_socket(TCPClient socket, uint16_t *buffer, int count)
{
    socket.write(buffer, count);
}

bool ledState = false;
void toggleLED()
{
    ledState = !ledState;
    digitalWrite(D7, (ledState) ? HIGH : LOW);
}
*/