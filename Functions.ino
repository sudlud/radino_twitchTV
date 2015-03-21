#define LINE_DELAY 700

// This function actually reads the twitch channel status from api.twitch.tv via HTTPS / SSL
unsigned char getTwitchChannelStatus(String channel)
{
  streamStatus = STREAM_UNCLEAR; // Reset Stream status
  streamParserStatus = STREAM_IDLE; // Reset parser state
  
  // Send request to Twitch API
  espUART.flush();
  espUART.println("sk=net.createConnection(net.TCP, 1)"); // SSL connection
  delay(LINE_DELAY);
  espUART.println("sk:on(\"receive\", function(sck, c) print(c) end )"); // enable print for received data
  delay(LINE_DELAY);
  espUART.println("sk:on(\"connection\", function(sck)"); // When conn. is open, send HTTP GET-request
  delay(LINE_DELAY);
  espUART.println("print(\"connected\")");
  delay(LINE_DELAY);
  espUART.print("sck:send(\"GET /kraken/streams/");
  espUART.print(channel);
  espUART.print(" HTTP/1.1\\r\\n");
  delay(LINE_DELAY);
  espUART.print("Host: api.twitch.tv\\r\\nConnection: keep-alive\\r\\n");
  delay(LINE_DELAY);
  espUART.println("Accept: application/vnd.twitchtv.v3+json\\r\\n\\r\\n\") end )");
  delay(LINE_DELAY);     
    
  espUART.println("sk:connect(443,\"api.twitch.tv\")");
  
  unsigned char waitCount = 30; // max. wait time
  
  while((streamStatus == STREAM_UNCLEAR) && waitCount) // Wait for parser to receive response
  {
    delay(250); 
    waitCount--;
  }

  delay(2000);
  
  espUART.println("sk:close()"); // close socket
  delay(500);
  espUART.println("sk = nil"); // delete object
  delay(500);
  
  return streamStatus;
}

// Interrupt handler for new characters received from ESP
unsigned char newCharReceived(unsigned char val)
{
  for(unsigned char nmbr = espUART.available(); nmbr>0; nmbr--)
  {
    unsigned char recv = espUART.read();
    //progSerial.write(recv); // Debug output of all characters
    
    switch(streamParserStatus){
      case STREAM_IDLE:
        if(recv == '"') streamParserStatus = STREAM_WAITFOR_S;
      break;
      
      case STREAM_WAITFOR_S:
        if(recv == 's') streamParserStatus = STREAM_WAITFOR_T;
          else streamParserStatus = STREAM_IDLE;        
      break;
      
      case STREAM_WAITFOR_T:
        if(recv == 't') streamParserStatus = STREAM_WAITFOR_R;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_R:
        if(recv == 'r') streamParserStatus = STREAM_WAITFOR_E;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_E:
        if(recv == 'e') streamParserStatus = STREAM_WAITFOR_A;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_A:
        if(recv == 'a') streamParserStatus = STREAM_WAITFOR_M;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_M:
        if(recv == 'm') streamParserStatus = STREAM_WAITFOR_II;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_II:
        if(recv == '"') streamParserStatus = STREAM_WAITFOR_POINTS;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_WAITFOR_POINTS:
        if(recv == ':') streamParserStatus = STREAM_DECIDE_STATUS_NOW;
          else streamParserStatus = STREAM_IDLE;
      break;
      
      case STREAM_DECIDE_STATUS_NOW:
        if(recv == 'n') {
           streamStatus = STREAM_OFFLINE;
        }  else {
          streamStatus = STREAM_ONLINE;
        }
        streamParserStatus = STREAM_IDLE;
      break;
      default:
        streamParserStatus = STREAM_IDLE;
      break;
    }
  }
  return 1;
}
