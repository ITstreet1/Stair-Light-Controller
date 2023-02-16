/*Stair LED Light sketch
 * Author Petrovic Dejan
 * Date 23. December 2022
 * 
 * v1.0
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
const byte rlyPins[8] = {2, 3, 4, 5, 13, 8, 7, 6};
#define sensorLightPin A0                         // Light sensor  Do not change!!!
///
int pot1 = A1;  //reserved
int pot2 = A2;  //dist
int pot3 = A3;  //stair nr
int pot4 = A6;  //open time
int pot5 = A7;  //light
int light, stairNR, dist, pwm = 0;
int mapLight, mapStairNR, mapDist, mapPwm = 0;
long opTime, mapOpTime = 0;
//
const int trigPin1 = 9;                          // Upper sensor  Do not change!!!
const int echoPin1 = 10;                          // Upper sensor  Do not change!!!
const int trigPin2 = 11;                           // Bottom sensor Do not change!!!
const int echoPin2 = 12;                          // Bottom sensor Do not change!!!
unsigned char maxBrightness = 100;                // Here you set the number of brightness levels. Adjust(0-255)!!
boolean sensorUpperActive;
boolean sensorBottomActive;
int numberOfstairs;                               // ! 3 ! Setup Number of Your stairs. Adjust UP TO 24 !!!!
int pause;                                        // ! 2 ! Setup delay ON/OFF  Adjust!!! 
int faidoutSpeed;                                // How fast to fadeout     Adjust!!!
int faidinSpeed;                                 // How fast to fadein      Adjust!!!
int val;
int photoCellTreshold;                            // ! 1 ! LDR treshold (optimal value 300)
long duration1, duration2;
int distance1, distance2;                         // ! 4 ! Sonar distance
int distanceCheck;                                // ! 4 ! Sonar distance
//oled
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.print("Loading...");
  display.display();
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  for(int i=0; i<sizeof(rlyPins)/sizeof(rlyPins[0]); i++){
    pinMode(rlyPins[i], OUTPUT);
    digitalWrite(rlyPins[i], HIGH);
    }
  pinMode(sensorLightPin, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  display.setRotation(1);
  display.setTextSize(2);
}

void loop(){
checkPot();
displayData();
val = analogRead(sensorLightPin);                 //Checking light level
  if (val <= photoCellTreshold){                  // Adjust photocell cencivity. (val<10 dark.....val>800 bright..)    !!!!!!!!
    Serial.print(val);
    Serial.println(" mrak");
    BottomTriggerFire();                          //Checking bottom sensor
    UpperTrigerFire();                            //Checking upper sensor
    switchONOFFfromdown();                        //Stairs on/off from down to up
    switchONOFFfromUp();                          //Stairs on/off from up to down
  }
}
void BottomTriggerFire(){
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  if (distance2 <= distanceCheck ){          //Depend of the sensor type, if 0 when triggered ,than change the comparison to opposite value
    sensorBottomActive = true;
    Serial.println("bottom active");
  }
}
void UpperTrigerFire() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;
  if (distance1 <= distanceCheck ) {          //Depend of the sensor type, if 0 when triggered ,than change the comparison to opposite value
    sensorUpperActive = true;
    Serial.println("upper active");
  }
}
void switchONOFFfromdown(){
  if (sensorBottomActive==true && sensorUpperActive==false){
        for(int i=0; i<(sizeof(rlyPins)/sizeof(rlyPins[0])-numberOfstairs); i++){
            digitalWrite(rlyPins[i], LOW);
            delay(faidoutSpeed);
            }
        Serial.println();
        Serial.println("Pause ON");
        delay(pause); 
        Serial.println();
        Serial.println("Pause OFF");
        for(int i=0; i<(sizeof(rlyPins)/sizeof(rlyPins[0])-numberOfstairs); i++){
            digitalWrite(rlyPins[i], HIGH);
            delay(faidinSpeed);
            }
        sensorBottomActive = false ;
        Serial.println("bottom false");
  }
}
 void switchONOFFfromUp(){
    if ( sensorUpperActive==true && sensorBottomActive==false) {
        for(int i=(sizeof(rlyPins)/sizeof(rlyPins[0])-(1+numberOfstairs)); i>=0; i--){
          Serial.print(i);
          Serial.print(" - ");
          Serial.println(rlyPins[i]);
            digitalWrite(rlyPins[i], LOW);
            delay(faidoutSpeed);
            }
        Serial.println();
        Serial.println("Pause ON");
        delay(pause); 
        Serial.println();
        Serial.println("Pause OFF");  
        for(int i=(sizeof(rlyPins)/sizeof(rlyPins[0])-(1+numberOfstairs)); i>=0; i--){
          Serial.print(i);
          Serial.print(" - ");
          Serial.println(rlyPins[i]);
            digitalWrite(rlyPins[i], HIGH);
            delay(faidinSpeed);
            }
        sensorUpperActive= false ;  
        Serial.println("Upper false");
  }
}
/*
 *   for(int i=0; i<(sizeof(rlyPins)/sizeof(rlyPins[0])-2); i++){
  digitalWrite(rlyPins[i], LOW);
  delay(100);
  }
  for(int i=0; i<(sizeof(rlyPins)/sizeof(rlyPins[0])-2); i++){
  digitalWrite(rlyPins[i], HIGH);
  delay(100);
  }
 */
 void checkPot(){
  //light
  light = analogRead(pot5);
  photoCellTreshold = light;  //declaration of light
  //Serial.print("light A ");
  //Serial.println(light);
  mapLight = map(light, 0, 1020, 1, 10);
  Serial.print("maplight ");
  Serial.print(mapLight);
  Serial.println(" u koracima 1 -10");
  //Time
  opTime = analogRead(pot4);
  //Serial.print("opTime A ");
  //Serial.println(opTime);
  mapOpTime = map(opTime, 0, 1020, 1000, 60000);
  Serial.print("mapOpTime ");
  pause = mapOpTime;          //declaration of open time
  Serial.print(mapOpTime);
  Serial.println(" u milisekundama");
  //Stair
  stairNR = analogRead(pot3);
  //Serial.print("stairNR A ");
  //Serial.println(stairNR);
  mapStairNR = map(stairNR, 0, 1020, 1, 8);
  numberOfstairs = 8 - mapStairNR; //declaration nr of stairs
  Serial.print("mapStairNR ");
  Serial.print(mapStairNR);
  Serial.println(" u kom");
  //Serial.print("numberOfstairs ");
  //Serial.println(numberOfstairs);
  //Dist
  dist = analogRead(pot2);
  //Serial.println(dist);
  mapDist = map(dist, 0, 1020, 1, 100);
  distanceCheck = mapDist;     //declaration of distance
  Serial.print("mapDist ");
  Serial.print(mapDist);
  Serial.println(" u cm 1 - 100");
  //PWM
  pwm = analogRead(pot1);
  //Serial.println(pwm);
  mapPwm = map(pwm, 0, 1020, 1, 20000);
  faidinSpeed = mapPwm/2;          //declaration of pwm
  faidoutSpeed = mapPwm/2;         //declaration of pwm
  Serial.print("mapPWM ");
  Serial.print(mapPwm/2);
  Serial.println(" u milisekundama");
}
void displayData(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(mapLight); //osvetljenje u koracima 1-10
  display.setCursor(0,25);
  int mapOpTimeToDisplay = mapOpTime/1000;
  display.print(mapOpTimeToDisplay); //otvoreno vreme u sekundama
  display.setCursor(0,50);
  display.print(mapStairNR); //broj stepenika
  display.display();
  display.setCursor(0,75);
  display.print(mapDist); //razdaljina u cm 1-100
  display.setCursor(0,100);
  display.print(mapPwm/2000); //pwm u sekundama
  display.display();
}
