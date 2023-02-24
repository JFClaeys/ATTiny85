#include <OneButton.h>
#include "LaserLineDefs.h"

#define CLICK_MS_DURATION 120

#define LASER_PINOUT PB4
#define PUSH_BUTTON PB3

enum LaserStates currentState;
uint16_t iNextCycleTime;      // current cycle before next sequence change (from on to off)
uint8_t currentFrequency = FREQUENCY_5_HZ;  // holds the currently selected frequency
uint8_t currentDutyCycle = DUTYCYCLE_50;    // holds the currently selected duty cycle

uint16_t GetSequenceMilli_On() {
  /* calculates the time the laser should be on */
  return (FREQUENCY_FULL_HZ / currentFrequency) * currentDutyCycle;
}

uint16_t GetSequenceMilli_Off() {
  /* calculates the time the laser should be off */
  return (FREQUENCY_FULL_HZ / currentFrequency) * (DUTYCYCLE_TOT - currentDutyCycle);
}

void CommandAcknowledge() {
  /* simply blink the lasers, to show we did something */
  for(byte i = 0; i < 3; i++ ) {
    digitalWrite(LASER_PINOUT, HIGH);
    delay(75);
    digitalWrite(LASER_PINOUT, LOW);
    delay(75);
  }
}

void processFrequency() {
  /* as the press of the button cycles the frequencies, we take the next freq from currently selected */ 
  switch (currentFrequency) {
    case FREQUENCY_5_HZ :
      currentFrequency = FREQUENCY_10_HZ;
      break;
    case FREQUENCY_10_HZ :
      currentFrequency = FREQUENCY_15_HZ;
      break;
    case FREQUENCY_15_HZ :
      currentFrequency = FREQUENCY_20_HZ;
      break;
    case FREQUENCY_20_HZ :
      currentFrequency = FREQUENCY_5_HZ;
      break;
  }
 CommandAcknowledge();
}

void processDutyCycles() {
  /* same as above, but with duty cycles */ 
  switch (currentDutyCycle) {
    case DUTYCYCLE_50 :
      currentDutyCycle = DUTYCYCLE_40;
      break;
    case DUTYCYCLE_40 :
      currentDutyCycle = DUTYCYCLE_30;
      break;
    case DUTYCYCLE_30 :
      currentDutyCycle = DUTYCYCLE_20;
      break;
    case DUTYCYCLE_20 :
      currentDutyCycle = DUTYCYCLE_10;
      break;
    case DUTYCYCLE_10 :
      currentDutyCycle = DUTYCYCLE_50;
      break;
  }
  CommandAcknowledge();
}

void SetState(enum LaserStates newState) {
  /* currently, this is just a setter.  maybe adding some logic would be usefull, but not now */
  currentState = newState;
}

/* the two following are switching states to signal new cycle of frequency */
void onDoubleClick() {
  if (currentState == COMMAND_WAIT) {
    SetState( COMMAND_DUTYCYCLE );
  }
}

void onLongPressed() {
  if (currentState == COMMAND_WAIT) {
    SetState( COMMAND_FREQUENCY );
  }
}

/* single press is a bit of a multi value system... */
void onSinglePressed() {
  switch (currentState) {
    /* a single press will stop any cycles, and put the system on command mode */
    case CYCLE_ON_WAIT:
    case CYCLE_OFF_WAIT:
      SetState(COMMAND_ON);
      break;

    case COMMAND_WAIT:
      /* this is the same as a resume */
      SetState( COMMAND_OFF );
      break;
  }
}

/////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

class Button{
private:
  OneButton button;
public:
  explicit Button(uint8_t pin):button(pin) {
    button.setClickTicks(CLICK_MS_DURATION);
    button.attachClick([](void *scope) { ((Button *) scope)->Clicked();}, this);
    button.attachDoubleClick([](void *scope) { ((Button *) scope)->DoubleClicked();}, this);
    button.attachLongPressStart([](void *scope) { ((Button *) scope)->LongPressed();}, this);
  }

  void Clicked() {
    onSinglePressed();
  }

  void DoubleClicked() {
    onDoubleClick();
  }

  void LongPressed() {
    onLongPressed();
  }

  void read() {
    button.tick();
  }
};

Button button(PUSH_BUTTON);


/////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void processInputs() {

  switch (currentState) {
    case MOMENTARY_ON :
    case MOMENTARY_WAIT:
    case MOMENTARY_OFF:
      if (digitalRead(PUSH_BUTTON) == LOW) {
        SetState(MOMENTARY_ON);
      } else {
        SetState(MOMENTARY_OFF);
      }
      break;

    default:
      button.read();
      break;
  }      
}

void processStates() {
  switch (currentState) {
    case CYCLE_ON_START:
      digitalWrite(LASER_PINOUT, HIGH);
      iNextCycleTime = millis() + GetSequenceMilli_On();
      SetState( CYCLE_ON_WAIT );
      break;

    case CYCLE_ON_WAIT:
      if (millis() >= iNextCycleTime) {
        SetState( CYCLE_ON_END );
      }
      break;

    case CYCLE_ON_END:
      SetState( CYCLE_OFF_START );
      break;

    case CYCLE_OFF_START:
      digitalWrite(LASER_PINOUT, LOW);
      iNextCycleTime = millis() + GetSequenceMilli_Off();
      SetState( CYCLE_OFF_WAIT );
      break;

    case CYCLE_OFF_WAIT:
      if (millis() >= iNextCycleTime) {
        SetState( CYCLE_OFF_END );
      }
      break;

    case CYCLE_OFF_END:
      SetState( CYCLE_ON_START );
      break;

    case COMMAND_ON:
      digitalWrite(LASER_PINOUT, LOW);
      SetState( COMMAND_WAIT );
      break;

    case COMMAND_WAIT:
      /*nothing to do, at least so far.*/
      break;

    case COMMAND_OFF:
      SetState( CYCLE_ON_START );
      break;

    case COMMAND_FREQUENCY:
      processFrequency();
      SetState( COMMAND_WAIT );
      break;

    case COMMAND_DUTYCYCLE:
      processDutyCycles();
      SetState( COMMAND_WAIT );
      break;

    case MOMENTARY_ON:
      if (digitalRead(LASER_PINOUT) == LOW) {
        digitalWrite(LASER_PINOUT, HIGH);
      } 
      SetState(MOMENTARY_WAIT);
      break;

   case  MOMENTARY_WAIT:
      break;

    case MOMENTARY_OFF:
      if (digitalRead(LASER_PINOUT) == HIGH) {
        digitalWrite(LASER_PINOUT, LOW);
      }
      SetState(MOMENTARY_WAIT);
      break;

    default:
      break;
  }
}

/*======================================================================*/

void setup() {
  pinMode(LASER_PINOUT, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);  // in cycle, button manager sets it already. This is for momentary mode, to be coherent

  digitalWrite(LASER_PINOUT, HIGH);    // light the laser, to show we are working
  delay(250);                          // a quarter of a second waiting, as to be sure the button will be correctly read
  
  if (digitalRead(PUSH_BUTTON) == LOW) { // is the button being pressed  at startup ?
    SetState(MOMENTARY_ON);  // so we are in momentary mode. Since the button is detected as pressed, set state alike
  } else {
    SetState(COMMAND_WAIT); // otherwise, we are waiting for a key press for cycle start. We enter the command mode for this reason
  }

  CommandAcknowledge();
}

void loop() {
  processInputs();
  processStates();
}