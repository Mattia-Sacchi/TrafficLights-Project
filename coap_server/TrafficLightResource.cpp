#include "TrafficLightResource.hpp"
#include <ArduinoJson.h>

uint32_t  defaultTimings[3]= {10 *1000,1*1000,5*1000};
static const String trafficLightUniqueTopic = "trafficlight";
static const uint8_t MaximumWhich = 7;

bool onTimerCalledCallback(void * v)
{
  if(v != nullptr)
    static_cast<TrafficLight*>(v)->onTimerCalled();
}

bool onRandomTimerCalledCallback(void * v)
{
  randomSeed(millis());
  if(v != nullptr)
    static_cast<TrafficLight*>(v)->onRandomTimerCalled();
}

template<typename T>
bool checkBit(T var,uint8_t pos)
{
  return ((var) & (1<<(pos)));
}



TrafficLight::TrafficLight() : Resource(trafficLightUniqueTopic), m_command(C_Invalid), m_finalized(false), m_which(0),m_frequency(MinFrequency) {
  
  
  memcpy(m_autoVariables.timings,defaultTimings,sizeof(defaultTimings));
}

void TrafficLight::loop()
{
    if (!m_finalized) // It cannot work without all 3 lights
        return;
    m_timer.tick();
    m_subCommand.m_randomTimer.tick();


}

void TrafficLight::reset()
{
  if(!m_finalized)
    return;

  for( int i =  TL_Start; i <TL_Count; i++)
    m_lights[i]->setState(false);

}

void TrafficLight::onCalled(Coap::Packet &packet, IPAddress ip, int port)
{
    if (!m_finalized) // It cannot work without all 3 lights
        return;
        Serial.println(topic());
    size_t const len = packet.payloadLength;
    JsonDocument doc;


  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, packet.payload);

  // Test if parsing succeeds.
  if (error || packet.code != Coap::M_PUT) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    m_responseInfo.code = Coap::RC_BAD_REQUEST;
    return;
  }

  
  m_responseInfo.code = Coap::RC_CHANGED;

  if(!doc["timings"].isNull()){
    if(doc["timings"].size() != 3)
    {
      m_responseInfo.payload = "Timings are 3";
      m_responseInfo.type = Coap::CT_TEXT_PLAIN;
      m_responseInfo.code = Coap::RC_BAD_REQUEST;
      return;
    }

    for(int i = TL_Start; i < TL_Count; i++){
      m_autoVariables.timings[i] = doc["timings"][i];
      Serial.print("Timing ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(m_autoVariables.timings[i]);
    }
  }

  if(!doc["which"].isNull())
    setWhich(doc["which"]);
  

  if(!doc["freq"].isNull())
    setFrequency(doc["freq"]);
  

  

    // The command is the last variable you set
  if( !doc["cmd"].isNull() )
    setCommand(doc["cmd"]);
}

void TrafficLight::onRandomTimerCalled()
{
  m_subCommand.cmd = static_cast<Command>(random(C_Automatic,C_Random));
  setRandomCommand(m_subCommand.cmd);
}

void TrafficLight::onTimerCalled()
{
  if (!m_finalized) // It cannot work without all 3 lights
        return;

  switch(m_command == C_Random ? m_subCommand.cmd : m_command )
  {
    case C_Strobe:
    case C_Blinking:
      for(int i = TL_Start; i < TL_Count; i++)
      {
        if(checkBit(m_which, i))
          m_lights[i]->setState(!m_lights[i]->state());
      }
    break;
    case C_Automatic:
      m_lights[m_autoVariables.last]->setState(false);
      
      m_autoVariables.last = m_autoVariables.last +1 >= TL_Count ? TL_Start : static_cast<TrafficLights>(m_autoVariables.last +1);
      m_lights[m_autoVariables.last]->setState(true);
      m_timer.in(m_autoVariables.timings[m_autoVariables.last],onTimerCalledCallback,this);
    break;
    default:
    break;

  }
}

bool TrafficLight::setCommand(Command c)
{
  m_command = c;

  if(m_command <= C_Invalid || m_command >= C_Count)
  {
    m_responseInfo.payload = "Invalid Commands, Valid commands are:\n1. Manual\n2. Automatic\n3. Blinking\n4. Strobe";
    m_responseInfo.type = Coap::CT_TEXT_PLAIN;
    m_responseInfo.code = Coap::RC_BAD_REQUEST;
    return false;
  }

  
  Serial.print("Command: ");
  Serial.println(m_command);

  // The command is the last variables you will receive so all the members are ready to go
  m_responseInfo.code = Coap::RC_CHANGED;
  
  reset();
  m_timer.cancel();
  m_subCommand.m_randomTimer.cancel();
  applyCommand(m_command);

  

   return true;
}

void TrafficLight::applyCommand(Command c)
{
  switch(c)
  {
    case C_Blinking:
      m_timer.every(m_frequency,onTimerCalledCallback,this);
      for(int i = TL_Start; i < TL_Count; i++)
      {
        if(checkBit(m_which, i))
          m_lights[i]->setState(!m_lights[i]->state());
      }
    break;
    case C_Automatic:
      m_timer.in(m_autoVariables.timings[TL_Start],onTimerCalledCallback,this);
      m_autoVariables.last = TL_Start;
      m_lights[TL_Start]->setState(true);

    break;
    case C_Strobe:
      m_which = MaximumWhich;
      m_timer.every(m_frequency,onTimerCalledCallback,this);
      for(int i = TL_Start; i < TL_Count; i++)
      {
        if(checkBit(m_which, i))
          m_lights[i]->setState(!m_lights[i]->state());
      }

    break;
    case C_Random:
    
      m_subCommand.cmd = static_cast<Command>(random(C_Automatic,C_Random));
      setRandomCommand(m_subCommand.cmd);
      
      
    break;
    default:
    break;

  }
  
}

void TrafficLight::setRandomCommand(Command c)
{
  m_subCommand.timeThisCommand = random(MinFrequency, 5 * 1000);
  m_subCommand.m_randomTimer.in(m_subCommand.timeThisCommand,onRandomTimerCalledCallback,this);
  m_which = random(0, MaximumWhich) +1;
  m_frequency = random(MinFrequency,10 * 1000);

  m_autoVariables.timings[0] = random(MinFrequency,10 * 1000);
  m_autoVariables.timings[1] = random(MinFrequency,10 * 1000);
  m_autoVariables.timings[2] = random(MinFrequency,10 * 1000);
  m_autoVariables.last = static_cast<TrafficLights>(random(TL_Start, TL_Count));
  
  
  Serial.print("Command: ");
  Serial.println(m_subCommand.cmd);
  Serial.print("Time this command: ");
  Serial.println(m_subCommand.timeThisCommand);
  Serial.print("Frequency: ");
  Serial.println(m_frequency);
  Serial.print("Which: ");
  Serial.println(m_which,2);

  for(int i = TL_Start; i < TL_Count; i++){
      Serial.print("Timing ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(m_autoVariables.timings[i]);
  }

  

  reset();
  m_timer.cancel();
  applyCommand(m_subCommand.cmd);

}

bool TrafficLight::setFrequency(uint32_t freq)
{
  if(freq < MinFrequency)
  {
    m_responseInfo.payload = "The minimum frequency is ";
    m_responseInfo.payload.concat(String(MinFrequency));
    m_responseInfo.type = Coap::CT_TEXT_PLAIN;
    m_responseInfo.code = Coap::RC_BAD_REQUEST;
    return false;
  }
  m_frequency = freq;
  Serial.print("Frequency: ");
  Serial.println(m_frequency);
  m_responseInfo.code = Coap::RC_CHANGED;
  return true;
}

bool TrafficLight::setWhich(uint8_t which)
{
  if(which > MaximumWhich)
  {
    m_responseInfo.payload = "Which variable must be between 0 and 7";
    m_responseInfo.type = Coap::CT_TEXT_PLAIN;
    m_responseInfo.code = Coap::RC_BAD_REQUEST;
    return false;
  }
  m_which = which;
  Serial.print("Which: ");
  Serial.println(m_which,2);
  m_responseInfo.code = Coap::RC_CHANGED;
  return true;
}
