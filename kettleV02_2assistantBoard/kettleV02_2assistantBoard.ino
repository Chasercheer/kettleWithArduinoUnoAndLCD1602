#include <Wire.h>

/////////////////////////////////////////////
/*
测重原理讲解：
1.
满量程输出电压=激励电压*传感器输出电压比值1.0mv/v
例如：供电电压是 5v 乘以 传感器输出电压比值1.0mv/v=传感器输出电压满量程 5mv。
相当于有 5Kg 重力产生时候传感器传感器传递给HX711 5mV 的电压信号。
2.
概述：711 模块 A 通道带有 128 倍信号增益，可以将 5mV 的电压放大 128 倍，然后采样输
出 24bit AD 转换的值，单片机通过指定时序将 24bit 数据读出。
详细讲解程序计算原理：
步骤 1：如何计算传感器供电电压
HX711 可以在产生 VAVDD 和 AGND 电压，即 711 模块上的 E+和 E-电压。
该电压通过 VAVDD=VBG(R1 +R2 )/R2 计算。
VBG 为模块儿基准电压 1.25v
R1 = 20K,R2 = 8.2K
因此得出 VAVDD = 4.3V
（为了降低功耗，该电压只在采样时刻才有输出，因此用万用表读取的值可能低于 4.3v，因
为万用表测量的是有效值。）
步骤 2：如何将 AD 值反向转换为重力值。
假设重力为 A Kg，（x<5Kg）,测量出来的 AD 值为 y
传感器输出，发送给 AD 模块儿的电压为 A Kg * 4.3mV / 5Kg = 0.86A mV
经过 128 倍增益后为 128 * 0.86A = 110.08AmV
转换为 24bit 数字信号为 110.08A mV * 2的24次方 / 4.3V = 429496.7296A
所以 y = 429496.7296A
因此得出 A = y / 429496.7296
所以得出程序中计算公式
Weight_Shiwu = (unsigned long)((float)Weight_Shiwu/429.5);（单位为g）
特别注意：
因为不同的传感器斜率特性曲线不是完全一样，因此，每一个传感器需要矫正这里的 429.5
这个除数，才能达到精度很高。
修改以下部分代码用于校准（该段程序在 main.c 最上面）
#define GapValue 430
当发现测试出来的重量偏大时，增加该数值。
如果测试出来的重量偏小时，减小改数值。该值可以为小数，例如 429.5 等。

另注：HX711的电源电压下限实际很高，远比说明书的2.6V高，甚至是在4.5V左右。所以它的电源线最好直接连接UNO的5V输出而不是通过面包板再连接5V输出。否则将产生供电电压不足而不工作的情况。
*/



class HX711with5KgSensor{
  public:
    HX711with5KgSensor(int HX711_SCK=2,int HX711_DT=3,float gapValue=389.99);
    void get_Maopi();
    float get_Weight();
    unsigned long HX711_Read();
  private:
    int HX711_SCK,HX711_DT;
    float gapValue,weight_Maopi ,weight_Shiwu;//修改这个值以修正传感器称重结果。
  

};


HX711with5KgSensor::HX711with5KgSensor(int HX711_SCK,int HX711_DT,float gapValue):HX711_SCK(HX711_SCK){
  this->HX711_DT=HX711_DT;
  this->gapValue=gapValue;
  weight_Maopi = 0;
  weight_Shiwu = 0;
  pinMode(HX711_SCK, OUTPUT);	//初始化HX711模块连接的IO设置
	pinMode(HX711_DT, INPUT);
}


void HX711with5KgSensor::get_Maopi() //获取皮重。
{
	weight_Maopi = HX711_Read();
} 

//****************************************************
//称重
//****************************************************
float HX711with5KgSensor::get_Weight()//去皮重量计算：计算去皮后的放在传感器上的重物重量
{
	weight_Shiwu =  HX711_Read();
	weight_Shiwu = weight_Shiwu - weight_Maopi;				//获取实物的AD采样数值。
	weight_Shiwu = weight_Shiwu/gapValue; 	
	return weight_Shiwu;
}


unsigned long HX711with5KgSensor::HX711_Read()	//增益128。从HX711芯片上读取原始值。
{
	unsigned long count=0; 
	unsigned char i;
	
  //下为原作者令人费解的操作
  //bool Flag = 0;
	//digitalWrite(HX711_SCK, HIGH);//而且这句与下句会引发一个错误：在get_Maopi（）之后立即调用get_Weight()将可能会导致出现一个错误的读数。这是因为HX711手册上并不允许在确认HX711_DT为低电平之前将HX711_SCK拉高。
	//delayMicroseconds(1);

	digitalWrite(HX711_SCK, LOW);//使能AD（PD_SCK 置低）
	delayMicroseconds(1); 
  while(digitalRead(HX711_DT)); 
  for(i=0;i<24;i++)
	{ 
	  digitalWrite(HX711_SCK, HIGH); 
		delayMicroseconds(1);
	  count=count<<1; 
		digitalWrite(HX711_SCK, LOW); 
		delayMicroseconds(1);
	  if(digitalRead(HX711_DT))
			count++; 
	} 
 	digitalWrite(HX711_SCK, HIGH); 
	count ^= 0x800000;//第25个脉冲下降沿来时，转换数据
	delayMicroseconds(1);
	digitalWrite(HX711_SCK, LOW); 
	delayMicroseconds(1);
	return(count);
  }
  

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
    HX711with5KgSensor hx711;
    PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA);
    void mainBoardCommandReciver();
 
};

PushBtnsBeTriggeredEventsButAssistantBoard::PushBtnsBeTriggeredEventsButAssistantBoard(int pinBoil,int pinAddWater,int pinFan,int pinHX711DT,int pinHX711SCK,int pinMLX90614SCL,int pinMLX90614SDA):mlx90614(),hx711(){
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
  //Serial.println(pbt->mlx90614.readObjectTempC());
}  


/*
void serialEvent(){
  //这个Arudino内置的函数会在每次loop函数后被调用（如果有新数据从RX脚写入）。在此暂时先搁置。
  
}
*/
