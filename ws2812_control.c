#include "ws2812_control.h"
#include "driver/rmt.h"
#include "esp_err.h"
#include "esp_check.h"

// Configure these based on your project needs using menuconfig ********
#define LED_RMT_TX_CHANNEL		CONFIG_WS2812_LED_RMT_TX_CHANNEL
#define LED_RMT_TX_GPIO			CONFIG_WS2812_LED_RMT_TX_GPIO
// ****************************************************


#if CONFIG_WS2812_LED_TYPE_RGB
#define BITS_PER_LED_CMD	24
#elif CONFIG_WS2812_LED_TYPE_RGBW
#define BITS_PER_LED_CMD	32
#endif

#define LED_BUFFER_ITEMS	(NUM_LEDS * BITS_PER_LED_CMD)

// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H	CONFIG_WS2812_T0H  // 0 bit high time
#define T1H	CONFIG_WS2812_T1H  // 1 bit high time
#define T0L	CONFIG_WS2812_T0L  // low time for either bit
#define T1L	CONFIG_WS2812_T1L

// Tag for log messages
static const char *TAG = "NeoPixel WS2812 Driver";

// This is the buffer which the hw peripheral will access while pulsing the output pin
static rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];

static void setup_rmt_data_buffer(struct led_state new_state);

esp_err_t ws2812_control_init(void)
{
  rmt_config_t config = {
    .rmt_mode = RMT_MODE_TX,
    .channel = LED_RMT_TX_CHANNEL,
    .gpio_num = LED_RMT_TX_GPIO,
    .mem_block_num = 3,
    .tx_config.loop_en = false,
    .tx_config.carrier_en = false,
    .tx_config.idle_output_en = true,
    .tx_config.idle_level = 0,
    .clk_div = 2
  };

  ESP_RETURN_ON_ERROR(rmt_config(&config), TAG, "Failed to configure RMT");
  ESP_RETURN_ON_ERROR(rmt_driver_install(config.channel, 0, 0), TAG, "Failed to install RMT driver");

  return ESP_OK;
}

esp_err_t ws2812_write_leds(struct led_state new_state)
{
  setup_rmt_data_buffer(new_state);
  ESP_RETURN_ON_ERROR(rmt_write_items(LED_RMT_TX_CHANNEL, led_data_buffer, LED_BUFFER_ITEMS, false), TAG, "Failed to write items");
  ESP_RETURN_ON_ERROR(rmt_wait_tx_done(LED_RMT_TX_CHANNEL, portMAX_DELAY), TAG, "Failed to wait for RMT transmission to finish");

  return ESP_OK;
}

static void setup_rmt_data_buffer(struct led_state new_state)
{
  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    uint32_t bits_to_send = new_state.leds[led];
    uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);

    for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++) {
      uint32_t bit_is_set = bits_to_send & mask;
      led_data_buffer[(led * BITS_PER_LED_CMD) + bit] = bit_is_set ?
                                                      (rmt_item32_t){{{T1H, 1, T1L, 0}}} :
                                                      (rmt_item32_t){{{T0H, 1, T0L, 0}}};
      mask >>= 1;
    }
  }
}
