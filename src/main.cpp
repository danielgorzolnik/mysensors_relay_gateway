#define MY_GATEWAY_SERIAL
#include <MySensors.h>
#include <EEPROM.h>

#define relay_count 8
#define eeprom_offset 100

//temperature sensors pins
#define relay_1_pin 13
#define relay_2_pin 12
#define relay_3_pin 11
#define relay_4_pin 10
#define relay_5_pin 9
#define relay_6_pin 8
#define relay_7_pin 7
#define relay_8_pin 6

byte relay_array[relay_count] = {
  relay_1_pin,
  relay_2_pin,
  relay_3_pin,
  relay_4_pin,
  relay_5_pin,
  relay_6_pin,
  relay_7_pin,
  relay_8_pin,
};

MyMessage msg(0,V_LIGHT);

void before(){
  for (byte i=0; i<relay_count; i++){
    pinMode(relay_array[i], OUTPUT);
    bool last_state = EEPROM.read(eeprom_offset + i);
    digitalWrite(relay_array[i], last_state);
  }
}


void presentation()
{
  sendSketchInfo("Relay Gateway", "1.0");
	for (byte i=0; i<relay_count; i++){
    byte tmp_id = i+1;
    present(tmp_id, S_LIGHT);
    send(msg.setSensor(tmp_id).set(digitalRead(relay_array[i])));
  }
}

void setup()
{

}

void loop()
{

}

void receive(const MyMessage &message) {
  if (message.type == V_LIGHT) {
    // get message data
    byte relay_id = message.getSensor();
    bool relay_state = message.getBool();

    if (relay_id <= relay_count){
      digitalWrite(relay_array[relay_id - 1], relay_state);
      EEPROM.update(eeprom_offset + (relay_id - 1), relay_state);
    }
   } 
}
