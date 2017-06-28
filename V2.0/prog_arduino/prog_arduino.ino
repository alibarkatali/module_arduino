#include <LiquidCrystal.h>
#include <math.h>


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

#define RAINNY        0
#define CLOUDY        1
#define SUNNY         2
#define HEATWAVE      3
#define THUNDERSTORM  4

typedef enum {
  H_INIT,
  H_IDLE,
  H_TICK_H,
  H_TICK_D
} HORLOGE_STATE;

typedef enum {
  M_IDLE,
  M_SELECT
} METROLOGY_STATE;

typedef enum {
  rainny,
  cloudy,
  sunny,
  heatwave,
  thunderstorm
} WEATHER;

typedef enum {
  C_IDLE,
  C_ON,
  C_HOLD
} CLICK_STATE;

// Etats courants
HORLOGE_STATE horloge_state;
METROLOGY_STATE metrology_state;
CLICK_STATE click_state;

// Variables globales
unsigned long timer = 0, click_timer = 0;
int tempo_hour = 12500;
int nb_h = 0, nb_d = 0, h_from_begin;
bool tick_hour = 0, tick_day = 0;
int current_weather, forecast_weather;

// Définitions des fonctions de la tâche Horloge
void horloge_init();
void horloge_update();
void horloge_output();

// Définitions des fonctions de la tâche Metrology
void metrology_init();
void metrology_update();
void metrology_output();

// Définitions des fonctions de la tâche Click
void click_init();
void click_update();
void click_output();

// Définitions des fonctions de l'affichage
int read_LCD_buttons();
void send_data();


/******************* Main *********************/

void setup() {
  // start serial
  Serial.begin(9600);

  // start the library
  lcd.begin(16, 2);

  // Initialisation de toutes les machines à états
  horloge_init();
  metrology_init();
  click_init();
  send_data();
}

void loop() {
  // Mise à jour des états
  horloge_update();
  metrology_update();
  click_update();

  // Mise à jour des sorties
  horloge_output();
  metrology_output();
  click_output();

}


/**************** Screen Functions ******************/

int read_LCD_buttons() {              // read the buttons
  adc_key_in = analogRead(0);       // read the value from the sensor

  // my buttons when read are centered at these values: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;

  return btnNONE;                // when all others fail, return this.
}


/**************** Horloge functions ******************/

void horloge_init() {
  horloge_state = H_INIT;
}

void horloge_update()
{
  HORLOGE_STATE next_state = horloge_state;

  switch (horloge_state)
  {
    case H_INIT :
      timer = millis() + tempo_hour;
      next_state = H_IDLE;
      break;
    case H_IDLE :
      if (timer < millis()) next_state = H_TICK_H;
      break;
    case H_TICK_H :
      nb_h ++;
      if (nb_h == 24) {
        nb_d++;
        nb_h = 0;
        next_state = H_TICK_D;
      } else if (nb_h < 24) {
        next_state = H_INIT;
      }
      break;
    case H_TICK_D :
      next_state = H_INIT;
      break;
  }
  horloge_state = next_state;
}

void horloge_output() {
  switch (horloge_state)
  {
    case H_INIT :
      tick_hour = 0;
      tick_day = 0;
      break;
    case H_IDLE :
      break;
    case H_TICK_H :
      send_data();
      tick_hour = 1;
      break;
    case H_TICK_D :
      tick_day = 1;
      break;
  }
}


/**************** Metrology functions ******************/

void metrology_init() {
  metrology_state = M_IDLE;
  current_weather = SUNNY;
  forecast_weather = CLOUDY;
  lcd.setCursor(0, 0);
  lcd.print("SUNNY          ");
  lcd.setCursor(0, 1);
  lcd.print("CLOUDY           ");

}

void metrology_update() {
  METROLOGY_STATE next_state = metrology_state;

  switch (metrology_state)
  {
    case M_IDLE :
      if (tick_day) next_state = M_SELECT;
      break;
    case M_SELECT :
      next_state = M_IDLE;
      break;
  }
  metrology_state = next_state;
}

void metrology_output() {

  switch (metrology_state)
  {
    case M_IDLE :
      break;
    case M_SELECT :

      int rand_current = random(1, 101);
      int rand_forecast = random(1, 101);

      if      (rand_current == 2)                      {
        current_weather = (forecast_weather + 2) % 5;
      }
      else if (rand_current == 1)                      {
        current_weather = (forecast_weather - 2) % 5;
      }
      else if (rand_current > 2 && rand_current <= 7)   {
        current_weather = (forecast_weather + 1) % 5;
      }
      else if (rand_current > 7 && rand_current <= 12)  {
        current_weather = (forecast_weather - 1) % 5;
      }
      else if (rand_current > 12 && rand_current <= 100) {
        current_weather = forecast_weather;
      }

      if      (rand_forecast >  0 && rand_forecast <= 15) {
        forecast_weather = RAINNY;
      }
      else if (rand_forecast > 15 && rand_forecast <= 35) {
        forecast_weather = CLOUDY;
      }
      else if (rand_forecast > 35 && rand_forecast <= 75) {
        forecast_weather = SUNNY;
      }
      else if (rand_forecast > 75 && rand_forecast <= 95) {
        forecast_weather = HEATWAVE;
      }
      else if (rand_forecast > 95 && rand_forecast <= 100) {
        forecast_weather = THUNDERSTORM;
      }
      break;
  }
}



/**************** Click functions ******************/

void click_init() {
  click_state = C_IDLE;
}

void click_update() {
  CLICK_STATE next_state = click_state;
  int btn = read_LCD_buttons();
  switch (click_state)
  {
    case C_IDLE :
      if (btn != btnNONE) {
        next_state = C_ON;
        click_timer = millis() + 10;
      }
      break;
    case C_ON :
      if (click_timer < millis() && read_LCD_buttons() != btnNONE) {
        next_state = C_HOLD;
      }
      break;
    case C_HOLD :
      if (read_LCD_buttons() == btnNONE)
        next_state = C_IDLE;
      break;
  }
  click_state = next_state;
}

void click_output() {

  int btn = read_LCD_buttons();
  static bool first = false;
  switch (click_state)
  {
    case C_IDLE :
      break;
    case C_ON :
      first = true;
      break;
    case C_HOLD :
      if (first) {
        switch (btn)
        {
          case btnRIGHT:
            break;
          case btnUP:
            if (tempo_hour < 30000)
              tempo_hour += 1000;
            break;
          case btnDOWN:
            if (tempo_hour >= 1000)
              tempo_hour -= 1000;
            break;
          case btnLEFT:
            break;
          case btnSELECT:
            break;
          case btnNONE:
            break;
        }
        first = false;
        lcd.setCursor(12, 1);
        lcd.print(tempo_hour);
      }
      break;
  }
}


void send_data() {

  Serial.print("{\"timestamp\" : \"");
  Serial.print(h_from_begin);
  Serial.print("\",\"weather\" : [{\"dfn\" : \"0\", \"weather\" : \"");

  lcd.setCursor(0, 0);
  switch (current_weather) {
    case RAINNY :
      lcd.print("RAINNY          ");
      Serial.print("rainny");
      break;
    case CLOUDY :
      lcd.print("CLOUDY          ");
      Serial.print("cloudy");
      break;
    case SUNNY :
      lcd.print("SUNNY           ");
      Serial.print("sunny");
      break;
    case HEATWAVE :
      lcd.print("HEATWAVE        ");
      Serial.print("heatwave");
      break;
    case THUNDERSTORM :
      lcd.print("THUNDERSTORM    ");
      Serial.print("thunderstorm");
      break;
  }
  Serial.print("\"},{\"dfn\" : \"1\", \"weather\" : \"");

  lcd.setCursor(0, 1);
  switch (forecast_weather) {
    case RAINNY :
      lcd.print("RAINNY          ");
      Serial.print("rainny");
      break;
    case CLOUDY :
      lcd.print("CLOUDY          ");
      Serial.print("cloudy");
      break;
    case SUNNY :
      lcd.print("SUNNY           ");
      Serial.print("sunny");
      break;
    case HEATWAVE :
      lcd.print("HEATWAVE        ");
      Serial.print("heatwave");
      break;
    case THUNDERSTORM :
      lcd.print("THUNDERSTROM    ");
      Serial.print("thunderstorm");
      break;
  }
  Serial.print("\"}]}");
  Serial.println("!");

  h_from_begin = nb_d * 24 + nb_h;

  lcd.setCursor(12, 0);
  lcd.print(h_from_begin);
  lcd.setCursor(12, 1);
  lcd.print(tempo_hour);
}

