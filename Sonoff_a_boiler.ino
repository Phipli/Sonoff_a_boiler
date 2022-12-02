/*
 * Phil Cranmer
 * 30 November 2021
 * Sonoff a Boiler - Heating Controller (Sonoff 4ch)
 * Version 1 revision 2
 * 
 * THIS VERSION (R2) IS JUST UPDATED TO REMOVE THE PUMP / HEAT INTERLOCK
 * BECAUSE I NEED TO RUN THE PUMP WITHOUT HEAT THIS WINTER
 * (TO AVOID FREEZING)
 * 
 * Settings for programming the board :
 * Generic ESP8285
 * 115200
 * CPU 80MHz
 * 1MB FS64k
 * LED 13
 * Crystal 26MHz
 * reset ck
 * Debug disabled, none
 * v2 lower mem
 * Flash
 * new can return nullptr
 * only sketch
 * 
 * Send commands as urls, for example, 192.168.4.1/on?relay=1
 *  which turns on relay number 1
 *  
 * Description of Sonoff I/O
    int relayCount = 4;
    int LED = 13;
    int relays[] = {12,5,4,15};
    int buttons[] = {0,9,10,14};

    Buttons are active low, relays are active high
 *
 * Things that need to be added :
 * Minimum switch time
 *    Always allow power off
 *    Maximum "On" frequency
 * Timer Programmer
 * External Sensors
 * External Relays
 * Custom logic?
 * Self Updating
 * Need to reserve space for strings??
 */
 
/*
 * Setup - this section needs to be expanded
 */
// Tempoary hard coded wifi SSID and password
#define tempSSID "your-ssid-here"
#define tempPsk "your-wifi-password-here"
// Enable/disable the interlock between the pump and boiler on - i.e. if you turn the pump on, the boiler automatically starts.
#define pumpHeatInterlock false
// Replace with your network credentials
#define defaultssid "Sonoff4Ch"
#define defaultpassword "Sonoff4Ch"
// Boiler controller's network name
#define serverName "heating" // not currently used
// ntp server
#define ntpServerName "uk.pool.ntp.org"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <FS.h>

extern "C" { // used by the interrupt code I think? Allows the inclusion of a proper C library.
  #include "user_interface.h"
}
 
MDNSResponder mdns;

const byte DNS_PORT = 53;
 
String strSSID;
String strPSWD;

DNSServer dnsServer;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
 
String webPage = "";

int relayCount = 4;
int LED = 13;
int relays[] = {12,5,4,15};
int buttons[] = {0,9,10,14};

// NTP related variables
boolean timeAvailable = false;
unsigned int localPort = 2390; // local port to listen for UDP packets (for time)
IPAddress timeServerIP; //
const int NTP_PACKET_SIZE = 48; // NTP time stamp is the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE ]; // buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP (for NTP)
WiFiUDP udp;
unsigned int NTPTime = 75; // 1:30 in the morning - the time we will update NTP
unsigned long timeout = 0; // the time in the future at which I should stop doing something
unsigned int minutes1 = 0; // store minutes into day - incremented in 30s
// create the interrupt object
os_timer_t myTimer;
bool tickOccured = false; // event has happened based on interrupt
 
void setup(void){
  startIO(); 
   
  delay(5000);
 
  loadWifiSettings();
  startWifi();
  attachPages();
 
  server.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTP server started");
  
  user_init(); // set up interrupt timing - time tab
  minutes1 = updateNTP(); // time tab
}
 
void loop(void){
  server.handleClient();
  digitalWrite(LED, LOW);

  // check physical buttons
  for(int x = 0; x<relayCount; x++){
    if(digitalRead(buttons[x]) == LOW){
      digitalWrite(relays[x], !digitalRead(relays[x]));
      digitalWrite(LED, HIGH);
      delay(1000);
    }
  }

  // Run Boiler if pump is switched on
  if(pumpHeatInterlock){ // setting the define at the top of this file to false disables this feature
    if( digitalRead(relays[1])){
      digitalWrite(relays[0], HIGH);
    }
  }
  
  if(tickOccured){
    processTick();
  }
}
