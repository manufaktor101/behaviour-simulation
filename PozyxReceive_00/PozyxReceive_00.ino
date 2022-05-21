//#define USE_PIXEL


#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>


#ifdef USE_PIXEL
#include <Adafruit_NeoPixel.h>
#endif

#define PIN 6
#define LED_COUNT 40
#define DELAYVAL 500

uint16_t source_id;
uint16_t destination_id = 0;
boolean stringComplete = false;

#ifdef USE_PIXEL
Adafruit_NeoPixel pixels(LED_COUNT, PIN, NEO_GRBW + NEO_KHZ800);
#endif

String center = "$0,0,0,0,0,0,0,0,0,0,0,0#";
String up = "$1,0,0,0,0,0,0,0,0,0,0,0#";
String down = "$2,0,0,0,0,0,0,0,0,0,0,0#";
String left = "$3,0,0,0,0,0,0,0,0,0,0,0#";
String right = "$4,0,0,0,0,0,0,0,0,0,0,0#";

enum RobotCommand {
  LEFT = 0,
  RIGHT = 1,
  FORWARD = 2,
  BACK = 3,
  STOP = 4,
};

void setup() {
  Serial.begin(115200);

#ifdef USE_PIXEL
  pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();             // Turn OFF all pixels ASAP
  pixels.setBrightness(10);  // Set BRIGHTNESS to about 1/5 (max = 255)
#endif

  //initialize the pozyx device
  if (!Pozyx.begin(false, MODE_POLLING, POZYX_INT_MASK_RX_DATA, 0)) {
    Serial.println("ERROR: Unable to connect to POZYX shield");
    Serial.println("Reset required");
    abort();
  }

  // read the network id of this device
  Pozyx.regRead(POZYX_NETWORK_ID, (uint8_t *)&source_id, 2);
}

void loop() {
  // we wait up to 50ms to see if we have received an incoming message (if so we receive an RX_DATA interrupt)
  if (Pozyx.waitForFlag_safe(POZYX_INT_STATUS_RX_DATA, 50)) {
    // we have received a message!

    uint8_t length = 0;
    uint16_t messenger = 0x00;
    // delay(1);
    // Let's read out some information about the message (i.e., how many bytes did we receive and who sent the message)
    Pozyx.getLastDataLength(&length);
    Pozyx.getLastNetworkId(&messenger);

    char data[length];

    // read the contents of the receive (RX) buffer into a character array called data with the
    //same length as the contents of the buffer, this is the message that was sent to this device
    Pozyx.readRXBufferData((uint8_t *)data, length);

    RobotCommand command = (RobotCommand)data[0];

    switch (command) {
      case FORWARD:
        MoveRobot(up);
        break;
      case BACK:
        MoveRobot(down);
        break;
      case LEFT:
        MoveRobot(left);
        break;
      case RIGHT:
        MoveRobot(right);
        break;
      case STOP:
        MoveRobot(center);
        break;
      default:
        Serial.println("unknown RobotCommand: " + command);
    }
  }
}

void MoveRobot(String command) {
  Serial.print(command);
}

//       pixels.setPixelColor(2, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(3, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(4, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(5, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware
//
//    pixels.setPixelColor(9, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(14, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(23, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(16, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(25, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(30, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(34, pixels.Color(250, 0, 0, 0));
// //   pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(35, pixels.Color(250, 0, 0, 0));
//  //  pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(36, pixels.Color(250, 0, 0, 0));
////    pixels.show();   // Send the updated pixel colors to the hardware.
//
//    pixels.setPixelColor(37, pixels.Color(250, 0, 0, 0));
//    //pixels.show();   // Send the updated pixel colors to the hardware.
//

//pupille
//    pixels.setPixelColor(20, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);  //je höher, je langsamer
//    pixels.setPixelColor(20, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(19, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(19, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(18, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(18, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(17, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(17, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(18, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(18, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(19, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(19, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(20, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(20, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(21, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(21, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(22, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(22, pixels.Color(0, 0, 0, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    pixels.setPixelColor(21, pixels.Color(250, 250, 250, 0));
//    pixels.show();   // Send the updated pixel colors to the hardware.
//    delay(100);
//    pixels.setPixelColor(21, pixels.Color(0, 0, 0, 0));
//    pixels.show();
//
