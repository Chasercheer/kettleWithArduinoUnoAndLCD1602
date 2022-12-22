#include <Ds1302.h>
#include <Wire.h>
#include<EEPROM.h>
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

#define fixBottomVal 200 //这是测重模块对底座的一个修正值。测重平面上还要安装一个电热盘，所以一开始的数据便是带这个电热盘的重量的。这个值就代表电热盘的重量值。

class HX711with5KgSensor{
  public:
    HX711with5KgSensor(int HX711_SCK=2,int HX711_DT=3,float gapValue=389.99);
    float get_Maopi();
    float get_Weight();
    unsigned long HX711_Read();
    float gapValue,grossWeight ,netWeight,allOnThePlateWeight;//修改gapValue这个值以修正传感器称重结果。netWeight是去壶重后的净水重，grossWeight是壶重，allOnThePlateWeight是所有处于测量平面上的物体的总重量。这重量包括发热盘底座的重量。weight_allOnthePlate = grossWeight + netWeight + fixBottomVal
  private:
    int HX711_SCK,HX711_DT;
    

};


HX711with5KgSensor::HX711with5KgSensor(int HX711_SCK,int HX711_DT,float gapValue):HX711_SCK(HX711_SCK){
  this->HX711_DT=HX711_DT;
  this->gapValue=gapValue;
  pinMode(HX711_SCK, OUTPUT);	//初始化HX711模块连接的IO设置
	pinMode(HX711_DT, INPUT);
}


float HX711with5KgSensor::get_Maopi() //获取皮重。
{
	grossWeight = HX711_Read()/gapValue-fixBottomVal;//
  return grossWeight;
} 

//****************************************************
//称重
//****************************************************
float HX711with5KgSensor::get_Weight()//去皮重量计算：计算去皮后的放在传感器上的重物重量
{
	allOnThePlateWeight =  HX711_Read()/gapValue; //HX711_Read()/gapValue测得的是所有处于测量平面上的物体的总重量。这重量包括发热盘底座的重量。
	netWeight = allOnThePlateWeight - grossWeight-fixBottomVal;				//获取实物的AD采样数值。它需要减去壶重再减去加热盘底座的重量。	
	return netWeight;
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
  SimpleMLX90614::SimpleMLX90614(uint8_t i2cAddr=0x5A);//MLX90614的说明书上表示它的SMBUS地址是0x0E,但它在两线制的IIC中的实际出厂设定的地址是0x5A（见说明书的“从动器地址”）。也可以使用群呼地址0x00。群呼地址0x00是IIC协议中的保留地址，当主机使用该地址寻址时，当前IIC总线上连接的所有器件都会应答，但最终只会有一个从器件会与主机通信。
  float readAmbientTempC();//读取传感器周围的环境摄氏度温度
  float readObjectTempC();//读取传感器前方的被测物体摄氏度温度
private:
  uint8_t i2cAddr;
  uint16_t readRawTempVal(uint8_t addr);//读取传感器返回的关于温度的原始数值。
};

SimpleMLX90614::SimpleMLX90614(uint8_t i2cAddr){
  Wire.begin();
  this->i2cAddr = i2cAddr;
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
  Wire.beginTransmission(i2cAddr);
  Wire.write(addr);  //addr为MLX90614储存温度的RAM地址。0x07是MLX90614储存被测物体温度的RAM地址；0x06是MLX90614储存其周围环境温度的RAM地址。
  Wire.endTransmission(false);
  Wire.requestFrom(i2cAddr,(uint8_t)3);
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
class PushBtnsButAssiatantBoard{//在副板上搭建一个PushBtnsBeTriggeredEvents类。
  public:
    int pinBoil,pinAddWater,pinFan;//控制发热盘/水泵/风扇工作的三个引脚
    int btnManualAddWater,btnManualHeat;//手动上水/手动加热的两个按钮所对应的引脚
    SimpleMLX90614 mlx90614;
    HX711with5KgSensor hx711;
    Ds1302::DateTime TimeBuffer;//用以接收从主板来的ds1302的时间数据。
    int currentWaterWeight,currentTemp;//用于缓存传感器温度值或重量值,即当前温度与当前去皮水重。诚然，传感器温度值和重量值都可以是浮点值，但是这种精度在此不必要。
    int cycleStartDay,addWaterLimHigh,addWaterLimLow,cycleGapDay,cycleClockHour,cycleClockMin,heatSaveTemp,heatTemp,bottleWeight;//这些变量将存入副板的EEPROM中
    bool SINGLEBOILFLAG,CYCLEBOILFLAG,AUTOBOILFLAG,HEATSAVEFLAG,HEATFLAG,MANUALWATERFLAG;//副板上各个烧水模式的状态标志。
    bool CYCLEBOILONETIME,CYCLEBOILONETIMESTARTED,AUTOBOILONETIME,AUTOBOILONETIMESTART;//这四个变量记录了周期烧水模式与自动续水烧水模式一次烧水的结束与否。
    int lastRecordMillis1,lastRecordMillis2,lastRecordMillis3;//用于boilStateController()的计时。设置三个变量是为了使用三个不同的间隔周期去监视控制各个状态的运行。
    int lastWaterWeight,failAddWaterTimes;//用于对自动加水的溢流保护。
    bool isBoiled;//用于boilStateController()的烧水判断。
    bool waterError;//上水故障标志
    
    //int fanRunningTime;//用于调控风扇的运行时间--*暂置
////////////////////data↑///////////////////////

////////////////////func↓///////////////////////    

//--------------------------构造函数------------------------------
    PushBtnsButAssiatantBoard::PushBtnsButAssiatantBoard(int pinBoil,int pinAddWater,int pinFan,int btnManualAddWater,int btnManualHeat,int pinHX711SCK=2,int pinHX711DT=3,uint8_t i2cAddr=0x5A);
//---------------------------------------------------------------

//--------------------------主板通讯函数------------------------------
    void mainBoardCommandReciver();
    void AssisBoardDataSender(String signal);
//-------------------------------------------------------------------

//--------------------------功能函数------------------------------   
    void boilStateController();//放在loop函数中，用以开启/关闭，监视，控制各个烧水模式的运行。
    bool ifKettleOn();//检测当前发热盘上是否有水壶。如有则返回True，如没有则返回False。
    bool safeHeater();//具有自动判断当前温度从而决定是否该上水加热的底层烧水函数。它调用addWater()。
    bool addWater();//调用该函数则开始加水。该函数可以判断当前水量是否低于上水阈值并决定是否加水，并且还有防止水意外流出水壶的功能。当检测到水意外流出壶的时候返回false，否则返回true。
//--------------------------------------------------------------- 
};

PushBtnsButAssiatantBoard::PushBtnsButAssiatantBoard(int pinBoil,int pinAddWater,int pinFan,int btnManualAddWater,int btnManualHeat,int pinHX711SCK,int pinHX711DT,uint8_t i2cAddr):mlx90614(i2cAddr),hx711(pinHX711SCK,pinHX711DT){
  this->pinBoil=pinBoil;
  this->pinAddWater=pinAddWater;
  this->pinFan=pinFan;
  this->btnManualAddWater=btnManualAddWater;
  this->btnManualHeat=btnManualHeat;//手动加热功能暂时取消。因为这个功能是较为不安全的，比较棘手。但在此先保留一个变量。
  pinMode(pinBoil,OUTPUT);
  pinMode(pinAddWater,OUTPUT);
  pinMode(pinFan,OUTPUT);
  pinMode(btnManualAddWater,INPUT_PULLUP);
  EEPROM.get(0,cycleStartDay);
  hx711.grossWeight=bottleWeight;
  EEPROM.get(2,cycleGapDay);
  EEPROM.get(4,cycleClockHour);
  EEPROM.get(6,cycleClockMin);  
  EEPROM.get(8,addWaterLimHigh);
  EEPROM.get(10,addWaterLimLow);  
  EEPROM.get(12,heatSaveTemp);
  EEPROM.get(14,heatTemp);
  EEPROM.get(16,bottleWeight);
  failAddWaterTimes=0;
  waterError = false;
  CYCLEBOILONETIME=false;
  CYCLEBOILONETIMESTARTED=false;
  AUTOBOILONETIME=false;
  AUTOBOILONETIMESTART=false;
}


void PushBtnsButAssiatantBoard::mainBoardCommandReciver(){
  String receiverBuffer;
  char receiverCBuffer;
  //while(!Serial) ;
  delay(50);//这是为确保在该函数被调用时副板能接收到主板的发送的（至少）一个的完整的数据包
  if(Serial.available()>0){
    receiverCBuffer=Serial.read();
  }
  if(receiverCBuffer =='@'){
    receiverCBuffer=Serial.read();
  }
  if(receiverCBuffer =='#'){
    receiverCBuffer=Serial.read();
    switch(receiverCBuffer){
      case 'S':
        //收到单次烧水模式开始指令 
        SINGLEBOILFLAG=true;
        break;
      case 's':
        //收到单次烧水模式结束指令
        SINGLEBOILFLAG=false;
        AssisBoardDataSender("#S"); 
        break;
      case 'C':
        //收到周期定时烧水模式开始指令
        CYCLEBOILFLAG=true;
        break;
      case 'c':
        //收到周期定时烧水模式结束指令
        CYCLEBOILFLAG=false;
        break;
      case 'A':
        //收到自动续水烧水模式开始指令
        AUTOBOILFLAG = true;        
        break;
      case 'a':
        //收到自动续水烧水模式结束指令
        AUTOBOILFLAG = false;
        break;
      case 'E':
        //收到烧水后保温模式开始指令          
        HEATSAVEFLAG=true;
        break; 
      case 'e':
        //收到烧水后保温模式结束指令
        HEATSAVEFLAG=false;
        break;
      case 'H':
        //收到加热模式开始指令
        HEATFLAG=true;
        break; 
      case 'h':
        //收到加热模式结束指令 
        HEATFLAG=false;
        break; 
/*     
      //暂时不需要这两条指令了。手动上水的按钮与副板直接连接，不需要通过软件来控制。但副板确实有必要向主板发送手动上水被启动和被关闭的信号。
      case 'M':
        //收到手动上水开始指令
        MANUALWATERFLAG=true;
        break; 
      case 'm':
        //收到手动上水结束指令
        MANUALWATERFLAG=false;
        break;
*/
      case 'W':
        //收到获取当前去皮后的水重指令
        break;
      case 'w':
        //收到获取当前壶重指令
        break;
      case 't':
        //收到获取当前温度指令
        break;
      case 'Z':
        //收到主板清除上水故障的指令
        waterError = false;
        break;      
    }
  }else{
    switch(receiverCBuffer){
      case 'W':
      //接收烧水水重上限
        receiverBuffer = Serial.readStringUntil('@');//读取寄存器中的数据，直到遇到‘@’字符串后停止并返回终止字符'@'前的所有字符信息（这些信息被组成一个字符串），同时'@'字符串会被丢弃。
        addWaterLimHigh = receiverBuffer.toInt();
        EEPROM.put(8,addWaterLimHigh);        
        break;
      case 'C':
      //接收周期烧水间隔日期
        receiverBuffer = Serial.readStringUntil('@');
        cycleStartDay = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        cycleGapDay = receiverBuffer.toInt();
        EEPROM.put(0,cycleStartDay);
        EEPROM.put(2,cycleGapDay);
        break;
      case 'c':
      //接收周期烧水时刻（先小时后分钟）
        receiverBuffer = Serial.readStringUntil('@');
        cycleClockHour = receiverBuffer.toInt();
        EEPROM.put(4,cycleClockHour);
        receiverBuffer = Serial.readStringUntil('@');
        cycleClockMin = receiverBuffer.toInt();
        EEPROM.put(6,cycleClockMin);        
        break;
      case 'A':
      //接收自动续水壶中最低水量阈值
        receiverBuffer = Serial.readStringUntil('@');
        addWaterLimLow = receiverBuffer.toInt();
        EEPROM.put(10,bottleWeight);
        break;
      case 'E':
      //接收保温功能触发温度阈值
        receiverBuffer = Serial.readStringUntil('@');
        heatSaveTemp = receiverBuffer.toInt();
        EEPROM.put(12,heatSaveTemp);
        break;
      case 'H':
      //加热功能停止温度
        receiverBuffer = Serial.readStringUntil('@');
        heatTemp = receiverBuffer.toInt();
        EEPROM.put(14,heatTemp);
        break;
      case 'T':
      //当前时钟芯片所储存时间数据信号
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.year = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.month = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.day = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.hour = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.minute = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.second = receiverBuffer.toInt();
        receiverBuffer = Serial.readStringUntil('@');
        TimeBuffer.dow = receiverBuffer.toInt();
        break;
      default://如果读到了错误的数据，便将这一整个数据包丢弃
        while(Serial.available()>0){
          receiverCBuffer=Serial.peek();//从串口输入寄存器中读取下一位数据，但与read()不同，它不将下一位数据从寄存器中删除。
          if(receiverCBuffer=='#') break;
          if(receiverCBuffer=='@') {
            Serial.read();
            break;
          }
          Serial.read();
        }
        break;
    }
  }  
/*
  while(!Serial);
  char a;
  if(Serial.available()>0){
    for(int i=0;i<Serial.available();i++){
      a = Serial.read();
      switch(a){
        case 'A'://开始烧水
          digitalWrite(pinBoil,HIGH);
          break;
        case 'B'://停止烧水
          digitalWrite(pinBoil,LOW);        
          break;
        case 'C'://开始加水
          digitalWrite(pinAddWater,HIGH);
          break;
        case 'D'://停止加水
          digitalWrite(pinAddWater,LOW);
          break;
        case 'E'://开启风扇
          digitalWrite(pinFan,HIGH);
          break;
        case 'F'://停止风扇
          digitalWrite(pinFan,LOW);
          break;
        case 'G'://温度反馈
          currentTemp = mlx90614.readObjectTempC();
          Serial.print(String('t'+currentTemp));//Arduino的串口通讯并不能直接发送float数据。只能将其转为String发送，然后在接收端使用Serial.parseFloat()方法读取。但parseFlaot方法需要接受一个非Flaot字符来终止它对接收缓冲区的读取。这里用字符‘t’来分隔上一个发送的浮点数与该被发送的浮点数，顺便为主板收听函数提供判断数据应该储存在哪个变量里的依据。
          Serial.flush();//等待信息从发送缓冲区全部发送完毕
          break;
        case 'H'://重量反馈
          currentWaterWeight = hx711.get_Weight();
          Serial.print('w'+String(currentWaterWeight));
          Serial.flush();
          break;
      }
    }  
  }
  delay(100);
  */  
}

void PushBtnsButAssiatantBoard::AssisBoardDataSender(String signal){
  //while(!Serial);
  if(signal[0] == '#'){
    Serial.print(signal);
    Serial.flush();    
  }else{
    switch(signal[0]){
      case 'W':
      //发送当前去皮水重信号
        currentWaterWeight = hx711.get_Weight();
        signal+=String(currentWaterWeight)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 'w':
      //发送壶重数据信号
        bottleWeight = hx711.get_Maopi();
        EEPROM.put(0,bottleWeight);
        signal+=String(bottleWeight)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 't':
      //发送当前温度信号
        currentTemp = mlx90614.readObjectTempC();
        signal+=String(currentTemp)+'@';
        Serial.print(signal);
        Serial.flush();      
        break;
    }
  }
}

void PushBtnsButAssiatantBoard::boilStateController(){
  /*
  SINGLEBOILFLAG,CYCLEBOILFLAG,AUTOBOILFLAG,HEATSAVEFLAG,HEATFLAG,MANUALWATERFLAG;
  currentTemp,currentWaterWeight;
  bottleWeight,addWaterLimHigh,addWaterLimLow,cycleGapDay,cycleClockHour,cycleClockMin,heatSaveTemp,heatTemp;
  TimeBuffer;
  lastRecordMillis1,lastRecordMillis2;
  */

  if(digitalRead(btnManualAddWater)==LOW){//手动上水功能。这个功能并非来自软件控制,所以不需要接收主板信号；相反，它需要向主板发送开始和结束信号。自动上水由一推杆按钮控制。这个模式会直接打开水泵，它不受安全上水函数addWater的控制。
    delay(100);//去抖动，并且防止误触
    if(MANUALWATERFLAG!=true){//可防止重复向主板发信号
      MANUALWATERFLAG=true;
      digitalWrite(pinAddWater,HIGH);
      AssisBoardDataSender("#M");                  
    }
  }else{
    if(MANUALWATERFLAG!=false){
      MANUALWATERFLAG=false;
      digitalWrite(pinAddWater,LOW);
      AssisBoardDataSender("#m");      
    }
  }
  

  if(millis()-lastRecordMillis1>1000){//每秒获取一次实时重量和温度数据。但是将重置计时器lastRecordMillis1的任务交给了下一个函数。
    currentTemp = mlx90614.readObjectTempC();
    currentWaterWeight = hx711.get_Weight();
  }
  if(millis()-lastRecordMillis1>1000 && ifKettleOn() && !waterError){//一秒检测一次各个功能的运行情况,但这需判断是否有水壶置于发热盘底座上，以及是否存在上水错误。
    lastRecordMillis1=millis();    
    
    if(SINGLEBOILFLAG){
      if(!safeHeater()){
        AssisBoardDataSender("#S");
        SINGLEBOILFLAG=false;
      }  
    }

    if(CYCLEBOILFLAG){
      if((TimeBuffer.dow-cycleStartDay)%(cycleGapDay+1) == 0){
        if(TimeBuffer.hour==cycleClockHour && TimeBuffer.minute==cycleClockMin){
          CYCLEBOILONETIMESTARTED=true;
        }          
      }
      if(CYCLEBOILONETIMESTARTED){
        if(CYCLEBOILONETIME){
          if(currentTemp>95){
            AssisBoardDataSender("#c");
            CYCLEBOILONETIME = false;//单次周期烧水标志关闭
            CYCLEBOILONETIMESTARTED=false;
          }
        }else{
          if(safeHeater()){
            CYCLEBOILONETIME = true;//单次周期烧水标志打开
          }else{
            //AssisBoardDataSender("#c");//如果当前烧水失败（即已有足量开水或者出现了上水错误），则返回烧水结束信号。但在定时到点的那一分钟内，它不会返回烧水结束信号，而是会不断尝试，直到safeHeater中的addWater锁定系统或成功烧水。
            CYCLEBOILONETIME = false;
            CYCLEBOILONETIMESTARTED=false;
          }              
        }          
      }
    }
    if(HEATFLAG){//软件加热功能。这个功能不会自动加水，只会加温并且对风扇进行控制。
      if(currentTemp<heatTemp){
        digitalWrite(pinBoil,HIGH);
      }else{
        AssisBoardDataSender("#H");
        HEATFLAG=false;
      }        
    }

    if(HEATSAVEFLAG){
      if(currentTemp<heatTemp){
        digitalWrite(pinBoil,HIGH);
      }else{
        digitalWrite(pinBoil,LOW);
      }         
    }
    
  }
  if(millis()-lastRecordMillis2>30000){//每三十秒调用一次。用于更新那些不需要每秒更新的数据，以及AUTOBOIL模式的运行。
    lastRecordMillis2=millis();
    AssisBoardDataSender("#T");//获取DS1302所记录的时间

    if(ifKettleOn() && !waterError){//AUTOBOIL模式由于需要不停地监视壶落在平面上的情况并立刻烧水，所以为了防止某些特殊情况造成的错误上水，它被放在这里，30秒进行一次监控。
      if(AUTOBOILFLAG)
      {
        if(AUTOBOILONETIME){
          if(currentTemp>95){
            AssisBoardDataSender("#a");
            AUTOBOILONETIME = false;//单次周期烧水标志关闭
          }
        }else{
          delay(3000);//有了一开始的ifKettleOn检测和该delay函数后safeHeater中的ifKettleOn检测，就可以很好地防止将水壶放至发热盘表面又迅速拿起后可能仍会自动上水的问题。只要在放下三秒内将壶拿起，就不会触发这样的意外。但这也使得这个模式不适合放在一秒检测一次的区域内。
          if(safeHeater()){//当水壶置于盘上时开始烧水
            AUTOBOILONETIME=true;
          }else{
            AUTOBOILONETIME = false;
          }
        }
      }      
    }
        
  }
  if(millis()-lastRecordMillis2>300000){//每五分钟调用一次。
    failAddWaterTimes=0;//清空一下加水失败的次数
  }
  if(!ifKettleOn()){//提壶保险模块。防止的是在一些模式开始加热之后将壶提走
    digitalWrite(pinBoil,LOW);
    if(currentTemp>95){
      digitalWrite(pinFan,HIGH);
    }else if(currentTemp>60){
      digitalWrite(pinFan,LOW);
    }
  }
}

bool PushBtnsButAssiatantBoard::ifKettleOn(){
  if(hx711.HX711_Read()/hx711.gapValue<fixBottomVal+5){//hx711.HX711_Read()/gapValue得到的是当前测重平面上方所有物体的总重量。如果当前这个测量平面上的重量仅约等于发热盘底座的重量，则可判断上面没有水壶。5是一个修正数,单位为g。
    return false;
  }else{
    return true;
  }
}

bool PushBtnsButAssiatantBoard::safeHeater(){
  //delay(delaiMillis);//当该值为0时，根据AVR芯片的DELAY（）函数源码，可知虽然delay会被调用，但是会直接跳到函数结束，不会进入while计时函数体，所以delay(0)基本上相当于什么也没干。-*弃置的函数以及形参。这样做会严重阻塞程序运行，故而废弃。
  if(waterError==true){
    return false;//如果出现上水故障，则底层的加热程序是不会运作的
  }
  if(ifKettleOn()==false){
    return false;//如果壶不在加热盘上，则这个程序也不会运作
  }
  if(currentTemp>95 && currentWaterWeight>addWaterLimLow){//保护性检查。避免在壶中有水且水烧开的情况下还有模式要求烧水。
    digitalWrite(pinBoil,LOW);
    digitalWrite(pinFan,HIGH);
    return false;
  }else if(currentTemp>95){//如果壶的温度很高但是里面没水，这时候便可以加水并烧水了。
    digitalWrite(pinFan,HIGH);
    addWater();
    digitalWrite(pinBoil,HIGH);      
    return true;
  }else if(currentTemp<95 && currentTemp >= 60){
    addWater();
    digitalWrite(pinBoil,HIGH);      
    return true;
  }else if(currentTemp<60){//如果温度降至设定温度下则关闭风扇。
    digitalWrite(pinFan,LOW);
    addWater();
    digitalWrite(pinBoil,HIGH);
    return true;
  }else{
    return false;
  }
}

bool PushBtnsButAssiatantBoard::addWater(){
  if(waterError==true){
    return false;//如果出现上水故障，则底层的上水程序也是不会运作的
  }
  if(ifKettleOn()==false){
    return false;//如果壶不在加热盘上，则这个程序也不会运作
  }
  while(currentWaterWeight<addWaterLimLow){
  lastWaterWeight=currentWaterWeight;
  digitalWrite(pinAddWater,HIGH);
  currentWaterWeight=hx711.get_Weight();
  delay(1000);
    if(currentWaterWeight-lastWaterWeight<50){//如果一秒后现水重比之前水重增加不超过50g（这个阈值可以后续修改），则可判断水已外溢，即停止加水，并使程序进入5秒钟的阻塞。
      ++failAddWaterTimes;//增加一次加水失败的次数
      digitalWrite(pinAddWater,LOW);
      if(failAddWaterTimes==3){//如果失败次数达到三次，则向主板发送上水故障信号
        AssisBoardDataSender("#Z");
        waterError = true;
        failAddWaterTimes=0;
      }
      delay(5000);//程序进入5秒阻塞
      return false;  //使用return代替break来结束while循环          
    }

  } 
  return true;//当正常执行完加水流程后返回true  
}
////////////////////////////////////////////

////////////////////////////////////////////

PushBtnsButAssiatantBoard *pbt ;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  pbt = new PushBtnsButAssiatantBoard(4,5,6,7,8);
  
/*
  while(!mlx.begin(0x5A)) {
    Serial.print("Error connecting to MLX sensor. Check wiring.");
  };
  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
  */
}

void loop() {
  pbt->boilStateController();  
/*
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



void serialEvent(){
  //这个Arudino内置的函数会在每次loop函数后被调用（如果有新数据从RX脚写入）。在此暂时先搁置。
  pbt->mainBoardCommandReciver();
  
}

