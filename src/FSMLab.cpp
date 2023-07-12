/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Rich/CTD2023/FSMLab/src/FSMLab.ino"
#include "MQTT.h"

void setup();
void loop();
#line 3 "c:/Users/Rich/CTD2023/FSMLab/src/FSMLab.ino"
#define NORTH 0
#define EAST 1
#define NTOE 2
#define ETON 3

struct State
{
  int output[6];
  unsigned long waitTime;
  int nextState[2];
};
typedef struct State State_t;

State_t fsm[4] = {
  {{0, 0, 1, 1, 0, 0}, 100, {0, 2}},  // NORTH if they put in fsm[0]
  {{1, 0, 0, 0, 0, 1}, 100, {3, 1}},  //East if they put in fsm[1]
  {{0, 1 ,0, 1, 0, 0}, 5000, {1, 1}}, //change North to East if they put in fsm[2]
  {{1, 0, 0, 0, 1, 0}, 5000, {0, 0}}  // change East to North if they put in fsm[3]
};

void callback(char *topic, byte *payload, unsigned int length);

MQTT client("lab.theWCL.com", 1883, callback);

int currentState = 0;
bool directionFlow = false;

SYSTEM_THREAD(ENABLED);
// false is north, true is east
void setup()
{
  client.subscribe("FSM/EpicTopic1");
}
void loop()
{
  if (client.isConnected())
  {
    client.loop();
  }
  else
  {
    client.connect(System.deviceID());
    client.subscribe("FSM/EpicTopic1");
  }
  for (int i = 0; i < 6; i++){
    int sendOut = fsm[currentState].output[i];
    String trueFalse = String(sendOut);
    String numberLED = String(i+1);
    String newestTopic = "FSM/LED" + numberLED;
    client.publish(newestTopic, trueFalse);
  }
  delay(fsm[currentState].waitTime);
  currentState = fsm[currentState].nextState[directionFlow];
}
void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  if (String(p).equals("true"))
  {
    directionFlow = true;
    client.publish("FSM/CoolTopic1", "EastWait");
    client.publish("FSM/LED1", "true");
  }
  else if (String(p).equals("false"))
  {
    directionFlow = false;
    client.publish("FSM/CoolTopic1", "NorthWait");
    client.publish("FSM/LED1", "false");
  }
  else
  {
    client.publish("FSM/CoolTopic1", "Other thing");
  }
}
// client.publish("outTopic/name", "test message");