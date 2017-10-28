#define PARTICULATE_LED_PIN 15
#define PARTICULATE_ANALOG_PIN 0
#define PARTICULATE_DELAY_A 280
#define PARTICULATE_DELAY_B 40
#define PARTICULATE_DELAY_C 9680

#define NUM_SAMPLES 25.0

float sampleSensor100() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += sampleSensor();
  }

  sum = sum / NUM_SAMPLES;
  return sum;
}

int sampleSensor() {
  digitalWrite(PARTICULATE_LED_PIN, HIGH); // power on the LED
  delayMicroseconds(PARTICULATE_DELAY_A);

  int dustVal = analogRead(PARTICULATE_ANALOG_PIN); // read the dust value via pin 5 on the sensor

  delayMicroseconds(PARTICULATE_DELAY_B);

  digitalWrite(PARTICULATE_LED_PIN, LOW); // turn the LED off
  delayMicroseconds(PARTICULATE_DELAY_C);
  return dustVal;
}


