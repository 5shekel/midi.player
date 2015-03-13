/* FILE:    ARD_SD_CARD_MODULE_HCARDU0008_Write_Example.pde
   DATE:    09/07/12
   VERSION: 0.1
   
REVISIONS:

09/07/12 Created version 0.1
   
01/08/12 Updated comments to include more instructions of how to 
         interface to module.


This is an example of how to use the HobbyComponents SD card reader module 
(HCARDU0008). This module allows reading and writing of data to a standard 
SD card and is useful for applications such as data loggers where a large 
amount of data needs to be stored. The module works with the standard 
Arduino SD card library.

This example program will create a test file on the SD card called test.txt
If the file already exists it will first delete it and then create a new 
one.

MODULE PINOUT:
                    Uno            | MEGA
PIN 1: GND    --->  Arduino GND
PIN 2: +3.3V  --->  N/A
PIN 3: +5V    --->  Arduino 5V
PIN 4: CS     --->  Arduino DIO 4
PIN 5: MOSI   --->  Arduino DIO 11 | 51 (MOSI)
PIN 6: SCLK   --->  Arduino DIO 13 | 52 (SCK)
PIN 7: MISO   --->  Arduino DIO 12 | 50 (MISO)
PIN 8: GND    --->  N/A

arduino mega



IMPORTANT: The modules 5V pin supplies an onboard 3.3V regulator that powers
the SD card. The output of this regulator is brought out to the 3.3V pin. You
may power the module via the 5V or 3.3V pins but you must not supply power to
both as this could damage the on board regulator. If interfacing to the module 
with 5V DIO it is recommended that you level shift the 5V DIO down to 3.3V
for the MOSI, SCLK, and CS pins. You can do this by using an appropriate level
shifter or a resistor divider. Disclaimer: We can not be held responsible for any
damage cause to an SD card by improper interfacing with the module.


also http://forum.hobbycomponents.com/viewtopic.php?f=25&t=5

*/

/* Include the standard SD card library */
#include <SD.h>

#define SD_CARD_CD_DIO 10 /* DIO pin used to control the modules CS pin */

File SDFileData;

/* Initialise serial and DIO */
void setup()
{
  Serial.begin(9600);
  /* DIO pin used for the CS function. Note that even if you are not driving this
     function from your Arduino board, you must still configure this as an output 
     otherwise the SD library functions will not work. */
  pinMode(10, OUTPUT);
}

/* Main program loop */
void loop()
{

  /* Initialise the SD card */
  if (!SD.begin(SD_CARD_CD_DIO)) 
  {
    /* If there was an error output this to the serial port and go no further */
    Serial.println("ERROR: SD card failed to initiliase");
    while(1);
  }else
  {
    Serial.println("SD Card OK");
  }
  
 
   /* Check if the text file already exists */
   while(SD.exists("test.txt"))
   {
     /* If so then delete it */
     Serial.println("test.txt already exists...DELETING");
     SD.remove("test.txt");
   }
 
  /* Create a new text file on the SD card */
  Serial.println("Creating test.txt");
  SDFileData = SD.open("test.txt", FILE_WRITE);
  
  /* If the file was created ok then add come content */
  if (SDFileData)
  {
    SDFileData.println("It worked !!!");
  
    /* Close the file */
    SDFileData.close();   
    
    Serial.println("done.");
  }else
  {
      Serial.println("Error writing to file !");
  }
  
  /* Do nothing */
  while (1);
}

