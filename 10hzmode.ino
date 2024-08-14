/*
 Single Frequency program
 This code generates a 10 Hz waveform for transcranial electrical stimulation.
 The stimulation can be stopped with a button attached to PIN 0 (IO0).
*/

#include <SPI.h>
#include "esp32-hal-timer.h"

#define TIMER_FREQ 80000000 // 80 MHz timer frequency

static const int spiClk = 1000000; // 1 MHz
SPIClass * hspi = NULL;
static uint8_t cs = 15; // using digital pin 15 for DAC7311 chip select

double offset = 0;
double amplitude = 1.0; // Set to 1.0 for 4mA peak-to-peak
const double frequency = 10; // 10 Hz stimulation
const int samples_per_cycle = 200; // 200 samples per cycle for smooth waveform
double wave_list[samples_per_cycle];
int wave_list_dacwrite[samples_per_cycle];
int sample_now;

volatile uint8_t should_send = 0;

#define BTN_STOP_ALARM    0

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&timerMux);
  should_send = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  if(offset > 4.002) offset = 4.001;
  if(offset < -4.002) offset = -4.001;
  if(amplitude > 4.002) amplitude = 4.001;

  // Generate sine wave
  for(int x = 0; x < samples_per_cycle; x++) {
    wave_list[x] = amplitude * sin(2.0 * PI * x / samples_per_cycle) + offset;
  }

  // Convert to DAC values
  for(int x = 0; x < samples_per_cycle; x++) {
    double value_in_mA = wave_list[x];
    if(value_in_mA > 4.002) value_in_mA = 4.001;
    if(value_in_mA < -4.002) value_in_mA = -4.001;
    wave_list_dacwrite[x] = (int(round(((16383*1.0866)/5)*(2.5-value_in_mA/2))));
  }

  hspi = new SPIClass(HSPI);
  hspi->begin();
  pinMode(cs, OUTPUT);
  pinMode(BTN_STOP_ALARM, INPUT);

  // Initialize timer
  timer = timerBegin(TIMER_FREQ / (frequency * samples_per_cycle));
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, true, true, true);

  sample_now = 0;
}

void DACwrite(uint16_t value) {
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE1));
  digitalWrite(cs, LOW);
  hspi->transfer16(value);
  digitalWrite(cs, HIGH);
  hspi->endTransaction();
}

void loop() {
  if(should_send == 1) {
    portENTER_CRITICAL(&timerMux);
    should_send = 0;
    portEXIT_CRITICAL(&timerMux);
    
    DACwrite(wave_list_dacwrite[sample_now]);
    sample_now = (sample_now + 1) % samples_per_cycle;
  }

  if (digitalRead(BTN_STOP_ALARM) == LOW) {
    if (timer) {
      timerAlarm(timer, false, false, false);
      timerDetachInterrupt(timer);
      timerEnd(timer);
      timer = NULL;
    }
  }
}