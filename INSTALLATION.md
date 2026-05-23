# 🛠️ دليل التثبيت والإعداد

## المتطلبات 📋

### المكونات الإلكترونية
- **ESP32 C6** (الكمبيوتر الصغير)
- **8 ليدات WS2812B** (ليدات RGB ذكية)
- **كابل USB** لبرمجة ESP32
- **مصدر طاقة 5V** (يمكن من USB)
- **أسلاك توصيل** (Jumper Wires)

### المتطلبات البرمجية
- **Arduino IDE** أو **Visual Studio Code + PlatformIO**
- **مكتبة Adafruit NeoPixel**
- **مكتبات ESP32** من Espressif

---

## الخطوات 1️⃣2️⃣3️⃣

### الخطوة 1: تثبيت Arduino IDE
1. حمّل [Arduino IDE](https://www.arduino.cc/en/software) من الموقع الرسمي
2. ثبّته على جهازك

### الخطوة 2: إضافة لوحة ESP32 C6

افتح Arduino IDE واتبع:

1. **File → Preferences** (أو Ctrl+,)
2. ابحث عن "Additional Board Manager URLs"
3. أضف هذا الرابط:
```
https://dl.espressif.com/dl/package_esp32_index.json
```
4. اضغط OK

الآن:
1. **Tools → Board → Boards Manager**
2. ابحث عن "esp32"
3. ثبّت **ESP32 by Espressif Systems**

### الخطوة 3: تثبيت المكتبات المطلوبة

1. **Tools → Manage Libraries** (أو Ctrl+Shift+I)
2. ابحث وثبّت:
   - **Adafruit NeoPixel** (بواسطة Adafruit)
   - **Adafruit BusIO** (تبعية)

---

## التوصيل الكهربائي 🔌

### اتصال ESP32 C6 مع WS2812B

```
ESP32 C6           WS2812B (Data)
GPIO 3    ------→  Din (Data Input)
GND       ------→  GND
5V        ------→  5V
```

### الرسم التوضيحي:
```
┌─────────────────────────────────────┐
│      ESP32 C6                       │
│                                     │
│  GPIO 3 (D3) ──────┐                │
│  GND         ──────┼────┐           │
│  5V          ──────┼────┼────┐      │
└──────────────────────┼────┼────┼─────┘
                       │    │    │
                    ┌──┴────┴────┴──┐
                    │ WS2812B LED   │
                    │ (8 Lights)    │
                    └───────────────┘
```

### ملاحظات مهمة ⚠️
- **تأكد من توصيل GND (الأرضي) للـ ESP32 والليدات!**
- استخدم كابل USB 2.0 أو أقوى للطاقة
- إذا لم تشتغل الليدات، تحقق من الأسلاك

---

## رفع الكود على ESP32 C6 📤

### الخطوة 1: فتح الملف
1. حمّل ملف `ESP32_LED_Control.ino` من المستودع
2. افتحه في Arduino IDE

### الخطوة 2: اختيار اللوحة والمنفذ

في Arduino IDE:
1. **Tools → Board → esp32 → ESP32C6 Dev Module**
2. **Tools → Port → اختر منفذ USB** (مثل COM3 أو /dev/ttyUSB0)

### الخطوة 3: الرفع
1. اضغط زر **Upload** (أو Ctrl+U)
2. انتظر حتى انتهاء العملية ✅

### الخطوة 4: اختبار
1. افتح **Tools → Serial Monitor**
2. اضبط السرعة على **115200**
3. يجب أن تظهر رسائل الترحيب

---

## الاتصال من الموبايل 📱

### المتطلبات على Android
- **تطبيق Serial Bluetooth Terminal** (مجاني)
  - [رابط Google Play](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)
  - أو أي تطبيق Bluetooth Terminal آخر

### خطوات الاتصال:

1. **تفعيل البلوتوث على الموبايل**
   - Settings → Bluetooth → On

2. **البحث عن الجهاز**
   - اضغط Scan
   - ابحث عن `LED_CONTROL_BLE`

3. **الاتصال**
   - اضغط على `LED_CONTROL_BLE`
   - ستظهر رسالة "Connected" ✅

4. **إرسال أوامر**
   - اكتب أمر (مثل: `ON`)
   - اضغط Send

---

## اختبار الأوامر 🧪

بعد الاتصال، جرّب هذه الأوامر:

```
1. أرسل: ON
   → يجب أن تضيء الليدات

2. أرسل: BRIGHTNESS_UP
   → السطوع يزيد

3. أرسل: COLOR_UP
   → اللون يتغير

4. أرسل: MODE_FIRE
   → تأثير النار يبدأ

5. أرسل: OFF
   → الليدات تطفأ
```

---

## استكشاف الأخطاء 🔧

### المشكلة: لا يظهر المنفذ (COM Port)
**الحل:**
- تحقق من اتصال USB
- ثبّت درايفرات CH340 أو CP210x من [silabs.com](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- أعد تشغيل Arduino IDE

### المشكلة: الليدات لا تضيء
**الحل:**
- تحقق من الأسلاك
- تأكد من توصيل 5V و GND
- جرّب LED مفردة أولاً

### المشكلة: عدم استقبال الأوامر من الموبايل
**الحل:**
- تحقق من اسم الجهاز `LED_CONTROL_BLE`
- أعد التشغيل (Restart)
- تأكد من اختيار Characteristic الصحيح في التطبيق

### المشكلة: أوامر غير معروفة
**الحل:**
- تحقق من كتابة الأمر بشكل صحيح
- استخدم أحرف كبيرة فقط
- بدون مسافات إضافية

---

## التوثيق الإضافي 📚

- [وثائق ESP32 الرسمية](https://docs.espressif.com/)
- [مكتبة Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [دليل BLE على ESP32](https://www.espressif.com/en/products/connrativity/bluetooth/ble)

---

## اتصل بنا 💬
إذا واجهت مشكلة، أنشئ issue على GitHub!

---

## الترخيص 📄
MIT License
