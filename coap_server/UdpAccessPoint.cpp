#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <WiFi.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include "UdpAccessPoint.hpp"

UdpAccessPoint::UdpAccessPoint()
    : m_server(nullptr), m_udp(nullptr),m_port(0)
{
}

bool UdpAccessPoint::init(String networkName, String networkPassword,int port)
{
  if(!port)
    return false;
  m_udp = new WiFiUDP();
  m_udp->begin(port);
  m_server = new WiFiServer(port);

  // Create open network. Change this line if you want to create an WEP network:
  m_status = WiFi.beginAP(networkName.c_str(), networkPassword.c_str());

  if (m_status != WL_AP_LISTENING)
  {
    Serial.println("Creating access point failed");
    // don't continue
    return false;
  }

  // wait 10 seconds for connection:
  delay(1000);

  Serial.println("Ready");

  m_status = WiFi.status();

  if (m_status == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    return false;
  }

  m_server->begin();

  return true;
}

void UdpAccessPoint::loop()
{
  m_status = WiFi.status();
}

void UdpAccessPoint::printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}