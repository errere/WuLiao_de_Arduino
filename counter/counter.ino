#include <LiquidCrystal.h>

uint8_t keys[4] = {PB12, PB13, PA15, PB3};
uint8_t LCD_EN = PB10;
uint8_t LCD_RS = PB11;
uint8_t LCD_DB[8] = {PB1, PB0, PA7, PA6, PA5, PA4, PA3, PA2};
uint8_t outPin = PA0;

int8_t NumberSet[6] = {0, 0, 0, 0, 0, 0}; //hhmmss

uint8_t KeyDown[4] = {0, 0, 0, 0};

int8_t UI_CUR = 0;
uint8_t UI_CurPos[6] = {0, 1, 3, 4, 6, 7};

uint8_t allowout = 0;
uint8_t ison = 0;

uint8_t startCounter = 0;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_DB[4], LCD_DB[5], LCD_DB[6], LCD_DB[7]);
HardwareTimer timer(1);

void Count();
void draw();
void oprAdd();
void oprSub();

void h1add();
void h2add();
void m1add();
void m2add();
void s1add();
void s2add();

uint8_t h1sub();
uint8_t h2sub();
uint8_t m1sub();
uint8_t m2sub();
uint8_t s1sub();
uint8_t s2sub();

uint8_t isZero();

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    lcd.begin(8, 2);
    for (uint8_t i = 0; i < 4; ++i)
        pinMode(keys[i], INPUT_PULLDOWN);

    timer.pause();
    timer.setPrescaleFactor(10000);
    timer.setOverflow(7200);
    timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH1, 7200);
    timer.attachCompare1Interrupt(Count);
    timer.refresh();
    timer.resume();
}

void loop()
{
    // put your main code here, to run repeatedly:
    for (uint8_t i = 0; i < 4; ++i)
    {
        if (digitalRead(keys[i]) == 1)
        {
            delay(10);
            if (digitalRead(keys[i]) == 1)
            {
                delay(10);
                while (digitalRead(keys[i]) == 1)
                    ;
                KeyDown[i] = 1;
                Serial.println(i);
            } //if
        }     //if
    }         //for

    if (KeyDown[0]) //start stop
    {

        if (startCounter && allowout)
        {
            allowout = 0;
        }
        startCounter = !startCounter;
    }

    else if (KeyDown[1]) //next
    {
        if (!startCounter)
        {
            UI_CUR++;
            UI_CUR = UI_CUR % 6;
        }
    }

    else if (KeyDown[2])
    { //up
        if (!startCounter)
        {
            oprAdd();
        }
    }

    else if (KeyDown[3])
    {
        if (!startCounter)
        {
            oprSub();
        }
    } //down

    for (uint8_t i = 0; i < 4; ++i)
        KeyDown[i] = 0;

    draw();
    if (allowout)
    {
        if (ison == 0)
        {
            tone(outPin, 2000);
            ison = 1;
        }
    }
    else
    {
        if (ison == 1)
        {
            noTone(outPin);
            ison = 0;
        }
    }
}

void Count()
{
    uint8_t zero = 0;
    if (startCounter)
    {
        s2sub();
        zero = isZero();
        if (zero == 1)
        {
            allowout = 1;
        }
    }
}
void draw()
{
    lcd.setCursor(2, 0);
    lcd.print(":");
    lcd.setCursor(5, 0);
    lcd.print(":");
    lcd.setCursor(0, 0);
    lcd.print(NumberSet[0]);
    lcd.print(NumberSet[1]);
    lcd.setCursor(3, 0);
    lcd.print(NumberSet[2]);
    lcd.print(NumberSet[3]);
    lcd.setCursor(6, 0);
    lcd.print(NumberSet[4]);
    lcd.print(NumberSet[5]);

    if (!startCounter)
    {
        switch (UI_CUR)
        {
        case 0:
            lcd.setCursor(0, 1);
            lcd.print("^       ");

            break;
        case 1:
            lcd.setCursor(0, 1);
            lcd.print(" ^      ");
            break;
        case 2:
            lcd.setCursor(0, 1);
            lcd.print("   ^    ");
            break;
        case 3:
            lcd.setCursor(0, 1);
            lcd.print("    ^   ");
            break;
        case 4:
            lcd.setCursor(0, 1);
            lcd.print("      ^ ");
            break;
        case 5:
            lcd.setCursor(0, 1);
            lcd.print("       ^");
            break;

        default:
            break;
        }
    } //!startCounter
    else
    {
        if (!allowout)
        {
            lcd.setCursor(0, 1);
            lcd.print("COUNTING");
        }
        else
        {
            lcd.setCursor(0, 1);
            lcd.print("--DOWN--");
        }
    }
}

void h1add()
{
    NumberSet[0]++;
    NumberSet[0] = NumberSet[0] % 10;
}
void h2add()
{
    NumberSet[1]++;
    if (NumberSet[1] >= 10)
    {
        NumberSet[1] = 0;
        h1add();
    }
}
void m1add()
{
    NumberSet[2]++;
    if (NumberSet[2] >= 6)
    {
        NumberSet[2] = 0;
        h2add();
    }
}
void m2add()
{
    NumberSet[3]++;
    if (NumberSet[3] >= 10)
    {
        NumberSet[3] = 0;
        m1add();
    }
}
void s1add()
{
    NumberSet[4]++;
    if (NumberSet[4] >= 6)
    {
        NumberSet[4] = 0;
        m2add();
    }
}
void s2add()
{
    NumberSet[5]++;
    if (NumberSet[5] >= 10)
    {
        NumberSet[5] = 0;
        s1add();
    }
}

uint8_t h1sub()
{
    if (NumberSet[0] == 0)
        return 0;
    NumberSet[0]--;
    if (NumberSet[0] < 0)
    {
        NumberSet[0] = 0;
    }
    return 1;
}
uint8_t h2sub()
{
    uint8_t temp = 1;
    if (NumberSet[1] == 0)
    {
        temp = h1sub();
    }
    if (temp == 0)
        return 0;
    NumberSet[1]--;
    if (NumberSet[1] < 0)
    {
        NumberSet[1] = 9;
    }
    return 1;
}
uint8_t m1sub()
{
    uint8_t temp = 1;
    if (NumberSet[2] == 0)
    {
        temp = h2sub();
    }
    if (temp == 0)
        return 0;
    NumberSet[2]--;
    if (NumberSet[2] < 0)
    {
        NumberSet[2] = 5;
    }
    return 1;
}
uint8_t m2sub()
{
    uint8_t temp = 1;
    if (NumberSet[3] == 0)
    {
        temp = m1sub();
    }
    if (temp == 0)
        return 0;
    NumberSet[3]--;
    if (NumberSet[3] < 0)
    {
        NumberSet[3] = 9;
    }
    return 1;
}
uint8_t s1sub()
{
    uint8_t temp = 1;
    if (NumberSet[4] == 0)
    {
        temp = m2sub();
    }
    if (temp == 0)
        return 0;
    NumberSet[4]--;
    if (NumberSet[4] < 0)
    {
        NumberSet[4] = 5;
    }
    return 1;
}

uint8_t s2sub()
{
    uint8_t temp = 1;
    if (NumberSet[5] == 0)
    {
        temp = s1sub();
    }
    if (temp == 0)
        return 0;
    NumberSet[5]--;
    if (NumberSet[5] < 0)
    {
        NumberSet[5] = 9;
    }
    return 1;
}

void oprAdd()
{
    switch (UI_CUR)
    {
    case 0: //h
        h1add();
        break;
    case 1: //h
        h2add();
        break;
    case 2: //m
        m1add();
        break;
    case 3: //m
        m2add();
        break;
    case 4: //s
        s1add();
        break;
    case 5: //s
        s2add();
        break;

    default:
        break;
    }
}
void oprSub()
{
    switch (UI_CUR)
    {
    case 0: //h
        h1sub();
        break;
    case 1: //h
        h2sub();
        break;
    case 2: //m
        m1sub();
        break;
    case 3: //m
        m2sub();
        break;
    case 4: //s
        s1sub();
        break;
    case 5: //s
        s2sub();
        break;

    default:
        break;
    }
}
uint8_t isZero()
{
    if (NumberSet[0] == 0)
        if (NumberSet[1] == 0)
            if (NumberSet[2] == 0)
                if (NumberSet[3] == 0)
                    if (NumberSet[4] == 0)
                        if (NumberSet[5] == 0)
                            return 1;
    return 0;
}
