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

void waitForResponse() {
    delay(1000);
    char tempchar;
    //int tempint;
    while (mySerial.available()) {
        tempchar = mySerial.read();
        Serial.print(tempchar);
        //tempint = tempchar;
        //Serial.print(tempint);
        //Serial.print(" ");
    }
    //Serial.print("\n");
}

char getFirstByteOfLineFromBT() {
    char retval;
    char nextbyte;
    int temp;
    Serial.println("Waiting for serial to have contents.");
    while (mySerial.available() == 0) {}
    Serial.println("Reading line from BT:");
    retval = mySerial.read();
    temp = retval;
    Serial.print(retval);
    // skip subsequent characters until we hit \n (ascii 10)
    
    nextbyte = mySerial.read();
    while (nextbyte != 10) {
        temp = nextbyte;
        Serial.print(nextbyte);
        Serial.print(" ");
        while (mySerial.available() == 0) {}
        nextbyte = mySerial.read();
    }
    Serial.print("\n");
    return retval;
}

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
    Serial.println("Delaying 8 seconds.");

    delay(8000);
    
    Serial.println("Waiting until we're through all the AT+ commands");
    
    // this code would be at the top of the loop if we didn't have any garbage messages
    // at the start of the connection, but we do, so 
    leftServoVal = 65;
    
    while (leftServoVal == 65) {
        leftServoVal = getFirstByteOfLineFromBT();
    }
    
    Serial.println("Done skipping.");
}

void loop() {
    //Serial.println("Top of the loop! Waiting half a second & listening for a message.");
    //waitForResponse();
    //delay(500);
    //waitForResponse();
    // this'll give us the left servo value, in two bytes

    Serial.println("Top of the loop.");
    // most significant bits
    while (mySerial.available() == 0) {};
    leftServoVal += getFirstByteOfLineFromBT() << 8;

    // least sig
    while (mySerial.available() == 0) {};
    rightServoVal = getFirstByteOfLineFromBT();

    // most sig
    while (mySerial.available() == 0) {};
    rightServoVal += getFirstByteOfLineFromBT() << 8;

    Serial.println("Left, then right");
    Serial.println(leftServoVal);
    Serial.println(rightServoVal);
    // set the servos
    //servoLeft.writeMicroseconds(leftServoVal);
    //servoRight.writeMicroseconds(rightServoVal);
    
    // least significant bits
    while (mySerial.available() == 0) {};
    leftServoVal = getFirstByteOfLineFromBT();
}
