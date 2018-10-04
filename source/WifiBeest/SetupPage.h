// Creates an setup page on a webserver which allows the user to configure the directions of the motors

#ifndef SetupPage_h
#define SetupPage_h

//Holds the setup webpage in the program memory
//The setup page needs a hidden attribute so that it still submits even with both checkboxes unticked
const char setupPage[] PROGMEM = 
    "<html>"
      "<head>"
        "<style>"
          "label, input[type='submit'] {font-size: 1.2em; width: 100%; max-width: 350px; display: block; margin: 15px auto; }" 
          "input[type='checkbox'] {float: left;  margin-right: 15px; }"
        "</style>"
      "</head>"
      "<body>"
        "<form id='form' action='/setup' method='post'>"
          "<label><input type='checkbox' name='leftRev' $vl>Left Motor Reverse</label>"
          "<label><input type='checkbox' name='rightRev' $vr>Right Motor Reverse</label>"
          "<input type='hidden' name='hid' value='0'>"          
          "<input type='submit' value='Update'>"
        "</form>"
      "</body>"
    "</html>";

//Creates a webpage that allows the user to change the SSID and Password from the browser
void serveSetup(ESP8266WebServer *webServer) {
  String message;  
    
  // Check to see if we've been sent any arguments and instantly return if not
  if(webServer->args() == 0) {
    message = setupPage;
    
    if(config.limits[LFT].dir < 0) 
      message.replace("$vl", "checked");
    else  
      message.replace("$vl", "");

    if(config.limits[RGT].dir < 0) 
      message.replace("$vr", "checked");
    else  
      message.replace("$vr", "");

    webServer->sendHeader("Content-Length", String(message.length()));
    webServer->send(200, "text/html", message);
  }
  else {      
    // Create a string containing all the arguments, send them out to the serial port
    // Check to see if there are new values (also doubles to check the length of the new value is long enough)
    if(webServer->arg("leftRev") == "on")
      config.limits[LFT].dir = -1;
    else
      config.limits[LFT].dir = 1;
      
    if(webServer->arg("rightRev") == "on")
      config.limits[RGT].dir = -1;
    else
      config.limits[RGT].dir = 1;

    servos[LFT].limits.dir = config.limits[LFT].dir;
    servos[RGT].limits.dir = config.limits[RGT].dir;
    
    // Store the new settings to EEPROM
    SaveConfig();
    PrintConfig();

    // Construct a message to tell the user that the change worked
    message = "Update successful";     

    // Reply with a web page to indicate success or failure
    message = "<html><head><meta http-equiv='refresh' content='2;url=/' /></head><body>" + message;
    message += "<br/>Redirecting in 2 seconds...</body></html>";
    webServer->sendHeader("Content-Length", String(message.length()));
    webServer->send(200, "text/html", message);
  } 
}

#endif
    

