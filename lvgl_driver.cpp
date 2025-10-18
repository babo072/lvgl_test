#include "lvgl_driver.h"

// 드라이버 객체 생성
jd9165_lcd lcd = jd9165_lcd(LCD_RST);
gt911_touch touch = gt911_touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

// LVGL 버퍼
lv_disp_draw_buf_t draw_buf;
lv_color_t *buf = nullptr;
lv_color_t *buf1 = nullptr;

// 디스플레이 flush 콜백
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  const int offsetx1 = area->x1;
  const int offsetx2 = area->x2;
  const int offsety1 = area->y1;
  const int offsety2 = area->y2;
  lcd.lcd_draw_bitmap(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, &color_p->full);
  lv_disp_flush_ready(disp);
}

// 터치 입력 콜백
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
    Serial.printf("Touch: x=%d, y=%d\r\n", touchX, touchY);
  }
}

// LVGL 드라이버 초기화
void lvgl_driver_init()
{
  Serial.println("Initializing LVGL driver...");
  
  // 하드웨어 초기화
  lcd.begin();
  touch.begin();
  Serial.println("  LCD and Touch initialized");

  // LVGL 초기화
  lv_init();
  Serial.println("  LVGL initialized");

  // 버퍼 할당
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  
  if (!buf || !buf1) {
    Serial.println("  ERROR: Failed to allocate buffers!");
    while(1) delay(1000);
  }
  Serial.println("  Buffers allocated");

  // 디스플레이 버퍼 초기화
  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  // 디스플레이 드라이버 설정
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.full_refresh = true;
  lv_disp_drv_register(&disp_drv);
  Serial.println("  Display driver registered");

  // 터치 입력 드라이버 설정
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  Serial.println("  Touch driver registered");
  
  Serial.println("LVGL driver initialization complete!");
}