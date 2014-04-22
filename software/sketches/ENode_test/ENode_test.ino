#include <EEPROM.h>
#include <Wire.h>
#include "nanosat_message.h"

#define I2C_ADD_ARD1            0x2B    // experiment arduino 1
#define I2C_ADD_ASSV_1          0x04     // master arduino

int led = 9;
nanosat_message_t msg;

// the setup routine runs once when you press reset:
void setup() {
  Wire.begin(I2C_ADD_ARD1);

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  Serial.begin(9600);  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("HIGH");
  delay(100);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("LOW");
  delay(1900);               // wait for a second
  

  msg.node_addr = I2C_ADD_ARD1;
  msg.prefix    = NODE_COMM_MESSAGE_PREFIX;
  msg.len       = NODE_COMM_MAX_BUFFER_SIZE;
  msg.type      = APPEND;
  for (uint8_t i=0; i<NODE_COMM_MAX_BUFFER_SIZE; i++)
    msg.buf[i] = i;
  
  Wire.beginTransmission(I2C_ADD_ASSV_1);
  uint8_t * array = (uint8_t *)&msg;
  for (int i=0; i<sizeof(nanosat_message_t); i++)
    Wire.write(array[i]);
  byte t_ret = Wire.endTransmission();
}
