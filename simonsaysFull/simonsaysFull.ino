#include <ServoTimer2.h>
#include <AccelStepper.h>
#include <Stepper.h>
#include <toneAC.h>

// C++ code
//

int led1(47);
int led2(49);
int led3(51);
int led4(53);
int ledPins[] = {47,49,51,53};
int button1(46);
int button2(48);
int button3(50);
int button4(52);
int startCoin(22);
ServoTimer2 myServo;

const int totalPositions = 12;
int stepCount = 0;
// Define the number of steps per revolution for the stepper motors
const int stepsPerRevolution = 1024;

// Motor pin definitions:
#define motorPin1  2      // IN1 on the ULN2003 driver
#define motorPin2  3     // IN2 on the ULN2003 driver
#define motorPin3  4     // IN3 on the ULN2003 driver
#define motorPin4  5     // IN4 on the ULN2003 driver

#define motor2Pin1  6      // IN1 on the ULN2003 driver
#define motor2Pin2  7      // IN2 on the ULN2003 driver
#define motor2Pin3  8     // IN3 on the ULN2003 driver
#define motor2Pin4  9     // IN4 on the ULN2003 driver
// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper1 = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2 = AccelStepper(MotorInterfaceType, motor2Pin1, motor2Pin3, motor2Pin2, motor2Pin4);
int stepperspeed = 1000;

const int levelMax(100);
int mySequence[levelMax];
int yourSequence[levelMax];
int pitchArray[levelMax];
int servoSequence[levelMax] = {0};
int caller(0);
long pitches[4] = {262,330,392,523};
int positions[7] = {-45, -30, -15, 0, 15, 30, 45};
int positionsH[4] = {-45, -30, -15, 0};
int positionsL[4] = {0, 15, 30, 45};
int level(0);
int ones(0);
int tens(0);
  
int velocity = 1000;

int pos = 90;
int posLatch = 90;

#define NOTE_A1 55.00
#define NOTE_Bb1 58.27
#define NOTE_B1 61.74
#define NOTE_C2 65.41
#define NOTE_C4 261.63
#define NOTE_D4 293.66
#define NOTE_E4 329.63
#define NOTE_F4 349.23
#define NOTE_G4 392.00
#define NOTE_A4 440.00
#define NOTE_B4 493.88
#define NOTE_C5 523.25
#define NOTE_REST 0

long melody[] = {
  NOTE_G4, NOTE_A4, NOTE_G4,
  NOTE_E4, NOTE_C5, NOTE_C5,
  NOTE_C4,
};

long noteDurations[] = {
  16, 8, 16,
  8, 8, 4,
  4,
};

long melody1[] = {
  NOTE_C2, NOTE_B1, NOTE_Bb1, NOTE_A1,
};

long noteDurations1[] = {
  6, 6, 6, 1,
};

void setup() 
{ 
    Serial.begin(9600); //initialize serial communication
  	pinMode(button1, INPUT); //define inputs
  	pinMode(button2, INPUT);
  	pinMode(button3, INPUT);
  	pinMode(button4, INPUT);
  	pinMode(startCoin, OUTPUT);
    pinMode(10, OUTPUT); //define outputs
    pinMode(11, OUTPUT);
  	pinMode(led1, OUTPUT);
  	pinMode(led2, OUTPUT);
  	pinMode(led3, OUTPUT);
  	pinMode(led4, OUTPUT);
    for (int i = 0; i < 4; i++) {
      pinMode(ledPins[i], OUTPUT);
    }
  	myServo.attach(0);
  	myServo.write(90);
  	servoSequence[0] = 90;

    stepper1.setMaxSpeed(500);
    stepper1.setAcceleration(500);
    stepper2.setMaxSpeed(500);
    // Set the maximum acceleration in steps per second^2:
    stepper2.setAcceleration(500);
}

void loop()
{
  if  (level == 0){
	createSequence(); //generate a sequence
  }
  
  if (digitalRead(startCoin) == HIGH && level == 0){
    stepper1.moveTo(-165);
    stepper2.moveTo(-165);
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); ++i) {
    	long noteDuration = 1000 / noteDurations[i];
    	digitalWrite(ledPins[i%2],HIGH);
      digitalWrite(ledPins[i%2+2],HIGH);
      toneAC(melody[i], 10, noteDuration);
      delay(2*noteDuration);
      // Stop the tone playing:
    	noToneAC();
      digitalWrite(ledPins[i%2],LOW);
      digitalWrite(ledPins[i%2+2],LOW);
  }
    delay(750);
    giveSequence();
    receiveSequence();
  }
  
  if (level!=0){
    giveSequence();
    receiveSequence();
  }
}


void giveSequence()
{	
  digitalWrite(8, LOW);
	digitalWrite(9, LOW);
	digitalWrite(10, LOW);
	digitalWrite(11, LOW);

	for (int i = 0; i < level+1; i++)
	{
      pos = servoSequence[i];
      move();
      toneAC(pitchArray[i],10,velocity);
      digitalWrite(mySequence[i],  HIGH);
      delay(velocity);
      digitalWrite(mySequence[i], LOW);
	  delay(200);
	}
  	delay(200);
  	myServo.write(90);
}

void  receiveSequence() //get the player's input
{
  for  (int i = 0; i < level+1; i++){
    int flag = 0;
    while (flag == 0){
      
      if (digitalRead(button1) == LOW){
        yourSequence[i] = 8;
        toneAC(pitches[0],10,400);
        digitalWrite(led1, HIGH);
        flag = 1;
        delay(400);
        if (yourSequence[i] != mySequence[i]){ //check correctness
          incorrect();
          return;
      	}
        digitalWrite(led1, LOW);
      }
      
      if (digitalRead(button2) == LOW){
        yourSequence[i] = 9;
        toneAC(pitches[1],10,400);
        digitalWrite(led2, HIGH);
        flag = 1;
        delay(400);
        if (yourSequence[i] != mySequence[i]){ //check correctness
          incorrect();
          return;
      	}
        digitalWrite(led2, LOW);
      }
      
      if (digitalRead(button3) == LOW){
        yourSequence[i] = 10;
        toneAC(pitches[2],10,400);
        digitalWrite(led3, HIGH);
        flag = 1;
        delay(400);
        if (yourSequence[i] != mySequence[i]){ //check correctness
          incorrect();
          return;
      	}
        digitalWrite(led3, LOW);
      }
      
      if (digitalRead(button4) == LOW){
        yourSequence[i] = 11;
        toneAC(pitches[3],10,400);
        digitalWrite(led4, HIGH);
        flag = 1;
        delay(400);
        if (yourSequence[i] != mySequence[i]){ //check correctness
          incorrect();
          return;
      	}
        digitalWrite(led4, LOW);
      }
    }
  }
  delay(500);
  correct();
}

void createSequence()
{
  randomSeed(millis());
  for (int i = 0; i < levelMax; i++){
    caller = random(0,4);
    pitchArray[i] = pitches[caller];
    mySequence[i] = ledPins[caller];
  }
  pos = 90;
  for (int i = 1; i < levelMax; i++){
    if (pos >= 180){
      caller = random(0,4);
      pos += positionsH[caller];
      servoSequence[i] = pos;
    }
    if (pos <= 180){
      caller = random(0,4);
      pos += positionsL[caller];
      servoSequence[i] = pos;
    }
    else{
   	  caller = random(0,7);
      pos += positions[caller];
      if(pos > 180){
        pos = 180;
      }
      else if(pos < 0){
        pos = 0;
      }
      servoSequence[i] = pos;
    }
  }
}

void incorrect()
{
  for (int i = 0; i < sizeof(melody1) / sizeof(melody1[0]); ++i) {
    	long noteDuration = 1000 / noteDurations1[i];
    	digitalWrite(ledPins[0],HIGH);
      	digitalWrite(ledPins[1],HIGH);
      	digitalWrite(ledPins[2],HIGH);
      	digitalWrite(ledPins[3],HIGH);
      	toneAC(melody1[i],10, noteDuration);
        delay(noteDuration);
      	digitalWrite(ledPins[0],LOW);
      	digitalWrite(ledPins[1],LOW);
      	digitalWrite(ledPins[2],LOW);
      	digitalWrite(ledPins[3],LOW);
      	noToneAC();
      	delay(noteDuration);
  }
  
  resetStepper();

  level = 0;
  ones = 0;
  tens = 0;
  velocity = 1000;
}

void correct()
{
  digitalWrite(led1,  LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  delay(150);
  toneAC(660,10,100);
  delay(100);
  toneAC(784,10,250);
  digitalWrite(led1,  HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);
  delay(1000);
  digitalWrite(led1,  LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  delay(500);

  if  (level < levelMax){
    level++;
    incrementStepper();
    velocity -= 9; //increase difficulty
  }
}

void move()
{
  unsigned int rate = 100 / abs(pos-posLatch);
  if (pos > posLatch){
    for (; posLatch <= pos; posLatch+=1){
      myServo.write(posLatch);
      delay(rate);
    }
  }
  
  if (pos < posLatch){
    for (; posLatch >= pos; posLatch-=1){
      myServo.write(posLatch);
      delay(rate);
    }
  }
  
  else {
    delay(100);
  }
  posLatch = pos;  
}

void incrementStepper()
{
  if(level>9){
    ones = level%10;
    tens = level/10;
  }
  else{
    ones = level;
    tens = 0;
  }

  if(ones==0 &! tens==0){
    for(int i = 0; i < 165 ; i++){
        stepper1.run();
        stepper2.run();
    }
    for(int i = 0; i < 330 ; i++){
        stepper1.run();
    }
  }
  if(ones!=0){
    for(int i = 0; i < 165 ; i++){
        stepper1.run();
    }
  }
}

void resetStepper()
{
  if(level>9){
    ones = level%10;
    tens = level/10;
  }
  else{
    ones = level;
    tens = 0;
  }
  for(int i = 0; i < (11-ones)*165 ; i++){
        stepper1.run();
  }
  for(int i = 0; i < (11-tens)*165 ; i++){
        stepper2.run();
  }
}