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

    Serial.println("Starting config -- robot");
    mySerial.begin(38400);
    delay(1000);

    // Should respond with OK
    Serial.println("Sending AT");
    mySerial.print("AT\r\n");
    waitForResponse();

    // Get the address for this thing so we can hardcode it into one of the
    // master controllers
    Serial.println("Sending ADDR query");
    mySerial.print("AT+ADDR\r\n");
    waitForResponse();

    // set the device as slave
    Serial.println("Sending ROLE=0");
    mySerial.print("AT+ROLE=0\r\n");
    waitForResponse();

    // clear bindings so that a controller can bind to the device
    Serial.println("BIND");
    mySerial.print("AT+BIND=98d3,31,50320b\r\n");
    waitForResponse();

    Serial.println("Done with setup!");
    Serial.println("Delaying 7 seconds.");

    delay(7000);
}

void waitForResponse() {
    delay(1000);
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
    Serial.write("\n");
}

void loop() {
    Serial.println("Top of the loop! Querying pair name.");
    delay(500);
    waitForResponse();
//    // this'll give us the left servo value, in two bytes
//    // least significant bits
//    while (mySerial.available() == 0);
//    leftServoVal = mySerial.read();
//
//    // most significant bits
//    while (mySerial.available() == 0);
//    leftServoVal += mySerial.read() << 8;
//
//    // least sig
//    while (mySerial.available() == 0);
//    rightServoVal = mySerial.read();
//
//    // most sig
//    while (mySerial.available() == 0);
//    rightServoVal += mySerial.read() << 8;
//
//    // set the servos
//    servoLeft.writeMicroseconds(leftServoVal);
//    servoRight.writeMicroseconds(rightServoVal);
}
