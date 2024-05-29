#include "LightResource.hpp"
#include "TrafficLightResource.hpp"

void Light::addManager(TrafficLight &tl)
{
  m_manager = &tl;
}


Light::Light(String topic) : Resource(topic), m_state(false) {}

void Light::onCalled(Coap::Packet &packet, IPAddress ip, int port)
{
  Serial.print(topic());
  Serial.println(" topic called");

  if(!m_manager)
    return;
  
  if(m_manager->command() != C_Manual)
    return;

  char p[packet.payloadLength + 1];
  memcpy(p, packet.payload, packet.payloadLength);
  p[packet.payloadLength] = NULL;

  String message(p);

  m_state = !message.equals("0");

  switch (packet.code)
  {
  case Coap::M_PUT:
    m_responseInfo.payload = m_state ? "ON" : "PFF";
    m_responseInfo.code = Coap::RC_CHANGED;
    m_responseInfo.type = Coap::CT_TEXT_PLAIN;
    break;
  case Coap::M_GET:
    m_responseInfo.payload = m_state ? "ON" : "OFF";
    m_responseInfo.code = Coap::RC_CONTENT;
    m_responseInfo.type = Coap::CT_TEXT_PLAIN;
    break;
  default:
    m_responseInfo.code = Coap::RC_INTERNAL_SERVER_ERROR;
    m_responseInfo.type = Coap::CT_NONE;
  }
}