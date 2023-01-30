#include <OneButton.h>

/* frequency formula : 500 DIV xFrequency = Sequence On and Off time, on a 50% duty cycle */
#define FREQUENCY_5_HZ 5   //  5 hertz, i.e.:  5 cycle per second
#define FREQUENCY_10_HZ 10 // 10 hertz, i.e.: 10 cycle per second
#define CALC_FREQUENCY_CYCLE( x )  500 / x

#define CLICK_MS_DURATION 120

#define LASER_PINOUT PB4
#define PUSH_BUTTON PB3

//forward declarations
void onSinglePressed();
void onDoubleClick();
void onLongPressed();

bool isLaser_lit = false;  // have we requested leds to be visible or not? (i.e: pause mode)
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
    if (currentFrequency == FREQUENCY_5_HZ) {
      currentFrequency = FREQUENCY_10_HZ;
    } else {
      currentFrequency = FREQUENCY_5_HZ;
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
  // put your setup code here, to run once:
  pinMode(LASER_PINOUT, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  CommandAcknowledge();
}

void loop() {
  // put your main code here, to run repeatedly:
  button.read();

  if (isLaser_lit) {
    processLoopContent();
  } 
}
