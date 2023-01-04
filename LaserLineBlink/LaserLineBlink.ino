#include <OneButton.h>

/* frequency formula : 500 DIV xFrequency = Sequence On and Off time, on a 50% duty cycle */
#define FREQUENCY 5  // 10 hertz, i.e.: 10 cycle per second
#define CALC_FREQUENCY_CYCLE( x )  500 / x

#define CLICK_MS_DURATION 120

#define LASER_PINOUT PB0
#define PUSH_BUTTON PB1

const uint16_t gSequence_On = CALC_FREQUENCY_CYCLE(FREQUENCY);
const uint16_t gSequence_Off = CALC_FREQUENCY_CYCLE(FREQUENCY);

//forward declarations
void onSinglePressed();
void onDoubleClick();

bool isLaser_lit = false;  // have we requested leds to be visible or not? (i.e: pause mode)

class Button{
private:
  OneButton button;
public:
  explicit Button(uint8_t pin):button(pin) {
    button.setClickTicks(CLICK_MS_DURATION);
    button.attachClick([](void *scope) { ((Button *) scope)->Clicked();}, this);
    button.attachDoubleClick([](void *scope) { ((Button *) scope)->DoubleClicked();}, this);
  }

  void Clicked() {
    onSinglePressed();
  }

  void DoubleClicked() {
    onDoubleClick();
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

Button button(PUSH_BUTTON);
uint16_t iWait = 0;
uint16_t iNextCycleTime = gSequence_On;  // current cycle before next serqunec change (from on to off)

void processLoopContent() {
  if (iWait < iNextCycleTime) {
    delay(1);
    iWait++;
    return;
  }

  iWait = 0;

  digitalWrite(LASER_PINOUT, !digitalRead(LASER_PINOUT));
  if (digitalRead(LASER_PINOUT)) {
    iNextCycleTime = gSequence_On;
  } else {
    iNextCycleTime = gSequence_Off;
  }
}
/*======================================================================*/

void setup() {
  // put your setup code here, to run once:
  pinMode(LASER_PINOUT, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  for(byte i = 0; i < 3; i++ ) {
    digitalWrite(LASER_PINOUT, HIGH);
    delay(100);
    digitalWrite(LASER_PINOUT, LOW);
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  button.read();

  if (isLaser_lit) {
    processLoopContent();
  } 
}
