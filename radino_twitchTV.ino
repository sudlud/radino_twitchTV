/*
  Twitch TV Status display on radino Wifi
  -> www.radino.cc or www.amazon.de -> "radino Wifi"
  
  https://youtu.be/PB9DiFt-3Hs -> short video to demonstrate this code
  
  Requirements: 
  - Arduino 1.6.x recommended
  - Download latest radino Suport packet for Arduino from http://wiki.in-circuit.de/index.php5?title=radino_WiFi -> ICT_Boards.zip
  - the ESP8266EX MUST run the NodeMCU firmware, build 20150126 (so you have to flash this into the ESp8266EX on your radino Wifi)
  -> other build will NOT work, because of SSL issues :o
  
  - ESP-Firmware: https://github.com/nodemcu/nodemcu-firmware
  - direct link to firmware-file: https://github.com/nodemcu/nodemcu-firmware/blob/master/pre_build/0.9.5/nodemcu_20150126.bin
  
  written by Ludwig K.; Germany; 21-03-2015
*/

// Put your home Wifi SSID & PW here
#define SSID "xxxxx"
#define PW   "xxxxx"

// Use SPI-library for communication with SC16IS750
#include <SPI.h>

// Include definitions and functions for SC16IS750
// SPI-to-UART-chip
#include "SPI_UART.h"

SPI_UART espUART = SPI_UART();

// Pin definitions of ESP8266
//       Pin-Name   Arduino-Pin-Number
#define  CH_PD      A4
#define  CH_RST     4

// Define Serial port that should be used for communication
// Valid values:
// USB-UART:       Serial
// Hardware-UART:  Serial1
#define progSerial Serial1

// Set baudrate of SPI-UART chip to communicate with ESP8266
#define ESP8266_BAUDRATE  9600

// Stream states
enum STREAM_STATUS{
  STREAM_UNCLEAR = 0,
  STREAM_OFFLINE,
  STREAM_ONLINE,
  };
unsigned char streamStatus = STREAM_UNCLEAR;

// Stream-Parser states
enum STREAM_PARSER{
  STREAM_IDLE = 0,
  STREAM_WAITFOR_S,
  STREAM_WAITFOR_T,
  STREAM_WAITFOR_R,
  STREAM_WAITFOR_E,
  STREAM_WAITFOR_A,
  STREAM_WAITFOR_M,
  STREAM_WAITFOR_II,
  STREAM_WAITFOR_POINTS,
  STREAM_DECIDE_STATUS_NOW,
  };
unsigned char streamParserStatus = STREAM_IDLE;

// LEDs
#define  GRONKH_RED      6
#define  GRONKH_GREEN    10   
#define  GPIPOKER_RED    12
#define  GPIPOKER_GREEN  3

#define DO_MAIN_WIFI_SETUP 1 // Define if main wifi setup (SSID, PW etc.) should be run on every startup

void setup()
{
  progSerial.begin(57600);  // Start serial port at baudrate 57600
  delay(100);  
  progSerial.println("Hello");  // Print a welcome message
  
  LED_init_all(); // Init LEDs
  
  ESP_init_all(); // Init ESP
  
  // Primary Setup finished //
  
  espUART.println("node.restart()"); // Restart NodeMCU on ESP
  delay(500);
  
#if DO_MAIN_WIFI_SETUP != 0 // Setup SSID & PW etc. -> ESP will remember settings even after a restart
  progSerial.println("Wifi-Setup...");
  doWifiSetup();
#endif   

  delay(2000);
}

void loop()
{ 
  // Request twitch.tv/gronkh status
  progSerial.println("Requesting status for \"gronkh\"");
  switch(getTwitchChannelStatus("gronkh")){
    case STREAM_ONLINE:   
      progSerial.println("online");
      digitalWrite(GRONKH_GREEN, HIGH);
      digitalWrite(GRONKH_RED, LOW);
      break;
    case STREAM_OFFLINE:  
      progSerial.println("offline");
      digitalWrite(GRONKH_GREEN, LOW);
      digitalWrite(GRONKH_RED, HIGH); 
      break;
    case STREAM_UNCLEAR:
    default:
      progSerial.println("Error");
      digitalWrite(GRONKH_GREEN, LOW);
      digitalWrite(GRONKH_RED, LOW);
  }
  delay(2000);
  
  // request twitch.tv/GPIPOKER status  
  progSerial.println("Requesting status for \"GPIPOKER\"");
  switch(getTwitchChannelStatus("GPIPOKER")){
      case STREAM_ONLINE:   
        progSerial.println("online");
        digitalWrite(GPIPOKER_GREEN, HIGH);
        digitalWrite(GPIPOKER_RED, LOW);
        break;
      case STREAM_OFFLINE:  
        progSerial.println("offline");
        digitalWrite(GPIPOKER_GREEN, LOW);
        digitalWrite(GPIPOKER_RED, HIGH); 
        break;
      case STREAM_UNCLEAR:
      default:
        progSerial.println("Error");
        digitalWrite(GPIPOKER_GREEN, LOW);
        digitalWrite(GPIPOKER_RED, LOW);
    }
  delay(2000);
  
//  // Let ESP sleep -> powersaving  
//  progSerial.println("Put ESP to sleep...");
//  espUART.setINT_None(); // Disable UART interrupts here
//  ESP_sleep_ms(10000); // ESP sleeps for 10 seconds
//  delay(500);
//  espUART.flush(); // Clear rubbish UART data from esp reboot
//  espUART.setINT_RXlvl(); // Enable RXlvl interrupt
//  progSerial.println("ESP awake again.");
//  delay(500);
  
}



