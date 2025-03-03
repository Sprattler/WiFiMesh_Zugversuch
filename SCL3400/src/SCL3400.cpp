/******************************************************************************
SCL3400.cpp
SCL3400 Arduino Driver
Leon Angele
Version 1.0.0 - Datum

Resources:
Uses SPI.h for SPI operation

Development environment specifics:
Visual Studio Code with Platform.io

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example.
Distributed as-is; no warranty is given.

******************************************************************************/

// include this library's description file
#include "SCL3400.h"

// Public Methods //////////////////////////////////////////////////////////
// Set the sensor mode to the number provided as modeNum.
boolean SCL3400::setMode(int modeNum) {
  // Set Sensor mode - If not called, the default is mode 4, as set in header file
  // Only allowed values are: 1,2,3,4
  if (modeNum > 0 && modeNum < 5) {
    SCL3400_mode = modeNum;
    if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
    transfer(modeCMD[SCL3400_mode]); //Set mode on hardware
    if (!setFastRead) endTransmission(); //Let go of SPI port/bus
    if (crcerr || statuserr) {
	  reset(); //Reset chip to fix the error state
      return false;  //Let the caller know something went wrong
    } else return true; // Valid value, and chip was set to that mode
  } else
    return false; // Invalid value
}

// Current Version of begin() to initialize the library and the SCL3400
boolean SCL3400::begin(void) {
  //This is the updated Version 3 begin function
  // Determine if we need to set up to use the default SPI interface, or some other one
  if (_spiPort == nullptr) _spiPort = &SPI;
  
  //Wait the required 1 ms before initializing the SCL3400 inclinomenter
  unsigned long startmillis = millis();
  while (millis() - startmillis < 1) ;
  
  initSPI();	// Initialize SPI Library
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  //Write SW Reset command
  transfer(SwtchBnk0);
  transfer(SWreset);
  startmillis = millis();
  while (millis() - startmillis < 1) ;
  //Set measurement mode
  transfer(modeCMD[SCL3400_mode]); //Set mode on hardware
  //We're good, so Enable angle outputs
  transfer(EnaAngOut);
  //The first response after reset is undefined and shall be discarded
  //wait 5 ms to stablize
  startmillis = millis();
  while (millis() - startmillis < 100) ;

  //Read Status to clear the status summary
  transfer(RdStatSum);
  transfer(RdStatSum); //Again, due to off-response protocol used
  transfer(RdStatSum); //And now we can get the real status
  
  //Read the WHOAMI register
  transfer(RdWHOAMI);
  //And again
  transfer(RdWHOAMI);
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  //We now wait until the end of begin() to report if an error occurred
  if (crcerr || statuserr) return false;
  // Once everything is initialized, return a known expected value
  // The WHOAMI command should give an 8 bit value of 0xc1
  return (SCL3400_DATA == WHOAMIID); //Let the caller know if this worked
}

// Set up the SPI communication with the SCL3400 with provided Chip Select pin number, and provided SPI port
boolean SCL3400::begin(SPIClass &spiPort, uint8_t csPin) {
  SCL3400_csPin = csPin;
  _spiPort = &spiPort; //Grab the port the user wants us to use
  return begin();
} // begin

// Set up the SPI communication with the SCL3400 with provided Chip Select pin number
boolean SCL3400::begin(uint8_t csPin) {
  SCL3400_csPin = csPin;
  _spiPort = &SPI; // With this call, we do the default SPI interface
  return begin();
} // begin

//Check to validate that the sensor is still reachable and ready to provide data
boolean SCL3400::isConnected() {
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk0);
  //Read the WHOAMI register
  transfer(RdWHOAMI);
  //And again
  transfer(RdWHOAMI);
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  if (crcerr || statuserr) return false;
  // Once everything is initialized, return a known expected value
  // The WHOAMI command should give an 8 bit value of 0xc1
  return (SCL3400_DATA == WHOAMIID); //Let the caller know if this worked
}

//Read all the sensor data together to keep it consistent
//This is required according to the datasheet
boolean SCL3400::available(void) {
  //Version 3 of this function
  boolean errorflag = false;
  //Read all Sensor Data, as per Datasheet requirements
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk0);

  transfer(RdAccX);
  if (crcerr || statuserr) errorflag = true;
  transfer(RdAccY);
  if (crcerr || statuserr) errorflag = true;
  sclData.AccX = SCL3400_DATA;
  transfer(RdSTO);
  if (crcerr || statuserr) errorflag = true;
  sclData.AccY = SCL3400_DATA;
  transfer(RdTemp);
  if (crcerr || statuserr) errorflag = true;
  sclData.STO = SCL3400_DATA;
  transfer(RdAngX);
  if (crcerr || statuserr) errorflag = true;
  sclData.TEMP = SCL3400_DATA;
  transfer(RdAngY);
  if (crcerr || statuserr) errorflag = true;
  sclData.AngX = SCL3400_DATA;
  transfer(RdStatSum);
  if (crcerr || statuserr) errorflag = true;
  sclData.AngY = SCL3400_DATA;
  transfer(RdWHOAMI);
  if (crcerr || statuserr) errorflag = true;
  sclData.StatusSum = SCL3400_DATA;
  transfer(RdWHOAMI);
  if (crcerr || statuserr) errorflag = true;
  sclData.WHOAMI = SCL3400_DATA;
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  if (errorflag) return false; //Inform caller that something went wrong
  // The WHOAMI command should give an 8 bit value of 0xc1
  return (SCL3400_DATA == WHOAMIID); //Let the caller know this worked
}

/* Set SCL3400 library into Fast Read Mode
 * Warning: Using Fast Read Mode in the library works by keeping the
 *          SPI connection continuously open.  This may or may not affect
 *          the behavior of other hardware interactions, depending on the
 *          sketch design.  Fast Read Mode is considered an advanced use case,
 *          and not recommended for the beginner.
*/
void SCL3400::setFastReadMode() {
  setFastRead = true;
  beginTransmission(); //Set up this SPI port/bus
  begin(); //Re-init chip
}

/* Stop Fast Read Mode
 * Warning: Using Fast Read Mode in the library works by keeping the
 *          SPI connection continuously open.  This may or may not affect
 *          the behavior of other hardware interactions, depending on the
 *          sketch design.  Fast Read Mode is considered an advanced use case,
 *          and not recommended for the beginner.
*/
void SCL3400::stopFastReadMode() {
  setFastRead = false;
  endTransmission();  //Close connection to SPI port/bus
  begin(); //Re-init chip
}

//Return the calculated X axis tilt angle in degrees
double SCL3400::getCalculatedAngleX() {

  return asin(acceleration(sclData.AccX))*180/PI;
}

//Return the calculated Y axis tilt angle in degrees
double SCL3400::getCalculatedAngleY() {

  return asin(acceleration(sclData.AccY))*180/PI;
}

//Return the calculated X axis offset tilt angle in degrees
double SCL3400::getTiltLevelOffsetAngleX() {
  return angle(sclData.AngX);
}

//Return the calculated Y axis offset tilt angle in degrees
double SCL3400::getTiltLevelOffsetAngleY() {
  return angle(sclData.AngY);
}

//Return the calculated Z axis offset tilt angle in degrees
double SCL3400::getTiltLevelOffsetAngleZ() {
  return angle(sclData.AngZ);
}

//Return the calculated X axis accelerometer value in units of 'g'
double SCL3400::getCalculatedAccelerometerX(void) {
  return acceleration(sclData.AccX);
}

//Return the calculated Y axis accelerometer value in units of 'g'
double SCL3400::getCalculatedAccelerometerY(void) {
  return acceleration(sclData.AccY);
}

//Return value of Error Flag 1 register
uint16_t SCL3400::getErrFlag1(void) {
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk0);
  transfer(RdErrFlg1);
  transfer(RdErrFlg1);
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  //Since we are fetching the Error Flag 1 value, we want to return what we got
  //to the caller, regardless of whether or not there was an error
  //if (crcerr || statuserr) return ((uint16_t)(SCL3400_CMD) & 0xff); //check CRC and RS bits
  return SCL3400_DATA;
}

//Return value of Error Flag 2 register
uint16_t SCL3400::getErrFlag2(void) {
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk0);
  transfer(RdErrFlg2);
  transfer(RdErrFlg2);
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  //Since we are fetching the Error Flag 2 value, we want to return what we got
  //to the caller, regardless of whether or not there was an error
  //if (crcerr || statuserr) return ((uint16_t)(SCL3400_CMD) & 0xff); //check CRC and RS bits
  return SCL3400_DATA;
}

// Read the sensor Serial Number as created by the manufacturer
unsigned long SCL3400::getSerialNumber(void) {
  //Return Device Serial number
  boolean errorflag = false;
  unsigned long serialNum = 0;
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk1);
  if (crcerr || statuserr) errorflag = true;
  transfer(RdSer1);
  if (crcerr || statuserr) errorflag = true;
  transfer(RdSer2);
  serialNum = SCL3400_DATA;
  if (crcerr || statuserr) errorflag = true;
  transfer(SwtchBnk0);
  serialNum = ((unsigned long)SCL3400_DATA << 16) | serialNum;
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  //We wait until now to return an error code
  //In this case we send a 0 since a real serial number will never be 0
  if (crcerr || statuserr || errorflag) return 0;
  return serialNum;
}

// Place the sensor in a Powered Down mode to save power
uint16_t SCL3400::powerDownMode(void) {
  //Software power down of sensor
  if (!setFastRead) beginTransmission(); //Set up this SPI port/bus
  transfer(SwtchBnk0);
  transfer(SetPwrDwn);
  endTransmission(); //Let go of SPI port/bus
  //Since an error is non-zero, we will return 0 if there was no error
  if (crcerr || statuserr) return (uint16_t)(SCL3400_CMD & 0xff); //check CRC and RS bits
  return 0;
}

// Revive the sensor from a power down mode so we can start getting data again
uint16_t SCL3400::WakeMeUp(void) {
  //Software Wake Up of sensor
  beginTransmission(); //Set up this SPI port/bus
  transfer(WakeUp);
  if (!setFastRead) endTransmission(); //Let go of SPI port/bus
  //Since an error is non-zero, we will return 0 if there was no error
  if (crcerr || statuserr) return (uint16_t)(SCL3400_CMD & 0xff); //check CRC and RS bits
  return 0;
}

// Hardware reset of the sensor electronics
uint16_t SCL3400::reset(void) {
  //Software reset of sensor
  //beginTransmission(); //Set up this SPI port/bus
  //transfer(SwtchBnk0);
  //transfer(SWreset);
  //endTransmission(); //Let go of SPI port/bus
  //we have to call begin() to set up the SCL3400 to the same state as before it was reset
  begin(); //Re-init chip
  //Since an error is non-zero, we will return 0 if there was no error
  if (crcerr || statuserr) return (uint16_t)(SCL3400_CMD & 0xff); //check CRC and RS bits
  return 0;
}

// Routine to get temperature in degrees Celsius
double SCL3400::getCalculatedTemperatureCelsius(void) {
  // Return calculated temperature in degrees C
  double Temperature = -273. + (sclData.TEMP / 18.9);
  return Temperature;
}

// Routine to get temperature in degrees Farenheit
double SCL3400::getCalculatedTemperatureFarenheit(void) {
  // Return calculated temperature in degrees F
  double Temperature = -273. + (sclData.TEMP / 18.9);
  Temperature = (Temperature * 9./5.) + 32.;
  return Temperature;
}

//Convert raw angle value to degrees tilt
double SCL3400::angle(int16_t SCL3400_ANG) { //two's complement value expected
  // Return Angle in degrees
  double Angle = (SCL3400_ANG / 16384.) * 90.; // 16384 = 2^14
  return Angle;
}
 
 //Convert raw accelerometer value to g's of acceleration
double SCL3400::acceleration(int16_t SCL3400_ACC) { //two's complement value expected
  // Return acceleration in g
  if (SCL3400_mode == 1) return (double)SCL3400_ACC / 32768.;
  if (SCL3400_mode == 2) return (double)SCL3400_ACC / 16384.;
  return (double)SCL3400_ACC / 16384.; //Default should never be reached
}

//private functions for serial transmission
// Begin SPI bus transmission to the device
void SCL3400::beginTransmission() {
  _spiPort->beginTransaction(spiSettings);
} //beginTransmission

// End SPI bus transmission to the device
void SCL3400::endTransmission() {
  // take the chip/slave select high to de-select:
  digitalWrite(SCL3400_csPin, HIGH);
  _spiPort->endTransaction();
  unsigned long startmillis = millis();
  while (millis() - startmillis < 1) ; //wait a bit
} //endTransmission

//Initialize the Arduino SPI library for the SCL3400 hardware
void SCL3400::initSPI() {
  //Initialize the Arduino SPI library for the SCL3400 hardware
  _spiPort->begin();
  // Maximum SPI frequency is 2 MHz - 4 MHz to achieve the best performance
  // initialize the chip select pin:
  pinMode(SCL3400_csPin, OUTPUT);
  digitalWrite(SCL3400_csPin, HIGH);
  // Data is read and written MSb first.
  // Data is captured on rising edge of clock (CPHA = 0)
  // Data is propagated on the falling edge (MISO line) of the SCK. (CPOL = 0)
}

// The following is taken directly from the Murata SCL3400 datasheet
// Calculate CRC for 24 MSB's of the 32 bit dword
// (8 LSB's are the CRC field and are not included in CRC calculation)
uint8_t SCL3400::CalculateCRC(uint32_t Data)
{
uint8_t BitIndex;
uint8_t BitValue;
uint8_t SCL3400_CRC;

SCL3400_CRC = 0xFF;
for (BitIndex = 31; BitIndex > 7; BitIndex--) {
  BitValue = (uint8_t)((Data >> BitIndex) & 0x01);
  SCL3400_CRC = CRC8(BitValue, SCL3400_CRC);
}
SCL3400_CRC = (uint8_t)~SCL3400_CRC;
return SCL3400_CRC;
}
uint8_t SCL3400::CRC8(uint8_t BitValue, uint8_t SCL3400_CRC)
{
  uint8_t Temp;
  Temp = (uint8_t)(SCL3400_CRC & 0x80);
  if (BitValue == 0x01) {
    Temp ^= 0x80;
  }
  SCL3400_CRC <<= 1;
  if (Temp > 0) {
    SCL3400_CRC ^= 0x1D;
  }
  return SCL3400_CRC;
}

// Routine to transfer a 32-bit integer to the SCL3400, and return the 32-bit data read
unsigned long SCL3400::transfer(unsigned long value) {
  FourByte dataorig;
  unsigned long startmicros;
  
  dataorig.bit32 = value; //Allow 32 bit value to be sent 8 bits at a time
  #ifdef debug_SCL3400
  Serial_SCL.print(dataorig.bit32, HEX);
  Serial_SCL.print(" ");
  for (int j = 3; j >= 0; j--) {
    Serial_SCL.print(dataorig.bit8[j], HEX);
    Serial_SCL.print(" ");
  }
  #endif
  //Must allow at least 10 uSec between SPI transfers
  //The datasheet shows the CS line must be high during this time
  if (!setFastRead) startmicros = micros();
  //while ((micros() - startmicros < 10) && (micros() > 10)) ;
  if (!setFastRead) while ((micros() - startmicros < 10)) ;
  
  digitalWrite(SCL3400_csPin, LOW); //Now chip select can be enabled for the full 32 bit xfer
  SCL3400_DATA = 0;
  for (int i = 3; i >= 0; i--) { //Xfers are done MSB first
    dataorig.bit8[i] = _spiPort->transfer(dataorig.bit8[i]);
  }
  SCL3400_DATA = dataorig.bit8[1] + (dataorig.bit8[2] << 8);
  SCL3400_CRC = dataorig.bit8[0];
  SCL3400_CMD = dataorig.bit8[3];
  digitalWrite(SCL3400_csPin, HIGH); //And we are done
  #ifdef debug_SCL3400
  for (int i = 3; i >= 0; i--) {
    Serial_SCL.print(" ");
    Serial_SCL.print(dataorig.bit8[i], HEX);
  }
  Serial_SCL.print("  ");
  #endif
  if (SCL3400_CRC == CalculateCRC(dataorig.bit32))
    crcerr = false;
  else
    crcerr = true;
  //check RS bits
  if ((SCL3400_CMD & 0x03) == 0x01)
    {statuserr = false;
      //Serial.printf("\n------%d-----\n",SCL3400_CMD);
      }
  else
    {statuserr = true;}
  #ifdef debug_SCL3400
  Serial_SCL.print((SCL3400_CMD & 0x03));
  Serial_SCL.print(" ");
  Serial_SCL.print(SCL3400_DATA, HEX);
  Serial_SCL.print(" ");
  Serial_SCL.print(SCL3400_CRC, HEX);
  Serial_SCL.print(" ");
  Serial_SCL.print(CalculateCRC(dataorig.bit32), HEX);
  Serial_SCL.print(" ");
  Serial_SCL.println(crcerr);
  #endif
  return dataorig.bit32;
}
