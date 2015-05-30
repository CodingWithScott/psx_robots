#include<SoftwareSerial.h>
#define RxD 0
#define TxD 1

SoftwareSerial BT(RxD,TxD);

void setup() {
	Serial.begin(9600);
	BT.begin(38400);
	delay(1000);
	initialize_bluetooth();

	/* connecting to the bluetooth device with the 
	   address "00:12:11:17:02:10" */ 
	/*Note that this command cannot be issued unless the module is in pairable state,
	in other words, the "AT+INIT" then the "AT+INQ" commands should be first issued in order.
	It worth mentioning that this is done 
	in the initialization function*/
	BT.print("AT+LINK=0012,11,170210\r\n"); //This command used only in one of the two devices
	/*Note that the address of the device which we want to connect to can be got using the 
	"AT+INQ" command. Then the response string will be on the form: "+INQ:00:12:11:17:02:10,xx,yy". 
	The address string can be then extracted from the response string by some simple code 
	manipulation and then used in the "AT+LINK" command to establish the connection. For more 
	information about string operations in Arduino, please consult the Arduino reference.*/

	delay(1000);
}

/* ---------------------------------------------------------
  Simple code for the master sending a message to the slave
  ---------------------------------------------------------- */
void loop() {
	/* Transmitting data to slave */
	BT.print("Hello from Emar to RAM!\r\n");
	BT.flush();   //this makes the code stops untill all serial data finish transmission
	delay(500);
	//waiting for the slave response and print the response on the serial monitor
	while(1)
	{
		if(BT.available()>0) {
			print_bt_response();
			break;
		}
	}

	/* Responding back to master */
	BT.print("Hello from RAM to Emar!\n");
	BT.flush();
	delay(500);

}

/*-----------------------------------------------------------------------
   Function that reads BT received data and returns 1 for success, 0 fail 
  ----------------------------------------------------------------------- */
int print_bt_response(void) {
	int response;
	char out;
	char outprev = '$';
	while (BT.available() > 0) {
		out = (char)BT.read();
		Serial.print(out);
		if ((outprev == 'O') && (out == 'K')) {
			Serial.print("\n");
			response = 1;
			return response;	// success!!
		}
		outprev = out;
	}
	response = 0;

	return response;			// fail... :(
}

/*--------------------------------------------------------------
  Initializing the bluetooth and set it into the pairable state
 --------------------------------------------------------------- */
void initialize_bluetooth() {
	int flag = 2;
	delay(500);

	/* Restore default status */
	BT.print("AT+ORGL\r\n");
	flag = print_bt_response();
	delay(500);
	if (flag == 1)
		Serial.print("Module default settings restored ... Success(1)\n\n");
	else if(flag == 0)
		Serial.print("Failed(1)\n\n");
	flag = 2;

	/* Set BT name as "Emar" */
	BT.print("AT+NAME=Emar\r\n");
	flag = print_bt_response();
	delay(500);
	if (flag == 1)
		Serial.print("Module took new name ... Success(2)\n\n");
	else if(flag == 0)
		Serial.print("Failed(2)\n\n");
	flag = 2;

	/* Set pin code (same for master/slave) for establishing connection */
	BT.print("AT+PSWD=1234\r\n");
	flag = print_bt_response();
	delay(500);
	if (flag == 1)
		Serial.print("Module took new password ... Success(3)\n\n");
	else if (flag == 0)
		Serial.print("Failed(3)\n\n");
	flag = 2;

	delay(1000);

	/* Init spp profile lib (should be issued before issuing the next command) */
	BT.print("AT+INIT\r\n");
	flag = print_bt_response();
	delay(1000);
	BT.print("AT+INIT\r\n");
	flag = print_bt_response();
	delay(1000);

	if (flag == 1)
		Serial.print("Module spp profile lib initialized ... Success(4)\n\n");
	else if (flag == 0)
		Serial.print("Failed(4)\n\n");
	flag = 2;

	/* Set to pairable state */
	BT.print("AT+INQ\r\n");
	flag = print_bt_response();
	delay(500);
	if (flag == 1)
		Serial.print("Module inquirable (set to pairable state) ... Success(5)\n\n");
	else if (flag == 0)
		Serial.print("Failed(5)\n\n");
	flag = 2;

	/* Set BT to be master --> then "AT+ROLE=1" 
	   set BT to be slave  --> then "AT+ROLE=0" */
	BT.print("AT+ROLE=1\r\n");      // Set as master here
	flag = print_bt_response();
	delay(500);
	if (flag == 1)
		Serial.print("Module role set to master ... Success(6)\n\n");
	else if (flag == 0)
		Serial.print("Failed(6)\n\n");
	flag == 2;
}

/* Get Bluetooth module address */
void get_device_name(void) {
	int ix;
	const int RESPONSE_LEN = 26; 
	char command[12] = "AT+ADDR?\r\n";
	/* Ex: Module Bluetooth address of 12:34:56:ab:cd:ef will give  
	   response: "+ADDR:1234:56:abcdef\r\nOK"    */
	char response[26] = ""; 
	
	/* Send cmd from computer to device over Bluetooth */
	BT.print(command);	
	
	printf("Querying Bluetooth device address...\n");
	for (ix = 0; ix < RESPONSE_LEN; ix++) {
		/* Read command back from Bluetooth, print to computer over serial */
		Serial.print(BT.read()); 
		delay(500);
	}
} 