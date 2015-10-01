/*Bart Alert Code
By Robert Foster

For use with an arduino uno+ethernet shield, and adafruits neo pixels
Many thanks to all the people at bildr.org who created the parsing logic
*/
#include <Ethernet.h>
#include <SPI.h>
//Below Libraries for NeoPixels
#include <Adafruit_NeoPixel.h>
#define PIN 6

////////////////////////////////////////////////////////////////////////
//CONFIGURE
////////////////////////////////////////////////////////////////////////
char server[] = "www.fosterport.co";  //ip Address of the server you will connect to
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

//The location to go to on the server
//make sure to keep HTTP/1.0 at the end, this is telling it what type of file it is
String location = "/arduinoTesting/ardiWeather.php HTTP/1.0";

 
// if need to change the MAC address (Very Rare)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
////////////////////////////////////////////////////////////////////////

EthernetClient client;
//EthernetClient client2;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?

//ThinkSpeak Variables
//char thingSpeakAddress[] = "api.thingspeak.com";
//String writeAPIKey = "3R7O6L569OUUMCUG";
//String tsData;
long lastConnectionTime=0;
void setup(){
   strip.begin();
  strip.show(); 
  Ethernet.begin(mac);
  Serial.begin(9600);
}

void loop(){
 
  String pageValue = connectAndRead(); //connect to the server and read the output

  Serial.println(pageValue); //print out the findings.

 
  
 
  delay(10000); //wait 10 seconds before connecting again
}

String connectAndRead(){
  //connect to the server

  Serial.println("connecting...");

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.print("GET ");
    client.println(location);
    client.println("Host: www.fosterport.co");
    client.println();

    //Connected - Read the page
    return readPage(); //go and read the output

  }else{
    return "connection failed";
  }

}

String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  if(inString[0]=='L'){
    //When time = 0 the API returns leaving
     colorWipe(strip.Color(0, 255, 0), 50); // Green
  }else{
    float time =atof(inString);
    if(time>15){
      Serial.println("Working ob boy");//output for serial testing
       colorWipe(strip.Color(0, 255, 0), 50); // Green
    }else{
     if(time>13){
        colorWipe(strip.Color(255, 255, 0), 50); // Yellow
        Serial.println("Ok yeah");
      }else{
        if(time>3){
          colorWipe(strip.Color(255, 0, 0), 50); // Red
        }else{
          if(time>0){
             colorWipe(strip.Color(0, 0, 255), 50); // Blue
          }
        }
      }     
    }
  }
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();
      //
      if (c == '<' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
        Serial.println("Starting to look");
      }else if(startRead){

        if(c != '>'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //got what we need here! We can disconnect now
          startRead = false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");
          return inString;

        }

      }
    }

  }

}
//This is the NeoPixel code for changing colors curtesy of Adafruit
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
