
#include "Servo.h";
#include <TM1637Display.h>

int sensorTor = 2;
int elfmeter = 3;
int LEDrot = 13;
int LEDblau = 12;
int sound = 6;
int Torwart = 7;
int resetTaster = 8;
int CLK = 4;
int DIO = 5;
Servo torwart;
TM1637Display display(CLK, DIO);
int randompos[5] = { 0, 45, 90, 135, 180 };
int ToreSP1 = 0;
int ToreSP2 = 0;
int Tore = 0;
int state = 0;
unsigned long stopuhrstart;
bool aktuellerSP = false;
bool spielModus = true;

void setup() {
  Serial.begin(9600);
  pinMode(LEDrot, OUTPUT);
  pinMode(LEDblau, OUTPUT);
  pinMode(resetTaster, INPUT_PULLUP);
  pinMode(sensorTor, INPUT_PULLUP);
  pinMode(elfmeter, INPUT_PULLUP);
  pinMode(sound, OUTPUT);
  display.setBrightness(7);
  display.clear();
  torwart.attach(7);
  randomSeed(analogRead(0));
  display.showNumberDec(Tore, true);
}


void loop() 
{
  
  switch (state) 
  {
    case 0:
      ballGeschossenOderNicht();
      RESET();
      break;

    case 1:
      actionTorwart();
      RESET();
      break;

    case 2:
      torCheck();
      RESET();
      break;

    case 3:
      torJa();
      RESET();
      break;

    case 4:
      torNein();
      RESET();
      break;

    case 5:
      versicherung();
      RESET();
      break;
  }
}

void RESET() {
  while (digitalRead(resetTaster) == LOW) {
    Serial.println("taster");
    Tore = 0;
    display.showNumberDec(Tore, true);
    state = 0;
    noTone(sound);
    torwart.write(90);
    digitalWrite(LEDrot, LOW);
    digitalWrite(LEDblau, LOW);
  }
}


void ballGeschossenOderNicht() {
  torwart.write(90);
  delay(1500);
  while (digitalRead(elfmeter) == HIGH) 
  {
    RESET();
  }
  stopuhrstart = millis();
  state = 1;
  Serial.println("schuss");
}

void actionTorwart() {
  delay(50);
  torwart.write(randompos[random(0, 5)]);
  Serial.println("torwart");
  state = 2;
}

void torCheck() {

  state = 3;
  while (digitalRead(sensorTor) == HIGH) {
    RESET();
    if (millis() - stopuhrstart > 3000) {
      state = 4;
      break;
    }
  }
}

void torJa() {
  Tore++;
  display.showNumberDec(Tore, true);
  Serial.println("Tor");
  delay(200);
  lichterShow();
  tone(sound, 1000, 1000);
  
  
  state = 5;
}


void torNein()
 {
    Tore = 0;
    display.showNumberDec(Tore, true);
    Serial.println("keinTor");
    tone(sound, 200, 1000);
    state = 5;
}



void lichterShow() {
  digitalWrite(LEDrot, HIGH);
  digitalWrite(LEDblau, HIGH);
  delay(200);
  digitalWrite(LEDrot, LOW);
  digitalWrite(LEDblau, LOW);
  delay(200);
}

void versicherung() {
  while (digitalRead(elfmeter) == LOW) 
  {
    RESET();
  }
  state = 0;
  Serial.println("versicherung");
}