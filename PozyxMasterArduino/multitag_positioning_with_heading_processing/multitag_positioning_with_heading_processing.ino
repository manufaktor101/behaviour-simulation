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

////////////////////////////////////////////////
////////////////// PARAMETERS //////////////////
////////////////////////////////////////////////

const int num_tags = 1;
uint16_t tags[num_tags] = {0x6834, 0x6a4a};

boolean use_processing = true;                         // set this to true to output data for the processing sketch

const uint8_t num_anchors = 4;                                    // the number of anchors
uint16_t anchors[num_anchors] = {0x6f4a, 0x6f22, 0x6f77, 0x1134};     // the network id of the anchors: change these to the network ids of your anchors.
int32_t anchors_x[num_anchors] = {0, 5922, 4754, 835};               // anchor x-coorindates in mm
int32_t anchors_y[num_anchors] = {0, 0, 3917, 2160};                  // anchor y-coordinates in mm
int32_t heights[num_anchors] = {434, 456, 443, 2031};              // anchor z-coordinates in mm

uint8_t algorithm = POZYX_POS_ALG_UWB_ONLY;             // positioning algorithm to use
uint8_t dimension = POZYX_3D;                           // positioning dimension
int32_t height = 1000;   // height of device, required in 2.5D positioning


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

  if (Serial.available() > 0)
  {
    parseAndSendCommand(Serial.readString());
  }

  for (int i = 0; i < num_tags; i++) {
    coordinates_t position;
    euler_angles_t euler_angles;

    int statusPos = Pozyx.doRemotePositioning(tags[i], &position, dimension, height, algorithm);
    int statusHeading = Pozyx.getEulerAngles_deg(&euler_angles, tags[i]);

    if (statusHeading == POZYX_SUCCESS && statusPos == POZYX_SUCCESS) {

      float32_t heading = euler_angles.heading;//y is "euler_angles.roll" and x is "euler_angles.pitch"

      // prints out the result
      printCoordinates(position, heading, tags[i]);

    } else {
      // prints out the error code
      printErrorCode((statusPos != POZYX_SUCCESS) ? "positioning" : "heading", tags[i]);
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
    // get device id

    String deviceStr = commandStr.substring(4, 10);
    uint16_t deviceId = hexStrToInt(deviceStr);

    Serial.print("Received command for device: ");
    Serial.println(deviceId, HEX);

    // get command
    byte command = commandStr[11] - '0';

    Serial.println("command in byte is");
    Serial.println(command, DEC);

    String arduinoCommand;

    // map command
    switch (command)
    {
      case 0:
        // map
        arduinoCommand = "left ";
        break;
      case 1:
        // map
        arduinoCommand = "right ";

        break;

      case 2:
        // map
        arduinoCommand = "up ";
        break;

      case 3:
        // map
        arduinoCommand = "down ";
        break;

      case 4:
        // map
        arduinoCommand = "center ";
        break;
      default:
        Serial.println("Unknown command received: " + command);
        return;

    }

    Serial.print("sending command: ");
    Serial.println(arduinoCommand);

    // send command
    int length = arduinoCommand.length();
    uint8_t buffer[length];

    arduinoCommand.getBytes(buffer, length);

    Pozyx.writeTXBufferData(buffer, length);
    Pozyx.sendTXBufferData(deviceId);


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
