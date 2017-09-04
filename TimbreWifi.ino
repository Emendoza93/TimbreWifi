#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h> 

/**************************************************************************************
 * this example shows how to set a static IP configuration for the ESP
 * although the IP shows in the config portal, the changes will revert 
 * to the IP set in the source file.
 * if you want the ability to configure and persist the new IP configuration
 * look at the FS examples, which save the config to file
 *************************************************************************************/
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
unsigned int server_port;  //server

char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Cama:1";  // a reply string to send back
char  replyPacekt2[] ="Cama:2";


//default custom static IP
//char static_ip[16] = "10.0.1.59";
//char static_gw[16] = "10.0.1.1";
//char static_sn[16] = "255.255.255.0";
int buttonState = 0 ; 
int buttonState2 =0;
int pinbutton = 2; // pulsador de cama 1
int pinbutton2 = 5; // pulsador de cama 2
IPAddress IP_server;
int contacto = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  int timbre;
  pinMode(pinbutton, INPUT_PULLUP);
  pinMode(pinbutton2, INPUT_PULLUP);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set static ip
  //block1 should be used for ESP8266 core 2.1.0 or newer, otherwise use block2

  //start-block1
  //IPAddress _ip,_gw,_sn;
  //_ip.fromString(static_ip);
  //_gw.fromString(static_gw);
  //_sn.fromString(static_sn);
  //end-block1

  //start-block2
  IPAddress _ip = IPAddress(192, 168, 1, 98);// se le asigna ip al modulo
  IPAddress _gw = IPAddress(192, 168, 1, 2);// puerta de enlace predeterminada
  IPAddress _sn = IPAddress(255, 255, 255, 0);//mascara de subred 
  //end-block2
   
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);


  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("LaLa", "921YTcmz")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");


  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
  // put your main code here, to run repeatedly:
if (contacto)
{
   buttonState=digitalRead(pinbutton);
   buttonState2=digitalRead(pinbutton2);
 
      if(buttonState==LOW){
         // Serial.println(replyPacekt);
          Udp.beginPacket(IP_server, server_port);
          Udp.write(replyPacekt);
          Udp.endPacket();
          }

    if(buttonState2==LOW){
         // Serial.println(replyPacekt);
          Udp.beginPacket(IP_server, server_port);
          Udp.write(replyPacekt2);
          Udp.endPacket();
         }
     
     delay(300);
  
}

int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    contacto = 1;
    server_port =  Udp.remotePort();
    IP_server = Udp.remoteIP();
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);      
      
  }

}


