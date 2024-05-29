#ifndef _COAP_SERVER_MANAGER_HPP_
#define _COAP_SERVER_MANAGER_HPP_

#include "UdpAccessPoint.hpp"
#include "CoapManager.hpp"


class CoapEndPoint
{
public:
  CoapEndPoint();
  bool init(UdpAccessPoint &ap);
  void loop();
  bool start();
  inline Coap::Manager *coap()  { return m_coap; }
  inline void addResource(Coap::Resource &c) { m_coap->server(c); }

private:
  UdpAccessPoint *m_ap;
  Coap::Manager *m_coap;
  bool m_running;
  bool m_initialized;
};

#endif