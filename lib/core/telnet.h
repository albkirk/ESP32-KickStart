//Variables used by telnet
#define BUFFER_PRINT 150						    // length of buffer
#define MAX_TIME_INACTIVE 30000				// Maximun time for inactivity (miliseconds)

unsigned long TELNET_Timer = millis();	// To measure Telnet idle time
String bufferPrint = "";						    // buffer for printable text

// declare telnet server and client (do NOT put in setup())
WiFiServer telnetServer(23);
WiFiClient telnetClient;
WiFiClient newClient;

void telnet_setup() {
	   //start Telnet service
	   telnetServer.begin();
	   telnetServer.setNoDelay(true);
	   Serial.println("Please, Open Telnet Client.");
	   bufferPrint.reserve(BUFFER_PRINT);
}

void telnet_loop() {
		blink_LED(3);
	   //check if there are any new clients
    if (telnetServer.hasClient()){
        if (telnetClient && telnetClient.connected()) {
            // Verify if the IP is same than actual conection
            newClient = telnetServer.available();
            if (newClient.remoteIP() == telnetClient.remoteIP() ) {
                // Reconnect
                telnetClient.stop();
                telnetClient = newClient;
            }
            else {
                // Disconnect (not allow more than one connection)
                newClient.stop();
                return;
            }
            if ((millis() - TELNET_Timer) > MAX_TIME_INACTIVE) {
                telnetClient.println("Closing Telnet session by inactivity");
                telnetClient.stop();
            }
        }
        else {
            // New TCP client
            telnetClient = telnetServer.available();
            telnetClient.setNoDelay(true); // More faster
            telnetClient.flush();  // clear input buffer, else you get strange characters
            TELNET_Timer = millis(); // initiate timer for inactivity
            Serial.println("New telnet client!");
        }
    }
    yield();
}


void telnet_print(String msg) {
		Serial.print(msg);
		if (config.TELNET && telnetClient && telnetClient.connected()) {  // send data to Client
				if (bufferPrint == "") {
						bufferPrint=msg;
						telnetClient.print(bufferPrint);
						telnetClient.flush();  // clear input buffer, else you get strange characters
						bufferPrint="";
						delay(10);  // to avoid strange characters left in buffer
				}
				else Serial.println("Buffer not empty");
		}
}

void telnet_println(String msg) {
		Serial.println(msg);
		if (config.TELNET && telnetClient && telnetClient.connected()) {  // send data to Client
				if (bufferPrint == "") {
						bufferPrint=msg;
						telnetClient.println(bufferPrint);
						telnetClient.flush();  // clear input buffer, else you get strange characters
						bufferPrint="";
						delay(10);  // to avoid strange characters left in buffer
				}
				else Serial.println("Buffer not empty");
		}
}

void telnet_stop() {
    // Stop Client
    if (telnetClient && telnetClient.connected()) {
        telnetClient.stop();
    }
    // Stop server
    telnetServer.stop();
}
