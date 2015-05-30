#define BLUETOOTH_SPEED 38400

#include <SoftwareSerial.h>
#include <Servo.h>

// robot code

// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
SoftwareSerial mySerial(5, 6); // RX, TX

Servo servoLeft;
Servo servoRight;

int leftServoVal;
int rightServoVal;

/*
  The posible baudrates are:
    AT+BAUD1-------1200
    AT+BAUD2-------2400
    AT+BAUD3-------4800
    AT+BAUD4-------9600 - Default for hc-06
    AT+BAUD5------19200
    AT+BAUD6------38400
    AT+BAUD7------57600
    AT+BAUD8-----115200
    AT+BAUD9-----230400
    AT+BAUDA-----460800
    AT+BAUDB-----921600
    AT+BAUDC----1382400
*/

void setup()
{
  servoLeft.attach(12);
  servoRight.attach(13);

  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  
  Serial.begin(9600);

  Serial.println("Starting config");
  mySerial.begin(BLUETOOTH_SPEED);
  delay(1000);

  // Should respond with OK
  Serial.println("Sending AT");
  mySerial.print("AT\r\n");
  waitForResponse();

  // Should respond with its version
  Serial.println("Sending VERSION");
  mySerial.print("AT+VERSION\r\n");
  waitForResponse();

  // Reset to factory default
  Serial.println("Sending ORGL");
  mySerial.print("AT+ORGL\r\n");
  waitForResponse();

  // Get the address for this thing
  Serial.println("Sending ADDR query");
  mySerial.print("AT+ADDR\r\n");
  waitForResponse();
  
  // next we initialize the SPP profile lib, whatever that means
  Serial.println("Sending INIT");
  Serial.println("If it returns error 17, that's ok, it just means the thing's already initialized.");
  mySerial.print("AT+INIT\r\n");
  waitForResponse();
  
  // now we put the device in pairable state
  Serial.println("Sending INQ");
  mySerial.print("AT+INQ\r\n");
  waitForResponse();
  
  // set the device as master (master=1 slave=0 some other shit = 2)
  Serial.println("Sending ROLE=0");
  mySerial.print("AT+ROLE=1\r\n");
  waitForResponse();
  
  Serial.println("Done with setup!");
  Serial.println("Delaying.");
  
  delay(3000);
  Serial.println("Trying to connect.");
  Serial.println("BIND");
  mySerial.print("AT+BIND=98d3,31,50320b\r\n");
  waitForResponse();
  
  Serial.println("PAIR");
  mySerial.print("AT+PAIR=98d3,31,50320b\r\n");
  waitForResponse();
  
  delay(2000);
  Serial.println("Failsafe try.");
  Serial.println("BIND");
  mySerial.print("AT+BIND=98d3,31,50320b\r\n");
  waitForResponse();
  
  Serial.println("PAIR");
  mySerial.print("AT+PAIR=98d3,31,50320b\r\n");
  waitForResponse();
  
  delay(2000);
  Serial.println("Failsafe try.");
  Serial.println("BIND");
  mySerial.print("AT+BIND=98d3,31,50320b\r\n");
  waitForResponse();
  
  Serial.println("PAIR");
  mySerial.print("AT+PAIR=98d3,31,50320b\r\n");
  waitForResponse();
}  

void waitForResponse() {
    delay(1000);
    while (mySerial.available()) {
      Serial.write(mySerial.read());
    }
    Serial.write("\n");
}

void loop() {
    // this'll give us the left servo value, in two bytes
    // least significant bits
    while (mySerial.available() == 0);
    leftServoVal = mySerial.read();

    // most significant bits
    while (mySerial.available() == 0);
    leftServoVal += mySerial.read() << 8;

    // least sig
    while (mySerial.available() == 0);
    rightServoVal = mySerial.read();

    // most sig
    while (mySerial.available() == 0);
    rightServoVal += mySerial.read() << 8;

    // set the servos
    servoLeft.writeMicroseconds(leftServoVal);
    servoRight.writeMicroseconds(rightServoVal);
}
