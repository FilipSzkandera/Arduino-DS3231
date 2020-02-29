#ifndef MYMAX7219_H
#define MYMAX7219_H

    #include <Arduino.h>
    #include <Wire.h>

    #define CHARSET_0 0b01111110
    #define CHARSET_1 0b00110000
    #define CHARSET_2 0b01101101
    #define CHARSET_3 0b01111001
    #define CHARSET_4 0b00110011
    #define CHARSET_5 0b01011011
    #define CHARSET_6 0b01011111
    #define CHARSET_7 0b01110000
    #define CHARSET_8 0b01111111
    #define CHARSET_9 0b01111011

    #define CHARSET_A 0b01110111
    #define CHARSET_C 0b01001110
    #define CHARSET_E 0b01001111
    #define CHARSET_F 0b01000111
    #define CHARSET_H 0b00110111
    #define CHARSET_I 0b00000110
    #define CHARSET_L 0b00001110
    #define CHARSET_P 0b01100111
    #define CHARSET_t 0b00001111
    #define CHARSET_S 0b01011011
    #define CHARSET_O 0b01111110
    #define CHARSET_r 0b00000101
    #define CHARSET_o 0b00011101
    #define CHARSET_DASH 0b00000001
    #define CHARSET_DEG 0b01100011
    #define CHARSET_BLANK 0b00000000
    #define CHARSET_DOT 0b10000000




    #define DECODE_NODECODE 0x0

    const byte CHARSET_DIGITS[] = {CHARSET_0,CHARSET_1,CHARSET_2,CHARSET_3,CHARSET_4,CHARSET_5,CHARSET_6,CHARSET_7,CHARSET_8,CHARSET_9};

    class MyMAX7219
    {
    private:
        int clk;
        int data;
        int load;
    public:
        MyMAX7219(int pin_load_, int pin_data_, int pin_clock_);
        void sendPacket(byte character, int index);
        void wakeup();
        void shutdown();
        void setIntensity(int instensity);
        void displayTest(bool state);
        void setDecodeMode(int state);
        void setScanLimit(int state);
        void clearDisplay();
        void print(byte character,bool dot,int index);
        void init();
    };

    MyMAX7219::MyMAX7219(int pin_load_, int pin_data_, int pin_clock_)
    {
        clk = pin_clock_;
        data = pin_data_;
        load = pin_load_;

        pinMode(clk,OUTPUT);
        pinMode(data,OUTPUT);
        pinMode(load,OUTPUT);
        digitalWrite(load,1);        
    }

    void MyMAX7219::init() 
    {
        sendPacket(1,0xC);
        setScanLimit(8);
        setDecodeMode(DECODE_NODECODE);
        sendPacket(0x1,0xA);
        displayTest(false);
        clearDisplay();
    }

    void MyMAX7219::wakeup() 
    {
        sendPacket(0x1,0xC);
    }
    void MyMAX7219::shutdown() 
    {
        sendPacket(0x0,0xC);
    }
    void MyMAX7219::displayTest(bool state)
    {
        if (state) sendPacket(0x1,0xF);
        else sendPacket(0x0,0xF);
    }

    void MyMAX7219::setIntensity(int instensity)
    {
        sendPacket((instensity & 0b1111),0xA);
    }

    void MyMAX7219::setScanLimit(int state)
    {
        sendPacket(((state-1) & 0b111),0xB);
    }

    void MyMAX7219::clearDisplay()
    {
        for (int i = 0; i < 8; i++)
        {
            sendPacket(0,i+1);
        }
    }

    void MyMAX7219::print(byte character, bool dot, int index)
    {
        if (dot) character |= 128;
        if (index <= 7) sendPacket(character,index+1);
    }


    void MyMAX7219::setDecodeMode(int state)
    {
        sendPacket(DECODE_NODECODE,0x9);
    }



    void MyMAX7219::sendPacket(byte character, int index)
    {
        int word = character | ((index & 0b1111) << 8);
        // delay(10);
        digitalWrite(load,0);
        for (int i = 15; i >= 0; i--)
        {
            int x = ((word & (1 << i)) >> i);
            digitalWrite(data,x);
            // delay(1);
            digitalWrite(clk,1);
            // delay(1);
            digitalWrite(clk,0);
            // delay(1);
        }
        digitalWrite(load,1);
    }



#endif