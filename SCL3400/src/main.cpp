

/* Read Tilt angles from Murata SCL3400 Inclinometer

*/

#include <SPI.h>
#include <SCL3400.h>

SCL3400 inclinometer;
//Default SPI chip/slave select pin is D10
/*
// Need the following define for SAMD processors
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif
*/
void setup() {
  Serial.begin(115200);
  delay(2000); //SAMD boards may need a long time to init SerialUSB
  Serial.println("Reading basic Tilt Level Offset values from SCL3400 Inclinometer");

  if (inclinometer.begin(5) == false) {
    Serial.println("Murata SCL3400 inclinometer not connected.");
    while(1); //Freeze
  }
}

void loop() {
  if (inclinometer.available()) { //Get next block of data from sensor

    Serial.print("X Tilt: ");
    Serial.print(inclinometer.getCalculatedAccelerometerX());
    Serial.print("\t");
    Serial.print("Y Tilt: ");
    Serial.print(inclinometer.getCalculatedAccelerometerY());

    Serial.print("\n");
    Serial.print("X DEG:");
    Serial.print(inclinometer.getCalculatedAngleX());
    Serial.print("\t");
    Serial.print("Y DEG:");
    Serial.print(inclinometer.getCalculatedAngleY());
    Serial.print("\n");
    delay(1000); //Allow a little time to see the output
  } else inclinometer.reset();

  delay(100);
}
