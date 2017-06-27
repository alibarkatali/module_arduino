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
}CLICK_STATE;

// Etats courants
HORLOGE_STATE horlogeState;
METROLOGY_STATE metrologyState;
CLICK_STATE clickState;

// Variables globales
unsigned long timer = 0, clickTimer = 0;
int tempoHour = 500;
int nb_h = 0, nb_d = 0, hFromBegin;
bool tickHour = 0, tickDay = 0;
//String currentWeatherString, forecastWeatherString;
int currentWeather, forecastWeather;

// Définitions des fonctions de la tâche Horloge
void horlogeInit();
void horlogeUpdate();
void horlogeOutput();

// Définitions des fonctions de la tâche Metrology
void metrologyInit();
void metrologyUpdate();
void metrologyOutput();

// Définitions des fonctions de la tâche Click
void clickInit();
void clickUpdate();
void clickOutput();

// Définitions des fonctions de l'affichage
int read_LCD_buttons();
void sendData();


/******************* Main *********************/

void setup() {
  // start serial
  Serial.begin(9600);
  
 // start the library
 lcd.begin(16, 2);               
  
  // Initialisation de toutes les machines à états
  horlogeInit();
  metrologyInit();
  clickInit();
}

void loop() {
  // Mise à jour des états
  horlogeUpdate();
  metrologyUpdate();
  clickUpdate();
  
  // Mise à jour des sorties
  horlogeOutput();
  metrologyOutput();
  clickOutput();

}


/**************** Screen Functions ******************/

int read_LCD_buttons(){               // read the buttons
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

void horlogeInit() {
  horlogeState = H_INIT;
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
      tickHour = 0;
      tickDay = 0;
      break;
    case H_IDLE :
      break;
    case H_TICK_H :
      sendData();
      tickHour = 1;
      break;
    case H_TICK_D :
      tickDay = 1;
      break;
  }
}


/**************** Metrology functions ******************/

void metrologyInit(){
  metrologyState = M_IDLE;
  currentWeather = SUNNY;
  forecastWeather = CLOUDY;
  lcd.setCursor(0,0);
  lcd.print("SUNNY          ");
  lcd.setCursor(0,1);
  lcd.print("CLOUDY           ");
  
}

void metrologyUpdate(){
  METROLOGY_STATE nextState = metrologyState;
  
  switch(metrologyState)
  {
    case M_IDLE :
      if (tickDay) nextState = M_SELECT;
      break;
    case M_SELECT :
      nextState = M_IDLE;
      break;
  }
  metrologyState = nextState;
}

void metrologyOutput(){
    
    switch(metrologyState)
    {
      case M_IDLE :
        break;
      case M_SELECT :
      
        int randCurrent = random(1,101);
        int randForecast = random(1,101);
        
        if      (randCurrent == 2)                      {currentWeather = (forecastWeather + 2)%5;}
        else if (randCurrent == 1)                      {currentWeather = (forecastWeather - 2)%5;}
        else if (randCurrent > 2 && randCurrent <= 7)   {currentWeather = (forecastWeather + 1)%5;}
        else if (randCurrent > 7 && randCurrent <= 12)  {currentWeather = (forecastWeather - 1)%5;}
        else if (randCurrent > 12 && randCurrent <= 100){currentWeather = forecastWeather;}

        if      (randForecast >  0 && randForecast <= 15) {forecastWeather = RAINNY;}
        else if (randForecast > 15 && randForecast <= 35) {forecastWeather = CLOUDY;}
        else if (randForecast > 35 && randForecast <= 75) {forecastWeather = SUNNY;}
        else if (randForecast > 75 && randForecast <= 95) {forecastWeather = HEATWAVE;}
        else if (randForecast > 95 && randForecast <= 100){forecastWeather = THUNDERSTORM;}
        
        break;
    }
}



/**************** Click functions ******************/

void clickInit(){
  clickState = C_IDLE;
}

void clickUpdate(){
  CLICK_STATE nextClickState = clickState;
  int btn = read_LCD_buttons();
  switch (clickState)
  {
    case C_IDLE :
      if (btn != btnNONE) {
        nextClickState = C_ON;
        clickTimer = millis()+10;
      }
      break;
    case C_ON :
      if (clickTimer < millis() && read_LCD_buttons() != btnNONE){
        nextClickState = C_HOLD;
      }
      break;
    case C_HOLD :
      if (read_LCD_buttons() == btnNONE)
        nextClickState = C_IDLE;
      break;
  }
  clickState = nextClickState;
}

void clickOutput(){

  int btn = read_LCD_buttons();
  static bool first = false;
  switch (clickState)
  {
    case C_IDLE :
      break;
    case C_ON :
      first = true;
      break;
    case C_HOLD :
      if (first){
        switch (btn)
        {
          case btnRIGHT:
            break; 
          case btnUP:
            if (tempoHour < 30000)
              tempoHour += 1000;
            break; 
          case btnDOWN:
            if (tempoHour >= 1000)
              tempoHour -= 1000;
            break; 
          case btnLEFT:
            break; 
          case btnSELECT:
            break; 
          case btnNONE:
            break; 
        }
        first = false;
        lcd.setCursor(12,1);
        lcd.print(tempoHour); 
      }
      break;
  }
}


void sendData(){
    
    Serial.print("{\"timestamp\" : \"");
    Serial.print(hFromBegin);
    Serial.print("\",\"weather\" : [{\"dfn\" : \"0\", \"weather\" : \"");
    
    lcd.setCursor(0,0);
    switch (currentWeather){
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
    
    lcd.setCursor(0,1);
    switch (forecastWeather){
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
    
    hFromBegin = nb_d * 24 + nb_h;
    
    lcd.setCursor(12,0);
    lcd.print(hFromBegin);  
    lcd.setCursor(12,1);
    lcd.print(tempoHour); 
}



