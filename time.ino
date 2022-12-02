// any routines relating to timing or scheduling
// timerCallback -- the interrupt code - short and sweet checks the boxes
// user_int -- sets up the reoccuring interupt - currently set to once every 60 seconds
// processTick -- update the time, check for scheduled events
// chkTimeOut -- check if the motor activity has timed out due to no limit switch pressed

// start of timerCallback
void timerCallback(void *pArg){
  // please place further code in "processTick" as an interrupt should be as short as possible to avoid issues
  tickOccured = true;
}

void user_init(void){
  //int duration1 = 30*1000*60; // thirty minutes
  int duration1 = 60*1000; // every minute
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, duration1, true);
}

void processTick(){
  Serial.println("Tick");
  minutes1 += 1;
  // check if it is tomorrow
  if(minutes1>=1440){// check if it is midnight (or the count has overrun somehow?!)
    minutes1=0;// reset to start of new day.
  }
  // update the time
  if(minutes1==NTPTime){ // if it is 1:15 in the morning...
    minutes1 = updateNTP();
  }
  // print the current time to serial
  int hour = minutes1/60;// temp variables to plot the time
  int minute = minutes1-(hour*60);
  Serial.println("Time is " + String(hour, DEC) + ":" + String(minute, DEC));

  // The following commented out code is from a previous project for reference
  
  // check if it is time to do something
  /////////////////////////////////if(minutes1==openTimeMins){// time to open door
  //  Serial.println("Time to open the door!");
  //  motorForward(tooLong);
  //}else if(minutes1==closeTimeMins){// time to open door
  //  Serial.println("Time to close the door!");
  //  motorBackward(tooLong);
  //}
  tickOccured = false; // reset so that tickOccured is false for another minute
}

// The following commented out code is from a previous project for reference

//void chkTimeOut(){
//  // check if the motor has been running longer than expected
//  if(timeout < millis() && mMode>0){
//    Serial.println("Motor timeout!\nStopping the motor - why didn't a limit switch trigger?");
//    motorOff(500);
//  }
//}
