//Author:WeiCen Dong

//#include "LedControl.h"

const byte DIN = 5;    //DIN腳位
const byte CLK = 3;     //CLK腳位
const byte CS = 4;      //CS腳位
LedControl lc = LedControl(DIN, CLK, CS, 1);

const byte btnPrava = 13;      //開關腳位
const byte btnLeva =  12;
const byte btnHorni = 11;
const byte btnDolni = 10;

const byte speaker = 9;


int had[8][8] = {0};       //矩陣定義

byte xHad = 4;
byte yHad = 5;
byte xPotrava = 0;
byte yPotrava = 0;

byte smer = 0;
int krok = 1;
long CasZmeny = 0;
int rychlost = 500;
byte skore = 0;

int val = 1;


boolean snezeno = false;
boolean zvuk = true;
boolean probehlo = false;


                        
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};                              //矩陣定義

int Cislo[11][7] = {
  {B11100000, B01000000, B01000000, B01000000, B01000000, B11000000, B01000000}, //1     矩陣LED顯示編碼
  {B11111000, B01000000, B00100000, B00010000, B00001000, B10001000, B01110000}, //2
  {B01110000, B10001000, B00001000, B00010000, B00100000, B00010000, B11111000}, //3
  {B00010000, B00010000, B11111000, B10010000, B01010000, B00110000, B00010000}, //4
  {B01110000, B10001000, B00001000, B00001000, B11110000, B10000000, B11111000}, //5
  {B01110000, B10001000, B10001000, B11110000, B10000000, B01000000, B00110000}, //6
  {B00100000, B00100000, B00100000, B00010000, B00001000, B10001000, B11111000}, //7
  {B01110000, B10001000, B10001000, B01110000, B10001000, B10001000, B01110000}, //8
  {B01100000, B00010000, B00001000, B01111000, B10001000, B10001000, B01110000}, //9
  {B01110000, B10001000, B11001000, B10101000, B10011000, B10001000, B01110000}, //0
  {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000},
};

void setup() {
  Serial.begin(9600);                 // 序列埠傳輸速率
  Serial.println("Snake Game ");      // 序列埠

  randomSeed(analogRead(A0));
  GenerujPotravu();

  pinMode(btnLeva, INPUT_PULLUP);     // 定義腳位為輸入
  pinMode(btnPrava, INPUT_PULLUP);
  pinMode(btnHorni, INPUT_PULLUP);
  pinMode(btnDolni, INPUT_PULLUP);
  pinMode(speaker, OUTPUT);           // 定義腳位為輸出

  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
}

void loop() {
  if ((digitalRead(btnLeva) == false) && (smer != 0)) smer = 1;           //開按下按鍵上 smer = 1
  if ((digitalRead(btnPrava) == false) && (smer != 1)) smer = 0;          //開按下按鍵上 smer = 0
  if ((digitalRead(btnHorni) == false) && (smer != 3)) smer = 2;          //開按下按鍵上 smer = 2
  if ((digitalRead(btnDolni) == false) && (smer != 2)) smer = 3;          //開按下按鍵上 smer = 3

  if ((millis() - CasZmeny) > rychlost) {
    CasZmeny = millis();

    VymazHada(had);
    Skore();                                                              //執行副程式Skore
    Pohyb();                                                              //執行副程式Pohyb

    if (xHad == xPotrava && yHad == yPotrava) snezeno = true;
    if (had[xHad][yHad] != 0) GameOver();                                //當位置重複

    krok++;
    had[xHad][yHad] = krok;
  }
  Vykres(had, xPotrava, yPotrava);
}

void Skore() {
  if (snezeno == true) {
    snezeno = false;
    GenerujPotravu();

    skore++;
    rychlost -= 20;

    Serial.print ("Skore je: ");
    Serial.println (skore);
  }
}

void Vykres(int matice[8][8], byte x, byte y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if ((matice[i][j] >= 1) || ((i == x) && (j == y) )) lc.setLed(0, i, j, true);
      else lc.setLed(0, i, j, false);
    }
  }
}

void VymazHada(int matice[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (matice[i][j] == (krok - skore)) matice[i][j] = 0;
    }
  }
}

void Pohyb() {            //邊界設定
  switch (smer) {
    case 0:
      xHad++;
      break;
    case 1:
      xHad--;
      break;
    case 2:
      yHad++;
      break;
    case 3:
      yHad--;
      break;
  }
  if (xHad == 8) xHad = 0;
  if (yHad == 8) yHad = 0;
  if (xHad == 255) xHad = 7;
  if (yHad == 255) yHad = 7;
}

void GenerujPotravu() {
  lc.setLed(0, xPotrava, yPotrava, false);
  do {
    xPotrava = random(0, 8);
    yPotrava = random(0, 8);
  } while (had[xPotrava][yPotrava] != 0 );
}

void VykresCislo(byte cislo) {
  if (cislo == 0) cislo = 10;
  for (int col = 0; col < 7; col++) {
    lc.setColumn(0, col, Cislo[cislo - 1][col]);
  }
}

void GameOver() {                            //遊戲結束副程式
  Serial.println ("Game over");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (zvuk == true)
      {
        int noteDuration = 1000 / noteDurations[col];           //當分數大於10
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
      }
      lc.setLed(0, col, row, true);
      delay(25);
    }
    zvuk = false;
  }
  for (int row = 0; row < 8; row++)                              //當分數小於10
  {
    for (int col = 0; col < 8; col++)
    {
      lc.setLed(0, col, row, false);
      delay(25);
    }
  }
  do {



    VykresCislo((skore / 10) % 10);    //lED閃爍顯示分數
    delay(1000);
    VykresCislo(skore % 10);
    delay(1000);
    VykresCislo(11);
    delay(1000);



  }
  while (true);
}
