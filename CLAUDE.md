# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an ESP32-P4 LVGL (Light and Versatile Graphics Library) test project that demonstrates touch screen interface functionality. The project creates a simple UI with interactive buttons on a 1024x600 touchscreen display using the JD9165 LCD controller and GT911 touch controller.

## Development Setup

This project requires:
- **Arduino ESP32 v3.1** - Must use this specific version as noted in README.md
- **LVGL v8** library with specific configuration settings

### Required LVGL Configuration (lv_conf.h)
```c
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_MEM_CUSTOM 1
#define LV_TICK_CUSTOM 1
#define LV_MEMCPY_MEMSET_STD 1
#define LV_ATTRIBUTE_FAST_MEM   IRAM_ATTR
```

## Build and Development Commands

This is an Arduino project. Use the Arduino IDE or Platform.IO to:
- **Build**: Compile through Arduino IDE or `pio run` if using Platform.IO
- **Upload**: Flash to ESP32-P4 via Arduino IDE or `pio run --target upload`
- **Monitor**: Use Arduino Serial Monitor or `pio device monitor`

## Hardware Configuration

- **Display**: 1024x600 resolution using JD9165 LCD controller
- **Touch**: GT911 capacitive touch controller via I2C
- **Pin Configuration** (pins_config.h):
  - LCD_RST: GPIO 27
  - LCD_LED: GPIO 23
  - Touch I2C SDA: GPIO 7
  - Touch I2C SCL: GPIO 8
  - Touch RST: GPIO 22
  - Touch INT: GPIO 21

## Architecture

### Core Components

1. **Main Application** (`lvgl_test.ino`):
   - Entry point with setup() and loop()
   - UI creation and button event handling
   - Uses Korean comments but functionality is universal

2. **LVGL Driver Layer** (`lvgl_driver.h/cpp`):
   - Abstracts LVGL initialization and callback functions
   - Manages display flush and touch input callbacks
   - Handles buffer allocation in SPIRAM

3. **Hardware Abstraction**:
   - **LCD Driver** (`src/lcd/jd9165_lcd.*`): JD9165 display controller interface
   - **Touch Driver** (`src/touch/gt911_touch.*`): GT911 touch controller interface
   - **ESP LCD Drivers** (`src/lcd/esp_lcd_jd9165.*`, `src/touch/esp_lcd_touch*`): ESP-IDF level drivers

### Memory Management
- Uses dual buffering with SPIRAM allocation for smooth graphics
- Full screen refresh mode enabled for optimal performance
- Buffer size: `sizeof(lv_color_t) * 1024 * 600`

### Key Design Patterns
- Hardware abstraction through driver classes
- LVGL callback-based architecture for display and input
- Event-driven UI with button callbacks
- Global driver object instances for hardware access

## Development Notes

- Display uses 16-bit color depth with no byte swapping
- Touch coordinates are directly mapped (no rotation by default)
- Serial output provides debugging information during initialization and touch events
- UI uses blue theme with white text and colored buttons