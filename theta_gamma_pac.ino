#include <SPI.h>
#include "esp32-hal-timer.h"

static const int spiClk = 1000000; // 1 MHz
SPIClass * hspi = NULL;
static uint8_t cs = 15; // using digital pin 15 for DAC7311 chip select

double offset = 0;
double amplitude = 0.5;
const double frequency = 84;
const int samples_per_cycle = int(round(1/(float(frequency) * .000512)));
const double frequency_base = 6;
const int samples_per_cycle_base = int(round(1/(float(frequency_base) * .000512)));
double wave_list[samples_per_cycle_base];
int wave_list_dacwrite[samples_per_cycle_base];
int sample_now;

volatile uint8_t should_send = 0;

#define BTN_STOP_ALARM    0

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  should_send = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void printESP32Version() {
  String version = ESP.getSdkVersion();
  Serial.print("ESP32 Arduino Core Version: ");
  Serial.println(version);
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give some time for Serial to initialize
  printESP32Version();

  if(offset > 2.002) offset = 2.001;
  if(offset < -2.002) offset = -2.001;
  if(amplitude > 2.002) amplitude = 2.001;

  for(int x = 0; x < samples_per_cycle_base; x++) {
    wave_list[x] = amplitude * (sin(2.0 * (3.1415926) * (double(x) / double(samples_per_cycle_base)))) + offset;
    if ((x >= samples_per_cycle_base/12) && (x < samples_per_cycle_base/2 - samples_per_cycle_base/12)) {
      wave_list[x] += amplitude * (4.0/5.0) * (sin(2.0 * (3.1415926) * (double(x) / double(samples_per_cycle)))) + offset;
    }
  }
  wave_list[samples_per_cycle_base - 1] = (wave_list[0] + wave_list[samples_per_cycle_base - 2]) / 2.0;

  for(int x = 0; x < samples_per_cycle_base; x++) {
    double value_in_mA = wave_list[x];
    if(value_in_mA > 2.002) value_in_mA = 2.001;
    if(value_in_mA < -2.002) value_in_mA = -2.001;
    wave_list_dacwrite[x] = (int(round(((16383*1.0866)/5)*(2.5-value_in_mA))));
  }

  hspi = new SPIClass(HSPI);
  hspi->begin();
  pinMode(cs, OUTPUT);
  pinMode(BTN_STOP_ALARM, INPUT);

  // Initialize timer
  timer = timerBegin(0);
  timerAttachInterrupt(timer, &onTimer);
  uint64_t alarmValue = 1000000 / (samples_per_cycle_base * frequency_base);
  timerWrite(timer, alarmValue);
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
    
    if(sample_now >= samples_per_cycle_base) {
      sample_now = 0;
    }
    DACwrite(wave_list_dacwrite[sample_now]);
    sample_now++;
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