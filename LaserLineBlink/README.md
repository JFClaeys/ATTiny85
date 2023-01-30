# first workable project on the ATTiny85

1. Laser toggling.  
2. currently blinking at 5 hertz.
3. laser is on PB0.
4. There is a button to stop/start is on PB1.  It's actually behaving like a toggling button.
5. The laser is fed directly from PB0.

chip is powered from a 3.7v lipo battery, adjusted to 3.3v with a buck boost converter

Library for button control : https://github.com/mathertel/OneButton
