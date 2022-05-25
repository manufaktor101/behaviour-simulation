//#define USE_PIXEL



#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>
#include <Arduino.h>


#ifdef USE_PIXEL
#include <Adafruit_NeoPixel.h>
#endif

#define PIN 6
#define LED_COUNT 40
#define DELAYVAL 500

uint16_t source_id;
uint16_t destination_id = 0;
boolean stringComplete = false;

const uint8_t num_anchors = 4;                                       // the number of anchors
uint16_t anchors[num_anchors] = { 0x6f4a, 0x6f22, 0x6f77, 0x1134 };  // the network id of the anchors: change these to the network ids of your anchors.
int32_t anchors_x[num_anchors] = { 0, 5922, 4754, 835 };             // anchor x-coorindates in mm
int32_t anchors_y[num_anchors] = { 0, 0, 3917, 2160 };               // anchor y-coordinates in mm
int32_t heights[num_anchors] = { 434, 456, 443, 2031 };              // anchor z-coordinates in mm

uint8_t algorithm = POZYX_POS_ALG_UWB_ONLY;  // positioning algorithm to use
uint8_t dimension = POZYX_3D;                // positioning dimension

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

  // initialize the pozyx device
  if (!Pozyx.begin(false, MODE_POLLING, POZYX_INT_MASK_RX_DATA, 0)) {
    Serial.println("ERROR: Unable to connect to POZYX shield");
    Serial.println("Reset required");
    abort();
  }

  // add anchors for calibration
  setAnchorsLocal();
  setTagsAlgorithmLocal();

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

// function to manually set the anchor coordinates
void setAnchorsLocal() {
  int status = Pozyx.clearDevices();
  for (int j = 0; j < num_anchors; j++) {
    device_coordinates_t anchor;
    anchor.network_id = anchors[j];
    anchor.flag = 0x1;
    anchor.pos.x = anchors_x[j];
    anchor.pos.y = anchors_y[j];
    anchor.pos.z = heights[j];
    status &= Pozyx.addDevice(anchor);
  }
  if (num_anchors > 4) {
    Pozyx.setSelectionOfAnchors(POZYX_ANCHOR_SEL_AUTO, num_anchors);
  }
  if (status == POZYX_SUCCESS) {
    Serial.println("Setting anchors locally successful!");
  } else {
    Serial.println("Error adding anchors locally");
  }
}

void setTagsAlgorithmLocal() {
  Pozyx.setPositionAlgorithm(algorithm, dimension);
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
