
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
bool aktuellerSP = true;
int ModusWechselTaster = A1;
bool spielModus = true;
#define ModusWechselTaster A1


void setup() 
{
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
  pinMode(ModusWechselTaster, INPUT_PULLUP);

  
  
}

void loop() 
{
  switch (state) 
  {
  
    case 0:
      schuss();
      modusWechsel ();
      Reset();
      break;

    case 1:
      torwartAction();
      modusWechsel ();
      Reset();
      break;

    case 2:
      uberprufung();
      modusWechsel ();
      Reset();
      break;

    case 3:
      torJaOf();
      modusWechsel ();
      Reset();
      break;

    case 4:
      torNeinOf();
      modusWechsel ();
      Reset();
      break;

    case 5:
     versicherungOf();
     modusWechsel ();
      Reset();
      break;
  } 
}
void Reset()
{
if (spielModus == true)
  {
    RESETone();
  }
else
  {
    RESET();
  }
}

void schuss()
{
if (spielModus == true)
  {
    einSpielerSchuss();
  }
else
  {
    ballGeschossenOderNicht();
  }
}

void torwartAction()
{
  if (spielModus == true)
  {
    actionTorwartEinSpieler();
  }
  else
  {
    actionTorwart();
  }
}

void uberprufung()
{
  if (spielModus == true)
  {
    torCheckEinSpieler();
  }
else
  {
    torCheck();
  }
}

void torJaOf()
{
  if (spielModus == true)
  {
  	torJaEinSpieler();
  }
else
  {
    torJa();
  }
}

void torNeinOf()
{
  if (spielModus == true)
  {
    torNeinEinSpieler();
  }
else
  {
    torNein();
  }
}

 void versicherungOf()
{
  if (spielModus == true)
  {
    versicherungEinSpieler();
  }
else
  {
    versicherung();
  }
}


void RESET() 
{
  while (digitalRead(resetTaster) == LOW) 
  {
    Serial.println("taster");
    ToreSP1 = 0;
    ToreSP2 = 0;
    display.showNumberDecEx(ToreSP1 * 100 + ToreSP2, 0b01000000, true);
    state = 0;
    noTone(sound);
    torwart.write(90);
    digitalWrite(LEDrot, LOW);
    digitalWrite(LEDblau, LOW);
  }
}


void ballGeschossenOderNicht() 
{
  display.showNumberDecEx(ToreSP1 * 100 + ToreSP2, 0b01000000, true);
  torwart.write(90);
  delay(1500);
  while (digitalRead(elfmeter) == HIGH) 
  {
    modusWechsel();
    RESET();
  }
  stopuhrstart = millis();
  state = 1;
  Serial.println("schuss");
}

void actionTorwart() 
{
  delay(50);
  torwart.write(randompos[random(0, 5)]);
  Serial.println("torwart");
  state = 2;
}

void torCheck() 
{
  state = 3;
  while (digitalRead(sensorTor) == LOW) 
  {
    modusWechsel();
    RESET();
    if (millis() - stopuhrstart > 3000) 
    {
      state = 4;
      break;
    }
  }
}

void torJa() 
{
  spielerWechsel();
  Serial.println("Tor");
  delay(200);
  lichterShow();
  tone(sound, 1000, 1000);
  if (ToreSP1 == 10 || ToreSP2 == 10) 
  {
    ToreSP1 = 0; ToreSP2 = 0;
    display.showNumberDecEx(ToreSP1 * 100 + ToreSP2, 0b01000000, true);
  }
  state = 5;
}


void torNein()
 {
  if (aktuellerSP == false)
  {
    aktuellerSP = true;
  }
  else
  {
    aktuellerSP = false;
  }
  if (ToreSP1 == 10 || ToreSP2 == 10) 
  {
    ToreSP1 = 0; ToreSP2 = 0;
    display.showNumberDecEx(ToreSP1 * 100 + ToreSP2, 0b01000000, true);
  }
  Serial.println("keinTor");
  tone(sound, 200, 1000);
  
  state = 5;
}


void spielerWechsel() 
{
  if (aktuellerSP == false) 
  {
    ToreSP1++;
    aktuellerSP = true;
  } 
  else 
  {
    ToreSP2++;
    aktuellerSP = false;
  }
  display.showNumberDecEx(ToreSP1 * 100 + ToreSP2, 0b01000000, true);
}


void lichterShow() 
{
  digitalWrite(LEDrot, HIGH);
  digitalWrite(LEDblau, HIGH);
  delay(1000);
  digitalWrite(LEDrot, LOW);
  digitalWrite(LEDblau, LOW);
}

void versicherung() 
{
  while (digitalRead(elfmeter) == LOW) 
  {
    modusWechsel();
    RESET();
  }
  state = 0;
  Serial.println("versicherung");
}


void RESETone() 
{
  while (digitalRead(resetTaster) == LOW) 
  {
    Serial.println("tasterOne");
    Tore = 0;
    display.showNumberDec(Tore, true);
    state = 0;
    noTone(sound);
    torwart.write(90);
    digitalWrite(LEDrot, LOW);
    digitalWrite(LEDblau, LOW);
  }
}


void einSpielerSchuss() 
{
   display.showNumberDec(Tore, true);
  torwart.write(90);
  delay(1500);
  while (digitalRead(elfmeter) == HIGH) 
  {
    modusWechsel();
    RESETone();
  }
  stopuhrstart = millis();
  state = 1;
  Serial.println("schussEinSpieler");
}

void actionTorwartEinSpieler() 
{
  delay(50);
  torwart.write(randompos[random(0, 5)]);
  Serial.println("torwartEinSpieler");
  state = 2;
}

void torCheckEinSpieler() 
{

  state = 3;
  while (digitalRead(sensorTor) == LOW) 
  {
    modusWechsel();
    RESETone();
    if (millis() - stopuhrstart > 3000) 
    {
      state = 4;
      break;
    }
  }
}

void torJaEinSpieler() 
{
  Tore++;
  display.showNumberDec(Tore, true);
  Serial.println("TorEinSpieler");
  delay(200);
  lichterShowEinSpieler();
  tone(sound, 1000, 1000);
  
  state = 5;
}


void torNeinEinSpieler()
{
  Tore = 0;
  display.showNumberDec(Tore, true);
  Serial.println("keinTorEinSpieler");
  tone(sound, 200, 1000);
  state = 5;
}

void lichterShowEinSpieler() 
{
  digitalWrite(LEDrot, HIGH);
  digitalWrite(LEDblau, HIGH);
  delay(200);
  digitalWrite(LEDrot, LOW);
  digitalWrite(LEDblau, LOW);
  delay(200);
}

void versicherungEinSpieler() 
{
  while (digitalRead(elfmeter) == LOW) 
  {
    modusWechsel();
    RESETone();
  }
  state = 0;
  Serial.println("versicherungEinSpieler");
}


void modusWechsel ()
{
  if (digitalRead(ModusWechselTaster) == LOW)
  {
    spielModus = true;
    Serial.println("modus1");
  }
  else
  {
  spielModus = false;
  Serial.println("modus2");
  }
}
