#define USE_PIXEL

#include <SoftwareSerial.h>

#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>
#include <Arduino.h>
#include "StringSplitter.h"

#ifdef USE_PIXEL
#include <Adafruit_NeoPixel.h>
#endif

#define PIN 6
#define LED_COUNT 40
#define DELAYVAL 500

//
#define trigPin 2
#define echoPin 3

//software serial
#define rxPin 10
#define txPin 11

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

SoftwareSerial roboSerial =  SoftwareSerial(rxPin, txPin);

//led matrix
struct Point {
  float x;
  float y;
};
float phase = 0.0;
float phaseIncrement = 0.01;  // Controls the speed of the moving points. Higher == faster. I like 0.08 ..03 change to .02
float colorStretch = 0.11;    // Higher numbers will produce tighter color bands. I like 0.11 . ok try .11 instead of .03

enum RobotCommand {

  STOP = 0,
  FORWARD = 1,
  BACK = 2,
  LEFT = 3,
  RIGHT = 4,
};


// Define variables:
unsigned long duration;
int distance;
int lastDistance;
unsigned long timeStamp = 0;
int trigMode = 3;
int threshold = 60;

// autonomous behavior
int territory = 200;
boolean turnFlag = true;
boolean runFlag = false;
boolean turn = false;
String mode = "NULL";
unsigned long idleTimeStamp = 0;
int idleTime = 0;
int pref = 0;

boolean gather = true; // If it's false, find empty spot to go.
// if it's true, find target to get closer

boolean backFlag = true;
long backInterval = 60000;

unsigned long externalCommandTimeStamp = 0;
int commandTime = 2000;

bool autonomousMode = true; // defines whether robot has autonomous behaviour. Can be controlled by master tag

void setup() {
  Serial.begin(115200);
  roboSerial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(4, OUTPUT);//led calibration MAG
  digitalWrite(4, LOW);

#ifdef USE_PIXEL
  pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();             // Turn OFF all pixels ASAP
  pixels.setBrightness(10);  // Set BRIGHTNESS to about 1/5 (max = 255)
#endif

  // initialize the pozyx device
  if (!Pozyx.begin(false, MODE_POLLING, POZYX_INT_MASK_RX_DATA, 0)) {
    roboSerial.println("ERROR: Unable to connect to POZYX shield");
    roboSerial.println("Reset required");
    abort();
  }

  // add anchors for calibration
  setAnchorsLocal();
  setTagsAlgorithmLocal();

  // read the network id of this device
  Pozyx.regRead(POZYX_NETWORK_ID, (uint8_t *)&source_id, 2);
  randomSeed(analogRead(0));
}

void MoveRobot(RobotCommand command,int motorSpeed = 127) { // if MoveRobot() is only invoked with one param, motorSpeed defaults to max speed
  Serial.print("$,");
  Serial.print(command);
  Serial.print(",");
  Serial.print(motorSpeed);
  Serial.print("#");
}

void loop() {

  if(autonomousMode)
    UltraSonic();
  
  ShowMagCalibrationStatusOnLeds();

  // we wait up to 50ms to see if we have received an incoming message (if so we receive an RX_DATA interrupt)
  if (Pozyx.waitForFlag_safe(POZYX_INT_STATUS_RX_DATA, 50)) {
    // we have received a message!

    uint8_t length = 0;
    // delay(1);
    // Let's read out some information about the message (i.e., how many bytes did we receive and who sent the message)
    Pozyx.getLastDataLength(&length);

    char data[length];
    // read the contents of the receive (RX) buffer into a character array called data with the
    //same length as the contents of the buffer, this is the message that was sent to this device
    Pozyx.readRXBufferData((uint8_t *)data, length);

    String commandStr(data); //copy constructor 
    Serial.println("Received command from master tag: " + commandStr);
    
    StringSplitter *splitter = new StringSplitter((commandStr), ',', 7);
    RobotCommand robotCommand = (RobotCommand)splitter->getItemAtIndex(2).toInt();
    int motorSpeed = splitter->getItemAtIndex(3).toInt();
    
    // set global autonomous mode  flag acc. to data from master
    autonomousMode = splitter->getItemAtIndex(5).toInt();

    MoveRobot(robotCommand,motorSpeed);
  }
}

void ShowMagCalibrationStatusOnLeds()
{
  //calibration MAG and led
  uint8_t calibration_status = 0;
  if (Pozyx.waitForFlag(POZYX_INT_STATUS_IMU, 10) == POZYX_SUCCESS) {
    Pozyx.getCalibrationStatus(&calibration_status);
  } else {
    uint8_t interrupt_status = 0;
    Pozyx.getInterruptStatus(&interrupt_status);
    return;
  }

  if (((calibration_status & 0xC0) >> 6) == 3) {
    digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else {
    digitalWrite(4, LOW);   // turn the LED on (HIGH is the voltage level)
  }
}

void UltraSonic(){
  //ULTRASONIC SENSOR

  if (trigMode == 3) {
    digitalWrite(trigPin, LOW);
    trigMode = 0;
    timeStamp = micros();
  }

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  if (micros() - timeStamp > 5 && trigMode == 0) {
    digitalWrite(trigPin, HIGH);
    trigMode = 1;
  }

  if (micros() - timeStamp > 15 && trigMode == 1) {
    digitalWrite(trigPin, LOW);
    trigMode = 2;
  }

  if (trigMode == 2) {
    // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
    duration = pulseIn(echoPin, HIGH);
    // Calculate the distance:
    distance = duration * 0.034 / 2;

    // Print the distance on the Serial Monitor (Ctrl+Shift+M):
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm ");
    Serial.print(" Mode: ");
    Serial.println(mode);

    trigMode = 3;
  }



  if (millis() - externalCommandTimeStamp > commandTime) {
    //  avoiding obstacle
    //  if (distance < threshold && lastDistance > threshold) { // avoid obstacle (first priority)
    if (distance < threshold && !turn) { // avoid obstacle (first priority)
      MoveRobot(STOP);
      turn = true;
    }
    if (distance < threshold && turn) {
      float r = random(100);
      if (r > 50) MoveRobot(RIGHT);
      else MoveRobot(LEFT);
      turnFlag = true;
      mode = "avoidObstacle";
    }
    else if (gather) {
      if (distance > territory && turnFlag) {
        float r = random(100);
        if (r > 50) MoveRobot(RIGHT);
        else MoveRobot(LEFT);
        turnFlag = false;
        runFlag = true;
        turn = false;
        mode = "lookforTarget";
      } else if (distance < territory && runFlag ) {
        //this is the time it goes "idle" mode
        idleTimeStamp = millis();
        idleTime = random(1000, 3000);
        pref = random(10);
        MoveRobot(STOP);
        mode = "idle";
        turnFlag = true;
        runFlag = false;
        turn = false;
        randomSeed(analogRead(0));
      }
      //
      if (mode == "idle" && millis() - idleTimeStamp > idleTime) {
        if (pref > 4) {
          MoveRobot(FORWARD);
          turnFlag = true;
          runFlag = false;
          turn = false;
          mode = "followingTarget";
        }
        else {
          float r = random(100);
          if (r > 50) MoveRobot(RIGHT);
          else MoveRobot(LEFT);
          turnFlag = false;
          runFlag = true;
          turn = false;
          mode = "lookforTarget";
        }
      }
    }
    else if (!gather) {
      if (distance < territory && turnFlag) {
        float r = random(100);
        if (r > 50) MoveRobot(RIGHT);
        else MoveRobot(LEFT);
        turnFlag = false;
        runFlag = true;
        turn = false;
        mode = "lookforTarget";
      } else if (distance > territory && runFlag ) {
        //this is the time it goes "idle" mode
        idleTimeStamp = millis();
        idleTime = random(1000, 3000);
        pref = random(10);
        MoveRobot(STOP);
        mode = "idle";
        turnFlag = true;
        runFlag = false;
        turn = false;
        randomSeed(analogRead(0));
      }
      //
      if (mode == "idle" && millis() - idleTimeStamp > idleTime) {
        if (pref > 2) {
          MoveRobot(FORWARD);
          turnFlag = true;
          runFlag = false;
          turn = false;
          mode = "followingTarget";
        }
        else {
          float r = random(100);
          if (r > 50) MoveRobot(RIGHT);
          else MoveRobot(LEFT);
          turnFlag = false;
          runFlag = true;
          turn = false;
          mode = "lookforTarget";
        }
      }
    }
    lastDistance = distance;
  }

  // every 30 sec
  if (millis() % backInterval < 500 && backFlag) {
    MoveRobot(BACK);
    Serial.println("back");
    backFlag = false;
  }

  if (millis() % backInterval > (backInterval - 1000)) {
    backFlag = true;
  }
}

void UpdateLeds()
{
  ///led

  phase += phaseIncrement;

  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  Point p1 = { (sin(phase * 1.000) + 1.0) * 4.5, (sin(phase * 1.310) + 1.0) * 4.0 };
  Point p2 = { (sin(phase * 1.770) + 1.0) * 4.5, (sin(phase * 2.865) + 1.0) * 4.0 };
  Point p3 = { (sin(phase * 0.250) + 1.0) * 4.5, (sin(phase * 0.750) + 1.0) * 4.0 };

  byte row, col;

  // For each row...
  for ( row = 0; row < 5; row++ ) {
    float row_f = float(row);  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

    // For each column...
    for ( col = 0; col < 8; col++ ) {
      float col_f = float(col);  // Optimization.

      // Calculate the distance between this LED, and p1.
      Point dist1 = { col_f - p1.x, row_f - p1.y };  // The vector from p1 to this LED.
      float distance1 = sqrt( dist1.x * dist1.x + dist1.y * dist1.y );

      // Calculate the distance between this LED, and p2.
      Point dist2 = { col_f - p2.x, row_f - p2.y };  // The vector from p2 to this LED.
      float distance2 = sqrt( dist2.x * dist2.x + dist2.y * dist2.y );

      // Calculate the distance between this LED, and p3.
      Point dist3 = { col_f - p3.x, row_f - p3.y };  // The vector from p3 to this LED.
      float distance3 = sqrt( dist3.x * dist3.x + dist3.y * dist3.y );

      // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
      // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
      float color_1 = distance1;  // range: 0.0...1.0
      float color_2 = distance2;
      float color_3 = distance3;
      float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;

      // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
      color_1 *= color_1 * color_4;
      color_2 *= color_2 * color_4;
      color_3 *= color_3 * color_4;
      color_4 *= color_4;

      // Scale the color up to 0..7 . Max brightness is 7.
      //strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
      pixels.setPixelColor(col + (8 * row), pixels.Color(0, color_2, color_3));
    }
  }
  pixels.show();
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
    roboSerial.println("Setting anchors locally successful!");
  } else {
    roboSerial.println("Error adding anchors locally");
  }
}

void setTagsAlgorithmLocal() {
  Pozyx.setPositionAlgorithm(algorithm, dimension);
}
