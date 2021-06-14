
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//definirea pinilor utilizati pentru primul senzor de proximitate
#define trigPin1 3                                   
#define echoPin1 2       // vom folosi acest pin pentru a citi semnalul de la senzor


//definirea pinilor utilizati pentru al doilea senzor de proximitate
#define trigPin2 10
#define echoPin2 9
#define LED 13
#define RELEU 6


//variabile utilizate
long duration, distance, UltraSensor1, UltraSensor2; //vom folosi aceste variabile pentru a stoca si genera date

char data;
char sms;
char OK;

String SerialData="";
String sequence = "";

int currentPeople = 0;
int timeoutCounter = 0;


void setup()
{
  Serial.begin(9600);                              
  lcd.begin(); // Initializarea display-ului
  lcd.backlight();
  // setarea pinilor pentru primul senzor
  pinMode(trigPin1, OUTPUT);             // from where we will transmit the ultrasonic wave
  pinMode(echoPin1, INPUT);              // from where we will read the reflected wave 
  pinMode(LED, OUTPUT);                  // from where we will control the LED
  pinMode(RELEU, OUTPUT);                // from where we will control the relay
  
  //setarea pinilor pentru al doilea senzor
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  //initializarea starii LED-ului 
  digitalWrite(LED,LOW);

  //initializarea starii releului
  digitalWrite(RELEU, HIGH);
}

void loop() 
{
  
  SonarSensor(trigPin1, echoPin1);              // apelam functia SonarSensor cu pinii senzorului 1
  UltraSensor1 = distance;                      // Stocam distanta calculata in variabila pentru senzorul 1
  SonarSensor(trigPin2,echoPin2);               // apelam functia SonarSensor cu pinii senzorului 2
  UltraSensor2 = distance;                      // Stocam distanta calculata in variabila pentru senzorul 2

  while(Serial.available())
  { 
    delay(10);
     if(Serial.available() != 0)
      {
        sms = Serial.read();
      }
      if(sms == 'a')
      {
        digitalWrite(RELEU, LOW);
        OK = 'a';
      }
      if( sms == 'b')
      {
        digitalWrite(RELEU, HIGH);
        OK = 'b';
      }

     if( sms == 'c')
      {
        OK = 'c';
      }
  }

 
  
  //Daca distanta citita de senzorul 1 este mai mica decat 50cm, atunci scriem 1 in secventa sequence.
  if(UltraSensor1 <= 50)
    {
    sequence += "1";
    }
  // Daca distanta citita de senzorul 2 este mai mica decat 50cm, atunci scriem 2 in secventa sequence.
  else if (UltraSensor2 <= 50){
    sequence += "2";
  }

  //Daca secventa contine mesajul "12", insemna ca primul senzor a fost activat primul, deci o persoana a intrat in camera
  if(sequence.equals("12")){
    currentPeople++;        // Incrementam numarul de persoane din camera
    sequence="";            // Resetam secventa
    //delay(100);
    Serial.print(currentPeople);
  }
  //Daca secventa contine mesajul "21", insemna ca al doilea senzor a fost activat primul, deci o persoana a iesit din camera
  else if(sequence.equals("21") && currentPeople > 0){
    currentPeople--;        // Decrementam numarul de persoane din camera
    sequence="";            // Resetam secventa
    Serial.print(currentPeople);
    //delay(10);
  }
  
  //Resetam secventa daca rezultatul e invalid, adica contine 11 / 22 sau mai mult de 2 cifre
  if(sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 200){
    sequence="";  
  }

  if(sequence.length() == 1){ //
    timeoutCounter++;
  }else{
    timeoutCounter=0;
  }

  // Verificam daca in camera exista cel putin o persoana.
  // Daca da, LED-ul se activeaza, iar pe display este afisat daca lumina este aprinsa sau nu.
  if( OK == 'c') 
  {
    if(currentPeople >= 1)
    {
      digitalWrite(RELEU, LOW);
      digitalWrite(LED, HIGH);
      lcd.setCursor(0,1);  
      lcd.print("Light is on ");
      //Serial.print("Light is on");
    }else{
      digitalWrite(RELEU, HIGH);
      digitalWrite(LED, LOW);
      lcd.setCursor(0,1);  
      lcd.print("Light is off ");
      //Serial.print("Light is off");
    }
  }
  
  // Afisam pe display numarul de persoane din camera
  lcd.setCursor(0,0);  
  lcd.print("People in room ");
  lcd.print(currentPeople);
  //delay(100); 
}

// Functia SonarSenzor
void SonarSensor(int trigPinSensor,int echoPinSensor)
{

  digitalWrite(trigPinSensor, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPinSensor, LOW);
  
  //  citim distanta
  duration = pulseIn(echoPinSensor, HIGH);// functia pulseIn returneaza cat timp senzorul echopin a fost HIGH
  distance= (duration/2) / 29.1;          // calculul distantei  
}
