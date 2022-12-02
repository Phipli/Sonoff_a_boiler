void loadWifiSettings(){ // SSID and Password are saved as files in the flash memory
  File s = SPIFFS.open("/ssid.txt", "r");
  if(!s){
    Serial.println("Failed to open file ssid.txt for read");
  }else{
    strSSID = s.readStringUntil('\n');
    strSSID.trim();
    s.close();
  }
  File p = SPIFFS.open("/password.txt", "r");
  if(!p){
    Serial.println("Failed to open file password.txt for read");
  }else{
    strPSWD = p.readStringUntil('\n');
    strPSWD.trim();
    p.close();
  }
  Serial.println("Wifi settings loaded...");
}

void startIO(){
  // preparing IO
  for(int x = 0; x<relayCount; x++){
    pinMode(relays[x], OUTPUT);
    pinMode(buttons[x], INPUT);
    //digitalWrite(buttons[x], HIGH);
    digitalWrite(relays[x], LOW);
  }
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
 
  Serial.begin(115200);
  Serial.println("Serial Started...");
  SPIFFS.begin();
  Serial.println("SPIFFS Started...");
}

void attachPages(){
  webPage += "<h1>SONOFF Web Server</h1><p><a href=\"toggle?relay=1\"><button>1</button></a> <a href=\"toggle?relay=2\"><button>2</button></a> <a href=\"toggle?relay=3\"><button>3</button></a> <a href=\"toggle?relay=4\"><button>4</button></a></p>";
 
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/wifi", handleWifi);//setupWifi
  server.on("/toggle", handleToggle);//toggle
  server.on("/on", handleOn);// on
  server.on("/off", handleOff);//off
  server.on("/set", handleSet);//specify
  //check status
  server.on("/status", [](){
    String response = "Status : ";
    for(int x = 0; x<relayCount; x++){
      response += String(digitalRead(relays[x]));
    }
    server.send(200, "text/html", response);
    //server.send(200, "text/html", "Relay status : "+String(digitalRead(relays[0]))+String(digitalRead(relays[1]))+String(digitalRead(relays[2]))+String(digitalRead(relays[3])));
  });
}

void startWifi(){
  // check if Button L4 is pressed to change wifi settings
  if(digitalRead(buttons[3])==LOW){// button pressed for wifi recovery mode, using default #defined credentials
    WiFi.softAP(defaultssid,defaultpassword);
    Serial.println();
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }else{ // load saved wifi credentials
    char ssid[strSSID.length()+1];
    char password[strPSWD.length()+1];
    strSSID.toCharArray(ssid, strSSID.length()+1);
    strPSWD.toCharArray(password, strPSWD.length()+1);
    delay(100);

    /*
     * I've commented out the bit that loads a saved ssid. I can't remember why, it might not have worked
     * or I was in a rush because we didn't have any heating and I couldn't be bothered to find out if
     * it was correct or not
     */
    Serial.print("Connecting to " + String(ssid));
    //if (String(WiFi.SSID()) != String(ssid)) {
    //  WiFi.mode(WIFI_STA);
    //  WiFi.begin(ssid, password);
    //}
    
    /* 
     *  The following line is a bodge, hard coded
     */
    WiFi.begin(tempSSID, tempPsk);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print(" connected. IP address: ");
    Serial.println(WiFi.localIP());
    dnsServer.start(DNS_PORT, "*", WiFi.localIP());
    MDNS.begin(serverName);
    httpUpdater.setup(&server);
    Serial.print("Open http://");
    Serial.print(serverName);
    Serial.println(".local/ to see interface");
  }
 
  
}
