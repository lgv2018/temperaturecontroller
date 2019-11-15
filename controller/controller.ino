/*

  Arduino fixed temperature controller

  Read a temperature sensor and control a heater and cooler to try to maintain
  a specific temperature within a set tolerance.

  Provides protection from overruns which might occur if the environment can't
  be brought to the desired temperature for any reason.

  LEDs display a startup sequence to indicate successful initialisation, then
  light individually to indicate heating or cooling. Both LEDs light together in
  the safety period after an over-run, and both flash in sync in the permanent error
  mode which is triggered if there are too many consecutive over-runs.

  Keep the poll interval reasonably long to prevent jitter, especially if controlling
  a peltier junction as they can be damaged by rapid reversal.

  Merlyn Kline arduino@binary.co.uk

*/

#include <Wire.h>
#include <DallasTemperature.h>

#define DEBUG


// Software config

#define TARGET_TEMPERATURE 20.5  // Celcius
#define TOLERANCE           0.5

#ifndef DEBUG
  #define POLL_INTERVAL    5ul * 1000ul        // Wait this number of ms between polls of the temperature sensor
  #define MAX_RUN_TIME    10ul * 60ul * 1000ul // Don't run heater or cooler for more than this number of ms
  #define PROTECTION_TIME  5ul * 60ul * 1000ul // Rest heater and cooler for this number of ms if they run for the time limit
  #define MAX_OVER_RUNS    3                   // Maxmimum number of consecutive over-runs before dropping into error mode
#else
  #define POLL_INTERVAL    300ul
  #define MAX_RUN_TIME    3000ul
  #define PROTECTION_TIME  500ul
  #define MAX_OVER_RUNS      3
#endif

#define FLASH_DELAY 250  // Number of ms between LED flashes in various displays


// Hardware config

#define PIN_RELAY_HEAT  4  // Relay control for heater
#define PIN_RELAY_COOL  6  // Relay control for cooler

#define PIN_LED_HEAT    8  // LED control to indicate heating (e.g. red LED)
#define PIN_LED_COOL    2  // LED control to indicate cooling (e.g. blue LED)

#define PIN_THERMOMETER 7


// Readability constants

#define MODE_OFF     0
#define MODE_HEATING 1
#define MODE_COOLING 2

#define LED_STATE_ON  HIGH
#define LED_STATE_OFF LOW

#define RELAY_STATE_ON  LOW
#define RELAY_STATE_OFF HIGH



OneWire           oneWireManager(PIN_THERMOMETER);
DallasTemperature thermometer(&oneWireManager);
unsigned long     lastModeChangeTime;
unsigned int      currentMode;
int               lastOverRunMode;
int               overRunCounter;


void setup() {
  Serial.begin(9600);
  thermometer.begin();

  pinMode(PIN_LED_HEAT, OUTPUT);
  pinMode(PIN_LED_COOL, OUTPUT);
  pinMode(PIN_RELAY_HEAT, OUTPUT);
  pinMode(PIN_RELAY_COOL, OUTPUT);

  lastModeChangeTime = millis();
  lastOverRunMode    = MODE_OFF;
  overRunCounter     = 0;

  setMode(MODE_OFF);
  startupDisplay();
}


void loop() {
  overRunProtection();

  thermometer.requestTemperatures();
  float temperature = thermometer.getTempCByIndex(0);

  if      (temperature < TARGET_TEMPERATURE - TOLERANCE / 2) setMode(MODE_HEATING);
  else if (temperature > TARGET_TEMPERATURE + TOLERANCE / 2) setMode(MODE_COOLING);
  else                                                       setMode(MODE_OFF);

  delay(POLL_INTERVAL);
}


void startupDisplay() {
  for (int i = 0; i < 4; i++) {
    setLEDState(LED_STATE_ON, LED_STATE_OFF);
    delay(FLASH_DELAY);
    setLEDState(LED_STATE_OFF, LED_STATE_ON);
    delay(FLASH_DELAY);
  }
  setLEDState(LED_STATE_ON, LED_STATE_ON);
  delay(FLASH_DELAY * 4);
  setLEDState(LED_STATE_OFF, LED_STATE_OFF);
  delay(FLASH_DELAY * 4);
}


void setMode(int mode) {
  if (mode == MODE_HEATING) {
    setRelayState(RELAY_STATE_ON, RELAY_STATE_OFF);
    setLEDState(LED_STATE_ON, LED_STATE_OFF);
  }
  else if (mode == MODE_COOLING) {
    setRelayState(RELAY_STATE_OFF, RELAY_STATE_ON);
    setLEDState(LED_STATE_OFF, LED_STATE_ON);
  }
  else {
    setRelayState(RELAY_STATE_OFF, RELAY_STATE_OFF);
    setLEDState(LED_STATE_OFF, LED_STATE_OFF);
    mode = MODE_OFF;
  }

  if (mode != currentMode) lastModeChangeTime = millis();

  if (mode != lastOverRunMode && mode != MODE_OFF) overRunCounter = 0;

  currentMode = mode;
}

void overRunProtection() {
  if (currentMode != MODE_OFF) {
    unsigned long int currentTime = millis();
    if ((currentTime - lastModeChangeTime) > MAX_RUN_TIME) {
      limitOverRuns();
      lastOverRunMode = currentMode;
      setMode(MODE_OFF);
      setLEDState(LED_STATE_ON, LED_STATE_ON);
      delay(PROTECTION_TIME);
      setLEDState(LED_STATE_OFF, LED_STATE_OFF);
    }
  }
}

void limitOverRuns() {
  if (currentMode == lastOverRunMode) {
    overRunCounter++;
    if (overRunCounter >= MAX_OVER_RUNS) {
      setMode(MODE_OFF);
      permanentErrorDisplay();
    }
  }
}

void permanentErrorDisplay() {
  while (1) {
    setLEDState(LED_STATE_ON, LED_STATE_ON);
    delay(FLASH_DELAY);
    setLEDState(LED_STATE_OFF, LED_STATE_OFF);
    delay(FLASH_DELAY);
  }
}

void setRelayState(int heatState, int coolState) {
  digitalWrite(PIN_RELAY_HEAT, heatState);
  digitalWrite(PIN_RELAY_COOL, coolState);
}

void setLEDState(int heatState, int coolState) {
  digitalWrite(PIN_LED_HEAT, heatState);
  digitalWrite(PIN_LED_COOL, coolState);
}
