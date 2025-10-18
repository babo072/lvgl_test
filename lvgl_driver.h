#ifndef LVGL_DRIVER_H
#define LVGL_DRIVER_H

#include <Arduino.h>
#include <lvgl.h>
#include "pins_config.h"
#include "src/lcd/jd9165_lcd.h"
#include "src/touch/gt911_touch.h"

// 전역 드라이버 객체
extern jd9165_lcd lcd;
extern gt911_touch touch;

// LVGL 버퍼
extern lv_disp_draw_buf_t draw_buf;
extern lv_color_t *buf;
extern lv_color_t *buf1;

// 초기화 함수
void lvgl_driver_init();

// 콜백 함수들
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

#endif // LVGL_DRIVER_H