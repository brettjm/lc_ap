#include <WiFi101.h>
#include <WifiUdp.h>

#define SERIAL_MESSAGE_SIZE 4
#define SERIAL_TERMINATE_CHAR '\r'
#define SERIAL_BUFFER_SIZE 64
#define SERIAL_BAUD_RATE 9600
#define SERVER_PORT 80

/** WPA/WPA2 security information for Wi-Fi connection */
#define MAIN_WLAN_SSID "atmelwifi" /* < Destination SSID */
#define MAIN_WLAN_AUTH M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define MAIN_WLAN_PSK "1234567890" /* < Password for Destination SSID */

char ssid[] = MAIN_WLAN_SSID;  // Create AP name
char pass[] = MAIN_WLAN_PSK;   // AP password for WPA security

char serialBuffer[SERIAL_BUFFER_SIZE];

int status = WL_IDLE_STATUS;
WiFiServer server(SERVER_PORT);

WiFiUDP Udp;

void setup() 
{
  WiFi.setPins(8,7,4);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    while (true);  // Freeze system
  }

  status = WiFi.beginAP(ssid, pass);

  if (status != WL_AP_LISTENING) 
  {
    Serial.println("Creating AP failed");
    while (true);  // Freeze
  }

  server.begin();
  printWiFiStatus();  

  // if you get a connection, report back via serial:
  Serial.println("\nStarting UDP connection...");
  Udp.begin(SERVER_PORT);
}

void printWiFiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

bool readSerial()
{
  for (int i = 0; i < SERIAL_MESSAGE_SIZE; i++) 
  {
    serialBuffer[i] = Serial.read();
    Serial.print(serialBuffer[i]);
  }
  Serial.print('\n');

  if (serialBuffer[SERIAL_MESSAGE_SIZE - 1] == SERIAL_TERMINATE_CHAR)
    return true;
  else
    return false;
}

enum serverControl_st_t
{
  init_st,
  serialRead_st,
  serverSend_st,
  error_st
} currentState = init_st;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void serverControl_debugStatePrint()
{
    static serverControl_st_t previousState;
    static bool firstPass = true;

    // Only print the message if:
    // 1. This the first pass and the value for previousState is unknown.
    // 2. previousState != currentState - this prevents reprinting the same state name over and over.
    if (previousState != currentState || firstPass)
    {
        firstPass     = false;            // previousState will be defined, firstPass is false.
        previousState = currentState;     // keep track of the last state that you were in.

        switch(currentState)
        {  // This prints messages based upon the state that you were in.
        case init_st:
            Serial.println("(init_st)");
            break;
        case serialRead_st:
            Serial.println("(serialRead_st)");
            break;
        case serverSend_st:
            Serial.println("(serverSend_st)");
            break;
        case error_st:
            Serial.println("(error_st)");
            break;
        }
    } //end if
}

void serverControl_tick()
{
  int packetSize = 0;

  // perform state action first
  switch (currentState)
  {
    case init_st:
      break;
    case serialRead_st:
      break;
    case serverSend_st:
      break;
    case error_st:
      break;
    default:
      Serial.print("Server state default error");
      break;
  }

  // perform state update next
  switch (currentState)
  {
    case init_st:
      // if there's data available, read a packet
      packetSize = Udp.parsePacket();
      
      // Wait for client to initiate handshake
      if (packetSize)
      {
        Serial.println('.');  // Give green light to STM32
        currentState = serialRead_st;
      }
      break;
    case serialRead_st:
      if (Serial.available() >= SERIAL_MESSAGE_SIZE) 
      {
        if (readSerial())
          currentState = serverSend_st;
        else
          Serial.println("Bad serial data. Trying again...");
      }
      break;
    case serverSend_st:
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(serialBuffer);
      Udp.endPacket();

      currentState = init_st;
      break;
    case error_st:
      break;
    default:
      Serial.print("Server state default error");
      break;
  }
}

void loop() 
{
  serverControl_tick();
  serverControl_debugStatePrint();
}

