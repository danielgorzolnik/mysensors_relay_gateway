#define MY_GATEWAY_SERIAL
#include <MySensors.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <TimerOne.h>

#define SEND_PERIOD 6000 //in ms/10 for example 100 = 1s
#define READ_ERROR_PERIOD 1000 //in ms/10 for example 100 = 1s

//temperature sensors pins
#define sensor_1_pin 6
#define sensor_2_pin 7
#define sensor_3_pin 8
#define sensor_4_pin 9
#define sensor_5_pin 10
#define sensor_6_pin 11
#define sensor_7_pin 12
#define sensor_8_pin 13

#define ONE_WIRE_1 sensor_1_pin
#define ONE_WIRE_2 sensor_2_pin
#define ONE_WIRE_3 sensor_3_pin
#define ONE_WIRE_4 sensor_4_pin
#define ONE_WIRE_5 sensor_5_pin
#define ONE_WIRE_6 sensor_6_pin
#define ONE_WIRE_7 sensor_7_pin
#define ONE_WIRE_8 sensor_8_pin
OneWire one_wire_1(ONE_WIRE_1);
OneWire one_wire_2(ONE_WIRE_2);
OneWire one_wire_3(ONE_WIRE_3);
OneWire one_wire_4(ONE_WIRE_4);
OneWire one_wire_5(ONE_WIRE_5);
OneWire one_wire_6(ONE_WIRE_6);
OneWire one_wire_7(ONE_WIRE_7);
OneWire one_wire_8(ONE_WIRE_8);
DallasTemperature sensor_1(&one_wire_1);
DallasTemperature sensor_2(&one_wire_2);
DallasTemperature sensor_3(&one_wire_3);
DallasTemperature sensor_4(&one_wire_4);
DallasTemperature sensor_5(&one_wire_5);
DallasTemperature sensor_6(&one_wire_6);
DallasTemperature sensor_7(&one_wire_7);
DallasTemperature sensor_8(&one_wire_8);

DallasTemperature *sensors[8] = {
  &sensor_1,
  &sensor_2,
  &sensor_3,
  &sensor_4,
  &sensor_5,
  &sensor_6,
  &sensor_7,
  &sensor_8,
};

MyMessage msg(0,V_TEMP);

float last_temp[8];
bool sensor_error[8] = {true, true, true, true, true, true, true, true};
bool send_flag = true;
bool read_error_flag = false;
uint16_t send_flag_counter = 0;
uint16_t read_error_counter = 0;

void timer();
void readTemperature(byte sensor_id);

void setup()
{
  Timer1.initialize(10000); //10ms
  Timer1.attachInterrupt(timer);
	for (byte i=0; i<8; i++){
    sensors[i]->begin();
    sensors[i]->setResolution(10);
  }
}

void presentation()
{
  sendSketchInfo("Temperature Sensor Gateway", "1.0");
	for (byte i=0; i<8; i++){
    present(i+1, S_TEMP);
  }
}

void loop()
{
  if (send_flag){
    for (byte i=0; i<8; i++){
      if (sensor_error[i] == false){
        readTemperature(i);
      }
    }
    send_flag = false;
  }
  if (read_error_flag){
    for (byte i=0; i<8; i++){
      if (sensor_error[i] == true){
        readTemperature(i);
      }
    }
    read_error_flag = false;
  }
}

void readTemperature(byte sensor_id){
  sensors[sensor_id]->requestTemperatures();
  float actual_temp = sensors[sensor_id]->getTempCByIndex(0);
  if (actual_temp != -127.0){
    if (actual_temp != last_temp[sensor_id] || sensor_error[sensor_id] == true){
      send(msg.setSensor(sensor_id).set(actual_temp, 1));
      last_temp[sensor_id] = actual_temp;
    }
    sensor_error[sensor_id] = false;
  }
  else {
    sensor_error[sensor_id] = true;
  }
}

void timer(){
  //10ms tick
  send_flag_counter++;
  read_error_counter++;
  if (send_flag_counter >= SEND_PERIOD){
    send_flag = true;
    send_flag_counter = 0;
  }
  if (read_error_counter >= READ_ERROR_PERIOD){
    read_error_flag = true;
    read_error_counter = 0;
  }
}
