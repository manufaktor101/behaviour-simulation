#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#define PIN 6
#define LED_COUNT 40
#define DELAYVAL 500 

uint16_t source_id;
uint16_t destination_id = 0;
String inputString = "";
boolean stringComplete = false;

Adafruit_NeoPixel pixels(LED_COUNT, PIN, NEO_GRBW + NEO_KHZ800);


String center = "$0,0,0,0,0,0,0,0,0,0,0,0#";
String up = "$1,0,0,0,0,0,0,0,0,0,0,0#";
String down = "$2,0,0,0,0,0,0,0,0,0,0,0#";
String left = "$3,0,0,0,0,0,0,0,0,0,0,0#";
String right = "$4,0,0,0,0,0,0,0,0,0,0,0#";

//String moveCom[5] = {"$0,0,0,0,0,0,0,0,0,0,0,0#", 
//"$1,0,0,0,0,0,0,0,0,0,0,0#", "$2,0,0,0,0,0,0,0,0,0,0,0#",
//"$3,0,0,0,0,0,0,0,0,0,0,0#", "$4,0,0,0,0,0,0,0,0,0,0,0#"};

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
//  Serial.begin(9600);
  pixels.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
  //randomSeed(analogRead(0));
  
  pixels.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
 

  //initialize the pozyx device
    if(! Pozyx.begin(false, MODE_INTERRUPT, POZYX_INT_MASK_RX_DATA, 0)){
    Serial.println("ERROR: Unable to connect to POZYX shield");
    Serial.println("Reset required");
    abort();
  }

    // read the network id of this device
  Pozyx.regRead(POZYX_NETWORK_ID, (uint8_t*)&source_id, 2);

  // reserve 100 bytes for the inputString:
  inputString.reserve(100);

}

void loop() {
  
  
  // we wait up to 50ms to see if we have received an incoming message (if so we receive an RX_DATA interrupt)
  if (Pozyx.waitForFlag(POZYX_INT_STATUS_RX_DATA,50))
  {
Serial.println("revd msg");
    
    // we have received a message!
    uint8_t length = 0;
    uint16_t messenger = 0x00;
    delay(1);
    // Let's read out some information about the message (i.e., how many bytes did we receive and who sent the message)
    Pozyx.getLastDataLength(&length);
    Pozyx.getLastNetworkId(&messenger);

    char data[length];

    // read the contents of the receive (RX) buffer into a character array called data with the 
    //same length as the contents of the buffer, this is the message that was sent to this device
    Pozyx.readRXBufferData((uint8_t *) data, length);

    //convert char array data into a string 

//    Serial.println(data);

    inputString = String(data);

//    right

    //compare the string literal with one of the string variables
    
    
    //refactor for loop with foreach loop if the resulting effect is desirable
    
    if (inputString == "up")
    {
      
        int len = up.length();

      for (int i=0; i<len; i++)
      {
        Serial.print(up.charAt(i));
      }
//    Serial.println("Goodbye Madonna");
    
    }
    else if 
     (inputString == "down")
    {
      
        int len = down.length();

      for (int i=0; i<len; i++)
      {
        Serial.print(down.charAt(i));
      }
//      Serial.println("Goodbye Anna");
    
    }
     else if(inputString == "center")
    {
      
        int len = center.length();

      for (int i=0; i<len; i++)
      {
        Serial.print(center.charAt(i));
      }
//      Serial.println("Goodbye lies");
    
    }
    else if(inputString == "left")
    {
      
        int len = left.length();

      for (int i=0; i<len; i++)
      {
        Serial.print(left.charAt(i));
      }

//      Serial.println("Goodbye problems");
    
    }
    else if(inputString == "right")
    {
      
        int len = right.length();

      for (int i=0; i<len; i++)
      {
        Serial.print(right.charAt(i));
      }
//      Serial.println("Hello single life");
    
    }
//    else
//    {
//      Serial.println(inputString);
//    }

   inputString="";



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


}
