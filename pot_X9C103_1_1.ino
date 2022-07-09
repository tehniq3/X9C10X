/*
 * v.1 - Nicu FLORICA (niq_ro) used LapX9C10X library, added i2c LCD1602 display 
 * v.1.1 - added EEPROM to store initial value after restart or at power-on
 */

#include <LapX9C10X.h>  // https://github.com/lucyamy/LapX9C10X   

/*                                                                                                                                                                                                                                                                       
 * This example assumes you're using an X9C103 (10k) chip. For other variants,
 * Change the value in the line:
 *   LapX9C10X led(INCPIN, UDPIN, CSPIN, LAPX9C10X_X9C103);
 * to one of:
 * LAPX9C10X_X9C102       (1k)
 * LAPX9C10X_X9C103       (10k)
 * LAPX9C10X_X9C503       (50k)
 * LAPX9C10X_X9C104       (100k)
 *
 * The chip is connected to the Arduino like this:
 * 1 - !INC - pin 9
 * 2 - U/!D - pin 10
 * 7 - !CS  - pin 11
 * (! indicates that the thing following has a line over the top, called a bar.)
 *   
 * The other pins on the chip must be wired like this:
 * 8 - VCC - 5/3.3V
 * 3 - VH  - 5/3.3V
 * 4 - VSS - GND
 * 6 - VL  - GND
 * 5 - VW  - Output: to A0
 *
 * other connection at Arduino bord
 * 5V to A1
 */

#include <EEPROM.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  // https://github.com/marcoschwartz/LiquidCrystal_I2C

int adc_read = 0;
int procent = 0;
int val_mem = 0;
int pauzamica = 200;

#define pin_read A0
#define pin_ref A1
#define lcd_adress 0x3F  // try also 0x27 if not see text on display

LiquidCrystal_I2C lcd(lcd_adress,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display
                                   // otherusual adress is 0x27

#define CSPIN 11
#define INCPIN 9
#define UDPIN 10


LapX9C10X pot(INCPIN, UDPIN, CSPIN, LAPX9C10X_X9C103);

#define adresa  200  // adress for store the
byte zero = 0;  // variable for control the initial read/write the eeprom memory


#define up_buttonPin   3     //Pin of Arduino Nano
#define down_buttonPin 2     //Pin of Arduino Nano
#define mem_buttonPin  4     //Pin of Arduino Nano


int up_buttonState = 0;
int down_buttonState = 0;
int mem_buttonState = 0;
int adc_ref = 0;
int treapta  = 0;

const int loopPeriod = 5;  //Loop period equals three times of this value, ex) 50 * 3 = 150ms


int loop_cnt = 0;

void setup() {   
  Serial.begin(9600);
    while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

zero = EEPROM.read(adresa - 1); // variable for write initial values in EEPROM
if (zero != 19)
{
EEPROM.update(adresa - 1, 19);  // zero
EEPROM.update(adresa, 10); // initial value, 0 (mute).. 99 (max)
}   
  
// read EEPROM memory;
val_mem = EEPROM.read(adresa);  // value:0..99 (max)
   
//  pot.begin(val_mem);
  pot.begin(-1);
  delay(100);
  pot.set(val_mem);
  treapta = val_mem;
  delay(5000);

  
  pinMode(pin_read, INPUT);
  pinMode(pin_ref, INPUT);
  
  lcd.init();        // initialize the lcd 
  // lcd.begin();    // initialize the lcd (for some libraries instead lcd.init)
  lcd.backlight();   // backlight on
  lcd.setCursor(0,0);
  lcd.print("X9C103 digital  ");
  lcd.setCursor(0,1);
  lcd.print(" potentiometer  ");
  delay(2000);
  lcd.clear();  // clear the lcd  
  lcd.setCursor(0,0);
  lcd.print("X9C103 pot v.1.1");
  lcd.setCursor(0,1);
  lcd.print("  sw by niq_ro  ");
  delay(2000);
  lcd.clear();  // clear the lcd  
  
  pinMode(up_buttonPin, INPUT_PULLUP);  //Internal Pullup on Up button (No external resistor required)
  pinMode(down_buttonPin, INPUT_PULLUP);//Internal Pullup on Down button (No external resistor required)
  pinMode(mem_buttonPin, INPUT_PULLUP);//Internal Pullup on Down button (No external resistor required)
}


void loop() {
  //Read the level of Up/Down button
  up_buttonState = digitalRead(up_buttonPin);
  down_buttonState = digitalRead(down_buttonPin);
  mem_buttonState = digitalRead(mem_buttonPin);

  //Check if Up/Down Button is Pressed
  //When Button is Pressed, Level becomes LOW
  if (up_buttonState == LOW)
  {
    treapta++;
    if (treapta > 99) treapta = 99;
    Serial.print(treapta);
    pot.set(treapta);
    Serial.print(", new resistance = ");
    Serial.print(pot.getK());
    Serial.println("kΩ");
    delay(100);
  }
  else if (down_buttonState == LOW)
  {
    treapta--;
    if (treapta < 0) treapta = 0;
    Serial.print(treapta);
    pot.set(treapta);
    Serial.print(", new resistance = ");
    Serial.print(pot.getK());
    Serial.println("kΩ");
    delay(100);
  }
  else if (mem_buttonState == LOW)
  {
    lcd.setCursor(15,1);
    lcd.print("*");
    delay(250);
    EEPROM.update(adresa, treapta); // store in eeprom
    lcd.setCursor(15,1);
    lcd.print(" ");
    delay(100);
  }
  else
  {
    delayMicroseconds(loopPeriod*3);
  }

  lcd.setCursor(0,0);
    lcd.print("Pot.real: ");
    adc_ref = analogRead(pin_ref);
    adc_read = analogRead(pin_read);
    procent = map(adc_read, 0, adc_ref, 0, 100);
    if (procent <100) lcd.print(" ");
    if (procent <10) lcd.print(" ");
    lcd.print(procent);
    lcd.print("% ");
  lcd.setCursor(0,1);
    lcd.print("Pot.set : ");
    if (treapta <100) lcd.print(" ");
    if (treapta <10) lcd.print(" ");
    lcd.print(treapta);
    lcd.print("% ");   

  delay(pauzamica);
} // end loop
