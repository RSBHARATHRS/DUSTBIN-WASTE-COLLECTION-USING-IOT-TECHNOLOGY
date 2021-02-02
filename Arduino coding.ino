#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>   //servo library
#include <dht.h>
#include<SPI.h>
#include <MFRC522.h>
#include<SoftwareSerial.h>
//#include<ArduinoJson.h>

#define DHT11_PIN 24
#define SS_PIN 9
#define RST_PIN 8
#define LED_B 24
#define LED_R 25
#define LED_G 26

#define BUZ 3

//---------------------------------------------Variable Declaration Section----------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);//I2C pins declaration
Servo servo;  
dht DHT;
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
SoftwareSerial s(11,12);

int servoPin = 2 ;

const int O_trigPin = 48;
const int O_echoPin = 49;

const int M_trigPin = 23;
const int M_echoPin = 22;


int storage, distanceInch,i=1,tag=0;
int MAX_STORAGE=26;
static int temp,hum;
int var=0;
long duration, dist_open, average;   
long aver[3];   //array for average

boolean O_tag= false;

String str;

//---------------------------------------------------Setup Section-------------------------------------------------------------
void setup() 
{
s.begin(9600);
Serial.begin(115200);
Serial1.begin(115200);
lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
lcd.backlight();//To Power ON the back light
pinMode(O_trigPin, OUTPUT);
pinMode(O_echoPin, INPUT);
pinMode(M_trigPin, OUTPUT);
pinMode(M_echoPin, INPUT);
pinMode(BUZ, OUTPUT);
pinMode(LED_R, OUTPUT);
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);

Serial.print(char(169)); // Copyright Symbol

SPI.begin();
rfid.PCD_Init();
}

//---------------------------------------------------display developers name--------------------------------------------------------------
void display_developers_name(){
    lcd.setCursor(3,0);
    lcd.print("DEVELOPED   ");
    lcd.setCursor(0,1);
    lcd.print("       BY    ");
    delay(3000);
    lcd.setCursor(2,0);
    lcd.print("Prabhu, Mani  ");
    lcd.setCursor(0,1);
    lcd.print("Kalepmani,siddiq");
    delay(3000);
    i++;
}
//---------------------------------------------------distance measure--------------------------------------------------------------

int measure(int trigPin,int echoPin) { 
  int dist;
  digitalWrite(10,HIGH);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  return dist = (duration/2) / 29.1;       //for obtain distance
}

void Measure_temp(){
  lcd.setCursor(0,0);
  temp=DHT.temperature;
  //hum=DHT.humidity;
  lcd.print("T:");
  lcd.print(temp);
  lcd.print(" ");
  lcd.print("H:");
  //lcd.print(hum);
  lcd.print(" ");
  lcd.print("S:No");
  delay(50);
}


//---------------------------------------------------Loop Section--------------------------------------------------------------
void loop() 
{
  while(i<1)
  {
    display_developers_name();
  }
  
  //digitalWrite(LED_B, HIGH);
  //digitalWrite(LED_G, HIGH);
  
  int chk = DHT.read11(DHT11_PIN);

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()){
        if(O_tag){
          return;
          }

    //Serial.println("loop inside ");

    //Measure_temp();


 storage=measure(M_trigPin,M_echoPin);
 Serial.println(storage);

  //****************Display Section********************
  lcd.setCursor(0,0);
  lcd.print(" Smart Dustbin ");
  lcd.setCursor(0,1);
  if(storage>19 and storage<MAX_STORAGE){
    lcd.print("     Empty     ");
    digitalWrite(LED_R, LOW);
    }
    else if(storage<=19 and storage>=16){
      lcd.setCursor(0,1);
      lcd.print("  25% Filled     ");
      digitalWrite(LED_R, LOW);
      }
      else if(storage<=15 and storage>=10){
        lcd.setCursor(0,1);
        lcd.print("  50% Filled     ");
        digitalWrite(LED_R, LOW);
      }
      else if(storage<10 and storage>=5){
        lcd.setCursor(0,1);
        lcd.print("  75% Filled     ");
        digitalWrite(LED_R, LOW);
      }
      else if(storage<=4){
        lcd.setCursor(0,1);
        lcd.print("  100% Filled      ");
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
          lcd.noBacklight();
          delay(250);
          lcd.backlight();
          delay(250);
          return;
      }else {
        lcd.setCursor(0,1);
        lcd.print(" Close Properly     ");
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
        return;
      }
    
  
      
  //lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
  //lcd.print("Storage"); // Prints string "Distance" on the LCD
  //lcd.print(distanceCm); // Prints the distance value from the sensor
  //lcd.print(" cm    ");
  //lcd.setCursor(0,1);
  //lcd.print("Distance: ");
  //lcd.print(storage);
  //lcd.print(" inch");
  //delay(50);

   //****************Average Distance Calculation********************
   
  for (int i=0;i<=2;i++) {       //average distance
   dist_open= measure(O_trigPin,O_echoPin);               
   aver[i]=dist_open;            
   delay(10);                  //delay between measurements
  }
 dist_open=(aver[0]+aver[1]+aver[2])/3;    

if ( dist_open<40 ) {
  tone(BUZ, 3000); // Send 1KHz sound signal... 
  delay(500);
  digitalWrite(LED_G, HIGH);
  lcd.setCursor(0,0);
  lcd.print("     Thanks   ");
  lcd.setCursor(0,1);
  lcd.print("  For Using Me  ");
  servo.attach(servoPin);
  delay(1);
  servo.write(0); 
  noTone(BUZ);   // Stop sound...
  delay(3000); 
        
  servo.write(150);    
  delay(1000);
  digitalWrite(LED_G, LOW);
  servo.detach();      
}
//Serial.print(dist);
return;
}








  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  lcd.setCursor(0,0);
  lcd.print(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();

  Serial.print("Tap card key: ");
  Serial.println(strID);


  if (strID.indexOf("CB:AD:A7:1B") >= 0  && tag==0) {
    tone(BUZ, 1000); // Send 1KHz sound signal... 
    Serial1.print(1); 
    tag=1;
    digitalWrite(LED_R, LOW);
    O_tag=true;
    lcd.setCursor(0,0);
    lcd.print("     OPENED   ");
    lcd.setCursor(0,1);
    lcd.print("ID : ");
    lcd.print(strID);
    delay(500);
    noTone(BUZ);
    digitalWrite(LED_G, HIGH);
    servo.attach(servoPin);
    delay(1);
    servo.write(0);
    delay(3000); 
       // Stop sound... 
    //check();
  } else if(strID.indexOf("CB:AD:A7:1B") >= 0  ) {
    tone(BUZ, 1000); // Send 1KHz sound signal... 
   
    Serial1.print(0); 
    tag=0;
    O_tag=false;
    lcd.setCursor(0,0);
    lcd.print("     Thanks   ");
    lcd.setCursor(0,1);
    lcd.print("  For Clean Me   ");
    delay(500);
    noTone(BUZ);
    servo.write(150);    
    delay(1000);
    digitalWrite(LED_G, LOW);
    servo.detach();  
      
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

   
  Serial.println("out");

  //servo.attach(servoPin);
  //delay(1);
  //servo.write(0);  
  //delay(3000);       
  //servo.write(150); 
  
    
  delay(1000);
  //servo.detach();  
    //delay(5000);

}
