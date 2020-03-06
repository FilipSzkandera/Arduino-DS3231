#ifndef My_RTC_H
#define My_RTC_H

    #include <Arduino.h>
    #include <Wire.h>

    #define AL1_EVERY_SECOND                  0b01111
    #define AL1_M_SECONDS                     0b01110
    #define AL1_M_MINUTES_SECONDS             0b01100
    #define AL1_M_HOURS_MINUTES_SECONDS       0b01000
    #define AL1_M_DATE_HOURS_MINUTES_SECONDS  0b00000
    #define AL1_M_DAY_HOURS_MINUTES_SECONDS   0b10000

    #define AL2_EVERY_MINUTE                  0b0111
    #define AL2_M_MINUTES                     0b0110
    #define AL2_M_HOURS_MINUTES               0b0100
    #define AL2_M_DATE_HOURS_MINUTES          0b0000
    #define AL2_M_DAY_HOURS_MINUTES           0b1000

    #define OSC_1HZ             0b00
    #define OSC_1024HZ          0b01
    #define OSC_4096HZ          0b10
    #define OSC_8192HZ          0b11

    #define ALARM_ONE_ADR       0x7
    #define ALARM_TWO_ADR       0xB 

    char CONTROL_REG_DATA = 0b00011100;  
    #define CONTROL_REG_ADR     0xE
    #define EOSC    7
    #define BBSQW   6
    #define CONV    5
    #define RS2     4
    #define RS1     3
    #define INTCN   2
    #define A2IE    1
    #define A1IE    0 

    char STATUS_REG_DATA = 0b10001000;
    #define STATUS_REG_ADR      0xF
    #define OSF     7
    #define EN32KHZ 3
    #define BSY     2
    #define A2F     1
    #define A1F     0

    #define AGING_OFFSET_REG    0x10
    #define TEMP_MSB_REG        0x11
    #define TEMP_LSB_REG        0x12

    const char DEVICE_ID = 0x68;


    class My_RTC
    {
    private:
    public:
        My_RTC(/* args */);
        char dec2bcd(char x);
        char bcd2bin(char x);
        char readByte(char adress);
        void writeByte(char adress, char data);
        void SetTime(char year,char month,char date,char day,char hour,char minute,char second);
        unsigned char readTimeSeconds();
        unsigned char readTimeMinutes();
        unsigned char readTimeHours();
        unsigned char readTimeDay(); 
        void SetAlarm1(char date,char day,char hour,char minute,char second,char ALARM_MATCH);
        unsigned char readAlarm1Seconds();
        unsigned char readAlarm1Minutes();
        unsigned char readAlarm1Hours();
        unsigned char readAlarm1Day(); 
        void SetAlarm2(char date,char day,char hour,char minute,char ALARM_MATCH);
        unsigned char readAlarm2Minutes();
        unsigned char readAlarm2Hours();
        unsigned char readAlarm2Day(); 
        void Alarm1Clear();
        void Alarm2Clear();
        void Alarm1InterruptEnable(bool enable);
        void Alarm2InterruptEnable(bool enable);
        void enableInterrupts(bool enable);
        bool Alarm1_isSet();
        bool Alarm2_isSet();
        void setOscillator(char setting);
        float readTemperature();
    };

    float My_RTC::readTemperature()
    {
        byte temp = readByte(0x11);
        byte temp_fraction = readByte(0x12);
        temp_fraction = (temp_fraction >> 6) * 0.25;
        bool minus = false;
        if (temp >= 128) { minus = true; temp = !temp + 1; }
        if (minus) temp = temp * -1;
        float tmp = temp + temp_fraction;
        return tmp;
    }

    void My_RTC::setOscillator(char setting){
        char data = readByte(CONTROL_REG_ADR);
        data &= ~(0b00011000);
        writeByte(CONTROL_REG_ADR,data);
    }


    char My_RTC::dec2bcd(char x)
    {
        char b = (x * 103) >> 10;
        return (b * 16 + x-(b*10));  
    }
    char My_RTC::bcd2bin(char x) {
        // converts from binary-coded decimal to a "regular" binary number
        return ((((x >> 4) & 0xF) * 10) + (x & 0xF)) ;
    }      

    /* Zapíše byte na danou adresu */
    void My_RTC::writeByte(char adress, char data){
        Wire.beginTransmission(DEVICE_ID);
        Wire.write((char)adress);                // nastavení adresy pro zápis 
        Wire.write(data);
        Wire.endTransmission();
    }

    /* Přečte a vrátí byte z dané adresy */
    char My_RTC::readByte(char adress){
        Wire.beginTransmission(DEVICE_ID);
        Wire.write((char)adress);                // nastavení adresy pro čtení
        Wire.endTransmission();
        Wire.requestFrom(DEVICE_ID,1);
        if(Wire.available() >= 1){ return Wire.read(); }
        else {return -1;}
        
    }

    My_RTC::My_RTC(/* args */)
    {
    }

    void My_RTC::SetTime(char year,char month,char date,char day,char hour,char minute,char second){
        Wire.beginTransmission(DEVICE_ID);
        Wire.write((char)0);                // nastavení adresy pro zápis 
        Wire.write(dec2bcd(second));
        Wire.write(dec2bcd(minute));
        Wire.write(dec2bcd(hour));
        Wire.write(day);
        Wire.write(dec2bcd(date));
        Wire.write(dec2bcd(month));
        Wire.write(dec2bcd(year));
        Wire.endTransmission();
    }

    unsigned char My_RTC::readTimeSeconds() { return bcd2bin(readByte(0x0)); }
    unsigned char My_RTC::readTimeMinutes() { return bcd2bin(readByte(0x1)); }
    unsigned char My_RTC::readTimeHours() { return bcd2bin(readByte(0x2)); }
    unsigned char My_RTC::readTimeDay() { return bcd2bin(readByte(0x3)); }

    unsigned char My_RTC::readAlarm1Seconds() { return bcd2bin(readByte(0x7)); }
    unsigned char My_RTC::readAlarm1Minutes() { return bcd2bin(readByte(0x8)); }
    unsigned char My_RTC::readAlarm1Hours() { return bcd2bin(readByte(0x9)); }
    unsigned char My_RTC::readAlarm1Day() { return bcd2bin(readByte(0xA)); }

    unsigned char My_RTC::readAlarm2Minutes() { return bcd2bin(readByte(0xB)); }
    unsigned char My_RTC::readAlarm2Hours() { return bcd2bin(readByte(0xC)); }
    unsigned char My_RTC::readAlarm2Day() { return bcd2bin(readByte(0xD)); }
    

    void My_RTC::SetAlarm1(char date,char day,char hour,char minute,char second,char ALARM_MATCH){
        Wire.beginTransmission(DEVICE_ID);
        Wire.write((char)0x7);                // nastavení adresy zápis hodnot 1. alarmu
        Wire.write(((ALARM_MATCH & (1 << 0)) >> 0) << 7 | dec2bcd(second));
        Wire.write(((ALARM_MATCH & (1 << 1)) >> 1) << 7 | dec2bcd(minute));
        Wire.write(((ALARM_MATCH & (1 << 2)) >> 2) << 7 | dec2bcd(hour));
        int x = 0;
        int y = day;
        if (ALARM_MATCH == AL1_M_DAY_HOURS_MINUTES_SECONDS)  x = 1;
        if (ALARM_MATCH == AL1_M_DATE_HOURS_MINUTES_SECONDS) y = date;
        Wire.write(((ALARM_MATCH & (1 << 3)) >> 3) << 7 | x << 6 | y);   
        Wire.endTransmission();
    }

    void My_RTC::SetAlarm2(char date,char day,char hour,char minute,char ALARM_MATCH){
        Wire.beginTransmission(DEVICE_ID);
        Wire.write((char)0xB);                // nastavení adresy zápis hodnot 2. alarmu
        Wire.write(((ALARM_MATCH & (1 << 0)) >> 0) << 7 | dec2bcd(minute));
        Wire.write(((ALARM_MATCH & (1 << 1)) >> 1) << 7 | dec2bcd(hour));
        int x = 0;
        int y = day;
        if (ALARM_MATCH == AL2_M_DAY_HOURS_MINUTES)  x = 1;
        if (ALARM_MATCH == AL2_M_DATE_HOURS_MINUTES) y = date;
        Wire.write(((ALARM_MATCH & (1 << 2)) >> 2) << 7 | x << 6 | y);   
        Wire.endTransmission();
    }

    void My_RTC::Alarm1Clear(){
        char al1_data = readByte(STATUS_REG_ADR);
        al1_data &= ~(1 << A1F);
        writeByte(STATUS_REG_ADR,al1_data);
    }
    void My_RTC::Alarm2Clear(){
        char al2_data = readByte(STATUS_REG_ADR);
        al2_data &= ~(1 << A2F);
        writeByte(STATUS_REG_ADR,al2_data);
    }
    void My_RTC::Alarm1InterruptEnable(bool enable){
        char al1_data = readByte(CONTROL_REG_ADR);
        if (enable == 1) al1_data |= (1 << A1IE);
        if (enable == 0) al1_data &= ~(1 << A1IE);
        writeByte(CONTROL_REG_ADR,al1_data);
    }
    void My_RTC::Alarm2InterruptEnable(bool enable){
        char al2_data = readByte(CONTROL_REG_ADR);
        if (enable == 1) al2_data |= (1 << A2IE);
        if (enable == 0) al2_data &= ~(1 << A2IE);
        writeByte(CONTROL_REG_ADR,al2_data);
    }
    
    /* If True, Timer interrupts will be enabled, and SQW output disabled */
    void My_RTC::enableInterrupts(bool enable){
        char data = readByte(CONTROL_REG_ADR);
        if (enable == 1) data |= (1 << INTCN);
        if (enable == 0) data &= ~(1 << INTCN);
        writeByte(CONTROL_REG_ADR,data);
    }

    bool My_RTC::Alarm1_isSet(){
        char data = readByte(STATUS_REG_ADR);
        if ((data & 1) == 1) return true;
        return false;
    }

    bool My_RTC::Alarm2_isSet(){
        char data = readByte(STATUS_REG_ADR);
        if ((data & 2) == 2) return true;
        return false;
    }

    // AHA

#endif