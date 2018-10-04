# ESP32-neopixel-WS2812-RMT
NeoPixel (WS2812) Driver Example code using RMT peripheral

This project contains example code for driving a chain of NeoPixels connected to an ESP32 using the RMT peripheral build into the micro.

### Pros
- It's very simple to use!
- The code utilizes hardware to drive the data out line so your application can be free to do other things.
- Easily and consistently meets the timing requirements spec'd out by the WS2812 datasheet.


## Cons
Because of the way the ESP32 RMT peripheral works this technique for driving NeoPixels is a little heavy on memory usage. It requires (4 bytes * 24 * NUM_LEDS) of dedicated memory.
