# 🤖 Hand Gesture Control System + ESP32 Log Manager

## 🌟 Features

- **ตรวจจับท่าทางมือแบบเรียลไทม์** ด้วย MediaPipe
- **ควบคุมแฟลช ESP32** ผ่าน HTTP requests
- **3 โหมดการทำงาน**: Normal, Symbol Control, Volume Control
- **ปรับปรุงประสิทธิภาพ** สำหรับการทำงานแบบ Real-time
- **รองรับกล้องหลายแหล่ง** (ESP32 stream และกล้องในเครื่อง)
- **ควบคุมผ่าน Telegram Bot ได้**
- **ESP32 เก็บ log ล่าสุด 20 รายการใน buffer**
- **สั่งส่ง log ผ่านคำสั่ง /sent ผ่าน Telegram ได้**
- **ESP32 ทำงานเป็น HTTP Server และเชื่อมต่อกับ WiFi**

## 📋 Requirements

### Hardware
- **ESP32-CAM AI Thinker Module** (พร้อม Flash LED)
- **FTDI USB to Serial Adapter** (สำหรับการอัปโหลดโค้ด)
- **ESP8266**
- **Telegram account** (พร้อม Flash LED)
- คอมพิวเตอร์ที่มีกล้อง (หากไม่มี ESP32)

### Software Dependencies

#### Python Libraries (รองรับ Mirror Server จีน)
```bash
# การติดตั้งแบบมาตรฐาน
pip install opencv-python mediapipe requests numpy

# หรือใช้ Mirror Server จีน (เร็วกว่า)
pip install --timeout=300 opencv-python mediapipe requests numpy -i https://pypi.tuna.tsinghua.edu.cn/simple

# หรือใช้ requirements.txt
pip install --timeout=300 -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple
```

#### สร้างไฟล์ requirements.txt
```text
opencv-python>=4.5.0
mediapipe>=0.10.0
requests>=2.25.0
numpy>=1.21.0
```

### Arduino IDE สำหรับ ESP32
1. ติดตั้ง Arduino IDE
2. เพิ่ม ESP32 Board Manager URL:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. ติดตั้ง ESP32 boards package
4. เลือก Board: "AI Thinker ESP32-CAM"

## ⚙️ Configuration

### ขั้นตอนที่ 1: ตั้งค่า ESP32-CAM

#### 1.1 อัปโหลดโค้ด ESP32
ใช้โค้ด ESP32 ที่ให้มา (esp32_camera_server.ino):

```cpp
// กำหนด WiFi credentials
const char *ssid = "ชื่อ_WiFi_ของคุณ";
const char *password = "รหัสผ่าน_WiFi";
```
### Arduino IDE สำหรับ ESP8266
- `ESP8266WiFi`
- `ESP8266WebServer`
- `WiFiClientSecure`
- `ESP8266HTTPClient`
- `UniversalTelegramBot`
- `ArduinoJson`

#### 1.3 หลังอัปโหลดเสร็จ
1. ถอด GPIO 0 จาก GND
2. Reset ESP32-CAM
3. เปิด Serial Monitor (115200 baud)
4. จดบันทึก IP address ที่แสดง

### ขั้นตอนที่ 2: ตั้งค่า Python Script

แก้ไขค่าใน section `CONFIG`:

```python
ESP32_IP = "192.168.x.x"  # เปลี่ยนเป็น IP ของ ESP32 ที่ได้จาก Serial Monitor
```

### ESP32 Endpoints และฟีเจอร์
- **Flash ON**: `http://[ESP32_IP]/flash/on`
- **Flash OFF**: `http://[ESP32_IP]/flash/off`  
- **Camera Stream**: `http://[ESP32_IP]/stream`

#### ESP32-CAM Pinout ที่ใช้:
- **Flash LED**: GPIO 4
- **Camera Pins**: ตาม AI Thinker ESP32-CAM Standard
  - PWDN: GPIO 32
  - RESET: -1 (ไม่ใช้)
  - XCLK: GPIO 0
  - และอื่นๆ ตามโค้ด

#### คุณสมบัติ ESP32 Server:
- **MJPEG Streaming**: สตรีมวิดีโอแบบเรียลไทม์
- **Flash Control**: ควบคุม LED แฟลชผ่าน HTTP GET
- **Auto Frame Size**: ปรับขนาดเฟรมอัตโนมัติตาม PSRAM
- **Quality Optimization**: ปรับคุณภาพภาพตามหน่วยความจำ

## 🎮 การใช้งาน

### การรันโปรแกรม
```bash
python hand_gesture_control.py
```

### ปุ่มควบคุม
- **ESC**: ออกจากโปรแกรม
- **F**: เปิด/ปิดแฟลชแบบ Manual

## 🤖 โหมดการทำงาน

### 1. NORMAL Mode (โหมดปกติ)
- **สีแสดง**: ฟ้า
- **วิธีเปลี่ยนโหมด**: แสดงฝ่ามือเปิด (5 นิ้ว) เป็นเวลา 1.5 วินาที
- **การทำงาน**: รอรับคำสั่งเพื่อเปลี่ยนโหมด

### 2. SYMBOL Control Mode (โหมดควบคุมสัญลักษณ์)
- **สีแสดง**: เหลือง
- **การควบคุมแฟลช**: 
  - กำมือ (Fist) = เปิดแฟลช
  - มือเปิด = ปิดแฟลช
- **เปลี่ยนโหมด**:
  - แสดงฝ่ามือซ้ายเปิด (5 นิ้ว) 1.5 วินาที → Volume Control
  - ทำสัญลักษณ์ OK เป็นเวลา 0.8 วินาที → Normal

### 3. VOLUME Control Mode (โหมดควบคุมเสียง)
- **สีแสดง**: เหลืองสว่าง
- **การควบคุม**: ระยะห่างนิ้วหัวแม่มือและนิ้วชี้มือซ้าย
  - ระยะใกล้ = เสียงต่ำ (0%)
  - ระยะไกล = เสียงสูง (100%)
- **เปลี่ยนโหมด**:
  - แสดงฝ่ามือขวาเปิด (5 นิ้ว) 1.5 วินาที → Symbol Control
  - ทำสัญลักษณ์ OK เป็นเวลา 0.8 วินาที → Normal

## 🔧 Technical Details

### การตรวจจับท่าทางมือ

#### ท่าทางที่รองรับ:
1. **Open Hand (ฝ่ามือเปิด)**: 5 นิ้วทั้งหมดเปิด
2. **Fist (กำมือ)**: นิ้วทั้งหมดหุบ
3. **OK Symbol**: นิ้วหัวแม่มือและนิ้วชี้แตะกัน นิ้วอื่นเปิด
4. **Finger Count**: นับจำนวนนิ้วที่เปิด

#### การปรับปรุงประสิทธิภาพ:
- **Frame Skipping**: ประมวลผลทุก 3 เฟรม
- **Optimized Detection**: ปรับค่า confidence และ tracking
- **Fast Calculations**: ใช้การคำนวณแบบเร็วขึ้น
- **Reduced History**: ลดข้อมูลประวัติที่เก็บ

### MediaPipe Configuration
```python
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=2,                # รองรับ 2 มือ
    min_detection_confidence=0.7,   # ความแม่นยำการตรวจจับ
    min_tracking_confidence=0.7,    # ความแม่นยำการติดตาม
    model_complexity=1              # โมเดลความซับซ้อนกลาง
)
```

### ค่าการปรับแต่ง
```python
OPEN_HAND_DURATION = 1.5    # เวลาแสดงฝ่ามือเพื่อเปลี่ยนโหมด
OK_HOLD_DURATION = 0.8      # เวลาทำสัญลักษณ์ OK
MAX_DISTANCE = 0.35         # ระยะสูงสุดสำหรับ Volume Control
MIN_DISTANCE = 0.02         # ระยะต่ำสุดสำหรับ Volume Control
```

## 🚀 การปรับปรุงประสิทธิภาพ

### เทคนิคที่ใช้:
1. **Frame Processing Optimization**: ประมวลผลเฟรมไม่ต่อเนื่อง
2. **Fast Distance Calculation**: ใช้ระยะกำลังสองแทน square root
3. **Simplified Gesture Detection**: ลดความซับซ้อนการตรวจจับ
4. **Reduced Memory Usage**: ใช้ deque จำกัดขนาด
5. **Silent Fail HTTP Requests**: ไม่แสดง error เพื่อความเร็ว

## 🛠️ การแก้ไขปัญหา

### ESP32-CAM ไม่สามารถอัปโหลดได้
1. **ตรวจสอบการเชื่อมต่อ FTDI**:
   - VCC → 5V (หรือ 3.3V ตาม FTDI)
   - GND → GND
   - TX → U0R
   - RX → U0T
2. **เข้าสู่ Programming Mode**:
   - เชื่อม GPIO 0 กับ GND
   - กด Reset บน ESP32-CAM
   - อัปโหลดโค้ด
   - ถอด GPIO 0 จาก GND
   - Reset อีกครั้ง

### กล้องไม่เชื่อมต่อได้
1. **ตรวจสอบ Serial Monitor**:
   ```
   WiFi connected
   IP address: 192.168.x.x
   Camera Ready! Use:
     Flash ON:  http://192.168.x.x/flash/on
     Flash OFF: http://192.168.x.x/flash/off
     Stream:    http://192.168.x.x/stream
   ```
2. **ทดสอบการเชื่อมต่อ**:
   - Ping IP address: `ping 192.168.x.x`
   - ทดสอบใน browser: `http://192.168.x.x/stream`
3. **ตรวจสอบ WiFi**:
   - SSID และ Password ถูกต้อง
   - ESP32 และคอมพิวเตอร์อยู่ใน Network เดียวกัน

### การตรวจจับไม่แม่นยำ
1. **ปรับแสง**: ใช้แสงเพียงพอและสม่ำเสมอ
2. **พื้นหลัง**: ใช้พื้นหลังเรียบๆ ไม่ซับซ้อน  
3. **ระยะห่าง**: วางมือห่างจากกล้อง 30-60 ซม.
4. **ปรับค่าใน Code**:
   ```python
   min_detection_confidence=0.5,  # ลดลงหากตรวจจับยาก
   min_tracking_confidence=0.5    # ลดลงเพื่อความเสถียร
   ```

### ประสิทธิภาพช้า
1. **ปรับ Frame Processing**:
   ```python
   # เพิ่มค่า skip_counter สำหรับ FPS ต่ำกว่า
   if skip_counter >= 5:  # แทน 3
   ```
2. **ลด Resolution**:
   ```python
   cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)   # แทน 640
   cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)  # แทน 480
   ```
3. **ปรับการตั้งค่า ESP32**:
   ```cpp
   config.frame_size = FRAMESIZE_QVGA;  // ใช้ขนาดเล็กกว่า
   config.jpeg_quality = 15;            // ลดคุณภาพเพื่อความเร็ว
   ```

### แฟลช LED ไม่ทำงาน
1. **ตรวจสอบ GPIO**: GPIO 4 เชื่อมต่อถูกต้อง
2. **ทดสอบ Manual**: `http://[ESP32_IP]/flash/on`
3. **ตรวจสอบ Hardware**: LED อาจเสียหายหรือต้องใช้แรงดันสูงกว่า

## 📊 Status Display

### ข้อมูลที่แสดงบนหน้าจอ:
- **Mode**: โหมดปัจจุบัน (สีต่างกัน)
- **Flash Status**: สถานะแฟลช (เฉพาะโหมด Symbol/Volume)
- **Finger Count**: จำนวนนิ้วแต่ละมือ (L=ซ้าย, R=ขวา)
- **Volume Level**: ระดับเสียง (เฉพาะโหมด Volume Control)
- **Hand Landmarks**: จุดและเส้นเชื่อมของมือ

## 💡 ESP32-CAM Code อธิบาย

### ฟีเจอร์หลัก
```cpp
// กำหนดพิน AI Thinker ESP32-CAM
#define LED_GPIO_NUM 4          // ขาแฟลช LED

// ฟังก์ชันหลัก
handleFlashOn()    // เปิดแฟลช
handleFlashOff()   // ปิดแฟลช  
handleStream()     // สตรีม MJPEG
```

### การตั้งค่ากล้อง
- **Frame Size**: FRAMESIZE_UXGA (หากมี PSRAM), FRAMESIZE_SVGA (หากไม่มี)
- **JPEG Quality**: 10-12 (ต่ำกว่า = คุณภาพดีกว่า)
- **Frame Buffer**: 2 buffers (หากมี PSRAM)
- **Grab Mode**: CAMERA_GRAB_LATEST

## 🚀 การพัฒนาต่อ

### เพิ่มท่าทางใหม่:
1. **เพิ่มฟังก์ชันตรวจจับ**:
   ```python
   def is_thumbs_up_fast(landmarks):
       # ใช้ตรรกะตรวจจับท่าทาง thumbs up
       pass
   ```

2. **เพิ่มการจัดการในโหมด**:
   ```python
   # ใน check_mode_transitions()
   if thumbs_up_detected:
       # ทำงานตามต้องการ
       pass
   ```

### เพิ่มการควบคุมอุปกรณ์อื่น:

#### เพิ่ม Servo Motor
```cpp
// ใน ESP32 code
#include <ESP32Servo.h>
Servo myservo;

void handleServoMove() {
  int angle = server.arg("angle").toInt();
  myservo.write(angle);
  server.send(200, "text/plain", "Servo moved to " + String(angle));
}
```

#### เพิ่ม RGB LED
```cpp
// ควบคุม RGB LED
#define RED_PIN 12
#define GREEN_PIN 13  
#define BLUE_PIN 14

void handleColorChange() {
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();
  
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  
  server.send(200, "text/plain", "Color changed");
}
```

### ปรับปรุง UI
```python
def draw_advanced_ui(frame):
    # เพิ่มแถบสี สำหรับโหมด
    cv2.rectangle(frame, (0, 0), (frame.shape[1], 30), mode_color, -1)
    
    # เพิ่ม progress bar สำหรับ transition
    if mode_transition_start_time:
        progress = (time.time() - mode_transition_start_time) / OPEN_HAND_DURATION
        bar_width = int(200 * min(progress, 1.0))
        cv2.rectangle(frame, (20, 50), (20 + bar_width, 70), (0, 255, 0), -1)
```

---

**หมายเหตุ**: โปรแกรมนี้ออกแบบมาเพื่อการเรียนรู้และทดลอง กรุณาใช้งานอย่างระมัดระวัง
