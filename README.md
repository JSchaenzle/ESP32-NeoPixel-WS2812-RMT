# ESP32-NeoPixel-WS2812-RMT
NeoPixel (WS2812) Driver Example code using RMT peripheral

This project contains example code for driving a chain of NeoPixels connected to an ESP32 using the RMT peripheral build into the micro.

This code assumes you are using FreeRTOS.

## Pros
- It's very simple to use!
- The code utilizes hardware to drive the data out line so your application can be free to do other things.
- Easily and consistently meets the timing requirements spec'd out by the WS2812 datasheet.

## Cons
Because of the way the ESP32 RMT peripheral works this technique for driving NeoPixels is a little heavy on memory usage. It requires (4 bytes * 24 * NUM_LEDS) of dedicated memory.

## Usage
Copy the source and header files into your project. Update the following config options based on your project needs / arrangement.

- CONFIG_WS2812_NUM_LEDS
- CONFIG_WS2812_LED_RMT_TX_GPIO
- CONFIG_WS2812_T0H  // 0 bit high time
- CONFIG_WS2812_T1H  // 1 bit high time
- CONFIG_WS2812_T0L  // 0 bit low time
- CONFIG_WS2812_T1L  // 1 bit low time

To calculate values for these configuration times, multiply the desired time in _micro_ seconds (μs or us in data sheets) by the default clock rate in MHz and divide by 2. E.g., for 280ns (0.28μs) on an ESP32 with an 80MHz clock: 0.28 * 80 / 2 = 11.2 => 11 (rounded when needed)

In your application init section call `void ws2812_control_init(void)` to initialize the RMT peripheral with the correct configuration.

Whenever you need to update the LEDs simply call `void ws2812_write_leds(struct led_state new_state)`. The `led_state` structure just contains an array of 32-bit integers - one for each LED - that you must set to the desired RGB values. The bottom three bytes of each value are R, G and B.

_Note:_ many surface-mount addressable LEDs that use this protocol arrange the bytes G, R, B instead of R, G, B.

### Example
```c
#include "ws2812_control.h"

#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

int main(void)
{
  ws2812_control_init();

  struct led_state new_state;
  new_state.leds[0] = RED;
  new_state.leds[1] = GREEN;
  new_state.leds[2] = BLUE;

  ws2812_write_leds(new_state);
}
```

### Timing
This code is tuned based on the timing specifications indicated in the following datasheet provided by Sparkfun: https://cdn.sparkfun.com/datasheets/Components/LED/COM-12877.pdf

#### LED TIMINGS, per their datasheets:

##### WS2811: (2.5us bit time, 400Kbps)
    T0H: 0.5us <-- 0 bit
    T0L: 2.0us
    T1H: 1.2us <-- 1 bit
    T1L: 1.3us
    RES: 50us
##### WS2812: (1.25us bit time, 800Kbps)
    T0H: 0.35us <-- 0 bit
    T0L: 0.8us
    T1H: 0.7us <-- 1 bit
    T1L: 0.6us
    RES: 50us
##### WS2812b: (1.25us bit time, 800Kbps)
    T0H: 0.4us <-- 0 bit
    T0L: 0.85us
    T1H: 0.8us <-- 1 bit
    T1L: 0.45us
    RES: 50us

If you use WiFi, you may find that it causes problems with the LEDs not being set correctly.
You can use xTaskCreatePinnedToCore to run the thread which initializes the library (and registers the rtm_isr), to run on core 1.

## Contribution
If you find a problem or have ideas about how to improve this please submit a PR and I will happily review and merge. Thanks!

Enjoy!
