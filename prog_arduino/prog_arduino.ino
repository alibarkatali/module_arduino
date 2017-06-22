#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 

    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result

    if (adc_key_in > 1000) return btnNONE; 

    // For V1.1 us this threshold
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;  

    return btnNONE;                // when all others fail, return this.
}




typedef enum {
  H_INIT,
  H_IDLE,
  H_TICK_H,
  H_TICK_D
} HORLOGE_STATE;

// Etats courants
HORLOGE_STATE horlogeState;

// Variables globales
unsigned long timer = 0;
int tempoHour = 1000;
int nb_h = 0, nb_d = 0;

// Définitions des fonctions de la tâche Horloge
void horlogeInit();
void horlogeUpdate();
void horlogeOutput();

bool ledOnOFF = LOW;

/******************* Main *********************/

void setup() {
  
 lcd.begin(16, 2);               // start the library
 lcd.setCursor(0,0);             // set the LCD cursor   position 
 lcd.print("Push the buttons");  // print a simple message on the LCD
  
  // Initialisation de toutes les machines à états
  horlogeInit();
}

void loop() {
  // Mise à jour des états
  horlogeUpdate();

  // Mise à jour des sorties
  horlogeOutput();
}

/**********************************************/

void horlogeInit() {
  horlogeState = H_INIT;
  pinMode(13, OUTPUT);
}

void horlogeUpdate()
{
  HORLOGE_STATE nextState = horlogeState;

  switch (horlogeState)
  {
    case H_INIT :
      timer = millis() + tempoHour;
      nextState = H_IDLE;
      break;
    case H_IDLE :
      if (timer < millis()) nextState = H_TICK_H;
      break;
    case H_TICK_H :
      nb_h ++;
      if (nb_h == 24){
        nb_d++;
        nb_h = 0;
        nextState = H_TICK_D;
      } else if (nb_h < 24) {
        nextState = H_INIT;
      }
      break;
    case H_TICK_D :
      nextState = H_INIT;
      break;
  }
  horlogeState = nextState;
}

void horlogeOutput() {
  switch (horlogeState)
  {
    case H_INIT :
      lcd.setCursor(0,1);
      lcd.print(nb_h);
      lcd.print(" ");
      break;
    case H_IDLE :
      break;
    case H_TICK_H :
      ledOnOFF = 1 - ledOnOFF;
      digitalWrite(13,ledOnOFF);
      break;
    case H_TICK_D :
      break;
  }
}

