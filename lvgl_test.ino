#pragma GCC push_options
#pragma GCC optimize("O3")

#include <Arduino.h>
#include <lvgl.h>
#include "lvgl_driver.h"

// 전역 변수들
static lv_obj_t *label_counter = nullptr;
static lv_obj_t *label_status = nullptr;
static int counter = 0;

// 키보드 UI 전역 변수들
static lv_obj_t *textarea_input = nullptr;
static lv_obj_t *keyboard = nullptr;
static lv_obj_t *label_typed = nullptr;
static bool keyboard_mode = false;

// 디바운싱을 위한 변수들
static uint32_t last_key_time = 0;
static const uint32_t DEBOUNCE_TIME = 300; // 300ms 디바운싱 (더 길게)
static char last_text[128] = "";
static size_t last_text_len = 0;
static bool processing_event = false;

// 키보드 이벤트 핸들러
void keyboard_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  
  // VALUE_CHANGED 이벤트만 처리
  if (code != LV_EVENT_VALUE_CHANGED) {
    return;
  }
  
  // 이벤트 처리 중이면 무시
  if (processing_event) {
    return;
  }
  
  // 디바운싱: 너무 빠른 연속 이벤트 무시
  uint32_t current_time = millis();
  if (current_time - last_key_time < DEBOUNCE_TIME) {
    return;
  }
  
  processing_event = true;
  last_key_time = current_time;
  
  // 텍스트 가져오기
  const char *txt = lv_textarea_get_text(textarea_input);
  size_t current_len = strlen(txt);
  
  // 텍스트 변경 여러 조건으로 확인
  bool text_changed = (strcmp(txt, last_text) != 0) || (current_len != last_text_len);
  
  if (text_changed) {
    strcpy(last_text, txt);
    last_text_len = current_len;
    
    char display_buf[128];
    snprintf(display_buf, sizeof(display_buf), "Typed: %s", txt);
    lv_label_set_text(label_typed, display_buf);
    
    Serial.printf("Keyboard input: %s (len:%d)\n", txt, current_len);
  }
  
  processing_event = false;
}

// 모드 전환 버튼 이벤트 핸들러
void mode_switch_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  
  if (code == LV_EVENT_CLICKED) {
    keyboard_mode = !keyboard_mode;
    
    // 화면 완전히 지우기
    lv_obj_clean(lv_scr_act());
    
    if (keyboard_mode) {
      create_keyboard_ui();
      Serial.println("Switched to keyboard mode");
    } else {
      // 키보드 변수들 초기화
      textarea_input = nullptr;
      keyboard = nullptr;
      label_typed = nullptr;
      
      // 버튼 UI 생성
      create_simple_ui();
      Serial.println("Switched to button mode");
    }
  }
}

// 버튼 이벤트 핸들러
void btn_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  
  if (code == LV_EVENT_CLICKED) {
    counter++;
    
    // 카운터 업데이트
    char buf[32];
    snprintf(buf, sizeof(buf), "Counter: %d", counter);
    lv_label_set_text(label_counter, buf);
    
    // 어떤 버튼이 눌렸는지 표시
    const char *btn_text = lv_label_get_text(lv_obj_get_child(btn, 0));
    char status_buf[64];
    snprintf(status_buf, sizeof(status_buf), "%s clicked! Count: %d", btn_text, counter);
    lv_label_set_text(label_status, status_buf);
    
    Serial.printf("Button clicked! Counter: %d\n", counter);
  }
}

// 간단한 UI 생성
void create_simple_ui()
{
  // 배경색 설정
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);

  // 제목 라벨d:\Arduino\libraries\lvgl\demos\lv_demos.h
  lv_obj_t *label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "ESP32-P4 LVGL Test");
  lv_obj_set_style_text_font(label_title, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(label_title, lv_color_white(), 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

  // 카운터 표시 (버튼 위에 배치)
  label_counter = lv_label_create(scr);
  lv_label_set_text(label_counter, "Counter: 0");
  lv_obj_set_style_text_font(label_counter, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(label_counter, lv_color_white(), 0);
  lv_obj_align(label_counter, LV_ALIGN_TOP_MID, 0, 70);

  // 버튼 1
  lv_obj_t *btn1 = lv_btn_create(scr);
  lv_obj_set_size(btn1, 200, 80);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -60);
  lv_obj_set_style_bg_color(btn1, lv_color_hex(0x2196F3), LV_PART_MAIN);
  lv_obj_add_event_cb(btn1, btn_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *label_btn1 = lv_label_create(btn1);
  lv_label_set_text(label_btn1, "Button 1");
  lv_obj_center(label_btn1);

  // 버튼 2
  lv_obj_t *btn2 = lv_btn_create(scr);
  lv_obj_set_size(btn2, 200, 80);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
  lv_obj_set_style_bg_color(btn2, lv_color_hex(0x4CAF50), LV_PART_MAIN);
  lv_obj_add_event_cb(btn2, btn_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *label_btn2 = lv_label_create(btn2);
  lv_label_set_text(label_btn2, "Button 2");
  lv_obj_center(label_btn2);

  // 키보드 모드 전환 버튼 (상단에 배치)
  lv_obj_t *btn_keyboard = lv_btn_create(scr);
  lv_obj_set_size(btn_keyboard, 180, 45);
  lv_obj_align(btn_keyboard, LV_ALIGN_TOP_RIGHT, -10, 20);
  lv_obj_set_style_bg_color(btn_keyboard, lv_color_hex(0xFF9800), LV_PART_MAIN);
  lv_obj_add_event_cb(btn_keyboard, mode_switch_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *label_kb_btn = lv_label_create(btn_keyboard);
  lv_label_set_text(label_kb_btn, "Keyboard Test");
  lv_obj_center(label_kb_btn);
  
  // 상태 라벨
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Touch the buttons!");
  lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFEB3B), 0);
  lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -20);
}

// 키보드 UI 생성
void create_keyboard_ui()
{
  
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);

  // 제목 라벨
  lv_obj_t *label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "Keyboard Input Test");
  lv_obj_set_style_text_font(label_title, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(label_title, lv_color_white(), 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

  // 텍스트 입력 영역
  textarea_input = lv_textarea_create(scr);
  lv_obj_set_size(textarea_input, 700, 80);
  lv_obj_align(textarea_input, LV_ALIGN_TOP_MID, 0, 80);
  lv_textarea_set_placeholder_text(textarea_input, "Type here...");
  lv_obj_set_style_text_font(textarea_input, &lv_font_montserrat_24, 0);

  // 키보드 생성
  keyboard = lv_keyboard_create(scr);
  lv_obj_set_size(keyboard, 800, 300);
  lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_keyboard_set_textarea(keyboard, textarea_input);
  lv_obj_add_event_cb(keyboard, keyboard_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // 입력된 텍스트 표시 라벨
  label_typed = lv_label_create(scr);
  lv_label_set_text(label_typed, "Typed text: ");
  lv_obj_set_style_text_color(label_typed, lv_color_hex(0xFFEB3B), 0);
  lv_obj_align(label_typed, LV_ALIGN_TOP_MID, 0, 180);
  
  // 모드 전환 버튼
  lv_obj_t *btn_back = lv_btn_create(scr);
  lv_obj_set_size(btn_back, 150, 50);
  lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 20, -20);
  lv_obj_set_style_bg_color(btn_back, lv_color_hex(0xF44336), LV_PART_MAIN);
  lv_obj_add_event_cb(btn_back, mode_switch_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *label_back = lv_label_create(btn_back);
  lv_label_set_text(label_back, "Button Mode");
  lv_obj_center(label_back);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n=========================");
  Serial.println("ESP32-P4 LVGL Simple Test");
  Serial.println("=========================");
  
  // 하드웨어 초기화
  lcd.begin();
  touch.begin();
  Serial.println("LCD and Touch initialized");

  // LVGL 초기화
  lv_init();
  Serial.println("LVGL initialized");

  // 버퍼 할당
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  
  if (!buf || !buf1) {
    Serial.println("ERROR: Failed to allocate buffers!");
    while(1) delay(1000);
  }
  Serial.println("Buffers allocated");

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
  Serial.println("Display driver registered");

  // 터치 입력 드라이버 설정
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  Serial.println("Touch driver registered");

  // 간단한 UI 생성
  create_simple_ui();
  Serial.println("UI created");
  
  Serial.println("\nSetup complete!");
  Serial.println("You should see a blue screen with two buttons.");
}

void loop()
{
  lv_timer_handler();
  delay(5);
}