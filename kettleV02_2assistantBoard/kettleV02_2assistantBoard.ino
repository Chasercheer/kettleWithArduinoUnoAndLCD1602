//
class PushBtnsBeTriggeredEventsButAssistantBoard{//在副板上搭建一个PushBtnsBeTriggeredEvents类。
  public:
    PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA);
    void mainBoardCommandReciver();
 
};

PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA){
  pinMode(pinBoil,OUTPUT);
  pinMode(pinAddWater,OUTPUT);
  pinMode(pinFan,OUTPUT);
}

void PushBtnsBeTriggeredEventsButAssistantBoard::mainBoardCommandReciver(){
  while(!Serial);
  char a;
  if(Serial.available()>0){
    for(int i=0;i<Serial.available();i++){
      a = Serial.read();
      switch(a){
        case 'A'://开始烧水
          break;
        case 'B'://停止烧水
          break;
        case 'C'://开始加水
          break;
        case 'D'://停止加水
          break;
        case 'E'://开启风扇
          break;
        case 'F'://停止风扇
          break;
        case 'G'://温度反馈
          break;
        case 'H'://重量反馈
          break;
      }
    }  
  }
  delay(100);
}

////////////////////////////////////////////

PushBtnsBeTriggeredEventsButAssistantBoard *pbt ;
void setup() {
  pbt = new PushBtnsBeTriggeredEventsButAssistantBoard(4,5,6,2,3,A5,A4);
  Serial.begin(9600);
  pinMode(A0,OUTPUT);
}

void loop() {
  pbt->mainBoardCommandReciver();
  for(int i =0;i<Serial.available();i++){
    char a = Serial.read();
    if(a=='s') digitalWrite(A0,HIGH);
  }
}

/*
void serialEvent(){
  //这个Arudino内置的函数会在每次loop函数后被调用（如果有新数据从RX脚写入）。在此暂时先搁置。
  
}
*/
