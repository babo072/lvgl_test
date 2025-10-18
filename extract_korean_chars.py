#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# 코드에서 실제 사용하는 한글 문자열들
korean_texts = [
    "입력: %s",
    "키보드 입력: %s (길이:%d)\\n", 
    "ESP32-P4 완전한 한글지원 테스트",
    "카운터: 0",
    "시작",
    "설정", 
    "키보드 테스트",
    "모든 한글이 완벽하게 표시됩니다! 깨지지 않아요.",
    "한글 입력 테스트",
    "여기에 입력하세요...",
    "입력된 텍스트: ",
    "버튼 모드"
]

# 모든 한글 문자 추출
korean_chars = set()
for text in korean_texts:
    for char in text:
        if ord(char) >= 44032 and ord(char) <= 55203:  # 한글 범위
            korean_chars.add(char)

# 정렬
korean_chars = sorted(list(korean_chars))

print("실제 사용하는 한글 문자들:")
print(''.join(korean_chars))
print()
print("총", len(korean_chars), "개 문자")
print()

# Unicode 값들
unicode_values = [str(ord(char)) for char in korean_chars]
print("Unicode 값들:")
print(','.join(unicode_values))
print()

# LVGL Font Converter용 Range
print("LVGL Font Converter Range:")
print("32-127," + ','.join(unicode_values))