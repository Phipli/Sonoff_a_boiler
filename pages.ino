void handleWifi(){
  String message = "";
  if(server.hasArg("ssid")){ // have I been sent a new SSID?
    String ssidTxt = server.arg("ssid");
    File s = SPIFFS.open("/ssid.txt", "w");
    if(!s){
      Serial.println("Failed to open file ssid.txt for write access.");
    }else{
      s.println(ssidTxt);
      s.close();
      Serial.println("Updated wifi ssid");
      server.send(200, "text/html", "Updated wifi ssid. Cycle power to activate.");
    }
  }
  if(server.hasArg("password")){ // have I been sent a new Password?
    String pswdTxt = server.arg("password");
    File p = SPIFFS.open("/password.txt", "w");
    if(!p){
      Serial.println("Failed to open file password.txt for write access.");
    }else{
      p.println(pswdTxt);
      p.close();
      Serial.println("Updated wifi password");
      server.send(200, "text/html", "Updated wifi password. Cycle power to activate.");
    }
  }
}

// Invert the current state of a specific relay
void handleToggle(){
  String message = "";
  String command = server.arg(0);
 
  if(server.hasArg("relay")){
    for(int n = 0; n<relayCount; n++){
      if(command.charAt(0)==n+48+1){
        digitalWrite(relays[n], !digitalRead(relays[n]));
        digitalWrite(LED, HIGH);
        server.send(200, "text/html", "Relay "+String(n+1)+" of 4 is : "+String(digitalRead(relays[n])));
        delay(100);
      }
    }
  }
}

// Set a specific relay to be ON
void handleOn(){
  String message = "";
  String command = server.arg(0);
 
  if(server.hasArg("relay")){
    for(int n = 0; n<relayCount; n++){
      if(command.charAt(0)==n+48+1){
        digitalWrite(relays[n], HIGH);
        digitalWrite(LED, HIGH);
        server.send(200, "text/html", "Relay "+String(n+1)+" of 4 is : "+String(digitalRead(relays[n])));
        delay(100);
      }
    }
  }
}

// Set a specific relay to be OFF
void handleOff(){
  String message = "";
  String command = server.arg(0);
 
  if(server.hasArg("relay")){
    for(int n = 0; n<relayCount; n++){
      if(command.charAt(0)==n+48+1){
        digitalWrite(relays[n], LOW);
        digitalWrite(LED, HIGH);
        server.send(200, "text/html", "Relay "+String(n+1)+" of 4 is : "+String(digitalRead(relays[n])));
        delay(100);
      }
    }
  }
}

// Set status of all relays
void handleSet(){
  String message = "";
  String command = server.arg(0);
 
  if(server.hasArg("data")){
    digitalWrite(LED, HIGH);
    for(int n = 0; n<relayCount; n++){
      if(command.charAt(n)=='0'){
        digitalWrite(relays[n], LOW);
      }else if(command.charAt(n)=='1'){
        digitalWrite(relays[n], HIGH);
      }
    }
    String response = "Status : ";
    for(int x = 0; x<relayCount; x++){
      response += String(digitalRead(relays[x]));
    }
    response += "\nTime : "+String(minutes1/60,DEC)+":"+String(minutes1-((int(minutes1/60))*60),DEC);
    server.send(200, "text/html", response);
    //server.send(200, "text/html", "Relay status : "+String(digitalRead(relays[0]))+String(digitalRead(relays[1]))+String(digitalRead(relays[2]))+String(digitalRead(relays[3])));
    delay(100);
  }
}
