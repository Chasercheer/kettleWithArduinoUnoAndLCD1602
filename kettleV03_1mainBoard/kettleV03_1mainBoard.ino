#include <Ds1302.h>//时钟芯片DS1302
#include <EEPROM.h>//用以实现向Arduino Uno R3的EEPROM中写入设置数据。这些设置数据不会在Arduino断电后消失，进而可以保证在每次开机时保持之前的设置。UnoR3有1KBytes的EPPROM的容量。一个Bytes是8个bit。这EEPROM大概有100000次的刷写寿命。
#include <LiquidCrystal.h>
//#include <ArxContainer.h>
//使用HardwareSerial类中的Serial系列方式，在UNO的TXRX口上进行UART（异步串行通讯接口，一般称为“串口”）通讯。主板与副班之间的Serial通讯代码的实现与一个板子同计算机的通讯类似，皆使用Serial的系列方法。
//主板与副板之间的通讯一共有四个函数：1，来自主板的开始/停止烧水指令；2，来自主板的开始/停止加水指令；3,来自主板的开始/停止风扇指令；4,来自主板的获取温度反馈指令；5，来自主板的获取重量反馈指令；6，来自副板的重量反馈以及来自副板的温度反馈。来自主板的指令信号使用单个char字符表示；来自副板的每一条反馈数据均使用一个特定char‘@’字符结尾，前跟一个要传输的float型数据。

//未完成：验证主板与副板的指令传递与反馈数据是否正确

////////////////////////////////////////////////////////////////////
class AllMenus {
public:

  enum { sizeOfArray = 10 };
  //static const int sizeOfArray = 10;//同样可行
  //int eachMenuRowsPointedIndex[3][sizeOfArray];//没必要了
  String menusAndTheirsContent;                              //将所有菜单的内容放进一个String中以节省运行时的内存空间。菜单与菜单之间用菜单索引数字“1，2，3······”隔开，行与行之间用“ ”空格隔开
//--------------------------------------------------------------------
  String pickALineInMenus(int menuIndex, int menuRowIndex);  //在menusAndTheirsContent选中一个菜单下的某一行并返回该行的值
  void ChangeALineInMenus(int menuIndex,int menuRowIndex,String changeLineContent);  
};

String AllMenus::pickALineInMenus(int menuIndex,int menuRowIndex) {
  String findstr;
  int rowsNum = -1;
  //char cmenuRowIndex = 48+menuRowIndex;
  //char cmenuIndex =48 + menuIndex;//48是字符'0'的十进制ASCII码（ASCII码中所有符号对应的值与UTF-8中对应得二进制值是相同的。UTF-8的映射包含了ASCII码的映射）---*弃置的目录索引
  String cmenuIndex = '*'+String(menuIndex);  //之前的菜单索引的缺陷在于，它是纯数字的，这导致如果当后续修改原菜单内容时向其中增加一些非索引的内容性的数字的时候，将可能引发菜单索引查找的错误。新的菜单目录索引号用“*+数字”的组合，从而避免了这个问题
  String nextCmenuIndex = '*'+String(menuIndex+1);
  if(menusAndTheirsContent.indexOf(cmenuIndex)==-1) return "1";
  if(menusAndTheirsContent.indexOf(nextCmenuIndex)==-1){
    findstr = menusAndTheirsContent.substring(menusAndTheirsContent.indexOf(cmenuIndex));
  }else{
    findstr = menusAndTheirsContent.substring(menusAndTheirsContent.indexOf(cmenuIndex),menusAndTheirsContent.indexOf(nextCmenuIndex));
  }
  for(int i = 0;i<findstr.length();i++){//string.length()返回string这个字符串中字符的个数。在给string赋值时的C风格字符串结尾隐式包含的\0（空字符）不会被length()计数。
    if(findstr[i]==' ') ++rowsNum;   
  }
  if(rowsNum-1<menuRowIndex) return "2";//对传入的超过现菜单最大行索引数的行索引参数，在这一步便予以处理
  // int String::indexOf(char Index),返回字符串中从前往后查找到的第一个Index的位置；String String::substring(int from,int to=String.lenth()+1)，返回一个字符串中从位置from至to的字串。一个String的第一个字符的位置是0.如果没有找到，则返回-1。
  for(int i = 0 ;i<menuRowIndex+1;i++){
    findstr.remove(0,findstr.indexOf(' ')+1);//void String::remove(int from,int to)，移除原字符串中从from至to（不包括to）位置之间的字符
  }
  findstr.remove(findstr.indexOf(' '));//void String::remove(int from,int to=String.lenth()+1),移除从from至字符串结尾之间的字符
  return findstr;
}

void AllMenus::ChangeALineInMenus(int menuIndex,int menuRowIndex,String changeLineContent){
  //String cmenuIndex = '*'+String(menuIndex);
  //if(menusAndTheirsContent.indexOf(cmenuIndex)==-1) return ;
  //void函数在C++中也是可以使用return的。这里使用return的目的是让函数如果运行到此句则退出该函数。void是一个类型，它的真正意义是：“大小不确定的类型”。既然是大小不确定的类型，那么编译器本着确定才能行动的原则，在面对这个类型时会束手无策，什么也不干。这就体现为void返回值的函数什么也不返回。
  menusAndTheirsContent.replace(pickALineInMenus(menuIndex,menuRowIndex),changeLineContent);  //string.replace(str1,str2)可将string中的子字符串str1替换为str2的内容。但这样做会将string中的所有str1替换为str2。但在这个项目中，这个行为并不会造成什么影响。
}
////////////////////////////////////////////////////////////////////
class LCD1602{
  public:
    AllMenus m;
    LiquidCrystal lcd;
    int menuIndexOfCurrentScreen;
    int firstLineNumOfCurrentScreen;
    LCD1602(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
    LCD1602(int pinDB4=2,int pinDB5=3,int pinDB6=4,int pinDB7=5,int pinE=6,int pinRS=7,int pinRW=0);
    void showOnLCD(String messageLine1,String messageLine2);
    bool showMenuContentOnLcd(int IndexOfTheMenu,int LineNumOfTheMenu); 
    void openLCD();
    void closeLCD();
    void menuScollUp();
    void menuScollDown();

};

LCD1602::LCD1602(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW):lcd(pinRS,pinRW,pinE,pinDB4,pinDB5,pinDB6,pinDB7){
  menuIndexOfCurrentScreen=0;
  firstLineNumOfCurrentScreen=0;
}

LCD1602::LCD1602(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW):lcd(pinRS,pinRW,pinE,pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7){
  menuIndexOfCurrentScreen=0;
  firstLineNumOfCurrentScreen=0;
}

void LCD1602::showOnLCD(String messageLine1,String messageLine2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(messageLine1);
  lcd.setCursor(0,1);
  lcd.print(messageLine2);
}

bool LCD1602::showMenuContentOnLcd(int IndexOfTheMenu,int LineNumOfTheMenu){
  if(IndexOfTheMenu<m.sizeOfArray&&LineNumOfTheMenu<9){
    menuIndexOfCurrentScreen=IndexOfTheMenu;
    firstLineNumOfCurrentScreen=LineNumOfTheMenu;    
    showOnLCD(m.pickALineInMenus(IndexOfTheMenu,LineNumOfTheMenu),m.pickALineInMenus(IndexOfTheMenu,LineNumOfTheMenu+1));
    return true;
  }else if(IndexOfTheMenu<m.sizeOfArray&&LineNumOfTheMenu==9){
    showOnLCD(m.pickALineInMenus(IndexOfTheMenu,LineNumOfTheMenu),"   ");
    return true;
  }else{
    return false;
  }
}

void LCD1602::openLCD(){
  lcd.display();
}
void LCD1602::closeLCD(){
  lcd.noDisplay();
}
void LCD1602::menuScollUp(){
  if(firstLineNumOfCurrentScreen<=0){
      //firstLineNumOfCurrentScreen=m.getAMenuByIndex(menuIndexOfCurrentScreen).size()-1;
      firstLineNumOfCurrentScreen=m.sizeOfArray-1;
      //lcd.clear();不必要的clear()，因为showMenuContentOnLcd调用showOnLcd，而showOnLcd方法中有clear()
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }else{
      firstLineNumOfCurrentScreen--;
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }
}
void LCD1602::menuScollDown(){
  if(firstLineNumOfCurrentScreen>=m.sizeOfArray-1){
      firstLineNumOfCurrentScreen=0;
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }else{
      firstLineNumOfCurrentScreen++;
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }  
}

////////////////////////////////////////////////////////////////////

class PushBtns{
  public:
  
  LCD1602 lcd1602;
  Ds1302::DateTime getTimeBuffer,setTimeBuffer;//一个用来储存要设置的时间；一个用来储存从DS1302中读到的时间。值得注意的是，这个结构体内的数据成员的类型为uint_8，意为八位无符号整数，即这些数据成员的取值范围为0-255。用int为其赋值时可能会溢出。
  Ds1302 ds1302;
  int bottleWeight,cycleGapDay,cycleClockHour,cycleClockMin,addWaterLimHigh,addWaterLimLow,heatSaveTemp,heatTemp;//这些是要写入EEPROM的数据。它们在开机时从EEPROM读取数据，每当修改时将修改上传至EEPROM。另注：在ARduino中byte数据的储存只需要1btye，而int和short一样需要2byte。但使用byte数据似乎需要再加载一个Arduino自己的库，所以如果是少量的数据使用Byte反而比int占空间。int可以赋值给byte，但值可能会溢出并且造成错误。
  int waterWeight,currentTemp;//这两个变量储存了工作时的实时水重与温度
  bool SINGLEBOILFLAG,CYCLEBOILFLAG,AUTOBOILFLAG,HEATSAVEFLAG,HEATFLAG,MANUALWATERFLAG;//设置LCD菜单行上显示相对应功能开关状态的FLAG
  float currentWaterWeight,currentWaterTemp;  
  //bool stateHeat,stateAddWater,stateFan;//这三个是具体表示当前烧水壶实际工作状态的变量
  int up,down,quit,select,onAndOff,onAndOffFlag,backLight;//储存各按钮所对应的开发板的引脚号的变量。其中backLinght是控制LCD1602的背光亮度的。它可接在LCD1602的背光正极引脚上，也可接在背光负极引脚上。原理是，当背光正极与负极之间的电压接近O时，背光亮度也接近于无；当正极到负极的电压为5V时，亮度最大。
  enum {triggeredLevelOfUp=LOW,triggeredLevelOfDown=LOW,triggeredLevelOfQuit=LOW,triggeredLevelOfSelect=LOW,triggeredLevelOfOnAndOff=LOW};
  
  
////////////////////data↑///////////////////////

////////////////////func↓///////////////////////
 
//--------------------------构造函数------------------------------
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);  
//-------------------------------------------------------------------

//--------------------------按钮事件------------------------------
  void upEvents();
  void downEvents();
  void selectEvents();
  void quitEvents();
  void onAndOffEvents();
//-------------------------------------------------------------------


//--------------------------按钮触发事件------------------------------
  void setTime();   
  void setTime(int year,int month,int day,int hour,int min,int sec,int dow);
  void getTime();
  void startOrStopBoilWater(bool flag);
  void startOrStopAddWater(bool flag);
  void startOrStopFan(bool flag);
  int numberChooseFunc(int start,int step,int lowerlim,int upperlim,int quitCode=-1);//数值选择事件，调用则出现一个临时菜单以供选择数值，返回值为其选择的数值。临时菜单在选择结束后会自动关闭且恢复之前的菜单页面。他接受一个初始值，一个步长，一个选择值上限值，一个选择值下限，一个未选择便退出时的标识值作为参数。    
  float callTempFeedback();
  float callWeightFeedback();
  void mainBoardCommandSender(String signal);//需要时调用，signal用来指出发送什么数据。
//-------------------------------------------------------------------

//--------------------------其它函数----------------------
  void AssisBoardDataReceiver();//放在serialEvent()函数中，时刻监听副板信号，并识别副板信号并将数据储存在对应变量中。
//-------------------------------------------------------------------


};

PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW):lcd1602(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW),ds1302(pinClkEn,pinClkClk,pinClkDat){
  //lcd1602.lcd.begin(16, 2);
  ds1302.init();//启用ds1302对象。否则开发板不会通过这个对象与时钟芯片进行通讯。
  getTime();
  setTimeBuffer.year=getTimeBuffer.year;
  setTimeBuffer.month=getTimeBuffer.month;
  setTimeBuffer.day=getTimeBuffer.day;
  setTimeBuffer.hour=getTimeBuffer.hour;
  setTimeBuffer.minute=getTimeBuffer.minute;
  setTimeBuffer.second=getTimeBuffer.second;
  setTimeBuffer.dow=getTimeBuffer.dow;  
  ////////////
  //waterWeight=EEPROM.read(0);//读取处于EEPROM中位置为0的数据。EEPROM中的位置是EEPROM中的地址的简化：一个位置代表这个内存中一个byte大小的内存区域，即相邻的8个bit所构成的一个储存区域。read可以读取该位置储存的一个字节；write则可以在该位置写入一个字节。但int值在Arduino中占两个字节，则如果将一个大于255的int或float值存入一个Btye位中将导致Arduino程序崩溃。
  //但是使用get()则可以在指定的位置作为起始，获得与实参2相同类型的的数据，这实参2的类型不作限制；并将数据存入传递的实参2内。同理，使用put(pos,val)函数可以将任意类型的val存入以int值pos为开始位置的相应大小的储存区域中，val不一定只用一个1个储存位置储存，可能是以pos为起始的，以及其后的多个位置作为其储存区域。另外，当一个位置未被储存数据时，其默认十进制值为255。
  EEPROM.get(0,bottleWeight);//int占两个byte，所以会占用两个位置进行储存。所以，不能将上一个储存位置值加1来储存这个int值，而是需要加2。可用sizeof()判断其类型大小。
  EEPROM.get(2,cycleGapDay);
  EEPROM.get(4,cycleClockHour);
  EEPROM.get(6,cycleClockMin);  
  EEPROM.get(8,addWaterLimHigh);
  EEPROM.get(10,addWaterLimLow);  
  EEPROM.get(12,heatSaveTemp);
  EEPROM.get(14,heatTemp);
  SINGLEBOILFLAG=false;
  CYCLEBOILFLAG=false;
  AUTOBOILFLAG=false;
  HEATSAVEFLAG=false;
  HEATFLAG=false;
  MANUALWATERFLAG=false;
  this->up=up;//使用this指针可以在方法中使用来自对象的与类形参变量同名的数据成员
  this->down=down;
  this->quit=quit;
  this->select=select;
  this->onAndOff=onAndOff;
  onAndOffFlag = 1;//值为1表示当前该Lcd的状态为开，值为0表示当前该Lcd的状态为关
  this->backLight = backLight;
  pinMode(up,INPUT_PULLUP);//设置up按钮的引脚，并将引脚模式设置为读取引脚电平且使用内部上拉电阻，使用内部上拉电阻将使得引脚在与地断开时电平为高，接地时电平为低
  pinMode(down,INPUT_PULLUP);
  pinMode(quit,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(onAndOff,INPUT_PULLUP);
  pinMode(backLight,OUTPUT);
  digitalWrite(backLight,LOW);
  
}

PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0):lcd1602(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW),ds1302(pinClkEn,pinClkClk,pinClkDat){
  //lcd1602.lcd.begin(16, 2);
  ds1302.init();
  getTime();
  setTimeBuffer.year=getTimeBuffer.year;
  setTimeBuffer.month=getTimeBuffer.month;
  setTimeBuffer.day=getTimeBuffer.day;
  setTimeBuffer.hour=getTimeBuffer.hour;
  setTimeBuffer.minute=getTimeBuffer.minute;
  setTimeBuffer.second=getTimeBuffer.second;
  setTimeBuffer.dow=getTimeBuffer.dow; 
  ///////////////////////
  EEPROM.get(0,bottleWeight);
  EEPROM.get(2,cycleGapDay);
  EEPROM.get(4,cycleClockHour);
  EEPROM.get(6,cycleClockMin);  
  EEPROM.get(8,addWaterLimHigh);
  EEPROM.get(10,addWaterLimLow);  
  EEPROM.get(12,heatSaveTemp);
  EEPROM.get(14,heatTemp);
  SINGLEBOILFLAG=false;
  CYCLEBOILFLAG=false;
  AUTOBOILFLAG=false;
  HEATSAVEFLAG=false;
  HEATFLAG=false;
  MANUALWATERFLAG=false;
  this->up=up;
  this->down=down;
  this->quit=quit;
  this->select=select;
  this->onAndOff=onAndOff;
  onAndOffFlag = 1;
  this->backLight = backLight;
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
  pinMode(quit,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(onAndOff,INPUT_PULLUP);
  pinMode(backLight,OUTPUT);
  digitalWrite(backLight,LOW);
}
void PushBtns::upEvents(){
  if(digitalRead(up)==triggeredLevelOfUp){
    delay(100);//去抖动
  }
  if(digitalRead(up)==triggeredLevelOfUp){
    lcd1602.menuScollUp();
    delay(300);//避免在LOOP函数中因刷新过快出现一些问题,另外，它还控制着当按住按钮不放时菜单栏滚动的速率
    //Serial.print("func");//测试
  }
}
void PushBtns::downEvents(){
  if(digitalRead(down)==triggeredLevelOfDown){
    delay(100);//去抖动
  }
  if(digitalRead(down)==triggeredLevelOfDown){
    lcd1602.menuScollDown();
    delay(300);
  }
  
}
void PushBtns::selectEvents(){
  //Serial.print(digitalRead(select));
  String menuBuffer;
  if(digitalRead(select)==triggeredLevelOfSelect){
    delay(100);//去抖动
  }  
  if(digitalRead(select)==triggeredLevelOfSelect){
    //Serial.print(11111);
    if(lcd1602.menuIndexOfCurrentScreen==0){
      switch(lcd1602.firstLineNumOfCurrentScreen){
        case 0:
        //SHUTDOWN
          SINGLEBOILFLAG=false;
          CYCLEBOILFLAG=false;
          AUTOBOILFLAG=false;
          HEATSAVEFLAG=false;
          HEATFLAG=false;
          lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
          lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
          lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
          lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
          lcd1602.m.ChangeALineInMenus(0,5,"HEAT:off");
          lcd1602.showOnLCD("ALL MODE","SHUTED DOWN");
          delay(2000);
          lcd1602.showMenuContentOnLcd(0, 0);

          //Serial.print('s');                  
          break;          
        case 1:
        //SINGLEBOIL
          if(SINGLEBOILFLAG){
            /*
            lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
            lcd1602.showMenuContentOnLcd(0, 1); 
            SINGLEBOILFLAG=false;
            */
            mainBoardCommandSender("#s");
          }else{
            lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:on");
            lcd1602.showMenuContentOnLcd(0, 1); 
            SINGLEBOILFLAG=true;
            mainBoardCommandSender("#S");          
          }
          
          break;
        case 2:
        //CYCLEBOIL
          if(CYCLEBOILFLAG){
          /*
            lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
            lcd1602.showMenuContentOnLcd(0, 2); 
            CYCLEBOILFLAG=false;
          */
            mainBoardCommandSender("#c");
          }else{
            lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:on");
            lcd1602.showMenuContentOnLcd(0, 2); 
            CYCLEBOILFLAG=true;
            mainBoardCommandSender("#C");          
          }
          
          break;
        case 3:
        //ATUOBOIL
          if(AUTOBOILFLAG){
            /*
            lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
            lcd1602.showMenuContentOnLcd(0, 3); 
            AUTOBOILFLAG=false;
            */
            mainBoardCommandSender("#a");
          }else{
            lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:on");
            lcd1602.showMenuContentOnLcd(0, 3); 
            AUTOBOILFLAG=true; 
            mainBoardCommandSender("#A");         
          }
          break;
        case 4:
        //HEATSAVE
          if(HEATSAVEFLAG){
            /*
            lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
            lcd1602.showMenuContentOnLcd(0, 4); 
            HEATSAVEFLAG=false;
            */
            mainBoardCommandSender("#e");
          }else{
            lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:on");
            lcd1602.showMenuContentOnLcd(0, 4); 
            HEATSAVEFLAG=true;  
            mainBoardCommandSender("#E");        
          }
          break;
        case 5:
        //HEAT，此为软件加热模式，可与上模式并行，另有可控温度的功能
          if(HEATFLAG){
            /*
            lcd1602.m.ChangeALineInMenus(0,5,"HEAT:off");
            lcd1602.showMenuContentOnLcd(0, 5);            
            HEATFLAG=false;
            */
            mainBoardCommandSender("#h");            
          }else{
            lcd1602.m.ChangeALineInMenus(0,5,"HEAT:on");
            lcd1602.showMenuContentOnLcd(0, 5);             
            HEATFLAG=true;
            mainBoardCommandSender("#H");          
          }
          break;
        case 6:
        //MANUALWATER
          if(MANUALWATERFLAG){
            /*
            lcd1602.m.ChangeALineInMenus(0,6,"MANUALWATER:off");
            lcd1602.showMenuContentOnLcd(0, 6); 
            MANUALWATERFLAG=false;
            */
            mainBoardCommandSender("#m");
          }else{
            lcd1602.m.ChangeALineInMenus(0,6,"MANUALWATER:on");
            lcd1602.showMenuContentOnLcd(0, 6); 
            MANUALWATERFLAG=true;
            mainBoardCommandSender("#M");          
          }
          break;          
        case 7:
        //SET
          menuBuffer="ADDWLIMHIGH:"+String(addWaterLimHigh)+"g";
          lcd1602.m.ChangeALineInMenus(1,0,menuBuffer);

          menuBuffer="ADDWLIMLOW:"+String(addWaterLimLow)+"g";
          lcd1602.m.ChangeALineInMenus(1,1,menuBuffer);

          menuBuffer="CYCLEGAPDAY:"+String(cycleGapDay)+"DS";
          lcd1602.m.ChangeALineInMenus(1,2,menuBuffer);

          menuBuffer="CYCLECLOCK:"+String(cycleClockHour)+":"+String(cycleClockMin);
          lcd1602.m.ChangeALineInMenus(1,3,menuBuffer);

          menuBuffer="HEATSAVETEMP:"+String(heatSaveTemp)+"C";
          lcd1602.m.ChangeALineInMenus(1,4,menuBuffer);

          menuBuffer="HEATTEMP:"+String(heatTemp)+"C";
          lcd1602.m.ChangeALineInMenus(1,5,menuBuffer);

          menuBuffer="BOTTLEWEIGHT:"+String(bottleWeight)+"g";
          lcd1602.m.ChangeALineInMenus(1,6,menuBuffer);
          //事实证明，这一个代码块中，menuBuffer被连续赋值七次，但是当第四次结束后，UNO便陷入宕机之中，它不再运行剩下的三次赋值，而是执行其它函数去了。
          lcd1602.showMenuContentOnLcd(1, 0); 
          break;
        case 8:
        //SETSYSTEMTIME->进入时间选择页面
          getTime();
          setTimeBuffer.year=getTimeBuffer.year;
          setTimeBuffer.month=getTimeBuffer.month;
          setTimeBuffer.day=getTimeBuffer.day;
          setTimeBuffer.hour=getTimeBuffer.hour;
          setTimeBuffer.minute=getTimeBuffer.minute;
          setTimeBuffer.second=getTimeBuffer.second;
          setTimeBuffer.dow=getTimeBuffer.dow; 

          menuBuffer="SETYEAR:20"+String(setTimeBuffer.year);          
          lcd1602.m.ChangeALineInMenus(2,1,menuBuffer);

          menuBuffer="SETMONTH:"+String(setTimeBuffer.month);
          lcd1602.m.ChangeALineInMenus(2,2,menuBuffer);

          menuBuffer="SETDATE:"+String(setTimeBuffer.day);          
          lcd1602.m.ChangeALineInMenus(2,3,menuBuffer);

          menuBuffer="SETHOUR:"+String(setTimeBuffer.hour);
          lcd1602.m.ChangeALineInMenus(2,4,menuBuffer);

          menuBuffer="SETMINUTE:"+String(setTimeBuffer.minute);
          lcd1602.m.ChangeALineInMenus(2,5,menuBuffer);

          menuBuffer="SETSECOND:"+String(setTimeBuffer.second);
          lcd1602.m.ChangeALineInMenus(2,6,menuBuffer);

          menuBuffer="SETWEEKDAY:"+String(setTimeBuffer.dow);
          Serial.print(setTimeBuffer.dow);
          lcd1602.m.ChangeALineInMenus(2,7,menuBuffer);
          //以上代码可以在进入时间选择页面的时候动态地刷新一下该页面

          lcd1602.showMenuContentOnLcd(2, 0);
          break;
        default:
          lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }      
    }else if(lcd1602.menuIndexOfCurrentScreen==1){
      switch(lcd1602.firstLineNumOfCurrentScreen){
        case 0:
        //ADDWLIMHIGH
          addWaterLimHigh=numberChooseFunc(100,30,50,200); 
          menuBuffer =String("ADDWLIMHIGH:"+String(addWaterLimHigh)+"g");               
          lcd1602.m.ChangeALineInMenus(1,0,menuBuffer); //在这个长长的函数中，由于频繁构造String对象，导致UNO板来不及及时将menuBuffer更改至菜单完毕，便开始了下一个showMenuContentOnLcd函数，这样会导致LCD屏在该行出现错误。但是将菜单上下滚动后其便会恢复正常。产生这个问题可能是因为UNO硬件不够强大？
          lcd1602.showMenuContentOnLcd(1, 0);
          EEPROM.put(8,addWaterLimHigh);//将addWaterLimHigh写入EEPROM中 
          break;
                
        case 1:
        //ADDWLIMLOW
          addWaterLimLow=numberChooseFunc(300,20,100,500);
          menuBuffer = String("ADDWLIMLOW:"+String(addWaterLimLow)+"g"); 
          lcd1602.m.ChangeALineInMenus(1,1,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 1); 
          EEPROM.put(10,addWaterLimLow);
          break;       
            
        case 2:
        //CYCLEGAPDAY
          cycleGapDay=numberChooseFunc(0,1,0,30); 
          menuBuffer="CYCLEGAPDAY:"+String(cycleGapDay)+"DS";
          lcd1602.m.ChangeALineInMenus(1,2,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 2); 
          EEPROM.put(2,cycleGapDay);
          break;
        case 3:
        //CYCLECLOCK
          lcd1602.showOnLCD("PLEASE SET HOUR:"," ");
          delay(1500);
          cycleClockHour=numberChooseFunc(12,1,0,23);
          lcd1602.showOnLCD("HOUR SET DONE"," ");
          delay(1500);          
          lcd1602.showOnLCD("PLEASE SET MIN:"," ");
          delay(1500);            
          cycleClockMin=numberChooseFunc(30,1,0,59);
          lcd1602.showOnLCD("MIN SET DONE"," ");
          delay(1500);
          EEPROM.put(4,cycleClockHour);
          EEPROM.put(6,cycleClockMin);            
          menuBuffer="CYCLECLOCK:"+String(cycleClockHour)+":"+String(cycleClockMin);
          lcd1602.m.ChangeALineInMenus(1,3,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 3);          
          break;
        
        case 4:
        //HEATSAVETEMP
          heatSaveTemp=numberChooseFunc(60,1,40,80); 
          menuBuffer="HEATSAVETEMP:"+String(heatSaveTemp)+"C";
          lcd1602.m.ChangeALineInMenus(1,4,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 4); 
          EEPROM.put(12,heatSaveTemp);       
          break;
        
        case 5:
        //HEATTEMP
          heatTemp=numberChooseFunc(65,1,40,90); 
          menuBuffer="HEATTEMP:"+String(heatTemp)+"C";
          lcd1602.m.ChangeALineInMenus(1,5,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 5); 
          EEPROM.put(14,heatTemp);  
          break;
        
        case 6:
        //BOTTLEWEIGHT
          bottleWeight=numberChooseFunc(300,10,10,1000); 
          menuBuffer="BOTTLEWEIGHT:"+String(bottleWeight)+"g";
          lcd1602.m.ChangeALineInMenus(1,6,menuBuffer);
          lcd1602.showMenuContentOnLcd(1, 6); 
          EEPROM.put(0,bottleWeight);
          break;
        
        case 7:
        //WATERWEIGHTNOW
          menuBuffer=String(currentWaterWeight)+"g";        
          lcd1602.showOnLCD("WATERWEIGHTNOW:",menuBuffer);
          delay(1500);
          lcd1602.showMenuContentOnLcd(1, 7);
          break;           
        
        default:
          lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }  
    }else if(lcd1602.menuIndexOfCurrentScreen==2){
      switch(lcd1602.firstLineNumOfCurrentScreen){
        
        case 0:
        //CHECKTIME
          getTime();          
          //ds1302.getDateTime(&getTimeBuffer);
          //String line1=String(setTimeBuffer.year)+'-'+String(setTimeBuffer.month)+'-'+String(setTimeBuffer.day);
          //String line2=String(setTimeBuffer.hour)+'-'+String(setTimeBuffer.minute)+'-'+String(setTimeBuffer.second);
          //lcd1602.showOnLCD(line1,line2);     
          //也许是因为创建两个临时的String对象对于UNO来讲太过于勉强，抑或是Arduino尚未完善switch中非匿名临时对象的析构或构造，上述语句将使得后续的case无法被触发。但下面这句就可以正常运行     
          lcd1602.showOnLCD(String("20"+String(getTimeBuffer.year)+'-'+String(getTimeBuffer.month)+'-'+String(getTimeBuffer.day)),String(getTimeBuffer.hour)+'-'+String(getTimeBuffer.minute)+'-'+String(getTimeBuffer.second)+'-'+String(getTimeBuffer.dow));
          delay(2000);
          lcd1602.showMenuContentOnLcd(2, 0);                  
          break;
                 
        case 1:
        //SETYEAR
        //setTimeBuffer.year=-1;//由于year的类型为uint_8(用八个比特储存的无符号整数类型)，所以它的取值范围为0-255，这样做只会使其值为255。
          setTimeBuffer.year=numberChooseFunc(25,1,0,99,25);
          menuBuffer = "SETYEAR:20"+String(setTimeBuffer.year);
          lcd1602.m.ChangeALineInMenus(2,1,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 1);       
          break;
        case 2:
        //SETMONTH
          setTimeBuffer.month=numberChooseFunc(6,1,1,12,6);
          menuBuffer = "SETMONTH:"+String(setTimeBuffer.month);
          lcd1602.m.ChangeALineInMenus(2,2,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 2);       
          break;
        case 3:
        //SETDATE
          if(setTimeBuffer.month==1 || setTimeBuffer.month==3 ||setTimeBuffer.month==5||setTimeBuffer.month==7||setTimeBuffer.month==8||setTimeBuffer.month==10||setTimeBuffer.month==12){
            setTimeBuffer.day=numberChooseFunc(15,1,1,31,15);
          }else if (setTimeBuffer.month==2 && (setTimeBuffer.year+2000)%4==0){//由于将设定日期限制在了2000-2099年，所以计算闰年时不需要考虑它是否能被100整除（不是则是闰年，是则再除以400）以及能被100整除后是否能被400整除（若是则为闰年，若不能被400整除则不是闰年）了；在这个时间范围内能被4整除的都是闰年
            setTimeBuffer.day=numberChooseFunc(15,1,1,29,15);
          }else if (setTimeBuffer.month==2){
            setTimeBuffer.day=numberChooseFunc(15,1,1,28,15);
          }else{
            setTimeBuffer.day=numberChooseFunc(15,1,1,30,15);              
          }
          //以上结合闰年判断了当前选择的月份为最多为多少天
          menuBuffer = "SETDATE:"+String(setTimeBuffer.day);
          lcd1602.m.ChangeALineInMenus(2,3,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 3);
          break;
        case 4:
        //SETHOUR
          setTimeBuffer.hour=numberChooseFunc(12,1,0,23,12);
          menuBuffer = "SETHOUR:"+String(setTimeBuffer.hour);
          lcd1602.m.ChangeALineInMenus(2,4,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 4);
          break;
        case 5:
        //SETMINUTE
          setTimeBuffer.minute=numberChooseFunc(30,1,1,59,30);
          menuBuffer = "SETMINUTE:"+String(setTimeBuffer.minute);
          lcd1602.m.ChangeALineInMenus(2,5,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 5);
          break;
        case 6:
        //SETSECOND
          setTimeBuffer.second=numberChooseFunc(30,1,1,59,30);
          menuBuffer = "SETSECOND:"+String(setTimeBuffer.second);
          lcd1602.m.ChangeALineInMenus(2,6,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 6);
          break;
        case 7:
        //SETWEEKDAY
          setTimeBuffer.dow=numberChooseFunc(3,1,1,7,3);
          menuBuffer = "SETWEEKDAY:"+String(setTimeBuffer.dow);
          lcd1602.m.ChangeALineInMenus(2,7,menuBuffer);
          lcd1602.showMenuContentOnLcd(2, 7);
          break;
        case 8:
        //APPLYCHANGE     
          setTime();
          lcd1602.showOnLCD("CHANGES","APPLIED");
          delay(1000);
          lcd1602.showMenuContentOnLcd(2, 0);             
          break;
          
        default:
          lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }      
    }else{
      lcd1602.menuIndexOfCurrentScreen=0;
      lcd1602.firstLineNumOfCurrentScreen=0;
    }
    
  }
  delay(100);
}

void PushBtns::quitEvents(){
  if(digitalRead(quit)==triggeredLevelOfQuit){
    delay(100);
  }  
  if(digitalRead(quit)==triggeredLevelOfQuit){
    if(lcd1602.menuIndexOfCurrentScreen!=0)
      lcd1602.showMenuContentOnLcd(0, 0); //基于目前的菜单内容，这个简陋的返回函数是可用的     
  }
}

void PushBtns::onAndOffEvents(){
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff) delay(100);
  //if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==1) delay(100);//去按钮抖动
  //if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==0) delay(100);
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==1){
    //Serial.print(digitalRead(onAndOff));
    //Serial.print(onAndOff);    
    lcd1602.lcd.noDisplay();//noDisplay函数只能使LCD不再显示文字，但背光不会熄灭
    onAndOffFlag = 0;
    digitalWrite(backLight,HIGH);//所以用此熄灭背光
    delay(1000);
  }
  else if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==0){
    //Serial.print(digitalRead(onAndOff));
    //Serial.print(onAndOff);
    lcd1602.lcd.display();
    onAndOffFlag = 1;
    digitalWrite(backLight,LOW);
    delay(1000);
  }
}


void PushBtns::setTime(){
  ds1302.setDateTime(&setTimeBuffer);
}

void PushBtns::setTime(int year,int month,int day,int hour,int min,int sec,int dow){//dow=day of week，该值不能大于7（星期日的枚举值）
  setTimeBuffer.year=year;
  setTimeBuffer.month=month;
  setTimeBuffer.day=day;
  setTimeBuffer.hour=hour;
  setTimeBuffer.minute=min;
  setTimeBuffer.second=sec;
  setTimeBuffer.dow=dow;
  ds1302.setDateTime(&setTimeBuffer);
}

void PushBtns::getTime(){
  ds1302.getDateTime(&getTimeBuffer);  
}

void PushBtns::startOrStopBoilWater(bool flag){
  while(!Serial){//判断串口是否准备好通讯了。当串口未连接（如未使用SERIAL.BEGIN或使用SERIAL.END关闭串口后）时，Serial返回False

  }
  callTempFeedback();
  if(currentWaterTemp<100){
    if(flag){//开始烧水
    
    Serial.write('A');
    Serial.flush();
    }else{//停止烧水
    Serial.write('B');
    Serial.flush();
    }
  }
}
void PushBtns::startOrStopAddWater(bool flag){
  while(!Serial);//判断串口是否准备好通讯了。当串口未连接（如未使用SERIAL.BEGIN或使用SERIAL.END关闭串口后）时，Serial返回False
  callWeightFeedback();
  if(currentWaterWeight<addWaterLimHigh){
    if(flag){//开始加水
      Serial.write('C');
      Serial.flush();//程序运行至该函数，会进入等待知道所有要通过串口发送出去的数据被发送，才会从该函数返回并运行下一句。
    }else{//停止加水
      Serial.write('D');
      Serial.flush();
    }    
  }

/*
  while(Serial.available()>0){
    Serial.read();
  }
  delay(100);
  while(Serial.available()>0){
    char a = Serial.read();

  }
  while(Serial.available()>0){
    Serial.read();//再次清空输出
  }
*/
}

void PushBtns::startOrStopFan(bool flag){
  while(!Serial);
  callTempFeedback();
  if(currentWaterTemp>70){
    if(flag){//开启风扇
      Serial.write('E');
      Serial.flush();
    }else{//停止风扇
      Serial.write('F');
      Serial.flush();
    }
  }
}

float PushBtns::callTempFeedback(){
  while(!Serial);
  while(Serial.available()>0){
    Serial.read();//read会从串口输入寄存器里按顺序取出（即返回）一个输入进来的字符，并且将这个字符从串口输入寄存器里删除。这里做的目的是用其清空串口输入寄存器。
  }
  Serial.write('G');
  delay(50);//在主板送出指令后副板接收到指令；当副版成功获取指令时会向主板反馈一个信息，这个信息需要被主板及时读取。所以等待100ms，以使副板有足够的时间发送回馈信息。
  currentWaterTemp = Serial.parseFloat();
  while(Serial.available()>0){
    Serial.read();//再次清空串口输入寄存器
  }
  return currentWaterTemp;
}
float PushBtns::callWeightFeedback(){
  while(!Serial);  
  while(Serial.available()>0){
    Serial.read();
  }
  Serial.write('H');
  delay(50);
  currentWaterWeight = Serial.parseFloat();
  while(Serial.available()>0){
  Serial.read();
  }
  return currentWaterWeight;
}


int PushBtns::numberChooseFunc(int start,int step,int lowerlim,int upperlim,int quitCode){
  bool numChoosing=true,singleSelectFlag=true;//在同一行中定义声明多个变量并初始化
  int choosedNum=start,lastChoosedNum=start;
  lcd1602.showOnLCD("PLEASESELECTNUM:",String(choosedNum));
  while(numChoosing){
    if(choosedNum!=lastChoosedNum){
      lcd1602.showOnLCD("PLEASESELECTNUM:",String(choosedNum));
      lastChoosedNum=choosedNum;
    }
    //delay(100);
    
    if(digitalRead(up)==triggeredLevelOfUp && choosedNum<=upperlim) delay(200);
    if(digitalRead(up)==triggeredLevelOfUp && choosedNum<=upperlim){
      choosedNum+=step;      
    }else if(choosedNum>upperlim){
      lcd1602.showOnLCD("ERROR:","BEYOND LIMMIT");
      choosedNum=start;
      delay(2000);
    }
    
    if(digitalRead(down)==triggeredLevelOfDown && choosedNum>=lowerlim) delay(200);
    if(digitalRead(down)==triggeredLevelOfDown && choosedNum>=lowerlim){
      choosedNum-=step;      
    }else if(choosedNum<lowerlim){
      lcd1602.showOnLCD("ERROR:","UNDER LIMMIT");
      choosedNum=start;
      delay(2000);
    }
    
    if(digitalRead(select)==triggeredLevelOfSelect) { //去按键抖动
      delay(100);      
    }    
    if(digitalRead(select)==triggeredLevelOfSelect && singleSelectFlag) {//这一段可以确保进入数值选择页面时的SELECT按键不会紧接着触发确定数值选择从而秒退页面，也就是说必须再次按下SELECT这个页面才会选择数值并退出。但上面的数值的上下选择就不需要这么复杂的解决方案。
      delay(100);      
    }else if (digitalRead(select) != triggeredLevelOfSelect && singleSelectFlag){
      singleSelectFlag=false;      
    }else if(digitalRead(select)==triggeredLevelOfSelect){
      delay(100);
      while(digitalRead(select)==triggeredLevelOfSelect) delay(50);//这句与上句是防止选中数据后按下确定按钮，由于未能及时松开按钮而再次进入数值选择界面的情况发生。
      numChoosing=false;
    }

    if(digitalRead(quit)==triggeredLevelOfQuit){
      numChoosing=false;
      choosedNum=quitCode;
      delay(100);            
    }

  }
  return choosedNum;
}



void PushBtns::mainBoardCommandSender(String signal){
  while(!Serial);
  if(signal[1] == '#'){
    Serial.print(signal);
    Serial.flush();    
  }else{
    switch(signal[1]){
      case 'W':
        signal+=String(addWaterLimHigh)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 'C':
        signal+=String(cycleGapDay)+'@';
        Serial.print(signal);
        Serial.flush();      
        break;
      case 'c':
        signal+=String(cycleClockHour)+'@'+String(cycleClockMin)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 'A':
        signal+=String(addWaterLimLow)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 'E':
        signal+=String(heatSaveTemp)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
      case 'H':
        signal+=String(heatTemp)+'@';
        Serial.print(signal);
        Serial.flush();
        break;
    }
  }
}



void PushBtns::AssisBoardDataReceiver(){
  String receiverBuffer;
  char receiverCBuffer;
  while(!Serial);
  delay(50);//这是为确保在该函数被调用时主板能接收到副板的发送的（至少）一个的完整的数据包
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
        lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
        lcd1602.showMenuContentOnLcd(0, 1); 
        SINGLEBOILFLAG=false;
        break;
      case 'C':
        lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
        lcd1602.showMenuContentOnLcd(0, 2); 
        CYCLEBOILFLAG=false;
        break;
      case 'c':
        lcd1602.showOnLCD("CYCLE BOIL","DONE ONE TIME");
        delay(2000);
        lcd1602.showMenuContentOnLcd(lcd1602.menuIndexOfCurrentScreen,lcd1602.firstLineNumOfCurrentScreen);
        break;
      case 'A':
        lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
        lcd1602.showMenuContentOnLcd(0, 3); 
        AUTOBOILFLAG=false;
        break;
      case 'a':
        lcd1602.showOnLCD("AUTOADDWATERBOIL","DONE ONE TIME");
        delay(2000);
        lcd1602.showMenuContentOnLcd(lcd1602.menuIndexOfCurrentScreen,lcd1602.firstLineNumOfCurrentScreen);
        break;
      case 'E':
        lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
        lcd1602.showMenuContentOnLcd(0, 4); 
        HEATSAVEFLAG=false;
        break;
      case 'H':
        lcd1602.m.ChangeALineInMenus(0,5,"HEAT:off");
        lcd1602.showMenuContentOnLcd(0, 5);            
        HEATFLAG=false;
        break; 
      case 'M':
        lcd1602.m.ChangeALineInMenus(0,6,"MANUALWATER:off");
        lcd1602.showMenuContentOnLcd(0, 6); 
        MANUALWATERFLAG=false;
        break;     
    }
  }else{
    switch(receiverCBuffer){
      case 'W':
        receiverBuffer = Serial.readStringUntil('@');//读取寄存器中的数据，直到遇到‘@’字符串后停止并返回终止字符'@'前的所有字符信息（这些信息被组成一个字符串），同时'@'字符串会被丢弃。
        currentWaterWeight = receiverBuffer.toInt();
        break;
      case 'w':
        receiverBuffer = Serial.readStringUntil('@');
        bottleWeight = receiverBuffer.toInt();
        break;
      case 't':
        receiverBuffer = Serial.readStringUntil('@');
        currentTemp = receiverBuffer.toInt();
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
}
////////////////////////////////////////////////////////////////////


////////////////////++++++++++++++++++++++++++++++++/////////////////////////

PushBtns *pushBtns;
void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);
  pushBtns = new PushBtns(A0,A1,A2,A3,A4,8,5,4,3,2,6,7,11,9,10,0); 
  pushBtns->lcd1602.lcd.begin(16, 2);
  /*
  //这是一种用多行为String赋值的方式。一旦Arduino不能运用标准C++对于string的多行赋值格式，那么或许这样可行
  allmenus->menusAndTheirsContent = 
  String("1 SHUTDOWN SINGLEBOIL: CYCLEBOIL: AUTOBOIL: HEATSAVE: HEAT: SET SETSYSTEMTIME ") +
  String("2 SETWATERWEIGHT SETCYCLEGAPDAY SETCYCLECLOCK AUTOADDWLIM SETHEATSAVETEMP SETHEATTEMP SETBOTTLEWEIGHT WATERWIGHT") ;
  */
  
  //但很赞的是，Arduino的String类尽管与C++STL的string类有着诸多不同，但多行赋值的语法是相同的。HOORAY！！！
  pushBtns->lcd1602.m.menusAndTheirsContent =
  "*0 SHUTDOWN SINGLEBOIL:off CYCLEBOIL:off AUTOBOIL:off HEATSAVE:off HEAT:off MANUALWATER:off SET SETSYSTEMTIME \n"//原始菜单1内容
  "*1 ADDWLIMHIGH: ADDWLIMLOW: CYCLEGAPDAY: CYCLECLOCK: HEATSAVETEMP: HEATTEMP: BOTTLEWEIGHT: WATERWIGHTNOW \n"//原始菜单2内容
  "*2 CHECKTIME SETYEAR: SETMONTH: SETDATE: SETHOUR: SETMINUTE: SETSECOND: SETWEEKDAY: APPLYCHANGE ";  //原始菜单3内容。作为最后一行，结尾需要加空格。否则最后一个菜单的最后一行无法被加载。

  //pushBtns->lcd1602.m.ChangeALineInMenus(1,0,String("SETWATERW:"+String(pushBtns->waterWeight)+"g"));//由于设置项中的一些数据是储存在EEPROM中的，所以此时把它们读取并加载出来以还原上次关机时的菜单设置
  //pushBtns->lcd1602.m.ChangeALineInMenus(1,1,String("GAPDAY:"+String(pushBtns->cycleGapDay)+"DS"));
 
  //pushBtns->lcd1602.showOnLCD(pushBtns->lcd1602.m.pickALineInMenus(1,0),pushBtns->lcd1602.m.pickALineInMenus(1,1));
  
  pushBtns->lcd1602.showMenuContentOnLcd(0, 0);  

}

void loop(){
  // put your main code here, to run repeatedly:
  //pushBtns->lcd1602.showMenuContentOnLcd(int IndexOfTheMenu, int LineNumOfTheMenu);
  pushBtns->upEvents();
  pushBtns->downEvents();
  pushBtns->onAndOffEvents();
  pushBtns->selectEvents();
  pushBtns->quitEvents();
  //Serial.print(pushBtns->getTimeBuffer.second);
}


void serialEvent(){
  //这个Arudino内置的函数会在每次loop函数后被调用（如果有新数据从RX脚写入）。
  pushBtns->AssisBoardDataReceiver();

}
