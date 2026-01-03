#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// Consumer Control usage for Play/Pause
static constexpr uint16_t CONSUMER_PLAY_PAUSE = 0x00CD;
static constexpr uint16_t CONSUMER_SCAN_PREV_TRACK = 0x00B6;
static constexpr uint16_t CONSUMER_SCAN_NEXT_TRACK = 0x00B5;

// TinyUSB HID report descriptor for Consumer Control.
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(2)),
};

// Create the HID device
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report),
                          HID_ITF_PROTOCOL_NONE, 2, false);

static void normalPress(uint8_t keycode, uint8_t modifier = 0)
{
  uint8_t report[8] = {0};
  report[0] = modifier;
  report[2] = keycode;
  // Send "press"
  usb_hid.sendReport(1, report, sizeof(report));
  delay(20);

  // Release - Normal keybaord mode needs the arrary reseting to all zeros
  memset(report, 0, sizeof(report));
  usb_hid.sendReport(1, report, sizeof(report));
  delay(20);
}

static void consumerPress(uint16_t usage)
{
  usb_hid.sendReport(2, &usage, sizeof(usage));
  delay(20);

  // Release
  usage = 0;
  usb_hid.sendReport(2, &usage, sizeof(usage));
  delay(20);
}

static void ledLight (bool on) {
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  usb_hid.begin();

  // Waits for the USB host to mount the device
  while (!TinyUSBDevice.mounted()) {
    delay(1);
  }

  // Setup the pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(16, INPUT_PULLUP); // Play/Pause Green
  pinMode(17, INPUT_PULLUP); // Previous Track Blue
  pinMode(18, INPUT_PULLUP); // Next Track Red
  pinMode(19, INPUT_PULLUP); // Random extra button for 'a' key
  delay(500); // wait for USB to settle
} 

void loop() {
  // Check if button on pin 16 is pressed for Play/Pause
  if (digitalRead(16) == LOW) {
    Serial.println("Play/Pause");
    ledLight(true);
    consumerPress(CONSUMER_PLAY_PAUSE);
    delay(300); // debounce
    ledLight(false);
  }

  // Check if button on pin 17 is pressed for Previous Track
  if (digitalRead(17) == LOW) {
    Serial.println("Previous Track");
    ledLight(true);
    consumerPress(CONSUMER_SCAN_PREV_TRACK);
    delay(300); // debounce
    ledLight(false);
  }

  // Check if button on pin 18 is pressed for Next Track
  if (digitalRead(18) == LOW) {
    Serial.println("Next Track");
    ledLight(true);
    consumerPress(CONSUMER_SCAN_NEXT_TRACK);
    delay(300); // debounce
    ledLight(false);
  }

  // Check if button on pin 19 is pressed for 'a' key
  if (digitalRead (19) == LOW) {
    Serial.println("LED On");
    ledLight(true);
    normalPress(HID_KEY_A); // 'a' key
    delay(300); // debounce
    ledLight(false);
  }
}
