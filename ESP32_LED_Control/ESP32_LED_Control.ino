#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

// ======================== التكوينات ========================
#define LED_PIN 3           // GPIO 3
#define NUM_LEDS 8          // 8 ليدات
#define LED_BRIGHTNESS 255  // السطوع الابتدائي

// ======================== التصريحات المسبقة ========================
void processCommand(String cmd);
void updateLEDs();
void pulseEffect();
void fireEffect();

// ======================== المتغيرات العامة ========================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// متغيرات التحكم
uint8_t globalBrightness = 255;    // السطوع (0-255)
uint8_t colorHue = 0;              // درجة اللون (0-255)
bool ledsOn = true;                // حالة الليدات
uint8_t currentMode = 0;           // 0: Static, 1: Pulse, 2: Fire

// ======================== BLE Callbacks ========================
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
        Serial.println("📱 تم الاتصال بالموبايل");
        pServer->updateConnParams(0, 0);
    }

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        Serial.println("📱 تم قطع الاتصال");
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.println("📖 طلب قراءة من الموبايل");
        pCharacteristic->setValue("LED_CONTROL_READY");
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        
        if (value.length() > 0) {
            String command = "";
            for (size_t i = 0; i < value.length(); i++) {
                command += (char)value[i];
            }
            command.trim();
            command.toUpperCase();
            
            Serial.print("📨 الأمر المستقبل: ");
            Serial.println(command);
            
            processCommand(command);
            
            // إرسال رد للموبايل
            pCharacteristic->setValue("OK");
            pCharacteristic->notify();
        }
    }
};

// ======================== معالجة الأوامر ========================
void processCommand(String cmd) {
    if (cmd == "ON") {
        ledsOn = true;
        currentMode = 0;
        Serial.println("✅ تشغيل الليدات");
        updateLEDs();
    }
    else if (cmd == "OFF") {
        ledsOn = false;
        Serial.println("❌ إطفاء الليدات");
        updateLEDs();
    }
    else if (cmd == "BRIGHTNESS_UP") {
        globalBrightness = min(255, globalBrightness + 13);
        Serial.print("⬆️ زيادة السطوع: ");
        Serial.println(globalBrightness);
        updateLEDs();
    }
    else if (cmd == "BRIGHTNESS_DOWN") {
        globalBrightness = max(0, globalBrightness - 13);
        Serial.print("⬇️ خفض السطوع: ");
        Serial.println(globalBrightness);
        updateLEDs();
    }
    else if (cmd == "COLOR_UP") {
        colorHue = (colorHue + 5) % 256;
        Serial.print("🔴 رفع درجة اللون: ");
        Serial.println(colorHue);
        updateLEDs();
    }
    else if (cmd == "COLOR_DOWN") {
        colorHue = (colorHue - 5 + 256) % 256;
        Serial.print("🟠 خفض درجة اللون: ");
        Serial.println(colorHue);
        updateLEDs();
    }
    else if (cmd == "MODE_PULSE") {
        currentMode = 1;
        ledsOn = true;
        Serial.println("💫 مود نبضات الألوان");
    }
    else if (cmd == "MODE_FIRE") {
        currentMode = 2;
        ledsOn = true;
        Serial.println("🔥 مود تأثير النار");
    }
    else {
        Serial.println("❓ أمر غير معروف");
    }
}

// ======================== تحديث الليدات ========================
void updateLEDs() {
    if (!ledsOn) {
        strip.clear();
        strip.show();
        return;
    }

    uint32_t color = strip.ColorHSV(colorHue * 257, 255, globalBrightness);
    
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

// ======================== مود نبضات الألوان (Pulse) ========================
void pulseEffect() {
    static unsigned long lastUpdate = 0;
    static uint8_t brightness = 255;
    static int8_t direction = -5;
    
    if (millis() - lastUpdate < 50) return;
    lastUpdate = millis();
    
    globalBrightness = brightness;
    
    brightness += direction;
    if (brightness <= 50 || brightness >= 255) {
        direction *= -1;
    }
    
    updateLEDs();
}

// ======================== مود تأثير النار (Fire) ========================
void fireEffect() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate < 100) return;
    lastUpdate = millis();
    
    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t fireColor = random(0, 3);
        uint8_t flicker = random(100, 256);
        
        uint32_t color;
        if (fireColor == 0) {
            color = strip.Color(255, 0, 0);
        } else if (fireColor == 1) {
            color = strip.Color(255, 100, 0);
        } else {
            color = strip.Color(255, 255, 0);
        }
        
        uint8_t r = (uint8_t)((uint16_t)((color >> 16) & 0xFF) * flicker / 256);
        uint8_t g = (uint8_t)((uint16_t)((color >> 8) & 0xFF) * flicker / 256);
        uint8_t b = (uint8_t)((uint16_t)(color & 0xFF) * flicker / 256);
        
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}

// ======================== الإعداد (Setup) ========================
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("═══════════════════════════════════════");
    Serial.println("🎨 LED Control BLE - ESP32 C6");
    Serial.println("═══════════════════════════════════════");
    
    // تهيئة الليدات
    strip.begin();
    strip.show();
    Serial.println("✅ الليدات جاهزة");
    
    // تهيئة البلوتوث
    BLEDevice::init("LED_CONTROL_BLE");
    Serial.println("✅ BLE Device initialized");
    
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    Serial.println("✅ BLE Server created");
    
    // إنشاء الخدمة
    BLEService *pService = pServer->createService("0000180A-0000-1000-8000-00805F9B34FB");
    Serial.println("✅ BLE Service created");
    
    // إنشاء الخاصية
    pCharacteristic = pService->createCharacteristic(
        "0000FFF1-0000-1000-8000-00805F9B34FB",
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_WRITE_NR |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    Serial.println("✅ BLE Characteristic created");
    
    // إضافة Descriptor للـ Notifications
    pCharacteristic->addDescriptor(new BLE2902());
    
    // إضافة Callback
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
    
    // القيمة الابتدائية
    pCharacteristic->setValue("READY");
    
    pService->start();
    Serial.println("✅ BLE Service started");
    
    // بدء الإعلان
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("0000180A-0000-1000-8000-00805F9B34FB");
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("✅ BLE Advertising started");
    
    Serial.println("═══════════════════════════════════════");
    Serial.println("📱 اسم الجهاز: LED_CONTROL_BLE");
    Serial.println("🔍 ابحث عنه في البلوتوث على الموبايل");
    Serial.println("═══════════════════════════════════════\n");
    
    updateLEDs();
}

// ======================== الحلقة الرئيسية (Loop) ========================
void loop() {
    // التحقق من تغيير حالة الاتصال
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        delay(500);
    }
    if (!deviceConnected && oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        delay(500);
        pServer->startAdvertising();
        Serial.println("🔄 بدء البحث مجدداً...");
    }
    
    // تنفيذ المودات
    switch (currentMode) {
        case 0:
            break;
        case 1:
            pulseEffect();
            break;
        case 2:
            fireEffect();
            break;
    }
    
    delay(10);
}
