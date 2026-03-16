### PiPedal

A bare-metal Raspberry Pi digital guitar pedal that implements overdrive, tone, and delay effects.
Featuring seven peripheral drivers and two hardware drivers written over the course of the quarter and a basic threads package.

## Structure

RX interrupt-driven audio processing takes in I2S data from the PCM1808 ADC and feeds it to the Raspberry Pi, which processes the data and feeds it back out over I2S to the PCM5102 DAC. We sample at 31.25 kHz on a 2 MHz clock, which is just over twice the highest typical frequency from a guitar harmonic to avoid aliasing.
Since guitar signals are mono, we only ever use the left channel of I2S and ignore the right channel.
The PCM1808 requires an external SCK at 512 times the sampling frequency, which we generate using the clock manager on a GPIO pin. 
The PCM5102 has internal PLL that can provide this signal if the input pin is grounded. 

ARM Timer interrupts support a round-robin threading system which runs the display task and the potentiometer monitoring. The potentiometers connect to an ADS1115 4-channel 16-bit ADC. This communicates with the Pi over I2C. Originally this was intended to be the ADC for the input but its low sampling rate made it unsuitable for audio. The display is a 240x320 TFT screen with an ST7789 driver chip, controlled over a 5-wire SPI interface with a D/C line. Operating at a fairly high SPI speed of 60 MHz, drawing the display is still a fairly time-consuming process as it needs to transmit the entire 240x320 screen of data at the maximum rate.

The thread package behind all this runs all the threads in supervisor mode (like our main code). We context-switch at a user-specified timer frequency with an assembly-implemented ISR that handles the register saving and jumps to a C handler for non-timer interrupts. To speed up what was a fairly slow program, a basic page table is implemented in the start assembly to support caching in program memory while leaving peripheral addresses uncached. This gives us an approximate 5 FPS display.

## Demos
In the demos folder, IMG_9489 demonstrates the effect of the lowpass filter on a signal with overdrive cranked all the way up, which effectively gives us a square wave. As the cutoff frequency drops, the square wave demonstrates the exponential growth and decay characteristic of lowpass filtered pulses. IMG_9492 is a general demo of a tone with moderate reverb and clipping, and compares the sound with and without the effects as toggled by the top slider.
