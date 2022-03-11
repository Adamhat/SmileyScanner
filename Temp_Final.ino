/***********************************************
 * Temp_Final.ino
 * Darius Salagean, Adam Farhat, Yovany Lopez Hernandez
 * Junior Design ECE 342
 * 2/11/22
 **********************************************/

#include <RH_ASK.h> // Library for RF communication
#include <SPI.h> // Library for RF communication
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Adafruit_MLX90614.h> //Library for MLX temp sensor
#include <NewPing.h>  //Library for Ultrasonic Sensor

//Definition statements for all devices
RH_ASK driver; //RF transmitter
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //Temp sensor
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); //LCD Display
NewPing sonar(9, 8, 400);  //Distance sensor
#define TRIGGER_PIN 9
#define ECHO_PIN 8

//Variable initialization
float distance, tempF, tempC, tempA;
int flag = 1, trig = 0, dis = 0, c = 0, f = 1;
float cel[20], amb[20], far[20];

//First time setp-up for devices
void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT); //Pin D4 used to drive buzzer
  mlx.begin(); //Temp sensor initialization
  lcd.init(); //LCD initialization
  lcd.backlight(); //LCD backlight initialization
  driver.init(); //RF transmitter initialization
 
}

//Main program. Performs all functions of the temperature
//sensor in a continuous, never ending loop.
void loop() {
  flag = 1; // Flag used to continue the while loop is set
  
  lcd.clear(); // LCD is set to print out a welcome message measuring distance
  lcd.setCursor(0, 0); // Set the cursor 
  lcd.print("Please stand 4cm"); 
  lcd.setCursor(0, 1); 
  lcd.print("away from sensor");
  lcd.setCursor(0, 2);
  lcd.print("Current distance:");

  //Main loop. Continues updating the distance until user
  //is within 4cm of the sensor
  while( flag == 1){
    
    distance = sonar.ping_cm(); //Get the distance as a float
    dis = (int)distance; //Cast the distance to an int value
    trig = digitalRead(3); //Check the motion sensor for C to F input
    lcd.setCursor(0, 3); //Print the current distance
    lcd.print(dis);
    lcd.print(" cm  ");
    
    if(dis <= 3){
      dis = 0;
    }else if(dis >= 200){
      dis = 200;
    }

    //Check if on F or C mode
    if(f == 1){ 
      lcd.print("Reading F  ");
    }
    else{
      lcd.print("Reading C  ");
    }
    
    if(trig == 0){ //0 means the motion sensor was tripped
       if(c == 0){ //Swap to C output mode
          f = 0;
          c = 1; 
          delay(250); //Small delay to prevent rapid inputs
       }else{   //Swap to F output mode.
          f = 1;
          c = 0;
          delay(250);
       }
     }
      
     delay(50); //Small delay to prevent the distance from updating too fast
     
     //Once the user is within 3cm of the sensor it will begin reading their temp
     if(dis == 4){
       lcd.clear(); //Reset the LCD screen for wait message
       lcd.setCursor(0, 0); 
       lcd.print("Please wait while"); 
       lcd.setCursor(0, 1); 
       lcd.print("your temperature is");
       lcd.setCursor(0, 2); 
       lcd.print("being read");

       tempF = 0; //Rest the temperature values so they can be reused
       tempC = 0;
       tempA = 0;   //Delay so that the temp starts reading after the user
       delay(1000); //acknowledges they are close enough and stops moving.
                    
       //Take 20 readings of user temp and ambient temp in C
       //Temp in C was chosen since C is more widely used.
       for (int i = 0; i < 20; i++) { 
          cel[i] = mlx.readObjectTempC();
          amb[i] = mlx.readAmbientTempC();
          delay(100); //Small delay between readings to get 2 seconds of data
       }
       
       //Compute the average of the readings
       for (int i = 0; i < 20; i++){
          tempC = tempC + cel[i];
          tempA = tempA + amb[i];
       }
       tempC = tempC / 20.00;
       tempC = tempC + 4.65;
       tempA = tempA / 20.00;
       Serial.println(tempC);
       Serial.println(tempA);
       
       //Using modifed formula found at https://www.medrxiv.org/content/10.1101/2020.12.04.20243923v1.full.pdf
       //and recorded data to estimate body temperature from forehead temperature
       tempC = 2.85 + pow(((pow(tempC, 4)-(0.02*(pow(tempA, 4))))/(0.98)), (0.25));
       tempF = (tempC*(9.000/5.000)) + 32.000; //Convert from C to F

       lcd.clear(); //Clear the LCD for temperature readout
       lcd.setCursor(0, 0); // Set the cursor 
       lcd.print("Your temperature is:");
       lcd.setCursor(0, 1);
       //Display F or C temperature depending on user choice at start
       if(f == 1){
          lcd.print(tempF);
          lcd.print(" F ");
       }else{
          lcd.print(tempC);
          lcd.print(" C ");
       }
       
       digitalWrite(4, HIGH); //Signal to the user their temp ws taken
       delay(250);
       digitalWrite(4, LOW);
       //Check if a fever was recorded
       if(tempF >= 100.40 || tempC >= 38.00 ){
          lcd.setCursor(0, 2); //Let the user know on the screen
          lcd.print("You may have a fever");
          //Make the buzzer give the user an audio que.
          for(int i = 0; i <5; i++){
            digitalWrite(4, HIGH);
            delay(250);
            digitalWrite(4, LOW);
            delay(250);
          }
        }
        //Let the user know data is being saved
        lcd.setCursor(0, 3);
        lcd.print("Data is being saved");
        //Transmit the data to the receiver
        driver.send((uint8_t *)&tempC, 4); //two bytes on AVR Arduino
        driver.waitPacketSent(); //Wait for the data to be sent before moving on
        
        delay(5000); //Small delay to allow the user to recognize that the reading went through
     
        c = 0;
        f = 1;
        flag = 0;
      }
   }
}
  
