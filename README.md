# LED Control BLE 🎨

تطبيق للتحكم بـ 8 ليدات WS2812B على ESP32 C6 عبر البلوتوث من الموبايل (Android)

## المميزات ✨
- ✅ تشغيل وإطفاء الليدات
- ✅ التحكم بدرجة السطوع (0-255)
- ✅ التحكم بدرجة اللون (0-255)
- ✅ مود نبضات الألوان (Pulse Mode)
- ✅ مود تأثير النار (Fire Effect)
- ✅ واجهة بسيطة عبر Bluetooth Terminal

## المكونات المطلوبة 🔧
- **ESP32 C6**
- **8 ليدات WS2812B**
- **GPIO 3** - اتصال البيانات للليدات
- **5V Power Supply**

## الأوامر المدعومة 📱

| الأمر | الوصف |
|------|-------|
| `ON` | تشغيل الليدات |
| `OFF` | إطفاء الليدات |
| `BRIGHTNESS_UP` | زيادة السطوع بـ 5% |
| `BRIGHTNESS_DOWN` | خفض السطوع بـ 5% |
| `COLOR_UP` | رفع درجة اللون بـ 5 |
| `COLOR_DOWN` | خفض درجة اللون بـ 5 |
| `MODE_PULSE` | تفعيل مود نبضات الألوان |
| `MODE_FIRE` | تفعيل مود تأثير النار |

## التثبيت على Arduino IDE 🛠️

1. تثبيت مكتبات:
   - Adafruit NeoPixel
   - ESP32 by Espressif Systems

2. رفع الكود على ESP32 C6

3. فتح تطبيق Serial Bluetooth Terminal على الموبايل

4. الاتصال بـ `LED_CONTROL_BLE`

## الاتصال البلوتوث 📡
- **اسم الجهاز**: `LED_CONTROL_BLE`
- **BLE UUID Service**: `12345678-1234-1234-1234-123456789012`
- **BLE UUID Characteristic**: `87654321-4321-4321-4321-210987654321`

## المؤلف 👨‍💻
- Amin Jakspar

## الترخيص 📄
MIT License
