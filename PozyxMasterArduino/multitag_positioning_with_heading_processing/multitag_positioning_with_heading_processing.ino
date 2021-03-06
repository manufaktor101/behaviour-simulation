// Please read the ready-to-localize tuturial together with this example.
// https://www.pozyx.io/Documentation/Tutorials/ready_to_localize
/**
  The Pozyx ready to localize tutorial (c) Pozyx Labs

  Please read the tutorial that accompanies this sketch: https://www.pozyx.io/Documentation/Tutorials/ready_to_localize/Arduino

  This tutorial requires at least the contents of the Pozyx Ready to Localize kit. It demonstrates the positioning capabilities
  of the Pozyx device both locally and remotely. Follow the steps to correctly set up your environment in the link, change the
  parameters and upload this sketch. Watch the coordinates change as you move your device around!
*/
#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>
#include "StringSplitter.h"

////////////////////////////////////////////////
////////////////// PARAMETERS //////////////////
////////////////////////////////////////////////

const int num_tags = 5;
uint16_t tags[num_tags] = {0x6834, 0x6a4a, 0x6748, 0x6833, 0x687D};

boolean use_processing = true;                         // set this to true to output data for the processing sketch

const uint8_t num_anchors = 4;                                    // the number of anchors
uint16_t anchors[num_anchors] = {0x6f4a, 0x6f22, 0x6f77, 0x1134};     // the network id of the anchors: change these to the network ids of your anchors.
int32_t anchors_x[num_anchors] = {0, 5932, 4480, 788};               // anchor x-coorindates in mm
int32_t anchors_y[num_anchors] = {0, 0, 3927, 1935};                  // anchor y-coordinates in mm
int32_t heights[num_anchors] = {457, 456, 443, 2036};              // anchor z-coordinates in mm

uint8_t algorithm = POZYX_POS_ALG_UWB_ONLY;             // positioning algorithm to use
uint8_t dimension = POZYX_3D;                           // positioning dimension
int32_t height = 1000;   // height of device, required in 2.5D positioning

String inputString = "";            // a string to hold incoming data
boolean stringComplete = false;     // whether the string is complete

////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  if (Pozyx.begin() == POZYX_FAILURE) {
    Serial.println(F("ERROR: Unable to connect to POZYX shield"));
    Serial.println(F("Reset required"));
    delay(100);
    abort();
  }

  Serial.println(F("----------POZYX POSITIONING V1.1----------"));
  Serial.println(F("NOTES:"));
  Serial.println(F("- No parameters required."));
  Serial.println();
  Serial.println(F("- System will auto start anchor configuration"));
  Serial.println();
  Serial.println(F("- System will auto start positioning"));
  Serial.println(F("----------POZYX POSITIONING V1.1----------"));
  Serial.println();
  Serial.println(F("Performing manual anchor configuration:"));

  // configures all remote tags and prints the success of their configuration.
  setAnchorsManual();
  setTagsAlgorithm();
  delay(1000);

  Serial.println(F("Starting positioning: "));
}

void loop() {

    // check if we received a newline character and if so, broadcast the inputString.
  if(stringComplete){

    parseAndSendCommand(inputString);

    inputString = "";
    stringComplete = false;
  }

  for (int i = 0; i < num_tags; i++) {
    coordinates_t position;
    euler_angles_t euler_angles;

    // long millis_before = millis();

    int statusPos = Pozyx.doRemotePositioning(tags[i], &position, dimension, height, algorithm);
    int statusHeading = Pozyx.getEulerAngles_deg(&euler_angles, tags[i]);

    // long millisForRetrieval = millis() - millis_before;

    if (statusHeading == POZYX_SUCCESS && statusPos == POZYX_SUCCESS) {

      float32_t heading = euler_angles.heading;

      // prints out the result
      printCoordinates(position, heading, tags[i]);

    } else {
      // prints out the error code
      printErrorCode((statusPos != POZYX_SUCCESS) ? "positioning" : "heading", tags[i]);
    }


  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 For more info check http://www.arduino.cc/en/Tutorial/SerialEvent
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it.
    // otherwise, add it to the inputString:
    if (inChar == '\n') {
      stringComplete = true;
    }else{
      inputString += inChar;
    }
  }
}

uint8_t computeDegAngle(uint8_t heading_raw) {
  return heading_raw / POZYX_EULER_DIV_DEG;
}


enum RobotCommand
{
  LEFT = 0,
  RIGHT = 1,
  FORWARD = 2,
  BACK = 3,
  STOP = 4,
};

uint16_t charArrToInt(char str[]) {
  return (uint16_t) strtol(str, 0, 16);
}

uint16_t hexStrToInt(String s) {
  String tmp = "";
  char xhex[6];
  tmp = s;  xhex[tmp.length() + 2]; tmp.toCharArray(xhex, tmp.length() + 2);
  return (uint16_t) charArrToInt(xhex);
}

void parseAndSendCommand(String commandStr) {

  if (commandStr.startsWith("CMD"))
  {
    Serial.println("Complete received string data: " + inputString);

    StringSplitter *splitter = new StringSplitter(inputString, ',', 7);

    int broadcastFlag = splitter->getItemAtIndex(6).toInt();

    // get device id
    uint16_t remoteId = hexStrToInt(splitter->getItemAtIndex(1));
    if (broadcastFlag == 1)
      remoteId = 0;
    
    Serial.println("Remote device id set to: ");
    Serial.print(remoteId, HEX);

    // convert String to byte array
    int length = commandStr.length();
    uint8_t buffer[length];
    inputString.getBytes(buffer, length);
    
    // write the message to the transmit (TX) buffer & send
    Pozyx.writeTXBufferData(buffer, length);
    Pozyx.sendTXBufferData(remoteId);
  }
}


// prints the coordinates for either humans or for processing
void printCoordinates(coordinates_t coor, float32_t heading, uint16_t network_id) {
  if (!use_processing) {
    Serial.print("POS ID 0x");
    Serial.print(network_id, HEX);
    Serial.print(", x(mm): ");
    Serial.print(coor.x);
    Serial.print(", y(mm): ");
    Serial.print(coor.y);
    Serial.print(", z(mm): ");
    Serial.print(coor.z);
    Serial.print(", heading(deg): ");
    Serial.println(heading);

  } else {
    Serial.print("POS,0x");
    Serial.print(network_id, HEX);
    Serial.print(",");
    Serial.print(coor.x);
    Serial.print(",");
    Serial.print(coor.y);
    Serial.print(",");
    Serial.print(coor.z);
    Serial.print(",");
    Serial.println(heading);
  }
}

// error printing function for debugging
void printErrorCode(String operation, uint16_t network_id) {
  uint8_t error_code;
  int status = Pozyx.getErrorCode(&error_code, network_id);
  if (status == POZYX_SUCCESS) {
    Serial.print("ERROR ");
    Serial.print(operation);
    Serial.print(" on ID 0x");
    Serial.print(network_id, HEX);
    Serial.print(", error code: 0x");
    Serial.println(error_code, HEX);
  } else {
    Pozyx.getErrorCode(&error_code);
    Serial.print("ERROR ");
    Serial.print(operation);
    Serial.print(", couldn't retrieve remote error code, local error: 0x");
    Serial.println(error_code, HEX);
  }
}

void setTagsAlgorithm() {
  for (int i = 0; i < num_tags; i++) {
    Pozyx.setPositionAlgorithm(algorithm, dimension, tags[i]);
  }
}

// function to manually set the anchor coordinates
void setAnchorsManual() {
  for (int i = 0; i < num_tags; i++) {
    int status = Pozyx.clearDevices(tags[i]);
    for (int j = 0; j < num_anchors; j++) {
      device_coordinates_t anchor;
      anchor.network_id = anchors[j];
      anchor.flag = 0x1;
      anchor.pos.x = anchors_x[j];
      anchor.pos.y = anchors_y[j];
      anchor.pos.z = heights[j];
      status &= Pozyx.addDevice(anchor, tags[i]);
    }
    if (num_anchors > 4) {
      Pozyx.setSelectionOfAnchors(POZYX_ANCHOR_SEL_AUTO, num_anchors, tags[i]);
    }
    if (status == POZYX_SUCCESS) {
      Serial.print("Configuring ID 0x");
      Serial.print(tags[i], HEX);
      Serial.println(" success!");
    } else {
      printErrorCode("configuration", tags[i]);
    }
  }
}
