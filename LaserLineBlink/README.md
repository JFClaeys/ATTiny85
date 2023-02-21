# first workable project on the ATTiny85

1. Laser is on PB4.
2. There is a button to stop/start is on PB3.
3. currently blinking at 5, 10, 15 or 20 hertz.  One can change the frequency by long pressing the button while not lit
4. Starts with a duty cycle of 50% but can be programmed to go down to 40%, 30%, 20% or even 10% by double clicking the button while not lit.
5. if button is pressed while booting, the button will then act as a temporary on/off button
6. The laser is fed directly from PB4.

chip is powered from a 3.7v lipo battery, adjusted to 3.3v with a buck boost converter

Library for button control : https://github.com/mathertel/OneButton
