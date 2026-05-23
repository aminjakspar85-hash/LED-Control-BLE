#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

// ======================== التكوينات ========================
#define LED_PIN 3           // GPIO 3
#define NUM_LEDS 8          // 8 ليدات
#define LED_BRIGHTNESS 255  // السطوع الابتدائي

// UUIDs للـ BLE
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

// ======================== المتغيرات العامة ========================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

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
    }

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        Serial.println("📱 تم قطع الاتصال");
        pServer->startAdvertising();
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        
        if (value.length() > 0) {
            String command = String((char *)value.c_str());
            command.trim();
            Serial.print("📨 الأمر المستقبل: ");
            Serial.println(command);
            
            processCommand(command);
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
        globalBrightness = min(255, globalBrightness + 13);  // 5% من 255
        Serial.print("⬆️ زيادة السطوع: ");
        Serial.println(globalBrightness);
        updateLEDs();
    }
    else if (cmd == "BRIGHTNESS_DOWN") {
        globalBrightness = max(0, globalBrightness - 13);   // 5% من 255
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
    static int8_t direction = -5;  // اتجاه التغيير
    
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
        // ألوان النار: أحمر، برتقالي، أصفر مع عشوائية
        uint8_t fireColor = random(0, 3);  // 0: أحمر، 1: برتقالي، 2: أصفر
        uint8_t flicker = random(100, 256);  // ارتعاش
        
        uint32_t color;
        if (fireColor == 0) {
            // أحمر
            color = strip.Color(255, 0, 0);
        } else if (fireColor == 1) {
            // برتقالي
            color = strip.Color(255, 100, 0);
        } else {
            // أصفر
            color = strip.Color(255, 255, 0);
        }
        
        // تطبيق الارتعاش
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
    
    // تهيئة البلوتوث
    BLEDevice::init("LED_CONTROL_BLE");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // إنشاء الخدمة (Service)
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // إنشاء الخاصية (Characteristic)
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Ready");
    
    pService->start();
    
    // بدء الإعلان
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    
    Serial.println("✅ البلوتوث جاهز - اسم الجهاز: LED_CONTROL_BLE");
    Serial.println("═══════════════════════════════════════");
    
    updateLEDs();
}

// ======================== الحلقة الرئيسية (Loop) ========================
void loop() {
    // تنفيذ المودات
    switch (currentMode) {
        case 0:  // Static mode
            break;
        case 1:  // Pulse mode
            pulseEffect();
            break;
        case 2:  // Fire mode
            fireEffect();
            break;
    }
    
    delay(10);
}
