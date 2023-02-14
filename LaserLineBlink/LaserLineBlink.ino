#include <OneButton.h>

#define FREQUENCY_5_HZ 5   //  5 hertz, i.e.:  5 cycle per second
#define FREQUENCY_10_HZ 10 // 10 hertz, i.e.: 10 cycle per second
#define FREQUENCY_15_HZ 15 // 15 hertz, i.e.: 15 cycle per second
#define FREQUENCY_20_HZ 20 // 20 hertz, i.e.: 02 cycle per second
#define FREQUENCY_FULL_HZ 100

#define DUTYCYCLE_TOT 10
#define DUTYCYCLE_50 5   // 50% on, 50% off
#define DUTYCYCLE_40 4   // 40% on, 60% off
#define DUTYCYCLE_30 3   // 40% on, 60% off
#define DUTYCYCLE_20 2   // 40% on, 60% off
#define DUTYCYCLE_10 1   // 40% on, 60% off

#define CLICK_MS_DURATION 120

#define LASER_PINOUT PB4
#define PUSH_BUTTON PB3

//forward declarations
void onSinglePressed();
void onDoubleClick();
void onLongPressed();

bool isLaser_lit = false;     // have we requested leds to be visible or not? (i.e: pause mode)
bool isMomentaryMode = false; // this mode will be activated only when button is pushed while booting
uint8_t currentFrequency = FREQUENCY_5_HZ;  // holds the currently selected frequency
uint8_t currentDutyCycle = DUTYCYCLE_50;    // holds the currently selected duty cycle
uint16_t seqMilli_On;         //stores the calculated ON duty cycle duration
uint16_t seqMilli_Off;        //stores the calculated OFF duty cycle duration
uint16_t iNextCycleTime;      // current cycle before next serqunec change (from on to off)
uint16_t iWait = 0;

uint16_t GetSequenceMilli_On() {
  return (FREQUENCY_FULL_HZ / currentFrequency) * currentDutyCycle;
}

uint16_t GetSequenceMilli_Off() {
  return (FREQUENCY_FULL_HZ / currentFrequency) * (DUTYCYCLE_TOT - currentDutyCycle);
}

void CommandAcknowledge() {
  seqMilli_On = GetSequenceMilli_On();
  seqMilli_Off = GetSequenceMilli_Off();

  for(byte i = 0; i < 3; i++ ) {
    digitalWrite(LASER_PINOUT, HIGH);
    delay(75);
    digitalWrite(LASER_PINOUT, LOW);
    delay(75);
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

void processLoopContent() {
  if (iWait < iNextCycleTime) {
    delay(1);
    iWait++;
    return;
  }

  iWait = 0;

  digitalWrite(LASER_PINOUT, !digitalRead(LASER_PINOUT));
  if (digitalRead(LASER_PINOUT)) {
    iNextCycleTime = seqMilli_On;
  } else {
    iNextCycleTime = seqMilli_Off;
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
  iNextCycleTime = seqMilli_On;  // current cycle before next serqunec change (from on to off)  
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
