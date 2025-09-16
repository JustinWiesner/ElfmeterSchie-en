
#include "Servo.h";
#include <TM1637Display.h>
int sensorTor = 2;
int elfMeter = 3;
int ledRot = 13;
int ledBlau = 12;
int sound = 6;
//int torwart = 7;
int resetTaster = 8;
int CLK = 4;
int DIO = 5;
Servo torwart;
TM1637Display display(CLK, DIO);
int randompos[5] = { 0, 45, 90, 135, 180 };
int toreSP1 = 0;
int toreSP2 = 0;
int tore = 0;
int state = 0;
unsigned long stopUhrStart;
bool aktuellerSP = true;
int modusWechselTaster = A1;
bool spielModus = true;


void setup() 
{
  Serial.begin(9600);
  pinMode(ledRot, OUTPUT);
  pinMode(ledBlau, OUTPUT);
  pinMode(resetTaster, INPUT_PULLUP);
  pinMode(sensorTor, INPUT_PULLUP);
  pinMode(elfMeter, INPUT_PULLUP);
  pinMode(sound, OUTPUT);
  display.setBrightness(7);
  display.clear();
  torwart.attach(7);
  randomSeed(analogRead(0));
  pinMode(modusWechselTaster, INPUT_PULLUP);
}

void loop() 
{
  switch (state) 
  {
    case 0:
      Schuss();
      ModusWechsel();
      Reset();
      break;

    case 1:
      ActionTorWart();
      ModusWechsel();
      Reset();
      break;

    case 2:
      TorCheck();
      ModusWechsel();
      Reset();
      break;

    case 3:
      TorJa();
      ModusWechsel();
      Reset();
      break;

    case 4:
      TorNein();
      ModusWechsel();
      Reset();
      break;

    case 5:
      Versicherung();
      ModusWechsel();
      Reset();
      break;
  }
}

void Reset() 
{
  if (spielModus == true) 
  {
    while (digitalRead(resetTaster) == LOW) 
    {
      Serial.println("taster");
      toreSP1 = 0;
      toreSP2 = 0;
      display.showNumberDecEx(toreSP1 * 100 + toreSP2, 0b01000000, true);
      state = 0;
      noTone(sound);
      torwart.write(90);
      digitalWrite(ledRot, LOW);
      digitalWrite(ledBlau, LOW);
    }
  }
   else 
   {
    while (digitalRead(resetTaster) == LOW) 
    {
      Serial.println("tasterOne");
      tore = 0;
      display.showNumberDec(tore, true);
      state = 0;
      noTone(sound);
      torwart.write(90);
      digitalWrite(ledRot, LOW);
      digitalWrite(ledBlau, LOW);
    }
  }
}

void Schuss() 
{
  if (spielModus == true) 
  {
    display.showNumberDecEx(toreSP1 * 100 + toreSP2, 0b01000000, true);
    torwart.write(90);
    delay(1500);
    while (digitalRead(elfMeter) == HIGH) 
    {
      ModusWechsel();
      Reset();
    }
    stopUhrStart = millis();
    state = 1;
    Serial.println("schuss");
  } 
  else
  {
    display.showNumberDec(tore, true);
    torwart.write(90);
    delay(1500);
    while (digitalRead(elfMeter) == HIGH) 
    {
      ModusWechsel();
      Reset();
    }
    stopUhrStart = millis();
    state = 1;
    Serial.println("schussEinSpieler");
  }
}

void ActionTorWart() 
{
  if (spielModus == true) 
  {
    delay(50);
    torwart.write(randompos[random(0, 5)]);
    Serial.println("torwartEinSpieler");
    state = 2;
  } else {
    delay(50);
    torwart.write(randompos[random(0, 5)]);
    Serial.println("torwart");
    state = 2;
  }
}

void TorCheck() {
  if (spielModus == true) 
  {
    state = 3;
    while (digitalRead(sensorTor) == LOW) 
    {
      ModusWechsel();
      Reset();
      if (millis() - stopUhrStart > 3000) 
      {
        state = 4;
        break;
      }
    }
  } 
  else 
  {
    state = 3;
    while (digitalRead(sensorTor) == LOW) 
    {
      ModusWechsel();
      Reset();
      if (millis() - stopUhrStart > 3000) 
      {
        state = 4;
        break;
      }
    }
  }
}

void TorJa() 
{
  if (spielModus == true) 
  {
    SpielerWechsel();
    Serial.println("Tor");
    delay(200);
    LichterShow();
    tone(sound, 1000, 1000);
    if (toreSP1 == 10 || toreSP2 == 10) 
    {
      toreSP1 = 0;
      toreSP2 = 0;
      display.showNumberDecEx(toreSP1 * 100 + toreSP2, 0b01000000, true);
    }
    state = 5;
  } 
  else 
  {
    tore++;
    display.showNumberDec(tore, true);
    Serial.println("TorEinSpieler");
    delay(200);
    LichterShow();
    tone(sound, 1000, 1000);

    state = 5;
  }
}


void TorNein() 
{
  if (spielModus == true) 
  {
    if (aktuellerSP == false) 
    {
      aktuellerSP = true;
    } 
    else 
    {
      aktuellerSP = false;
    }
    if (toreSP1 == 10 || toreSP2 == 10) 
    {
      toreSP1 = 0;
      toreSP2 = 0;
      display.showNumberDecEx(toreSP1 * 100 + toreSP2, 0b01000000, true);
    }
    Serial.println("keinTor");
    tone(sound, 200, 1000);

    state = 5;
  } 
  else 
  {
    tore = 0;
    display.showNumberDec(tore, true);
    Serial.println("keinTorEinSpieler");
    tone(sound, 200, 1000);
    state = 5;
  }
}

void Versicherung() 
{
  if (spielModus == true) 
  {
    while (digitalRead(elfMeter) == LOW) 
    {
      ModusWechsel();
      Reset();
    }
    state = 0;
    Serial.println("versicherung");
  } else {
    while (digitalRead(elfMeter) == LOW) 
    {
      ModusWechsel();
      Reset();
    }
    state = 0;
    Serial.println("versicherungEinSpieler");
  }
}

void LichterShow() 
{
  digitalWrite(ledRot, HIGH);
  digitalWrite(ledBlau, HIGH);
  delay(200);
  digitalWrite(ledRot, LOW);
  digitalWrite(ledBlau, LOW);
  delay(200);
}

void ModusWechsel() 
{
  if (digitalRead(modusWechselTaster) == LOW) 
  {
    spielModus = true;
    Serial.println("modus1");
  } else {
    spielModus = false;
    Serial.println("modus2");
  }
}

void SpielerWechsel() 
{
  if (aktuellerSP == false) 
  {
    toreSP1++;
    aktuellerSP = true;
  } 
  else 
  {
    toreSP2++;
    aktuellerSP = false;
  }
  display.showNumberDecEx(toreSP1 * 100 + toreSP2, 0b01000000, true);
}
