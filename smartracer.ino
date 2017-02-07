#include "application.h"

int buttonBack = D0;
int buttonRight = D1;
int buttonForward = D2;
int buttonLeft = D3;
bool isForward;
bool isMoving;
static bool init = false;
void setup()

{
    pinMode(buttonBack, OUTPUT);
    pinMode(buttonRight, OUTPUT);
    pinMode(buttonForward, OUTPUT);
    pinMode(buttonLeft, OUTPUT);
    
    resetRacer();
    
    Particle.subscribe("SMARTRACER_MOVE", moveRacer);
    
    Serial.begin(115200);
}

void loop()
{
    if (isMoving)
    {
        uint32_t inchesFront, inchesLeft, inchesRight;
        
        //forward sensor
        inchesFront = ping(D4, D5, 0, false);
        
        if (isForward)
        {
            if (inchesFront < 30)
            {
                brakeRacer();
            }

            //if (inchesFront < 60)
            //{
                //left sensor
                //inchesLeft = ping(D4, D6, 0, false);
                
                //right sensor
                //inchesRight = ping(D4, D7, 0, false);

/*
                if (inchesLeft < inchesRight and inchesLeft < 30)// and isMoving and isForward)
                {
                    digitalWrite(buttonLeft, HIGH);
                    digitalWrite(buttonRight, LOW);
                }
                if (inchesLeft > inchesRight and inchesRight < 30)// and isMoving and isForward)
                {
                    digitalWrite(buttonRight, HIGH);
                    digitalWrite(buttonLeft, LOW);
                }
*/
            //}
        }
        
    
    /*
        if (inchesFront > 0) Particle.publish("inchesFront", String(inchesFront), 60, PRIVATE);
        if (inchesLeft > 0) Particle.publish("inchesLeft", String(inchesLeft), 60, PRIVATE);
        if (inchesRight > 0) Particle.publish("inchesRight", String(inchesRight), 60, PRIVATE);
    */
        //delay(100);
    }
}

void resetRacer()

{
    digitalWrite(buttonBack, HIGH);
    digitalWrite(buttonRight, HIGH);
    digitalWrite(buttonForward, HIGH);
    digitalWrite(buttonLeft, HIGH);
    
    isMoving = false;
}

void brakeRacer()
{
    resetRacer();
    
    if (isForward)
    {
        digitalWrite(buttonBack, LOW);
        delay(100);
        digitalWrite(buttonBack, HIGH);
        digitalWrite(buttonForward, LOW);
        delay(100);
        digitalWrite(buttonForward, HIGH);
    }
    else
    {
        digitalWrite(buttonForward, LOW);
        delay(100);
        digitalWrite(buttonForward, HIGH);
        digitalWrite(buttonBack, LOW);
        delay(100);
        digitalWrite(buttonBack, HIGH);
    }
    
    isMoving = false;
}

void moveRacer(const char *event, const char *data)
{
    if (strcmp(data, "left") == 0)
    {
        digitalWrite(buttonRight, HIGH);
        digitalWrite(buttonLeft, LOW);
    }
    else if (strcmp(data, "right") == 0)
    {
        digitalWrite(buttonLeft, HIGH);
        digitalWrite(buttonRight, LOW);
    }
    else if (strcmp(data, "forward") == 0)
    {
        if (isMoving and isForward)
        {
            digitalWrite(buttonRight, HIGH);
            digitalWrite(buttonLeft, HIGH);
        }
        
        digitalWrite(buttonBack, HIGH);
        digitalWrite(buttonForward, LOW);
        isForward = true;
        isMoving = true;
    }
    else if (strcmp(data, "back") == 0)
    {
        if (isMoving and !isForward)
        {
            digitalWrite(buttonRight, HIGH);
            digitalWrite(buttonLeft, HIGH);
        }

        digitalWrite(buttonForward, HIGH);
        digitalWrite(buttonBack, LOW);
        isForward = false;
        isMoving = true;
    }
    else if (strcmp(data, "brake") == 0)
    {
        brakeRacer();
    }
    else if (strcmp(data, "reset") == 0)
    {
        resetRacer();
    }
}

uint32_t ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info)
{
    uint32_t duration, inches, cm;

    if (!init) {
        pinMode(trig_pin, OUTPUT);
        digitalWriteFast(trig_pin, LOW);
        pinMode(echo_pin, INPUT);
        delay(50);
        init = true;
    }

    /* Trigger the sensor by sending a HIGH pulse of 10 or more microseconds */
    digitalWriteFast(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWriteFast(trig_pin, LOW);
  
    duration = pulseIn(echo_pin, HIGH);
    
    /* Convert the time into a distance */
    // Sound travels at 1130 ft/s (73.746 us/inch)
    // or 340 m/s (29 us/cm), out and back so divide by 2
    // Ref: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
    inches = duration / 74 / 2;
    //cm = duration / 29 / 2;
    
/*      
    if (info) { // Visual Output
        Serial.printf("%2d:", inches);
        for(int x=0;x<inches;x++) Serial.print("#");
        Serial.println();
    } else { // Informational Output
        Serial.printlnf("%6d in / %6d cm / %6d us", inches, cm, duration);
        Particle.publish("in: " + String(inches) + " cm: " + String(cm) + " us: " + String(duration), NULL, 60, PRIVATE);
    }
*/
    delay(wait); // slow down the output
    
    return inches;
}