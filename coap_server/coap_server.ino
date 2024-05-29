
#include <Arduino.h>
#include "LightResource.hpp"
#include "CoapEndPoint.hpp"
#include "TrafficLightResource.hpp"



UdpAccessPoint g_ap;

CoapEndPoint g_endpoint;

Light g_redLight("light/red");
Light g_yellowLight("light/yellow");
Light g_greenLight("light/green");

enum OutPins{
  OP_Red = 7,
  OP_Yellow = 4,
  OP_Green = 2,
};

TrafficLight g_trafficLight;

void setup()
{
  Serial.begin(9600);

  g_ap.init("Test_AP", "0123456789", CoapConfig::DefaultCoapPort);
  g_ap.printWiFiStatus();
  g_endpoint.init(g_ap);
  g_endpoint.start();

  g_endpoint.addResource(g_redLight);
  g_endpoint.addResource(g_yellowLight);
  g_endpoint.addResource(g_greenLight);
  g_endpoint.addResource(g_trafficLight);

  g_trafficLight.addLight(TL_Red,g_redLight);
  g_trafficLight.addLight(TL_Yellow,g_yellowLight);
  g_trafficLight.addLight(TL_Green,g_greenLight);

  pinMode(OP_Red, OUTPUT);
  pinMode(OP_Yellow, OUTPUT);
  pinMode(OP_Green, OUTPUT);




}


void loop()
{
  
  g_endpoint.loop();

  g_trafficLight.loop();

  digitalWrite(OP_Red, g_redLight.state() ? HIGH : LOW);
  digitalWrite(OP_Yellow, g_yellowLight.state() ? HIGH : LOW);
  digitalWrite(OP_Green, g_greenLight.state() ? HIGH : LOW);



}