#ifndef _LIGHT_RESOURCES_HPP_
#define _LIGHT_RESOURCES_HPP_

#include "CoapManager.hpp"


class TrafficLight;

class Light : public Coap::Resource
{
public:
  Light(String topic);
  void addManager(TrafficLight &tl); // in case i need to notify something

  void onCalled(Coap::Packet &packet, IPAddress ip, int port) override;

  inline void setState(bool state) { m_state = state; }
  inline bool state() const { return m_state; }

private: 
  bool m_state;
  TrafficLight * m_manager;

};

#endif
