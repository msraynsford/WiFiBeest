// Handles a class of data for storing variables, and the reading/writing of those variables to EEPROM
// It is able to detect invalid data being read from EEPROM and restore some basic values in those circumstances

#ifndef Config_H
#define Config_H

#include "EEPROMAnything.h"

#define MIN_STR_LEN 8
#define MAX_STR_LEN 17

#define SSID_DEFAULT "Beest"
#define PASS_DEFAULT "password"

#define SERVO_MIN   500
#define SERVO_MAX   2500

// Define a structure to hold all the variables that are going to be stored in EEPROM
struct config_t{
  char ssid[MAX_STR_LEN] = SSID_DEFAULT;
  char pass[MAX_STR_LEN] = PASS_DEFAULT;
  limits_t limits[NUMSERVOS];
} config;

// Declare an area of EEPROM where the variables are stored
void InitConfig() {
  EEPROM.begin(sizeof(config));
}

// Writes the config values back to EEPROM
void SaveConfig() {
#ifdef USESERIAL
  Serial.printf("Save Config\n");
#endif

  // Store the new settings to EEPROM
  EEPROM_writeAnything(0,  config);    
  EEPROM.commit();
}

// Restores the values of SSID and Password if the EEPROM has been corrupted
void ResetConfig() {
#ifdef USESERIAL
  Serial.printf("Reset Config\n");
#endif
  
  // If the EEROM isn't valid then create a unique name for the wifi
  sprintf(config.ssid, "%s %06X", SSID_DEFAULT, ESP.getChipId());
  sprintf(config.pass, PASS_DEFAULT);

  for(byte id = 0; id < NUMSERVOS; id++) {
    config.limits[id].min = SERVO_MIN;
    config.limits[id].max = SERVO_MAX;
    config.limits[id].zero = (config.limits[id].min + config.limits[id].max) / 2;
  }
  
  SaveConfig();
}

// Checks all of the bytes in the string array to make sure they are valid characters
bool ValidateString(char* value) {
  bool valid = true;
  
  //Check to see if the string is an acceptable length
  if((strlen(value) < MIN_STR_LEN) || (strlen(value) >= MAX_STR_LEN)) {
    valid = false;
  }
  else {
    //Check each character in the string to make sure it is alphanumeric or space
    for(uint8_t i = 0; i < strlen(value); i++)
      if(!isAlphaNumeric(value[i]))
        if(!isSpace(value[i]))
          valid = false;
  }
  
  return valid;
}

// Loads the config values from EEPROM, leaves the default values if the EEPROM hasn't been set yet
void LoadConfig() {
  bool eepromValid = true;
  
  // Load the config variables from EEPROM
  config_t eepromConfig;
  EEPROM_readAnything(0, eepromConfig);

  //Check to see if the config variables loaded from eeprom are valid
  eepromValid &= ValidateString(eepromConfig.ssid);
  eepromValid &= ValidateString(eepromConfig.pass);

  if(eepromValid) {
    strcpy(config.ssid, eepromConfig.ssid);
    strcpy(config.pass, eepromConfig.pass);

    for(byte id = 0; id < NUMSERVOS; id++) 
      config.limits[id] = eepromConfig.limits[id];
  }  
  else {
    // If the EEROM isn't valid then create a unique name for the wifi
    ResetConfig();
    SaveConfig();
  }
}


// Sends a copy of the config values out to the serial port
void PrintConfig() {
#ifdef USESERIAL
  Serial.printf("SSID: '%s'\n", config.ssid);  
  Serial.printf("Pass: '%s'\n", config.pass);

  for(byte id = 0; id < NUMSERVOS; id++) 
    Serial.printf("Servos:%d, min:%d, zero:%d, max:%d\n", id, config.limits[id].min, config.limits[id].zero, config.limits[id].max);
#endif
}

#endif
