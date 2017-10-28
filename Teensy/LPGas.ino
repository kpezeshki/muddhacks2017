#define MQ6_HEAT_TIME_MILLIS 30000
#define MQ6_COOL_TIME_MILLIS 30000
#define MQ6_HEATER_PIN 6
#define MQ6_ANALOG_PIN 2
#define MQ6_SAMPLE_PERIOD_MILLIS 1000

typedef enum {
  ST_OFF,
  ST_CYCLE_0_HIGH,
  ST_DONE
}
MQ_STATE;

MQ_STATE mq6State = ST_OFF;


unsigned long mq6_next_state_ms = 5000;   // start 5s after powerup
unsigned long mq6_next_reading_ms = 0;

void updateLPGas() {
  switch (mq6State) {
    case ST_OFF :
      {
        if (millis() > mq6_next_state_ms) {
          digitalWrite(MQ6_HEATER_PIN, HIGH);

          mq6State = ST_CYCLE_0_HIGH;
          mq6_next_state_ms = millis() + MQ6_HEAT_TIME_MILLIS;

          Serial.print("Heating");

        }
        break;
      }

    case ST_CYCLE_0_HIGH :
      {
        if (millis() > mq6_next_reading_ms) {
          int lpGas = analogRead(MQ6_ANALOG_PIN);
          Serial.println(lpGas);


          mq6_next_reading_ms = millis() + MQ6_SAMPLE_PERIOD_MILLIS;
        }

        if (millis() > mq6_next_state_ms) {
          digitalWrite(MQ6_HEATER_PIN, LOW);
          Serial.print("Cooling");
          mq6State = ST_DONE;
        }

        break;
      }

    case ST_DONE :
      {
        mq6_next_state_ms = millis() + MQ6_COOL_TIME_MILLIS;
        mq6State = ST_OFF;
        break;
      }
  }
}


