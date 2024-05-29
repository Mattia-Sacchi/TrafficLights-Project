#include "CoapEndPoint.hpp"
#include <WiFiUdp.h>


CoapEndPoint::CoapEndPoint()
    : m_ap(nullptr), m_coap(nullptr),m_running(false),m_initialized(false)
{
}

/*
  // send GET or PUT coap request to CoAP server.
  // To test, use libcoap, microcoap server...etc
  // int msgid = coap.put(IPAddress(10, 0, 0, 1), 5683, "light", "1");
  // Serial.println("Send Request");
  // int msgid = coap.get(IPAddress(1, XXX, XXX, XXX), 5683, "time");
if you change LED, req/res test with coap-client(libcoap), run following.
coap-client -m get coap://(arduino ip addr)/light
coap-client -e "1" -m put coap://(arduino ip addr)/light
coap-client -e "0" -m put coap://(arduino ip addr)/light
*/

bool CoapEndPoint::init(UdpAccessPoint &ap)
{
  m_ap = &ap;
  WiFiUDP udp = *m_ap->udp();
  m_coap = new Coap::Manager(udp);
  // Debug
  return m_initialized = true;
}




bool CoapEndPoint::start()
{
  m_coap->start(CoapConfig::DefaultCoapPort);

  return m_running = true;
}

void CoapEndPoint::loop()
{
  if (!m_running || !m_initialized)
    return;
  m_ap->loop();
  m_coap->loop();
}
