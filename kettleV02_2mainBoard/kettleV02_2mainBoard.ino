#include <Ds1302.h>//时钟芯片DS1302
#include <EEPROM.h>//用以实现向Arduino Uno R3的EEPROM中写入设置数据。这些设置数据不会在Arduino断电后消失，进而可以保证在每次开机时保持之前的设置。UnoR3有1KBytes的EPPROM的容量。一个Bytes是8个bit。这EEPROM大概有100000次的刷写寿命。
#include <LiquidCrystal.h>
//#include <ArxContainer.h>


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
  char cmenuRowIndex = 48+menuRowIndex;
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
  char cmenuIndex = 48+menuIndex;
  char cmenuRowIndex = 48+menuRowIndex;
  if(menusAndTheirsContent.indexOf(cmenuIndex)==-1) return ;
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
    LCD1602();
    LCD1602(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
    LCD1602(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
    void showOnLCD(String messageLine1,String messageLine2);
    bool showMenuContentOnLcd(int IndexOfTheMenu,int LineNumOfTheMenu); 
    //void selectedEventsOfMenuContentOnLcd(int (*numberChooseFunc)()); //在按钮按下后调用该函数可触发当前屏幕上第一行所对应的事件。它接受一个无形参且返回int值的数值选择函数作为参数，供其内部调用--*弃置的方法
    //void flushMenuContentOnLcd();//动态更新当前屏幕内容，调用一次更新一次--*弃置的方法
    void openLCD();
    void closeLCD();
    void menuScollUp();
    void menuScollDown();
    //void menuRefresh();
};

LCD1602::LCD1602(int pinDB4=2,int pinDB5=3,int pinDB6=4,int pinDB7=5,int pinE=6,int pinRS=7,int pinRW=0):lcd(pinRS,pinRW,pinE,pinDB4,pinDB5,pinDB6,pinDB7){
  menuIndexOfCurrentScreen=0;
  firstLineNumOfCurrentScreen=0;
}

LCD1602::LCD1602(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd(pinRS,pinRW,pinE,pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7){
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
/*
void LCD1602::selectedEventsOfMenuContentOnLcd(int (*numberChooseFunc)()){

}
*/
/*
void LCD1602::flushMenuContentOnLcd(){

}
*/
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
      Serial.print(firstLineNumOfCurrentScreen);
  }  
}

////////////////////////////////////////////////////////////////////
class PushBtnsBeTriggeredEvents{
  public:
    LCD1602 lcd1602;
    Ds1302::DateTime getTimeBuffer,setTimeBuffer;//一个用来储存要设置的时间；一个用来储存从DS1302中读到的时间。值得注意的是，这个结构体内的数据成员的类型为uint_8，意为八位无符号整数，即这些数据成员的取值范围为0-255。用int为其赋值时可能会溢出。
    Ds1302 ds1302;
    void setTime();   
    void setTime(int year,int month,int day,int hour,int min,int sec,int dow);
    void getTime();
    PushBtnsBeTriggeredEvents(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);
    PushBtnsBeTriggeredEvents(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);
};

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0):lcd1602(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW),ds1302(pinClkEn,pinClkClk,pinClkDat){
  ds1302.init();//启用ds1302对象。否则开发板不会通过这个对象与时钟芯片进行通讯。
  getTime();
  setTimeBuffer.year=getTimeBuffer.year;
  setTimeBuffer.month=getTimeBuffer.month;
  setTimeBuffer.day=getTimeBuffer.day;
  setTimeBuffer.hour=getTimeBuffer.hour;
  setTimeBuffer.minute=getTimeBuffer.minute;
  setTimeBuffer.second=getTimeBuffer.second;
  setTimeBuffer.dow=getTimeBuffer.dow;  
}

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0):lcd1602(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW),ds1302(pinClkEn,pinClkClk,pinClkDat){
  ds1302.init();
  getTime();
  setTimeBuffer.year=getTimeBuffer.year;
  setTimeBuffer.month=getTimeBuffer.month;
  setTimeBuffer.day=getTimeBuffer.day;
  setTimeBuffer.hour=getTimeBuffer.hour;
  setTimeBuffer.minute=getTimeBuffer.minute;
  setTimeBuffer.second=getTimeBuffer.second;
  setTimeBuffer.dow=getTimeBuffer.dow; 
}

void PushBtnsBeTriggeredEvents::setTime(){
  ds1302.setDateTime(&setTimeBuffer);
}

void PushBtnsBeTriggeredEvents::setTime(int year,int month,int day,int hour,int min,int sec,int dow){//dow=day of week，该值不能大于7（星期日的枚举值）
  setTimeBuffer.year=year;
  setTimeBuffer.month=month;
  setTimeBuffer.day=day;
  setTimeBuffer.hour=hour;
  setTimeBuffer.minute=min;
  setTimeBuffer.second=sec;
  setTimeBuffer.dow=dow;
  ds1302.setDateTime(&setTimeBuffer);
}

void PushBtnsBeTriggeredEvents::getTime(){
  ds1302.getDateTime(&getTimeBuffer);  
}




////////////////////////////////////////////////////////////////////
class PushBtns{
  public:
  int waterWeight,cycleGapDay,cycleClock,addWaterLim,heatSaveTemp,heatTemp,bottleWeight;//这些是要写入EEPROM的数据。它们在开机时从EEPROM读取数据，每当修改时将修改上传至EEPROM。另注：在ARduino中byte数据的储存只需要1btye，而int和short一样需要2byte。但使用byte数据似乎需要再加载一个Arduino自己的库，所以如果是少量的数据使用Byte反而比int占空间。int可以赋值给byte，但值可能会溢出并且造成错误。
  bool SINGLEBOILFLAG,CYCLEBOILFLAG,AUTOBOILFLAG,HEATSAVEFLAG,HEATFLAG;//设置LCD菜单行上显示相对应功能开关状态的FLAG
  int up,down,quit,select,onAndOff,onAndOffFlag,backLight;//储存各按钮所对应的开发板的引脚号的变量。其中backLinght是控制LCD1602的背光亮度的。它可接在LCD1602的背光正极引脚上，也可接在背光负极引脚上。原理是，当背光正极与负极之间的电压接近O时，背光亮度也接近于无；当正极到负极的电压为5V时，亮度最大。
  enum {triggeredLevelOfUp=LOW,triggeredLevelOfDown=LOW,triggeredLevelOfQuit=LOW,triggeredLevelOfSelect=LOW,triggeredLevelOfOnAndOff=LOW};
  PushBtnsBeTriggeredEvents pushBtnsBeTriggeredEvents;
  //-------------------------------------------------//
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0);  
  void upEvents();
  void downEvents();
  void selectEvents();
  void quitEvents();
  void onAndOffEvents();
  int numberChooseFunc(int start,int step,int lowerlim,int upperlim,int quitCode=-1);//数值选择事件，调用则出现一个临时菜单以供选择数值，返回值为其选择的数值。临时菜单在选择结束后会自动关闭且恢复之前的菜单页面。他接受一个初始值，一个步长，一个选择值上限值，一个选择值下限，一个未选择便退出时的标识值作为参数。
};

PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW):pushBtnsBeTriggeredEvents(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinClkEn,pinClkClk,pinClkDat,pinRW=0){
  //waterWeight=EEPROM.read(0);//读取处于EEPROM中位置为0的数据。EEPROM中的位置是EEPROM中的地址的简化：一个位置代表这个内存中一个byte大小的内存区域，即相邻的8个bit所构成的一个储存区域。read可以读取该位置储存的一个字节；write则可以在该位置写入一个字节。但int值在Arduino中占两个字节，则如果将一个大于255的int或float值存入一个Btye位中将导致Arduino程序崩溃。
  EEPROM.get(0,waterWeight);//但是使用get()则可以在指定的位置作为起始，获得与实参2相同类型的的数据，这实参2的类型不作限制；并将数据存入传递的实参2内。同理，使用put(pos,val)函数可以将任意类型的val存入以int值pos为开始位置的相应大小的储存区域中，val不一定只用一个1个储存位置储存，可能是以pos为起始的，以及其后的多个位置作为其储存区域。另外，当一个位置未被储存数据时，其默认十进制值为255。
  EEPROM.get(2,cycleGapDay);//int占两个byte，所以会占用两个位置进行储存。所以，不能将上一个储存位置值加1来储存这个int值，而是需要加2。可用sizeof()判断其类型大小。
  EEPROM.get(4,cycleClock);
  EEPROM.get(6,addWaterLim);
  EEPROM.get(8,heatSaveTemp);
  EEPROM.get(10,heatTemp);
  EEPROM.get(12,bottleWeight);
  SINGLEBOILFLAG=false;
  CYCLEBOILFLAG=false;
  AUTOBOILFLAG=false;
  HEATSAVEFLAG=false;
  HEATFLAG=false;
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

PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinClkEn,int pinClkClk,int pinClkDat,int pinRW=0):pushBtnsBeTriggeredEvents(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinClkEn,pinClkClk,pinClkDat,pinRW){
  EEPROM.get(0,waterWeight);
  EEPROM.get(2,
  cycleGapDay);
  EEPROM.get(4,cycleClock);
  EEPROM.get(6,addWaterLim);
  EEPROM.get(8,heatSaveTemp);
  EEPROM.get(10,heatTemp);
  EEPROM.get(12,bottleWeight);
  SINGLEBOILFLAG=false;
  CYCLEBOILFLAG=false;
  AUTOBOILFLAG=false;
  HEATSAVEFLAG=false;
  HEATFLAG=false;
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
    pushBtnsBeTriggeredEvents.lcd1602.menuScollUp();
    delay(500);//避免在LOOP函数中因刷新过快出现一些问题,另外，它还控制着当按住按钮不放时菜单栏滚动的速率
    //Serial.print("func");//测试
  }
}
void PushBtns::downEvents(){
  if(digitalRead(down)==triggeredLevelOfDown){
    delay(100);//去抖动
  }
  if(digitalRead(down)==triggeredLevelOfDown){
    pushBtnsBeTriggeredEvents.lcd1602.menuScollDown();
    delay(500);
  }
  
}
void PushBtns::selectEvents(){
  //Serial.print(digitalRead(select));

  if(digitalRead(select)==triggeredLevelOfSelect){
    delay(100);//去抖动
  }  
  if(digitalRead(select)==triggeredLevelOfSelect){
    //Serial.print(11111);
    if(pushBtnsBeTriggeredEvents.lcd1602.menuIndexOfCurrentScreen==0){
      switch(pushBtnsBeTriggeredEvents.lcd1602.firstLineNumOfCurrentScreen){
        case 0:
        //SHUTDOWN
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,5,"HEAT:off");
          pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("ALL MODE","SHUTED DOWN");
          delay(2000);
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);                  
          break;          
        case 1:
        //SINGLEBOIL
          if(SINGLEBOILFLAG){
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 1); 
            SINGLEBOILFLAG=false;
          }else{
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 1); 
            SINGLEBOILFLAG=true;          
          }
          
          break;
        case 2:
        //CYCLEBOIL
          if(CYCLEBOILFLAG){
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 2); 
            CYCLEBOILFLAG=false;
          }else{
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 2); 
            CYCLEBOILFLAG=true;          
          }
          
          break;
        case 3:
        //ATUOBOIL
          if(AUTOBOILFLAG){
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 3); 
            AUTOBOILFLAG=false;
          }else{
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 3); 
            AUTOBOILFLAG=true;          
          }
          break;
        case 4:
        //HEATSAVE
          if(HEATSAVEFLAG){
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 4); 
            HEATSAVEFLAG=false;
          }else{
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:on");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 4); 
            HEATSAVEFLAG=true;          
          }
          break;
        case 5:
        //HEAT，但与上模式不并行
          if(HEATFLAG){
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,5,"HEAT:off");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:on");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:on");            
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 5); 
            HEATFLAG=false;
          }else{
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,5,"HEAT:on");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,1,"SINGLEBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,2,"CYCLEBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,3,"AUTOBOIL:off");
            pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(0,4,"HEATSAVE:off");
            pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 5); 
            HEATFLAG=true;          
          }
          break;
        case 6:
        //SET
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(1, 0); 
          break;
        case 7:
        //SETSYSTEMTIME->进入时间选择页面
          pushBtnsBeTriggeredEvents.getTime();
          pushBtnsBeTriggeredEvents.setTimeBuffer.year=pushBtnsBeTriggeredEvents.getTimeBuffer.year;
          pushBtnsBeTriggeredEvents.setTimeBuffer.month=pushBtnsBeTriggeredEvents.getTimeBuffer.month;
          pushBtnsBeTriggeredEvents.setTimeBuffer.day=pushBtnsBeTriggeredEvents.getTimeBuffer.day;
          pushBtnsBeTriggeredEvents.setTimeBuffer.hour=pushBtnsBeTriggeredEvents.getTimeBuffer.hour;
          pushBtnsBeTriggeredEvents.setTimeBuffer.minute=pushBtnsBeTriggeredEvents.getTimeBuffer.minute;
          pushBtnsBeTriggeredEvents.setTimeBuffer.second=pushBtnsBeTriggeredEvents.getTimeBuffer.second;
          pushBtnsBeTriggeredEvents.setTimeBuffer.dow=pushBtnsBeTriggeredEvents.getTimeBuffer.dow; 
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,1,String("SETYEAR:20"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.year)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,2,String("SETMONTH:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.month)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,3,String("SETDATE:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.day)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,4,String("SETHOUR:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.hour)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,5,String("SETMINUTE:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.minute)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,6,String("SETSECOND:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.second)));
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,7,String("SETWEEKDAY:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.dow)));
          //以上代码可以在进入时间选择页面的时候动态地刷新一下该页面
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 0);
          break;
        default:
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }      
    }else if(pushBtnsBeTriggeredEvents.lcd1602.menuIndexOfCurrentScreen==1){
      switch(pushBtnsBeTriggeredEvents.lcd1602.firstLineNumOfCurrentScreen){
        case 0:
        //SETWATERWEIGHT
          waterWeight=numberChooseFunc(100,30,50,200);     
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(1,0,String("SETWATERW:"+String(waterWeight)+"g"));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(1, 0); 
          EEPROM.put(0,waterWeight);//将waterWeight写入EEPROM中 
          break;          
        case 1:
        //SETCYCLEGAPDAY
          cycleGapDay=numberChooseFunc(0,1,0,30); 
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(1,1,String("SETWATERW:"+String(cycleGapDay)+"DS"));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(1, 1); 
          EEPROM.put(2,cycleGapDay);
          break;
        case 2:
        //SETCYCLECLOCK
          break;
        case 3:
        //AUTOADDWATERLIM
          break;
        case 4:
        //SETHEATSAVETEMP
          break;
        case 5:
        //SETHEATTEMP
          break;
        case 6:
        //SETBOTTLEWEIGHT
          break;
        case 7:
        //WATERWIGHT
          break;
        default:
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }      
    }else if(pushBtnsBeTriggeredEvents.lcd1602.menuIndexOfCurrentScreen==2){
      switch(pushBtnsBeTriggeredEvents.lcd1602.firstLineNumOfCurrentScreen){
        
        case 0:
        //CHECKTIME
          pushBtnsBeTriggeredEvents.getTime();          
          //pushBtnsBeTriggeredEvents.ds1302.getDateTime(&pushBtnsBeTriggeredEvents.getTimeBuffer);
          //String line1=String(pushBtnsBeTriggeredEvents.setTimeBuffer.year)+'-'+String(pushBtnsBeTriggeredEvents.setTimeBuffer.month)+'-'+String(pushBtnsBeTriggeredEvents.setTimeBuffer.day);
          //String line2=String(pushBtnsBeTriggeredEvents.setTimeBuffer.hour)+'-'+String(pushBtnsBeTriggeredEvents.setTimeBuffer.minute)+'-'+String(pushBtnsBeTriggeredEvents.setTimeBuffer.second);
          //pushBtnsBeTriggeredEvents.lcd1602.showOnLCD(line1,line2);     
          //也许是因为创建两个临时的String对象对于UNO来讲太过于勉强，抑或是Arduino尚未完善switch中非匿名临时对象的析构或构造，上述语句将使得后续的case无法被触发。但下面这句就可以正常运行     
          pushBtnsBeTriggeredEvents.lcd1602.showOnLCD(String("20"+String(pushBtnsBeTriggeredEvents.getTimeBuffer.year)+'-'+String(pushBtnsBeTriggeredEvents.getTimeBuffer.month)+'-'+String(pushBtnsBeTriggeredEvents.getTimeBuffer.day)),String(pushBtnsBeTriggeredEvents.getTimeBuffer.hour)+'-'+String(pushBtnsBeTriggeredEvents.getTimeBuffer.minute)+'-'+String(pushBtnsBeTriggeredEvents.getTimeBuffer.second)+'-'+String(pushBtnsBeTriggeredEvents.getTimeBuffer.dow));
          delay(2000);
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 0);                  
          break;
                 
        case 1:
        //SETYEAR
        //pushBtnsBeTriggeredEvents.setTimeBuffer.year=-1;//由于year的类型为uint_8(用八个比特储存的无符号整数类型)，所以它的取值范围为0-255，这样做只会使其值为255。
          pushBtnsBeTriggeredEvents.setTimeBuffer.year=numberChooseFunc(25,1,0,99,25);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,1,String("SETYEAR:20"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.year)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 1);       
          break;
        case 2:
        //SETMONTH
          pushBtnsBeTriggeredEvents.setTimeBuffer.month=numberChooseFunc(6,1,1,12,6);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,2,String("SETMONTH:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.month)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 2);       
          break;
        case 3:
        //SETDATE
          if(pushBtnsBeTriggeredEvents.setTimeBuffer.month==1 || pushBtnsBeTriggeredEvents.setTimeBuffer.month==3 ||pushBtnsBeTriggeredEvents.setTimeBuffer.month==5||pushBtnsBeTriggeredEvents.setTimeBuffer.month==7||pushBtnsBeTriggeredEvents.setTimeBuffer.month==8||pushBtnsBeTriggeredEvents.setTimeBuffer.month==10||pushBtnsBeTriggeredEvents.setTimeBuffer.month==12){
            pushBtnsBeTriggeredEvents.setTimeBuffer.day=numberChooseFunc(15,1,1,31,15);
          }else if (pushBtnsBeTriggeredEvents.setTimeBuffer.month==2 && (pushBtnsBeTriggeredEvents.setTimeBuffer.year+2000)%4==0){//由于将设定日期限制在了2000-2099年，所以计算闰年时不需要考虑它是否能被100整除（不是则是闰年，是则再除以400）以及能被100整除后是否能被400整除（若是则为闰年，若不能被400整除则不是闰年）了；在这个时间范围内能被4整除的都是闰年
            pushBtnsBeTriggeredEvents.setTimeBuffer.day=numberChooseFunc(15,1,1,29,15);
          }else if (pushBtnsBeTriggeredEvents.setTimeBuffer.month==2){
            pushBtnsBeTriggeredEvents.setTimeBuffer.day=numberChooseFunc(15,1,1,28,15);
          }else{
            pushBtnsBeTriggeredEvents.setTimeBuffer.day=numberChooseFunc(15,1,1,30,15);              
          }
          //以上结合闰年判断了当前选择的月份为最多为多少天
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,3,String("SETDATE:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.day)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 3);
          break;
        case 4:
        //SETHOUR
          pushBtnsBeTriggeredEvents.setTimeBuffer.hour=numberChooseFunc(12,1,0,23,12);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,4,String("SETHOUR:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.hour)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 4);
          break;
        case 5:
        //SETMINUTE
          pushBtnsBeTriggeredEvents.setTimeBuffer.minute=numberChooseFunc(30,1,1,59,30);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,5,String("SETMINUTE:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.minute)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 5);
          break;
        case 6:
        //SETSECOND
          pushBtnsBeTriggeredEvents.setTimeBuffer.second=numberChooseFunc(30,1,1,59,30);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,6,String("SETSECOND:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.second)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 6);
          break;
        case 7:
        //SETWEEKDAY
          pushBtnsBeTriggeredEvents.setTimeBuffer.dow=numberChooseFunc(3,1,1,7,3);
          pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(2,7,String("SETWEEKDAY:"+String(pushBtnsBeTriggeredEvents.setTimeBuffer.dow)));
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 7);
          break;
        case 8:
        //APPLYCHANGE     
          pushBtnsBeTriggeredEvents.setTime();
          pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("CHANGES","APPLIED");
          delay(1000);
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(2, 0);             
          break;
          
        default:
          pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);
          break;
      }      
    }else{
      pushBtnsBeTriggeredEvents.lcd1602.menuIndexOfCurrentScreen=0;
      pushBtnsBeTriggeredEvents.lcd1602.firstLineNumOfCurrentScreen=0;
    }
    
  }
  delay(100);
}

void PushBtns::quitEvents(){
  if(digitalRead(quit)==triggeredLevelOfQuit){
    delay(100);
  }  
  if(digitalRead(quit)==triggeredLevelOfQuit){
    if(pushBtnsBeTriggeredEvents.lcd1602.menuIndexOfCurrentScreen!=0)
      pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0); //基于目前的菜单内容，这个简陋的返回函数是可用的     
  }
}

void PushBtns::onAndOffEvents(){
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff) delay(100);
  //if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==1) delay(100);//去按钮抖动
  //if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==0) delay(100);
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==1){
    //Serial.print(digitalRead(onAndOff));
    Serial.print(onAndOff);    
    pushBtnsBeTriggeredEvents.lcd1602.lcd.noDisplay();//noDisplay函数只能使LCD不再显示文字，但背光不会熄灭
    onAndOffFlag = 0;
    digitalWrite(backLight,HIGH);//所以用此熄灭背光
    delay(1000);
  }
  else if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==0){
    //Serial.print(digitalRead(onAndOff));
    Serial.print(onAndOff);
    pushBtnsBeTriggeredEvents.lcd1602.lcd.display();
    onAndOffFlag = 1;
    digitalWrite(backLight,LOW);
    delay(1000);
  }
}

int PushBtns::numberChooseFunc(int start,int step,int lowerlim,int upperlim,int quitCode){
  bool numChoosing=true,singleSelectFlag=true;//在同一行中定义声明多个变量并初始化
  int choosedNum=start,lastChoosedNum=start;
  pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("PLEASESELECTNUM:",String(choosedNum));
  while(numChoosing){
    if(choosedNum!=lastChoosedNum){
      pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("PLEASESELECTNUM:",String(choosedNum));
      lastChoosedNum=choosedNum;
    }
    //delay(100);
    
    if(digitalRead(up)==triggeredLevelOfUp && choosedNum<=upperlim) delay(200);
    if(digitalRead(up)==triggeredLevelOfUp && choosedNum<=upperlim){
      choosedNum+=step;      
    }else if(choosedNum>upperlim){
      pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("ERROR:","BEYOND LIMMIT");
      choosedNum=start;
      delay(2000);
    }
    
    if(digitalRead(down)==triggeredLevelOfDown && choosedNum>=lowerlim) delay(200);
    if(digitalRead(down)==triggeredLevelOfDown && choosedNum>=lowerlim){
      choosedNum-=step;      
    }else if(choosedNum<lowerlim){
      pushBtnsBeTriggeredEvents.lcd1602.showOnLCD("ERROR:","UNDER LIMMIT");
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
////////////////////////////////////////////////////////////////////
class Kettle{

};



////////////////////++++++++++++++++++++++++++++++++/////////////////////////

PushBtns *pushBtns;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pushBtns = new PushBtns(A0,A1,A2,A3,A4,8,5,4,3,2,6,7,11,9,10,0);
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.lcd.begin(16, 2);  
  
  /*
  //这是一种用多行为String赋值的方式。一旦Arduino不能运用标准C++对于string的多行赋值格式，那么或许这样可行
  allmenus->menusAndTheirsContent = 
  String("1 SHUTDOWN SINGLEBOIL: CYCLEBOIL: AUTOBOIL: HEATSAVE: HEAT: SET SETSYSTEMTIME ") +
  String("2 SETWATERWEIGHT SETCYCLEGAPDAY SETCYCLECLOCK AUTOADDWATERLIM SETHEATSAVETEMP SETHEATTEMP SETBOTTLEWEIGHT WATERWIGHT") ;
  */
  
  //但很赞的是，Arduino的String类尽管与C++STL的string类有着诸多不同，但多行赋值的语法是相同的。HOORAY！！！
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.menusAndTheirsContent =
  "*0 SHUTDOWN SINGLEBOIL:off CYCLEBOIL:off AUTOBOIL:off HEATSAVE:off HEAT:off SET SETSYSTEMTIME \n"//原始菜单1内容
  "*1 SETWATERW: SETCYCLEGAPDAY SETCYCLECLOCK AUTOADDWATERLIM SETHEATSAVETEMP SETHEATTEMP SETBOTTLEWEIGHT WATERWIGHT \n"//原始菜单2内容
  "*2 CHECKTIME SETYEAR: SETMONTH: SETDATE: SETHOUR: SETMINUTE: SETSECOND: SETWEEKDAY: APPLYCHANGE ";  //原始菜单3内容。作为最后一行，结尾需要加空格。否则最后一个菜单的最后一行无法被加载。

  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(1,0,String("SETWATERW:"+String(pushBtns->waterWeight)+"g"));//由于设置项中的一些数据是储存在EEPROM中的，所以此时把它们读取并加载出来以还原上次关机时的菜单设置
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.ChangeALineInMenus(1,1,String("GAPDAY:"+String(pushBtns->cycleGapDay)+"DS"));
 
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showOnLCD(pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.pickALineInMenus(1,0),pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.pickALineInMenus(1,1));
  
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);  
  /*
  
 
  Serial.begin(9600);
  //-----------------------------------菜单1装载------------------------------
  String menu1content[10]={"SHUTDOWN","SINGLEBOIL:","CYCLEBOIL:","AUTOBOIL:","HEATSAVE:","HEAT:","SET","SETSYSTEMTIME"," "," "};
  int menu1rowIndex[10] = {0,1,2,3,4,5,6,7,8,9};
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.creatAMenu(0, menu1content, menu1rowIndex);
  //-----------------------------------菜单1装载------------------------------
  //-----------------------------------菜单2装载------------------------------
  //String menu2content[10]={"SETWATERWEIGHT","SETCYCLEGAPDAY","SETCYCLECLOCK","AUTOADDWATERLIM","SETHEATSAVETEMP","SETHEATTEMP","SETBOTTLEWEIGHT","WATERWIGHT:"," "," "};
  //int menu2rowIndex[10] = {0,1,2,3,4,5,6,7,8,9};
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.creatAMenu(1, menu2content, menu2rowIndex);
  //-----------------------------------菜单2装载------------------------------
  //-----------------------------------菜单3装载------------------------------
  //String menu3content[10]={"TIME","SETYEAR","SETMONTH","SETDATE","SETHOUR","SETMINUTE","SETSECOND"," "," "," "};
  //int menu3rowIndex[10] = {0,1,2,3,4,5,6,7,8,9};
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.creatAMenu(2, menu3content, menu3rowIndex);
  //-----------------------------------菜单3装载------------------------------

  
*/
  //Serial.print(EEPROM[50]);
  
  //pushBtns->pushBtnsBeTriggeredEvents.ds1302.getDateTime(&pushBtns->pushBtnsBeTriggeredEvents.getTimeBuffer);
}

void loop() {
  // put your main code here, to run repeatedly:
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(int IndexOfTheMenu, int LineNumOfTheMenu);
  pushBtns->upEvents();
  pushBtns->downEvents();
  pushBtns->onAndOffEvents();
  pushBtns->selectEvents();
  pushBtns->quitEvents();
  //Serial.print(pushBtns->pushBtnsBeTriggeredEvents.getTimeBuffer.second);
}


