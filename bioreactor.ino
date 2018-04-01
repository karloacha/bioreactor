#include <max6675.h>

const float HEAT_RATE    = 20.0;
const float TERMINAL     = 600.0;
const float INITIAL      = 50.0;
float       limit        = 0.0;
const int8_t   INTERVALS    = 10;
const int8_t   SSR_PIN      = 9;

const int8_t DO   = 50;
const int8_t CS_1 = 48;
const int8_t CS_2 = 46;
const int8_t CS_3 = 44;
const int8_t CS_4 = 42;
const int8_t CS_5 = 40;
const int8_t CLK  = 52;

const float temp_per_interval  = (HEAT_RATE / INTERVALS); // get the target incremental value per 6 seconds
unsigned long PERIOD           = 1000*60/INTERVALS; // duration per interval in milliseconds
float dummy_reading            = 0.0;            // just a placeholder
const float DIFF               = 0.3000;         // window for error is set as +- 0.3000

void setup() {
  Serial.begin(9600);
  pinMode(SSR_PIN, OUTPUT);
  change_temp(millis(), false); // set the initial temperature to 50C
}

void loop() { 
      unsigned long lapse = millis();
      // Do the following functions until 6 seconds is reached
      // read temperatures from the 5 thermocouples
      //double temp_1 = readTemp(CLK, CS_1 ,DO); // top of furnace
      double temp_2 = readTemp(CLK, CS_2 ,DO); // middle of furnace
      double temp_3 = readTemp(CLK, CS_3 ,DO); // bottom of furnace
      double temp_4 = readTemp(CLK, CS_4 ,DO); // heating element
      double temp_5 = readTemp(CLK, CS_5 ,DO); // vapor inlet
      Serial.println(temp_2);
      Serial.println(temp_3);
      Serial.println(temp_4);
      Serial.println(temp_5);
      Serial.println(" ");
      while (millis() - lapse < 6000) {
        change_temp(lapse, true);
      }
}

// Change the temperature up to the set limit
// Argument: target, Type: boolean
/// set false if there is no target temperature per interval (e.g. initializing temperature)
/// set true if there is a target per intervall (e.g. raising the temperature up to the TERMINAL) 
void change_temp (unsigned long dur, boolean target) {
  unsigned long SPAN  = 0;
  if (target) {
    SPAN = PERIOD;
    if (limit < TERMINAL) {
      limit = limit + temp_per_interval;
    }
    else {
      limit = TERMINAL;
    }
  } else {
    limit = INITIAL;
  }
  do {
    dummy_reading = readTemp(CLK, CS_2 ,DO);
    Serial.print("T2 = ");
    Serial.println(dummy_reading);
    Serial.print("limit = ");
    Serial.println(limit);
    Serial.print("temp_per_interval = ");
    Serial.println(temp_per_interval);
    Serial.println(" ");
    if (dummy_reading < limit) {
      digitalWrite(SSR_PIN, HIGH); 
      delay(100);
    } else {
      digitalWrite(SSR_PIN, LOW);
      delay(100);
    }
  }while ((millis() - dur) < SPAN);
}

double readTemp(int8_t clk, int8_t cs, int8_t miso) {
  MAX6675 thermo(clk, cs, miso);
  double temperature = thermo.readCelsius();
  delay(250);
  return temperature;
}
