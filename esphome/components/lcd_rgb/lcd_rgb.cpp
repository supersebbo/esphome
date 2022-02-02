#include "lcd_rgb.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace lcd_rgb {

static const char *const TAG = "lcd_pcf8574";

static const uint8_t LCD_DISPLAY_BACKLIGHT_ON = 0x08;
static const uint8_t LCD_DISPLAY_BACKLIGHT_OFF = 0x00;

void RGBLCDDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up RGB LCD Display...");
  this->backlight_value_ = LCD_DISPLAY_BACKLIGHT_ON;
  if (!this->write_bytes(this->backlight_value_, nullptr, 0)) {
    this->mark_failed();
    return;
  }

  LCDDisplay::setup();
}
void RGBLCDDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "RGB LCD Display:");
  ESP_LOGCONFIG(TAG, "  Columns: %u, Rows: %u", this->columns_, this->rows_);
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with LCD Display failed!");
  }
}
void RGBLCDDisplay::write_n_bits(uint8_t value, uint8_t n) {
  if (n == 4) {
    // Ugly fix: in the super setup() with n == 4 value needs to be shifted left
    value <<= 4;
  }
  uint8_t data = value | this->backlight_value_;  // Set backlight state
  this->write_bytes(data, nullptr, 0);
  // Pulse ENABLE
  this->write_bytes(data | 0x04, nullptr, 0);
  delayMicroseconds(1);  // >450ns
  this->write_bytes(data, nullptr, 0);
  delayMicroseconds(100);  // >37us
}
void RGBLCDDisplay::send(uint8_t value, bool rs) {
  this->write_n_bits((value & 0xF0) | rs, 0);
  this->write_n_bits(((value << 4) & 0xF0) | rs, 0);
}
void RGBLCDDisplay::backlight() {
  this->backlight_value_ = LCD_DISPLAY_BACKLIGHT_ON;
  this->write_bytes(this->backlight_value_, nullptr, 0);
}
void RGBLCDDisplay::no_backlight() {
  this->backlight_value_ = LCD_DISPLAY_BACKLIGHT_OFF;
  this->write_bytes(this->backlight_value_, nullptr, 0);
}

}  // namespace lcd_rgb
}  // namespace esphome
