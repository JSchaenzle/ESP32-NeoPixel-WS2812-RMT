# ESP32-neopixel-WS2812-RMT
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

In your application init section call `void ws2812_control_init(void)` to initialize the RMT peripheral with the correct configuration.

Whenever you need to update the LEDs simply call `void ws2812_write_leds(struct led_state new_state)`. The `led_state` structure just contains an array of 32-bit integers - one for each LED - that you must set to the desired RGB values. The bottom three bytes of each value are R, G and B.