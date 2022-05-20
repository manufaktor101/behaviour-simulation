 //<>// //<>// //<>//

import oscP5.*;
import netP5.*;
import processing.serial.*;
import java.lang.Math.*;

/////////////////////////////////////////////////////////////
//////////////////////  parameters //////////////////////////
/////////////////////////////////////////////////////////////

boolean serial = false;          // set to true to use Serial, false to use OSC messages.

String serialPort = "/dev/cu.usbmodem141101";      // change this to your COM port

// start parameters
static final int inboundPort = 6970;
static final int outboundPort = 6969;
static final String outboundAddress = "127.0.0.1";
// end parameters

OscP5 oscP5 = new OscP5(this, inboundPort);
Serial  myPort;

int     lf = 10;       //ASCII linefeed
NetAddress address = new NetAddress(outboundAddress, outboundPort);

void setup() {
  size(1000, 700, P3D);
  surface.setResizable(true);
  stroke(0, 0, 0);
  colorMode(RGB, 256);
  // sets up the input

  try {
    myPort = new Serial(this, serialPort, 115200);
    myPort.clear();
    myPort.bufferUntil(lf);
  }
  catch(Exception e) {
    println("Cannot open serial port.");
  }
}

void draw() {
  background(126, 161, 172);
  fill(0, 0, 0);
  text("(c) Pozyx Labs", width-100, 20);
}


// Pozyx Master -> OSC
// -------------------------------------

void serialEvent(Serial p) {
  // expected string: POS,network_id,posx,posy,posz
  String inString = (myPort.readString());
  //print(inString);
  try {

    // parse the data
    String[] dataStrings = split(inString, ',');

    if (dataStrings[0].equals("POS")) {
      String id = dataStrings[1];
      int x = int(dataStrings[2]);
      int y = int(dataStrings[3]);
      int z = int(dataStrings[4]);
      float heading = float(dataStrings[5]);

      SendOsc(id, x, y, z, heading);
    }
    
    else
    {
      println("Received from arduino serial unknown command: " + inString);
    }
    
  }
  catch (Exception e) {
    println("Error while reading serial data.");
  }
}

void SendOsc(String id, int x, int y, int z, float heading)
{
  try {
    OscMessage myMessage = new OscMessage("/" + id);

    myMessage.add(x);
    myMessage.add(y);
    myMessage.add(z);
    myMessage.add(heading);

    oscP5.send(myMessage, address);

    //println("sending osc for tag " + id);
  }
  catch(Exception e)
  {
    println("error sending OSC message. not sending anything...");
    println(e.toString());
  }
}


// OSC -> Pozyx Master
// -------------------------------------

//public enum RobotCommand
//{
//  LEFT (0),
//  RIGHT (1),
//  FORWARD (2),
//  BACK (3),
//  STOP (4),
//};

void oscEvent(OscMessage theOscMessage) {
  // osc: /<deviceId> <command as int>
  // to
  // serial: CMD,<device id as two bytes>,<command as one byte>
  
  String deviceId = theOscMessage.addrPattern().substring(1); // get /<deviceId> and strip off leading slash //<>//
  int command = theOscMessage.get(0).intValue();
  
  String arduinoCommandString = "CMD,"+deviceId+","+command;
  
  println("Sending command to serial: " + arduinoCommandString);
  
  // send to Arduino
  myPort.write(arduinoCommandString);
  myPort.write(lf);
}
