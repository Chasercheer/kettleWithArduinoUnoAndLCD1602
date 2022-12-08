#include <Wire.h>

//#include <Adafruit_MLX90614.h>

//////////////////////////////////////////

class SimpleMLX90614{//简易的MLX90614的通信类。需要导入Arduino Wire库后使用。
public:
  SimpleMLX90614::SimpleMLX90614(uint8_t i2caddr=0x5A);//MLX90614的说明书上表示它的SMBUS地址是0x0E,但它在两线制的IIC中的实际出厂设定的地址是0x5A（见说明书的“从动器地址”）。也可以使用群呼地址0x00。群呼地址0x00是IIC协议中的保留地址，当主机使用该地址寻址时，当前IIC总线上连接的所有器件都会应答，但最终只会有一个从器件会与主机通信。
  float readAmbientTempC();//读取传感器周围的环境摄氏度温度
  float readObjectTempC();//读取传感器前方的被测物体摄氏度温度
private:
  uint8_t i2caddr;
  uint16_t readRawTempVal(uint8_t addr);//读取传感器返回的关于温度的原始数值。
};

SimpleMLX90614::SimpleMLX90614(uint8_t i2caddr){
  Wire.begin();
  this->i2caddr = i2caddr;
}

float SimpleMLX90614::readAmbientTempC(){
  float res;
  res = readRawTempVal(0x06);
  res *=.02;
  res -=273.15;
  return res;
}

float SimpleMLX90614::readObjectTempC(){
  float res;
  res = readRawTempVal(0x07);
  res *=.02;
  res -=273.15;
  return res;
}

uint16_t SimpleMLX90614::readRawTempVal(uint8_t addr){
  uint16_t res;
  Wire.beginTransmission(i2caddr);
  Wire.write(addr);  //addr为MLX90614储存温度的RAM地址。0x07是MLX90614储存被测物体温度的RAM地址；0x06是MLX90614储存其周围环境温度的RAM地址。
  Wire.endTransmission(false);
  Wire.requestFrom(i2caddr,(uint8_t)3);
  if(Wire.available()>0){
    res = Wire.read();
    res |= Wire.read()<<8;
    uint8_t pec = Wire.read();
    //Serial.print(res);
  }
  delay(500);
  return res;    
}

///////////////////////////////////////////
class PushBtnsBeTriggeredEventsButAssistantBoard{//在副板上搭建一个PushBtnsBeTriggeredEvents类。
  public:
    SimpleMLX90614 mlx90614;
    PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA);
    void mainBoardCommandReciver();
 
};

PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA):mlx90614(){
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

////////////////////////////////////////////

PushBtnsBeTriggeredEventsButAssistantBoard *pbt ;
void setup() {
  pbt = new PushBtnsBeTriggeredEventsButAssistantBoard(4,5,6,2,3,A5,A4);
  Serial.begin(9600);
  
  while (!Serial);
/*
  while(!mlx.begin(0x5A)) {
    Serial.print("Error connecting to MLX sensor. Check wiring.");
  };
  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
  */
}

void loop() {
/*
  pbt->mainBoardCommandReciver();
  for(int i =0;i<Serial.available();i++){
    char a = Serial.read();
    if(a=='s') digitalWrite(A0,HIGH);
  }
  
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF());
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  delay(500);  
*/  
  Serial.println(pbt->mlx90614.readObjectTempC());
}  


/*
void serialEvent(){
  //这个Arudino内置的函数会在每次loop函数后被调用（如果有新数据从RX脚写入）。在此暂时先搁置。
  
}
*/
