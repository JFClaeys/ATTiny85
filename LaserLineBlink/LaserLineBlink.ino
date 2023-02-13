#include <OneButton.h>

/* frequency formula : 500 DIV xFrequency = Sequence On and Off time, on a 50% duty cycle */
#define FREQUENCY_5_HZ 5   //  5 hertz, i.e.:  5 cycle per second
#define FREQUENCY_10_HZ 10 // 10 hertz, i.e.: 10 cycle per second
#define FREQUENCY_15_HZ 15 // 15 hertz, i.e.: 15 cycle per second
#define FREQUENCY_20_HZ 20 // 20 hertz, i.e.: 02 cycle per second

#define DUTYCYCLE_50 500   // 50% on, 50% off

#define CALC_FREQUENCY_CYCLE( x )  DUTYCYCLE_50 / x

#define CLICK_MS_DURATION 120

#define LASER_PINOUT PB4
#define PUSH_BUTTON PB3

//forward declarations
void onSinglePressed();
void onDoubleClick();
void onLongPressed();

bool isLaser_lit = false;  // have we requested leds to be visible or not? (i.e: pause mode)
bool isMomentaryMode = false; // this mode will be activated only when button is pushed while booting
uint8_t currentFrequency = FREQUENCY_5_HZ;

uint16_t GetSequenceMilli_On( uint8_t currentFreq ) {
  return CALC_FREQUENCY_CYCLE( currentFreq );
}

uint16_t GetSequenceMilli_Off( uint8_t currentFreq ) {
  return CALC_FREQUENCY_CYCLE( currentFreq );
}

void CommandAcknowledge() {
  for(byte i = 0; i < 3; i++ ) {
    digitalWrite(LASER_PINOUT, HIGH);
    delay(100);
    digitalWrite(LASER_PINOUT, LOW);
    delay(100);
  }
}

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

void onSinglePressed() {
  isLaser_lit = !isLaser_lit;
  if (!isLaser_lit) {
    digitalWrite(LASER_PINOUT, LOW);
  }
}

void onDoubleClick() {  // test to see double clicking behaviour
  if (!isLaser_lit) {
    // right now, nothing, but I'd like eventually to increase either the frequency or increase/decrease the duty cycle
  }
}

void onLongPressed() {
  if (!isLaser_lit) {
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
}

Button button(PUSH_BUTTON);
uint16_t iWait = 0;
uint16_t iNextCycleTime = GetSequenceMilli_On(currentFrequency);  // current cycle before next serqunec change (from on to off)

void processLoopContent() {
  if (iWait < iNextCycleTime) {
    delay(1);
    iWait++;
    return;
  }

  iWait = 0;

  digitalWrite(LASER_PINOUT, !digitalRead(LASER_PINOUT));
  if (digitalRead(LASER_PINOUT)) {
    iNextCycleTime = GetSequenceMilli_On(currentFrequency);
  } else {
    iNextCycleTime = GetSequenceMilli_Off(currentFrequency);
  }
}

/*======================================================================*/

void setup() {
  pinMode(LASER_PINOUT, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);  // not realy required if not iusing momntsary mode, as it would the the OneButton default

  digitalWrite(LASER_PINOUT, HIGH);    // light the laser, to show we are working
  delay(250);                          // a little quarter of a second waiting, as to be sure the button will be correctly read
  isMomentaryMode = (digitalRead(PUSH_BUTTON) == LOW); // is the button being pressed  at startup ?

  if (isMomentaryMode) {
    CommandAcknowledge();  //extra one to prove we are in this mode
  }
  // standard for both mode
  CommandAcknowledge();
}

void loop() {
  if (isMomentaryMode) {
    // in momentory mode, we are not using the button manager, we are just checking it directly
    isLaser_lit = (digitalRead(PUSH_BUTTON) == LOW);  // input_pullup thus high when not pressed, low when pressed
    digitalWrite(LASER_PINOUT, isLaser_lit);  // adjust the output of the leaser accordingly
  } else {
    // i not in monentary mode, read the button state, react to normal events in the processLoopContent
    button.read();

    if (isLaser_lit) {
      processLoopContent();
    }
  }  
}
