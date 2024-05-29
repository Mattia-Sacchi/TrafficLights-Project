#ifndef _TRAFFICLIGHT_RESOURCES_HPP_
#define _TRAFFICLIGHT_RESOURCES_HPP_

#include "CoapManager.hpp"
#include "LightResource.hpp"
#include <arduino-timer.h>

enum Command
{
  C_Invalid = 0,
  C_Manual,
  C_Automatic,
  C_Blinking,
  C_Strobe,
  C_Random,
  C_Count,
};

enum TrafficLights
{
  TL_Green = 0,
  TL_Yellow,
  TL_Red,
  TL_Count,
  TL_Start = TL_Green,
};

class TrafficLight : public Coap::Resource
{
public:
  TrafficLight();

  void loop();

  inline bool addLight(TrafficLights type, Light &l)
  {
    if (type < TL_Start || type >= TL_Count)
      return false;
    l.addManager(*this); // In order to have synchrony
    m_lights[type] = &l;
    if(checkFinalize())
      setCommand(C_Manual);

    return true;
  }

  void onCalled(Coap::Packet &packet, IPAddress ip, int port) override;

  void reset();


  bool setCommand(Command);
  void setRandomCommand(Command);
  bool setFrequency(uint32_t);
  bool setWhich(uint8_t);
  void onTimerCalled();
  void onRandomTimerCalled();

  inline Command command() const { return m_command; }

private:
  void applyCommand(Command c);
  static const uint32_t MinFrequency = 250;
  inline bool checkFinalize()
  {
    for (int i = TL_Start; i < TL_Count; i++)
    {
      if (!m_lights[i])
        return false;
    }
    return m_finalized = true;
  }
  bool m_finalized;
  Timer<> m_timer;
  Light *m_lights[TL_Count];
  struct AutomaticVariables{
    uint32_t timings[TL_Count];
    TrafficLights last;
  }m_autoVariables;
  struct SubCommand{
    Command cmd;
    uint32_t timeThisCommand;
    Timer<> m_randomTimer;
  }m_subCommand;
  Command m_command;
  uint32_t m_frequency;
  uint8_t m_which; // Bitmask
};

#endif
