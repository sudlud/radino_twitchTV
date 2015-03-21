// Init LEDs
void LED_init_all()
{
  pinMode(GRONKH_RED, OUTPUT);
  digitalWrite(GRONKH_RED, LOW);
  pinMode(GRONKH_GREEN, OUTPUT);
  digitalWrite(GRONKH_GREEN, LOW);
  pinMode(GPIPOKER_RED, OUTPUT);
  digitalWrite(GPIPOKER_RED, LOW);
  pinMode(GPIPOKER_GREEN, OUTPUT);
  digitalWrite(GPIPOKER_GREEN, LOW);
}

// Wifi setup, lua-based code for NodeMCU firmware
void doWifiSetup()
{
  espUART.println("wifi.setmode(wifi.STATION)"); // Station mode
  delay(500);
  espUART.print("wifi.sta.config(\"");// Set SSID & PW
  espUART.print(SSID);
  espUART.print("\",\"");
  espUART.print(PW);
  espUART.println("\")"); 
  delay(500);
  
  espUART.println("wifi.sta.autoconnect(1)"); // Enable auto-connecting
  delay(500);
}

// Init SPI-UART which connects ESP with Arduino chip
void ESP_init_all()
{
  espUART.init();  // Init SPI-UART-Module
  espUART.setINT_None(); // Disable interrupts here
  espUART.enableSleeping();  // Sleep when idle (UART-chip only)
  espUART.setBaudrate(ESP8266_BAUDRATE);  // Set Baudrate of SPI-UART-Module
  
  // Put ESP8266 into normal operation mode
  pinMode(CH_PD, OUTPUT);  // Chip-Enable of ESP8266
  digitalWrite(CH_PD, HIGH);  // HIGH
 
  // Perform RESET of ESP8266
  pinMode(CH_RST,OUTPUT);
  digitalWrite(CH_RST, LOW);  // Pull RESET of WIFI-Chip
  delay(200);
  digitalWrite(CH_RST, HIGH);  // Disable RESET of WIFI-Chip
  delay(250);
  	
  espUART.flush(); // Clear buffers
  espUART.attachInterruptCallback(newCharReceived); // Connect "newCharReceived" with ESP-UART interrupt
  espUART.setINT_RXlvl(); // Enable RXlvl interrupt
}

// Put ESP to sleep for a specified amount of time
void ESP_sleep_ms(unsigned short ms)
{
  espUART.println("node.dsleep(1)"); // Send to sleep, will never wake up alone
  
  delay(ms); // Wait / "Sleep"
  
  // Wake up again
  digitalWrite(CH_RST, LOW);  // Pull RESET of WIFI-Chip
  delay(200);
  digitalWrite(CH_RST, HIGH);  // Disable RESET of WIFI-Chip
}





