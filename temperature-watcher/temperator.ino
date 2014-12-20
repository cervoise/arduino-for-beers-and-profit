/*
  Needs Arduino Uno, Ethernet Shield (for SD card), BMP180 from Sparkfun, and a RTC module (DS1338 for example).
  Temperature mesure every DELAY and write it on SD card (needs FAT16) in CSV format (standard LibreOffice Calc format with UK locale so you can create a chart immediately).
  First column is the date and time of the measurement with following format: "DD/MM/YYYY HH:MM:SS". 
  Second column is the temperature.
  
  BMP180 and RTC are I²C devices and will both be connected to SDA and SCL pins, but take care that only one set of pull-up resistors is enabled.
*/

#include <SFE_BMP180.h>
#include <Wire.h>
#include <SD.h>
#include <Time.h>
#include <DS1307RTC.h>

File tempTxtFile;
SFE_BMP180 pressure;

//On pin 13 a led is connected on the Arduino board but is hidden by the Ethernet shield. You can connect an external led if you wish.
#define ERROR_LED 13
//in meters
#define ALTITUDE 30.0
//in millisecond
#define DELAY 5000
//CSV file separator
#define CSV_SEPARATOR ","

void setup()
{
  pinMode(ERROR_LED, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).
  if (pressure.begin()) {
    Serial.println("BMP init success.");
  }
  else {
    Serial.println("BMP init fail.");
    fatalError();
  }
  
  //init the SD card
  pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("SD initialization failed.");
    fatalError();
  }
  
  Serial.println("SD initialization done.");
}

void loop()
{
  
  char status;
  double T;
  tmElements_t tm;
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      if (RTC.read(tm)) {
        
        // date/time format is : DD/MM/YYYY HH:MM:SS
        String time = String(tm.Day) + "/" + String(tm.Month) + "/" + tmYearToCalendar(tm.Year) + " " + getTwoDigitsNumber(tm.Hour) + ":" + getTwoDigitsNumber(tm.Minute) + ":" + getTwoDigitsNumber(tm.Second);
        
        // Print out the measurement:
        Serial.print("At ");
        Serial.print(time);
        Serial.print(" - temperature: ");
        Serial.print(T,2);
        Serial.println(" deg C.");
        
        // Write on SD card. Open and close the file for each measurement.
        tempTxtFile = SD.open("temp.csv", FILE_WRITE);
       
        // if the file opened okay, write to it:
        if (tempTxtFile) {
          Serial.print("Writing to temp.csv...");
          tempTxtFile.print(time);
          tempTxtFile.print(CSV_SEPARATOR);
          tempTxtFile.println(T,2);
          // close the file:
          tempTxtFile.close();
          Serial.println("done.");
          
        }
        else {
          // if the file didn't open, print an error:
          Serial.println("error opening temp.csv");
          fatalError();
        }
      } 
      else {
        Serial.println("DS1307 read error!  Please check the circuitry.");
        fatalError();
      }
    }
    else {
      Serial.println("error retrieving temperature measurement\n");
    }
  }
  else {
    Serial.println("error starting temperature measurement\n");
  }
  
  delay(DELAY);
}


void fatalError() {
  while(true) {
    digitalWrite(ERROR_LED, HIGH);
    delay(300);
    digitalWrite(ERROR_LED, LOW);
    delay(300);
  }
}

String getTwoDigitsNumber(int number) {
  if (number >= 0 && number < 10) {
    return "0" + String(number);
  }
  else {
    return String(number);
  }
}
