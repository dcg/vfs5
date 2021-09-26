/**
*
* This firmware is a software implementation of VOX vfs5 footswitch, 
* compatible with the VT/VT+ series of guitar amplifiers. 
* it's tested on a VT20+.
*
* Have a look to http://www.whitelamp.com/public/vox_vfs5.html for the 
* analog circuit reverse engineered from the original VOX footswitch.
*
* we generate a PWM on arduino's digital pin 5,
* setting the port to the highest possible frequency (~60kHz)
* and apply to it a simple dual pole low-pass filter to convert 
* the PWM signal in a continous analog voltage:
*
*  pin11 >------- 1.2KOhm ------ 1.2KOhm ------> out 
*                          |              |
*                         47nF           47nF
*                         _|_            _|_
*                           ///            ///
*
*
*  pin 0,3,4,6  >------- BTN ------ GND 
*  pin 9,10,7,8 >------- LED ------ 330Ohm ------ GND 
*             
* WARNING: the voltage provided by the amp drops to 4.25V when arduino is connected... see MAX_OUTV const
*
*
**/

#include <stdio.h>
#include <Bounce2.h>

#define dbg true

int I_BNK = 2;
int I_CH1 = 3;
int I_CH2 = 4;
int I_CH3 = 6;
int I_CH4 = 0;

int O_CH1 = 9;
int O_CH2 = 10;
int O_CH3 = 7;
int O_CH4 = 8;

int OUT = 11;
const double MAX_OUTV = 3.975f;

const double CONTINOUSG = 2.85f;
const double CONTINOUSR = 3.24f;

double table_bankG(int ch)
{
    switch (ch)
    {
    case 1:
        return 2.33f;
    case 2:
        return 1.95f;
    case 3:
        return 1.32f;
    case 4:
        return 0.82f;
    }
}

double table_bankR(int ch)
{
    switch (ch)
    {
    case 1:
        return 2.58f;
    case 2:
        return 2.12f;
    case 3:
        return 1.40f;
    case 4:
        return 0.85f;
    }
}

double table(boolean bank, int ch)
{
    if (bank)
        return table_bankG(ch);
    else
        return table_bankR(ch);
}
Bounce bNK = Bounce();
Bounce b2 = Bounce();
Bounce b3 = Bounce();
Bounce b4 = Bounce();
Bounce b1 = Bounce();

// the setup routine runs once when you press reset:
void setup()
{
    bNK.attach(I_BNK, INPUT_PULLUP);
    bNK.interval(25);

    b2.attach(I_CH2, INPUT_PULLUP);
    b2.interval(25);

    b3.attach(I_CH3, INPUT_PULLUP);
    b3.interval(25);

    b4.attach(I_CH4, INPUT_PULLUP);
    b4.interval(25);

    b1.attach(I_CH1, INPUT_PULLUP);
    b1.interval(25);
    //Set timer2 freq (ports D9 & D10) to 30KHz
    //TCCR2B = TCCR2B & B11111000 | B00000001;   //set timer2 divisor to 1 for PWM frequency of 31372.55 Hz
    TCCR0B = ((TCCR0B & B11111000) | B00000001); //Set timer0 freq (ports D5 & D6) to 60KHz
    TCCR2B = TCCR2B & B11111000 | B00000001;

    pinMode(OUT, OUTPUT);
    pinMode(O_CH1, OUTPUT);
    pinMode(O_CH2, OUTPUT);
    pinMode(O_CH3, OUTPUT);
    pinMode(O_CH4, OUTPUT);

    TCCR0B = ((TCCR0B & B11111000) | B00000001);
    TCCR2B = TCCR2B & B11111000 | B00000001;
    if (dbg)
    {
        Serial.begin(9600);
        char s[120];
        sprintf(s, "debug active");
        Serial.println(s);
    }
}

char s[120];
boolean bank = true;
int ch = 2;
double v;

double continous = CONTINOUSG;
unsigned long T1 = 0;
unsigned int blinkCounter;
boolean isBlinkOff = false;
// the loop routine runs over and over again forever:
void loop()
{
    blinkCounter++;
    b1.update();
    b2.update();
    b3.update();
    b4.update();
    bNK.update();

    if (bNK.fell())
    {
        bank = !bank;
        T1 = millis();
    }
    else if (b1.fell())
    {
        ch = 1;
        v = table(bank, ch);
        T1 = millis();
    }
    else if (b2.fell())
    {
        ch = 2;
        v = table(bank, ch);
        T1 = millis();
    }
    else if (b3.fell())
    {
        ch = 3;
        v = table(bank, ch);
        T1 = millis();
    }
    else if (b4.fell())
    {
        ch = 4;
        v = table(bank, ch);
        T1 = millis();
    }
    else if (T1 + 5000 > millis())
    {
    }
    else
    {
        v = bank ? CONTINOUSG : CONTINOUSR;
    }

    int out = v * 255 / MAX_OUTV;
    analogWrite(OUT, out);

    if (ch == 1)
    {
        digitalWrite(O_CH1, HIGH);
        digitalWrite(O_CH2, LOW);
        digitalWrite(O_CH3, LOW);
        digitalWrite(O_CH4, LOW);
    }
    if (ch == 2)
    {
        digitalWrite(O_CH1, LOW);
        digitalWrite(O_CH2, HIGH);
        digitalWrite(O_CH3, LOW);
        digitalWrite(O_CH4, LOW);
    }
    if (ch == 3)
    {
        digitalWrite(O_CH1, LOW);
        digitalWrite(O_CH2, LOW);
        digitalWrite(O_CH3, HIGH);
        digitalWrite(O_CH4, LOW);
    }
    if (ch == 4)
    {
        digitalWrite(O_CH1, LOW);
        digitalWrite(O_CH2, LOW);
        digitalWrite(O_CH3, LOW);
        digitalWrite(O_CH4, HIGH);
    }
    if (!bank && blinkCounter % 20 == 0) //let it blink
    {
        isBlinkOff = !isBlinkOff;
    }
    if (bank) 
    {
      isBlinkOff = false;
    }
    if (isBlinkOff) 
    {
        digitalWrite(O_CH1, LOW);
        digitalWrite(O_CH2, LOW);
        digitalWrite(O_CH3, LOW);
        digitalWrite(O_CH4, LOW);
    }
    if (dbg)
    {
        Serial.print(blinkCounter);
        sprintf(s, " bank %d ", bank);
        Serial.print(s);
        sprintf(s, "ch %d ", ch);
        Serial.print(s);
        sprintf(s, "v=%d out=%d", (int)(v * 100), out);
        Serial.println(s);
    }
}
