// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <PubNub.h>

char channelSmartLamp[] = "SMARTLAMP";
OneWire ds = OneWire(D3);
unsigned long lastUpdate = 0;
float lastTemp;
int lastTempReading;

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
    
    int timeOffset = Time.minute() - lastTempReading;
    
    if (timeOffset < 0 || timeOffset >= 10)
    {
        while (!ReadTemperature("temperature")) delay(1000);
        lastTempReading = Time.minute();
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

bool ReadTemperature(String eventName)
{
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;
    
    if (!ds.search(addr)) 
    {
        Serial.println("No more addresses.");
        Serial.println();
        ds.reset_search();
        delay(250);
        return false;
    }
    
    // The order is changed a bit in this example
    // first the returned address is printed
    
    Serial.print("ROM =");
    for( i = 0; i < 8; i++)
    {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
    }
    
    // second the CRC is checked, on fail,
    // print error and just return to try again
    
    if (OneWire::crc8(addr, 7) != addr[7]) 
    {
        Serial.println("CRC is not valid!");
        return false;
    }
    Serial.println();
    
    // we have a good address at this point
    // what kind of chip do we have?
    // we will set a type_s value for known types or just return
    
    // the first ROM byte indicates which chip
    switch (addr[0])
    {
        case 0x10:
            Serial.println("  Chip = DS1820/DS18S20");
            type_s = 1;
            break;
        case 0x28:
            Serial.println("  Chip = DS18B20");
            type_s = 0;
            break;
        case 0x22:
            Serial.println("  Chip = DS1822");
            type_s = 0;
            break;
        case 0x26:
            Serial.println("  Chip = DS2438");
            type_s = 2;
            break;
        default:
            Serial.println("Unknown device type.");
            return false;
    }
    
    // this device has temp so let's read it
    
    ds.reset();               // first clear the 1-wire bus
    ds.select(addr);          // now select the device we just found
    // ds.write(0x44, 1);     // tell it to start a conversion, with parasite power on at the end
    ds.write(0x44, 0);        // or start conversion in powered mode (bus finishes low)
    
    // just wait a second while the conversion takes place
    // different chips have different conversion times, check the specs, 1 sec is worse case + 250ms
    // you could also communicate with other devices if you like but you would need
    // to already know their address to select them.
    
    delay(1000);     // maybe 750ms is enough, maybe not, wait 1 sec for conversion
    
    // we might do a ds.depower() (parasite) here, but the reset will take care of it.
    
    // first make sure current values are in the scratch pad
    
    present = ds.reset();
    ds.select(addr);
    ds.write(0xB8,0);         // Recall Memory 0
    ds.write(0x00,0);         // Recall Memory 0
    
    // now read the scratch pad
    
    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE,0);         // Read Scratchpad
    if (type_s == 2)
    {
        ds.write(0x00,0);       // The DS2438 needs a page# to read
    }
    
    // transfer and print the values
    
    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++)
    {           // we need 9 bytes
        data[i] = ds.read();
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
    
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s == 2) raw = (data[2] << 8) | data[1];
    byte cfg = (data[4] & 0x60);
    
    switch (type_s)
    {
        case 1:
            raw = raw << 3; // 9 bit resolution default
            if (data[7] == 0x10)
            {
                // "count remain" gives full 12 bit resolution
                raw = (raw & 0xFFF0) + 12 - data[6];
            }
            celsius = (float)raw * 0.0625;
            break;
        case 0:
            // at lower res, the low bits are undefined, so let's zero them
            if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
            if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
            if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
            // default is 12 bit resolution, 750 ms conversion time
            celsius = (float)raw * 0.0625;
            break;
        case 2:
            data[1] = (data[1] >> 3) & 0x1f;
            if (data[2] > 127)
            {
                celsius = (float)data[2] - ((float)data[1] * .03125);
            }
            else
            {
                celsius = (float)data[2] + ((float)data[1] * .03125);
            }
            break;
    }
    
    // remove random errors
    if((((celsius <= 0 && celsius > -1) && lastTemp > 5)) || celsius > 125)
    {
        celsius = lastTemp;
    }
    
    fahrenheit = celsius * 1.8 + 32.0;
    lastTemp = celsius;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    
    // now that we have the readings, we can publish them to the cloud
    String temperature = String(fahrenheit); // store temp in "temperature" string
    Particle.publish(eventName, temperature, PRIVATE); // publish to cloud

    //char *fahrenheitString;
    //fahrenheitString = ftoa(fahrenheitString, (double)fahrenheit, 6);
    //dtostrf(fahrenheit, 4, 6, fahrenheitString);
    //char *celsiusString;
    //celsiusString = ftoa(celsiusString, (double)celsius, 6);
    //dtostrf(celsius, 4, 6, celsiusString);
    
    //Particle.variable("Temp in F", String(fahrenheitString));
    //Particle.variable("Temp in C", String(celsiusString));
    
    return true;
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

char *ftoa(char *a, double f, int precision)
{
    long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
    
    char *ret = a;
    long number = (long)f;
    itoa(number, a, 10);
    while (*a != '\0') a++;
    *a++ = '.';
    long decimal = abs((long)((f - number) * p[precision]));
    itoa(decimal, a, 10);
    
    return ret;
}