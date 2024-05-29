#ifndef _UDP_ACCESS_POINT_HPP_
#define _UDP_ACCESS_POINT_HPP_
#include <Arduino.h>

class WiFiUDP;
class WiFiServer;

class UdpAccessPoint
{
public:
  UdpAccessPoint();
  bool init(String networkName, String networkPassword,int port);
  void loop();


  inline WiFiUDP* udp() { return m_udp; } ;
  inline WiFiServer* server() { return m_server; };

  void printWiFiStatus();

private:
  WiFiUDP* m_udp;
  WiFiServer* m_server;
  int m_port;
  int m_status;       
};

#endif