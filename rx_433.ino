#include <RH_ASK.h>
#include <TimeLib.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

void setup()
{
    Serial.begin(9600);  // Debugging only
    if (!driver.init()){
         Serial.println("init failed");
    }
    //Serial.print("Date of Recording: ");
    //Serial.println(F(__DATE__));
    //Serial.print("Time of Recording: ");
    //Serial.println(F(__TIME__));
}

    //gloabls
    String dataLabel1 = "Fahrenheit";
    String dataLabel2 = "Celsius";
    float fahrenheit;
    float celsius;
    bool label = true;

void loop()
{
    while(label){ //runs once
        Serial.print(dataLabel2);
        Serial.print(",");
        Serial.println(dataLabel1);
        label=false;
  }
  
    uint8_t buflen = 4; // max number of bytes allowed in message
    if (driver.recv((uint8_t *) &celsius, &buflen)) // Non-blocking
    {
      if (buflen == 4)  //got correct message size?
     {  
      // Message with a good checksum received, dump it.
      //Serial.print("TEMP: ");
      Serial.print(celsius); 
      Serial.print(" C");
      
      Serial.print(",");
      fahrenheit = (celsius *(9.000/5.000)) + 32.000;
      
      Serial.print(fahrenheit); 
      Serial.println(" F");
     
      delay(1000); // ms
     }       
    }
}
