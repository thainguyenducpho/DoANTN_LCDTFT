//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Arduino TFT Touchscreen DHT11 & Control LED
//----------------------------------------Include Library
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>

#include "DHT.h"

//----------------------------------------Pin configuration and initialization for Touchscreen
#define YP A1 //--> must be an analog pin, use "An" notation!
#define XM A2 //--> must be an analog pin, use "An" notation!
#define YM 7  //--> can be a digital pin
#define XP 6  //--> can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
//----------------------

//----------------------------------------Defines colors
// Assign human-readable names to some common 16-bit color values:
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define DARKORANGE  0xFB60
#define MAROON      0x7800
#define BLACKM      0x18E3
//----------------------------------------

//----------------------------------------The results of the x and y point calibration on the touch screen
#define TS_MINX 243
#define TS_MINY 941
#define TS_MAXX 915
#define TS_MAXY 219
//----------------------------------------

//----------------------------------------Variable for detecting touch screen when touched
#define MINPRESSURE 10
#define MAXPRESSURE 1000
//----------------------------------------

//----------------------------------------Defines an LED pin
#define LED 12
//----------------------------------------

//----------------------------------------Button location at point x and y
int BtnGreenX = 50;
int BtnGreenY = 30;
int BtnRedX = 230;
int BtnRedY = 30;
//----------------------------------------

//----------------------------------------DHT11 configuration and data variables
#define DHTPIN A5     
#define DHTTYPE DHT11   
DHT dht11(DHTPIN, DHTTYPE);

int h;
float t;
float f;
float hif;
float hic;
//----------------------------------------

//----------------------------------------The x and y points for the Temperature bar
int x_bar_t = 20;
int y_bar_t = 60;
//----------------------------------------

//----------------------------------------The variable to hold the conversion value from the temperature value to the value for the temperature bar
int T_to_Bar;
//----------------------------------------

//----------------------------------------Menu = 0 to display the Main Menu Display, Menu = 1 to control the LED and Menu = 2 to display DHT11 sensor data
int Menu = 0;
//----------------------------------------

//----------------------------------------Variable for the x, y and z points on the touch screen
int TSPointZ;
int x_set_rotatoon_135;
int y_set_rotatoon_135;
//----------------------------------------

//----------------------------------------Millis variable to update the temperature and humidity values
unsigned long previousMillis = 0; //--> will store last time updated
// constants won't change:
const long interval = 2000; //--> interval 
//----------------------------------------

//========================================================================VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.reset();

  tft.begin(0x9341); //--> SPFD5408/ILI9341

  tft.setRotation(135); //--> Need for the Mega, please changed for your choice or rotation initial

  tft.fillScreen(WHITE); //--> TFT LCD background color

  pinMode(13, OUTPUT);

  pinMode(LED, OUTPUT);

  dht11.begin();

  Menu_display();
}
//========================================================================

//========================================================================VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  //----------------------------------------Main Menu Display
  if (Menu == 0) {
    GetTSPoint();

    //----------------------------------------Conditions for detecting touch screen when touched
    if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE) {

      //----------------------------------------Conditions for detecting when the Button for controlling the LED is touched and its command (Enter the LED controlling menu)
      if (x_set_rotatoon_135 > 17 && x_set_rotatoon_135 < (17+280) && y_set_rotatoon_135 > 90 && y_set_rotatoon_135 < (90+40)) {  
        Menu = 1;
        DrawButtonControlLEDPress();
        delay(150);
        DrawButtonControlLED();
        delay(500);
        tft.fillScreen(WHITE);
        delay(500);
        DrawButtonGreen(BtnGreenX,BtnGreenY);
        DrawButtonRed(BtnRedX, BtnRedY);
        DrawButtonGreen(BtnGreenX,BtnGreenY+64);
        DrawButtonRed(BtnRedX, BtnRedY+64);
        DrawButtonGreen(BtnGreenX,BtnGreenY+128);
        DrawButtonRed(BtnRedX, BtnRedY+128);
        //-------------------------fan
        if (digitalRead(LED) == 0) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 50);
          tft.print("FAN OFF");
        } else if (digitalRead(LED) == 1) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 50);
          tft.print("FAN ON");
        }
        //-------------------------
        //-------------------------lamp
        if (digitalRead(LED) == 0) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 114);
          tft.print("LAMP OFF");
        } else if (digitalRead(LED) == 1) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 114);
          tft.print("LAMP ON");
        }
        //-------------------------
        //-------------------------spray
        if (digitalRead(LED) == 0) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 178);
          tft.print("LAMP OFF");
        } else if (digitalRead(LED) == 1) {
          tft.setTextSize(2);
          tft.setTextColor(BLUE, WHITE);
          tft.setCursor(120, 178);
          tft.print("LAMP ON");
        }
        //-------------------------
        
        DrawButtonBack(10, 200);
      }
      //----------------------------------------

      //----------------------------------------Condition to detect when the button to display DHT11 sensor data is touched and the command (Enter the menu displays DHT11 sensor data)
      if (x_set_rotatoon_135 > 17 && x_set_rotatoon_135 < (17+280) && y_set_rotatoon_135 > 160 && y_set_rotatoon_135 < (160+40)) {
        Menu = 2;
        DrawButtonTempHumPress();
        delay(150);
        DrawButtonTempHum();
        delay(500);
        tft.fillScreen(WHITE);
        delay(500);
        tft.drawLine(15, 40, 300, 40, MAROON);
        tft.drawLine(15, 39, 300, 39, MAROON);
      
        tft.setTextSize(2);
        tft.setTextColor(BLUE);
        tft.setCursor(40, 13);
        tft.print("Temperature & Humidity");
        
        draw_bar(x_bar_t, y_bar_t);
      
        tft.drawLine(190, 60, 190, 227, MAROON);
        tft.drawLine(190, 127, 300, 127, MAROON);
      
        tft.fillRect(202, 60, 100, 27, CYAN);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.setCursor(205, 65);
        tft.print("Humidity");
      
        tft.fillRect(202, 140, 100, 43, GREEN);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.setCursor(227, 145);
        tft.print("Soil");
        tft.setCursor(220, 165);
        tft.print("Moist");

        DrawButtonBack(8, 6);
      
        GetDHT11Data();
        delay(1000);
      }
      //----------------------------------------
    }  
    //----------------------------------------
  }
  //----------------------------------------

  //----------------------------------------Menu or Mode to control the LED
  if (Menu == 1) {
    ControlTheLED();
  }
  //----------------------------------------

  //----------------------------------------Menu or Mode to display DHT11 sensor data
  if (Menu == 2) {
    ShowDHT11Data();
  }
  //----------------------------------------
}
//========================================================================

//========================================================================GetTSPoint()
void GetTSPoint() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());

  y_set_rotatoon_135 = map(p.x, 0, 240, 0, tft.height());
  x_set_rotatoon_135 = map(tft.width() - p.y, 0, 320, 0, tft.width());

  TSPointZ = p.z;
}
//========================================================================

//========================================================================DrawButtonControlLED()
void DrawButtonControlLED() {
  tft.fillRoundRect(17, 90, 280, 40, 10, BLACKM);
  tft.fillRoundRect(19, 92, 276, 36, 10, GREEN);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(65, 103);
  tft.print("Control the LED");
}
//========================================================================

//========================================================================DrawButtonControlLEDPress()
void DrawButtonControlLEDPress() {
  tft.fillRoundRect(17, 90, 280, 40, 10, BLACKM);
}
//========================================================================

//========================================================================DrawButtonTempHum()
void DrawButtonTempHum() {
  tft.fillRoundRect(17, 160, 280, 40, 10, BLACKM);
  tft.fillRoundRect(19, 162, 276, 36, 10, BLUE);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 173);
  tft.print("Temperature & Humidity");
}
//========================================================================

//========================================================================DrawButtonTempHumPress()
void DrawButtonTempHumPress() {
  tft.fillRoundRect(17, 160, 280, 40, 10, BLACKM);
}
//========================================================================

//========================================================================DrawButtonGreen(x, y)
void DrawButtonGreen(int xp, int yp) {
  tft.fillRoundRect(xp, yp, 54, 54, 10, BLACKM);
  tft.fillRoundRect(xp+2, yp+2, 50, 50, 10, GREEN);
  tft.setCursor(xp+16, yp+20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("ON");
}
//========================================================================

//========================================================================DrawButtonGreenPress(x, y)
void DrawButtonGreenPress(int xp, int yp) {
  tft.fillRoundRect(xp, yp, 54, 54, 10, BLACKM);
}
//========================================================================

//========================================================================DrawButtonRed(x, y)
void DrawButtonRed(int xp, int yp) {
  tft.fillRoundRect(xp, yp, 54, 54, 10, BLACKM);
  tft.fillRoundRect(xp+2, yp+2, 50, 50, 10, RED);
  tft.setCursor(xp+ 10, yp+20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("OFF");
}
//========================================================================

//========================================================================DrawButtonRedPress(x, y)
void DrawButtonRedPress(int xp, int yp) {
  tft.fillRoundRect(xp, yp, 54, 54, 10, BLACKM);
}
//========================================================================

//========================================================================DrawButtonBack(x, y)
void DrawButtonBack(int x_btn_back, int y_btn_back) {
  tft.fillRoundRect(x_btn_back, y_btn_back, 30, 30, 5, BLACKM);
  tft.fillRoundRect(x_btn_back+2, y_btn_back+2, 26, 26, 5, YELLOW);
  tft.setTextSize(2);
  tft.setTextColor(BLACKM);
  tft.setCursor(x_btn_back+7, y_btn_back+7);
  tft.print("<");  
}
//========================================================================

//========================================================================DrawButtonBackPress(x, y)
void DrawButtonBackPress(int x_btn_back, int y_btn_back) {
  tft.fillRoundRect(x_btn_back, y_btn_back, 30, 30, 5, BLACKM);
}
//========================================================================

//========================================================================GetDHT11Data()
void GetDHT11Data() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht11.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht11.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht11.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht11.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht11.computeHeatIndex(t, h, false);
}
//========================================================================

//========================================================================draw_bar (Temperature Bar)
void draw_bar(int x_bar, int y_bar) {
  tft.fillRoundRect(x_bar, y_bar, 35, 120, 5, BLACK);
  tft.fillCircle(x_bar+17, y_bar+140, 30, BLACK);
  tft.fillRoundRect(x_bar+4, y_bar+4, 27, 120, 2, WHITE);
  tft.fillCircle(x_bar+17, y_bar+140, 25, WHITE);
  tft.fillRect(x_bar+8, y_bar+8, 19, 120, DARKORANGE);
  tft.fillCircle(x_bar+17, y_bar+140, 21, DARKORANGE);

  //tft.fillRect(41, 58, 19, 108, RED);

  tft.drawLine(x_bar+37, y_bar+8, x_bar+42, y_bar+8, RED);
  tft.setTextSize(1);
  tft.setTextColor(RED);
  tft.setCursor(x_bar+47, y_bar+4);
  tft.println("50");
  
  tft.drawLine(x_bar+37, y_bar+115, x_bar+42, y_bar+115, RED);
  tft.setCursor(x_bar+47, y_bar+111);
  tft.println("0");
}
//========================================================================

//========================================================================Menu_display()
void Menu_display() {
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.setCursor(120, 43);
  tft.print("MENU");  

  DrawButtonControlLED();
  DrawButtonTempHum();
}
//========================================================================

//========================================================================ControlTheLED()
void ControlTheLED() {
  GetTSPoint();
  
  if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE) {
    //==================================================== btn fan
    if (x_set_rotatoon_135 > BtnGreenX && x_set_rotatoon_135 < (BtnGreenX+54) && y_set_rotatoon_135 > BtnGreenY && y_set_rotatoon_135 < (BtnGreenY+54)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 50);
      tft.print("FAN ON ");

      digitalWrite(LED, HIGH);
      
      DrawButtonGreenPress(BtnGreenX, BtnGreenY);
      delay(100);
      DrawButtonGreen(BtnGreenX,BtnGreenY);
    }

    if (x_set_rotatoon_135 > BtnRedX && x_set_rotatoon_135 < (BtnRedX+54) && y_set_rotatoon_135 > BtnRedY && y_set_rotatoon_135 < (BtnRedY+54)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 50);
      tft.print("FAN OFF");

      digitalWrite(LED, LOW);
  
      DrawButtonRedPress(BtnRedX, BtnRedY);
      delay(100);
      DrawButtonRed(BtnRedX, BtnRedY);
    }
    //================================================
    //================================================ btn lamp
    if (x_set_rotatoon_135 > BtnGreenX && x_set_rotatoon_135 < (BtnGreenX+54) && y_set_rotatoon_135 > (BtnGreenY+64) && y_set_rotatoon_135 < (BtnGreenY+54+64)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 114);
      tft.print("LAMP ON ");

      digitalWrite(LED, HIGH);
      
      DrawButtonGreenPress(BtnGreenX, BtnGreenY+64);
      delay(100);
      DrawButtonGreen(BtnGreenX,BtnGreenY+64);
    }

    if (x_set_rotatoon_135 > BtnRedX && x_set_rotatoon_135 < (BtnRedX+54) && y_set_rotatoon_135 > (BtnRedY+64) && y_set_rotatoon_135 < (BtnRedY+54+64)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 114);
      tft.print("LAMP OFF");

      digitalWrite(LED, LOW);
  
      DrawButtonRedPress(BtnRedX, BtnRedY+64);
      delay(100);
      DrawButtonRed(BtnRedX, BtnRedY+64);
    }
    //===============================================
    //================================================ btn spray
    if (x_set_rotatoon_135 > BtnGreenX && x_set_rotatoon_135 < (BtnGreenX+54) && y_set_rotatoon_135 > (BtnGreenY+128) && y_set_rotatoon_135 < (BtnGreenY+54+128)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 178);
      tft.print("SPRAY ON ");

      digitalWrite(LED, HIGH);
      
      DrawButtonGreenPress(BtnGreenX, BtnGreenY+128);
      delay(100);
      DrawButtonGreen(BtnGreenX,BtnGreenY+128);
    }

    if (x_set_rotatoon_135 > BtnRedX && x_set_rotatoon_135 < (BtnRedX+54) && y_set_rotatoon_135 > (BtnRedY+128) && y_set_rotatoon_135 < (BtnRedY+54+128)) {
      tft.setTextSize(2);
      tft.setTextColor(BLUE, WHITE);
      tft.setCursor(120, 178);
      tft.print("SPRAY OFF");

      digitalWrite(LED, LOW);
  
      DrawButtonRedPress(BtnRedX, BtnRedY+128);
      delay(100);
      DrawButtonRed(BtnRedX, BtnRedY+128);
    }
    //===============================================
    if (x_set_rotatoon_135 > 10 && x_set_rotatoon_135 < (10+30) && y_set_rotatoon_135 > 200 && y_set_rotatoon_135 < (200+30)) {
      Menu = 0;
      DrawButtonBackPress(10, 200);
      delay(100);
      DrawButtonBack(10, 200);
      delay(500);
      tft.fillScreen(WHITE);
      delay(500);
      Menu_display();
    }
  }
}
//========================================================================

//========================================================================ShowDHT11Data()
void ShowDHT11Data() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    
    GetDHT11Data();
  }

  T_to_Bar = map(t, 0.0, 50.0, 108, 0);

  tft.fillRect(x_bar_t+8, (y_bar_t+8)+T_to_Bar, 19, 108-T_to_Bar, ORANGE);
  tft.fillRect(x_bar_t+8, y_bar_t+8, 19, T_to_Bar, WHITE);

  tft.setTextSize(2);
  tft.setTextColor(ORANGE, WHITE);
  tft.setCursor(75, 100);
  tft.print(t);
  if (t < 10) tft.print(" ");
  tft.setCursor(160, 100);
  tft.print((char)247);
  tft.println("C");

  
  tft.setCursor(75, 135);
  tft.print(f);
  if (f < 100) tft.print(" ");
  tft.setCursor(160, 135);
  tft.print((char)247);
  tft.println("F");

  tft.setTextSize(3);
  tft.setTextColor(CYAN, WHITE);
  tft.setCursor(205, 95);
  tft.print(h);
  tft.print(" %");

  tft.setTextSize(1);
  tft.setTextColor(GREEN, WHITE);
  tft.setCursor(205, 200);
  tft.print(hic);
  tft.print(" ");
  tft.print((char)247);
  tft.print("C");
  if (hic < 10) tft.print(" ");

  tft.setTextSize(1);
  tft.setTextColor(GREEN, WHITE);
  tft.setCursor(205, 220);
  tft.print(hif);
  tft.print(" ");
  tft.print((char)247);
  tft.print("F");
  if (hif < 100) tft.print(" "); 

  GetTSPoint();

  if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE) {
    if (x_set_rotatoon_135 > 8 && x_set_rotatoon_135 < (8+30) && y_set_rotatoon_135 > 6 && y_set_rotatoon_135 < (6+30)) {
      Menu = 0;
      DrawButtonBackPress(8, 6);
      delay(100);
      DrawButtonBack(8, 6);
      delay(500);
      tft.fillScreen(WHITE);
      delay(500);
      Menu_display();
    }
  }
}
//========================================================================
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
