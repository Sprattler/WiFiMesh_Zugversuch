/******************************************************************************
SCL3400.cpp
SCL3400 Arduino Driver
Leon Angele
Version 1.0.0 - Datum

Resources:
Uses SPI.h for SPI operation

Development environment specifics:
Visual Studio Code with Platform.io


******************************************************************************/

// ensure this library description is only included once
#ifndef __SCL3400_h
#define __SCL3400_h

// Uncomment the following line for debugging output
//#define debug_SCL3400

// Need the following define for SAMD processors
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial_SCL SERIAL_PORT_USBVIRTUAL
#else
  #define Serial_SCL Serial
#endif

#include <stdint.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <SPI.h>  // SPI library is used for...SPI.

#ifndef SCL3400_SPI_CLOCK
#ifdef ARDUINO_ARCH_ESP32
#define SCL3400_SPI_CLOCK 4000000
#else
#define SCL3400_SPI_CLOCK 4000000
#endif
#endif

#ifndef SCL3400_SPI_MODE
#define SCL3400_SPI_MODE SPI_MODE0
#endif

//Select Board

#define WHOAMIID 0xe0

//Define allowed commands to SCL3400 inclinometer
#define RdAccX		0x040000f7
#define RdAccY		0x080000fd
#define RdSTO		0x100000e9
#define EnaAngOut	0xb0001f6f
#define RdAngX		0x240000c7
#define RdAngY		0x280000cd
#define RdTemp		0x140000ef
#define RdStatSum	0x180000e5
#define RdErrFlg1	0x1c0000e3
#define RdErrFlg2	0x200000c1
#define RdCMD		0x340000df
#define ChgModeA	0xb400001f
#define ChgModeB	0xb4000338
#define SetPwrDwn	0xb400046b
#define WakeUp		0xb400001f
#define SWreset		0xb4002098
#define RdWHOAMI	0x40000091
#define RdSer1		0x640000a7
#define RdSer2		0x680000AD
#define RdCurBank	0x7c0000b3
#define SwtchBnk0	0xfc000073
#define SwtchBnk1	0xfc00016e

// Structure to hold raw sensor data
// We need to populate all this every time we read a set of data
struct SCL3400data {
  public:
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t STO;
    int16_t TEMP;
    int16_t AngX;
    int16_t AngY;
    int16_t AngZ;
    uint16_t StatusSum;
    uint16_t WHOAMI;
};

// SCL3400 library interface description
class SCL3400 {
  // user-accessible "public" interface
  public:
    SPISettings spiSettings{SCL3400_SPI_CLOCK, MSBFIRST, SCL3400_SPI_MODE};
	
    SCL3400data sclData;
    boolean setMode(int mode);
    boolean begin(void);
    boolean begin(uint8_t csPin);
    boolean begin(SPIClass &spiPort, uint8_t csPin);
    //Functions to retrieve sensor data
    boolean isConnected();
    boolean available(void);
    void setFastReadMode();
    void stopFastReadMode();
    double getCalculatedAngleX(void);
    double getCalculatedAngleY(void);

    double getTiltLevelOffsetAngleX(void);
    double getTiltLevelOffsetAngleY(void);
    double getTiltLevelOffsetAngleZ(void);
    double getCalculatedAccelerometerX(void);
    double getCalculatedAccelerometerY(void);

    uint16_t getErrFlag1(void);
    uint16_t getErrFlag2(void);
    unsigned long getSerialNumber(void);
    double getCalculatedTemperatureCelsius(void);
    double getCalculatedTemperatureFarenheit(void);
    double angle(int16_t SCL3400_ANG); //two's complement value expected
    double acceleration(int16_t SCL3400_ACC);
    bool crcerr, statuserr;
    uint16_t powerDownMode(void);
    uint16_t WakeMeUp(void);
    uint16_t reset(void);
	
  // library-accessible "private" interface
  private:
    SPIClass *_spiPort = NULL;  //The generic connection to user's chosen spi hardware

    uint8_t SCL3400_csPin = 10; // Default SPI chip select  -- ESp32-> PIN5 , set on .begin(5) 
    uint8_t SCL3400_mode = 2; // Default inclinometer mode
    uint8_t SCL3400_CMD, SCL3400_CRC;
    uint16_t SCL3400_DATA;
    double Temperature, X_angle, Y_angle, Z_angle;
    bool setFastRead = false;
	
    void initSPI();
    void beginTransmission();
    void endTransmission();
    uint8_t CalculateCRC(uint32_t Data);
    uint8_t CRC8(uint8_t BitValue, uint8_t SCL3400_CRC);
    unsigned long transfer(unsigned long value);

    union FourByte {
      unsigned long bit32;
      unsigned int bit16[2];
      unsigned char bit8[4];
    };
    unsigned long modeCMD[5]  = { 0, ChgModeA, ChgModeB,};
};
#endif
