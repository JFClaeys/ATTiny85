#include <OneButton.h>
#include "LaserLineDefs.h"

#define CLICK_MS_DURATION 120
#define LASER_PINOUT PB4
#define PUSH_BUTTON PB3

enum LaserStates currentState;
enum LaserStates initialState;             // state that is defined at boot time, either cycling or variable
bool isDoingSinus = false;
unsigned long iNextCycleTime = 0;          // current cycle before next sequence change (from on to off)
unsigned int iNextCycleStep = 0;           // while doing variable OFF cycles,will tell which steps we are in
byte currentFrequency = FREQUENCY_20_HZ;   // holds the currently selected frequency
byte currentDutyCycle = DUTYCYCLE_10;      // holds the currently selected duty cycle  

/********sequences controlers********/

unsigned int calculateNextSinValue( unsigned int sinAngle) {
  //return sin( sinAngle * PI_180) and scaled it to 255, even if wee are going to scale it down afterward;
  return ((sin( sinAngle * 0.0174532955) * 2.5) + 2.5 ) * 51;
 }

uint16_t GetSequenceMilli_On() {
  /* calculates the time the laser should be on */
  return DutiesByFreq[currentFrequency][currentDutyCycle].cycleON;
}

uint16_t GetSequenceMilli_Off() {
  /* calculates the time the laser should be off */
  return DutiesByFreq[currentFrequency][currentDutyCycle].cycleOFF; 
}

byte GetVariableMilli_Off(){
    iNextCycleStep = iNextCycleStep + 1;
    if (iNextCycleStep > 36-4) {
      iNextCycleStep = 3;
    }
    return TriangleAndSinDualvalues[iNextCycleStep][isDoingSinus];
}

/********settings controlers********/

void CommandAcknowledge( byte ackonLvl ) {
  /* simply blink the lasers, to show we did something */
  for(byte i = 0; i < ackonLvl + 1; i++ ) {
    digitalWrite(LASER_PINOUT, HIGH);
    delay(50);
    digitalWrite(LASER_PINOUT, LOW);
    delay(50);
  }
}

void processFrequency() {
  /* as the press of the button cycles the frequencies, we take the next freq from currently selected */ 
  currentFrequency++;
  if (currentFrequency >= FREQUENCY_MAX) {
    currentFrequency = FREQUENCY_5_HZ;
  }
 CommandAcknowledge(currentFrequency);
}

void processDutyCycles() {
  /* same as above, but with duty cycles */ 
  currentDutyCycle++;
  if (currentDutyCycle >= DUTYCYCLE_MAX) {
    currentDutyCycle = DUTYCYCLE_50;
  }
  CommandAcknowledge(currentDutyCycle);
}

/********Helpers********/

void SetState(enum LaserStates newState) {
  /* currently, this is just a setter.  maybe adding some logic would be usefull, but not now */
  currentState = newState;
}

LaserStates GetNextState(enum LaserStates newState) {
  switch (newState) {
    case CYCLE_ON_START:  return CYCLE_ON_WAIT;
    case CYCLE_ON_WAIT:   return CYCLE_ON_END;
    case CYCLE_ON_END:    return CYCLE_OFF_START;
    case CYCLE_OFF_START: return CYCLE_OFF_WAIT;
    case CYCLE_OFF_WAIT:  return CYCLE_OFF_END;
    case CYCLE_OFF_END:   return CYCLE_ON_START;

    case VARIABLE_ON_START:  return VARIABLE_ON_WAIT;
    case VARIABLE_ON_WAIT:   return VARIABLE_ON_END;
    case VARIABLE_ON_END:    return VARIABLE_OFF_START;
    case VARIABLE_OFF_START: return VARIABLE_OFF_WAIT;
    case VARIABLE_OFF_WAIT:  return VARIABLE_OFF_END;
    case VARIABLE_OFF_END:   return VARIABLE_ON_START;
  }
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
    case VARIABLE_ON_WAIT:
    case VARIABLE_OFF_WAIT:
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
    case VARIABLE_ON_START:
      digitalWrite(LASER_PINOUT, HIGH);
      iNextCycleTime = millis() + GetSequenceMilli_On();
      SetState( GetNextState(currentState) );
      break;

    case CYCLE_ON_WAIT:
    case VARIABLE_ON_WAIT:
      if (millis() >= iNextCycleTime) {
        SetState( GetNextState(currentState) );
      }
      break;

    case CYCLE_ON_END:
    case VARIABLE_ON_END:
      SetState( GetNextState(currentState) );
      break;

    case CYCLE_OFF_START:
    case VARIABLE_OFF_START:
      if (currentState == CYCLE_OFF_START) {
        iNextCycleTime = millis() + GetSequenceMilli_Off();
      } else {
        iNextCycleTime = millis() + GetVariableMilli_Off();
      }
      // common work
      digitalWrite(LASER_PINOUT, LOW);
      SetState( GetNextState(currentState) );
      break;

    case CYCLE_OFF_WAIT:
    case VARIABLE_OFF_WAIT:
      if (millis() >= iNextCycleTime) {
        SetState( GetNextState(currentState) );
      }
      break;

    case CYCLE_OFF_END:
    case VARIABLE_OFF_END:
      SetState( GetNextState(currentState) );
      break;

    case COMMAND_ON:
      digitalWrite(LASER_PINOUT, LOW);
      SetState( COMMAND_WAIT );
      break;

    case COMMAND_WAIT:
      /*nothing to do, at least so far.*/
      break;

    case COMMAND_OFF:
      SetState( initialState );
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
      /*this is more a placeholder, as to remember we are in momentary mode*/
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

  if (digitalRead(PUSH_BUTTON) == LOW) {
    initialState = VARIABLE_ON_START;
  } else {
    initialState = CYCLE_ON_START;
  };
  CommandAcknowledge(1);
  SetState(COMMAND_WAIT);

  if (initialState == VARIABLE_ON_START) {
    delay(500);
    isDoingSinus = (digitalRead(PUSH_BUTTON) == LOW);
    CommandAcknowledge(2);
  }
}

void loop() {
  processInputs();
  processStates();
}