#include <Psx.h>
#include <SoftwareSerial.h>

// constants for bluetooth and controller params
int FUDGE_LOWER = 95;
int FUDGE_UPPER = 160;
int BT_DELAY = 500;

// constants for interfacing with the controller
int dataPin = 8;
int cmndPin = 9;
int attPin = 11;
int clockPin = 10;

unsigned int RStick  = 0;
unsigned int RStickX = 0;
unsigned int RStickY = 0;

unsigned int LStick  = 0;
unsigned int LStickX = 0;
unsigned int LStickY = 0;

int leftServoVal = 0;
int rightServoVal = 0;

//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
SoftwareSerial mySerial(5, 6); // RX, TX

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

Psx PsxObj;

void setup()
{
    Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10);

    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);

    Serial.begin(9600);

    Serial.println("Starting config -- controller");
    mySerial.begin(38400);
    delay(1000);

    // Should respond with OK
    Serial.println("Sending AT");
    mySerial.print("AT\r\n");
    waitForResponse();
    
    Serial.println("Setting device name to psxone");
    mySerial.print("AT+NAME=psxone\r\n");
    waitForResponse();
    
    Serial.println("Querying device name");
    mySerial.print("AT+NAME?\r\n");
    waitForResponse();

    // set the device as master (0=slave 1=master 2=some other shit)
    Serial.println("Sending ROLE=1");
    mySerial.print("AT+ROLE=1\r\n");
    waitForResponse();

    Serial.println("Done with setup!");
    Serial.println("Delaying.");

    delay(3000);
    Serial.println("Trying to connect.");
    Serial.println("BIND");
    mySerial.print("AT+BIND=98d3,31,6014a5\r\n");
    waitForResponse();

    Serial.println("Delaying five seconds.");
    delay(5000);

    Serial.println("Done!");
    rightServoVal = 1300;
    leftServoVal = 1300;
}

void waitForResponse() {
    delay(1000);
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
    Serial.write("\n");
}

void write_line(char byteval) {
    char message[2];
    message[0] = byteval;
    message[1] = '\n';
    mySerial.write(message, 2);
}

void loop() {
  /* Sending 10000000 should dislay INT_MIN */
  /*write_line(1);
  int ix;
  for (ix = 0; ix < 7; ix++) {
    write_line(0);
    delay(100);  
  }  

  delay(1000);*/
    Serial.println("About to read stick values.");
    LStick = PsxObj.readLStick();
    Serial.println("Left stick read.");
    RStick = PsxObj.readRStick();
    Serial.println("Stick values read.");
    LStickX = LStick >> 8;
    LStickY = LStick & 0xFF;

    RStickX = RStick >> 8;
    RStickY = RStick & 0xFF;

    if ((FUDGE_LOWER < LStickY) && (LStickY < FUDGE_UPPER)) {
        leftServoVal = 1500;
    } else {
        leftServoVal = 1245 + (2*LStickY);
    }
  
    if ((FUDGE_LOWER < RStickY) && (RStickY < FUDGE_UPPER)) {
        rightServoVal = 1500;
    } else {
        rightServoVal = 1755 - (2*RStickY);
    }

    // sending the data
    // order: LSBytes, MSBytes
    // for example, 1500 = bx00000101 11011100 // so to send 1500 we send:
    // 11011100 followed by 00000101
    Serial.println("Sending left servo values.");
    delay(BT_DELAY);
    write_line(leftServoVal >> 8);
    delay(BT_DELAY);
    write_line(leftServoVal & 0xFF);

    Serial.println("Sending right servo values.");
    delay(BT_DELAY);
    write_line(rightServoVal >> 8);
    delay(BT_DELAY);
    write_line(rightServoVal & 0xFF);
}

