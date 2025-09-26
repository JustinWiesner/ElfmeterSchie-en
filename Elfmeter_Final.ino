
#include "Servo.h";
#include <TM1637Display.h>
int sensorTor = 2;
int elfMeter = 3;
int ledRot = 13;
int ledBlau = 12;
int sound = 6;
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
bool modusReset = true;

#define NOTE_C6  1047
#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E6  1319
#define NOTE_E7  2637
#define NOTE_G6  1568
#define NOTE_G7  3136
#define NOTE_B5   988
#define NOTE_AS5  932
#define NOTE_A5   880
#define NOTE_G5   784
#define NOTE_FS5  740
#define NOTE_F5   698
#define NOTE_E5   659
#define NOTE_DS5  622
#define NOTE_D5   587

//1-Up
int melody1Up[]       = { NOTE_E6, NOTE_G6, NOTE_E7, NOTE_C7, NOTE_D7, NOTE_G7 };
int duration1Up[]     = { 150,     150,     150,     150,     150,     150   };
//1-Down
int melodyShrink[]    = { NOTE_C6, NOTE_B5, NOTE_AS5, NOTE_A5, NOTE_G5,
                          NOTE_FS5, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_D5 };
int durationShrink[]  = { 100, 100, 100, 100, 100,
                          100, 100, 100, 100, 300 };

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
      ModusUndReset();
      break;

    case 1:
      ActionTorWart();
      ModusUndReset();
      break;

    case 2:
      TorCheck();
      ModusUndReset();
      break;

    case 3:
      TorJa();
      ModusUndReset();
      break;

    case 4:
      TorNein();
      ModusUndReset();
      break;

    case 5:
      Versicherung();
      ModusUndReset();
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
      ModusUndReset();
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
      ModusUndReset();
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
      ModusUndReset();
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
      ModusUndReset();
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
    play1Up();
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
    play1Up();

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
    playShrink();

    state = 5;
  } 
  else 
  {
    tore = 0;
    display.showNumberDec(tore, true);
    Serial.println("keinTorEinSpieler");
    playShrink();
    state = 5;
  }
}

void Versicherung() 
{
  if (spielModus == true) 
  {
    while (digitalRead(elfMeter) == LOW) 
    {
      ModusUndReset();
    }
    state = 0;
    Serial.println("versicherung");
  } else {
    while (digitalRead(elfMeter) == LOW) 
    {
      ModusUndReset();
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
    if(modusReset == true)
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
      modusReset = false;
    }
    spielModus = true;
    Serial.println("modus1");
  }
   else 
  {
    if(modusReset == false)
    {
      Serial.println("tasterOne");
      tore = 0;
      display.showNumberDec(tore, true);
      state = 0;
      noTone(sound);
      torwart.write(90);
      digitalWrite(ledRot, LOW);
      digitalWrite(ledBlau, LOW);
      modusReset = true;
    }
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
void ModusUndReset()
{
  Reset();
  ModusWechsel();
}

void play1Up() 
{
  for (size_t i = 0; i < sizeof(melody1Up) / sizeof(melody1Up[0]); i++) 
  {
    tone(sound, melody1Up[i], duration1Up[i]);
    delay(duration1Up[i] + 30);
    noTone(sound);
  }
}

void playShrink() 
{
  for (size_t i = 0; i < sizeof(melodyShrink) / sizeof(melodyShrink[0]); i++) 
  {
    tone(sound, melodyShrink[i], durationShrink[i]);
    delay(durationShrink[i] + 20);
    noTone(sound);
  }
}