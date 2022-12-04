 
class PushBtnsBeTriggeredEventsButAssistantBoard{//在副板上搭建一个PushBtnsBeTriggeredEvents类。
  public:
    PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA);
};
PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA){
  pinMode(pinBoil,OUTPUT);
  pinMode(pinAddWater,OUTPUT);
  pinMode(pinFan,OUTPUT);
}
////////////////////////////////////////////
PushBtnsBeTriggeredEventsButAssistantBoard *pbt ;
void setup() {
  pbt = new PushBtnsBeTriggeredEventsButAssistantBoard(4,5,6,2,3,A5,A4);
  Serial.begin(9600);
  pinMode(A0,OUTPUT);
}

void loop() {
  for(int i =0;i<Serial.available();i++){
    char a = Serial.read();
    if(a=='s') digitalWrite(A0,HIGH);
  }
}


