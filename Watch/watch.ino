// This #include statement was automatically added by the Particle IDE.
#include "GOFi2cOLED.h"
#include "Wire.h"

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

//Mario main theme melody
int mario_melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int mario_tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Mario song length
int mario_length = 78;
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underworld tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};
//Underworld song length
int underworld_length = 56;

GOFi2cOLED oled;
int lastSecond = 0;
static bool init = false;

void setup()
{
    pinMode(A4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, INPUT_PULLDOWN);
    pinMode(D7, INPUT_PULLDOWN);

    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D
    
    oled.display(); // show splashscreen
    delay(2000);
    
    oled.clearDisplay();
    
    Time.zone(-4);
    
    Serial.begin(115200);
}
 
void loop()
{
    //showClock();
    //showDistance();
    showInfo();
    
    if (buttonPressed(D6))
    {
        playSong(underworld_length, underworld_melody, underworld_tempo, 800);
    }

    if (buttonPressed(D7))
    {
        /*
        char notes[] = "cdfda ag cdfdg gf ";
        int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
        int tempo = 113;
        int songLength = 18;
        */
        
        playSong(mario_length, mario_melody, mario_tempo, 800);
    }
}

void playSong(int songLength, int notes[], int beats[], int tempo)
{
  int i, duration;

  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    //duration = beats[i] * tempo;  // length of note/rest in ms
    duration = tempo / beats[i];

    if (notes[i] == ' ')          // is this a rest? 
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      //tone(A4, frequency(notes[i]), duration);
      tone(A4, notes[i], duration);
      delay(duration);            // wait for tone to finish
    }

    //delay(tempo / 10);              // brief pause between notes
    delay(duration * 1.30);
  }
}

void showInfo()
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

        uint32_t inches = ping(D2, D3, 100, false);
        oled.setTextSize(1);
        oled.println("");
        oled.println(String(inches) + " inches");

        oled.display();

        lastSecond = Time.second();
    }
}

void showDistance()
{
    uint32_t inches;
    
    inches = ping(D2, D3, 100, false);
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.println(String(inches));
    oled.display();
}

void showClock()
{
    if (Time.second() != lastSecond)
    {
        String hour = (Time.hour() < 10 ? " 0" : " ") + String(Time.hour());
        String minute = (Time.minute() < 10 ? ":0": ":") + String(Time.minute());
        String second = (Time.second() < 10 ? ":0": ":") + String(Time.second());
        
        //setText(getCurrentDate(), hour + minute + second);

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

int frequency(char note) 
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;
  const int numNotes = 8;  // number of notes we're storing

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // We looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
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