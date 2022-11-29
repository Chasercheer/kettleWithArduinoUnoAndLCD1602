#include <LiquidCrystal.h>
#include <ArxContainer.h>


////////////////////////////////////////////////////////////////////
class AllMenus {
public:

  enum {sizeOfArray=10,aaa=10};
  //static const int sizeOfArray = 10;//同样可行
  int eachMenuRowsPointedIndex[sizeOfArray][sizeOfArray];
  String menusAndTheirsContent[sizeOfArray][sizeOfArray];
  //-----------------------------------------------↑data  ↓func-------------------------------------------------
  void creatAMenu(int menuIndex,String menucontent[], int rowsPointedIndex[]);
 

};


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
    void selectedEventsOfMenuContentOnLcd(int (*numberChooseFunc)()); //在按钮按下后调用该函数可触发当前屏幕上第一行所对应的事件。它接受一个无形参且返回int值的数值选择函数作为参数，供其内部调用。
    void flushMenuContentOnLcd();//动态更新当前屏幕内容，调用一次更新一次
    void openLCD();
    void closeLCD();
    void menuScollUp();
    void menuScollDown();
    //void menuRefresh();
};

////////////////////////////////////////////////////////////////////
class PushBtnsBeTriggeredEvents{
  public:
    LCD1602 lcd1602;
    PushBtnsBeTriggeredEvents(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
    PushBtnsBeTriggeredEvents(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
};
////////////////////////////////////////////////////////////////////
class PushBtns{
  public:
  int up,down,quit,select,onAndOff,onAndOffFlag,backLight;//储存各按钮所对应的开发板的引脚号的变量。其中backLinght是控制LCD1602的背光亮度的。它可接在LCD1602的背光正极引脚上，也可接在背光负极引脚上。原理是，当背光正极与负极之间的电压接近O时，背光亮度也接近于无；当正极到负极的电压为5V时，亮度最大。
  enum {triggeredLevelOfUp=LOW,triggeredLevelOfDown=LOW,triggeredLevelOfQuit=LOW,triggeredLevelOfSelect=LOW,triggeredLevelOfOnAndOff=LOW};
  PushBtnsBeTriggeredEvents pushBtnsBeTriggeredEvents;
  //-------------------------------------------------//
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);  
  void upEvents();
  void downEvents();
  void selectEvents();
  void quitEvents();
  void onAndOffEvents();
  int numberChooseFunc();//数值选择事件，调用则出现一个临时菜单以供选择数值，返回值为其选择的数值。临时菜单在选择结束后会自动关闭且恢复之前的菜单页面
};
////////////////////////////////////////////////////////////////////
class Kettle{

};



////////////////////++++++++++++++++++++++++++++++++/////////////////////////

  PushBtns *pushBtns;
void setup() {
  // put your setup code here, to run once:
  pushBtns = new PushBtns(A0,A1,A2,A3,A4,11,5,4,3,2,6,7,0);
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
  //-----------------------------------菜单4装载------------------------------
  //-----------------------------------菜单4装载------------------------------
  //-----------------------------------菜单5装载------------------------------
  //-----------------------------------菜单5装载------------------------------  
  //-----------------------------------菜单6装载------------------------------
  //-----------------------------------菜单6装载------------------------------
  //-----------------------------------菜单7装载------------------------------
  //-----------------------------------菜单7装载------------------------------
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.lcd.begin(16, 2);
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 0);

  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(int IndexOfTheMenu, int LineNumOfTheMenu);
  pushBtns->upEvents();
  pushBtns->downEvents();
  pushBtns->onAndOffEvents();
  
}

////////////////////++++++++++++++++++++++++++++++++/////////////////////////



void AllMenus::creatAMenu(int menuIndex, String menucontent[], int rowsPointedIndex[]) {
  for(int i = 0;i<sizeOfArray;i++){
    menusAndTheirsContent[menuIndex][i]=menucontent[i];
  }
  for(int i = 0;i<sizeOfArray;i++){
    eachMenuRowsPointedIndex[menuIndex][i]=rowsPointedIndex[i];
  }
}


////////////////////////////////////////////////////////////////////

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
    showOnLCD(m.menusAndTheirsContent[IndexOfTheMenu][LineNumOfTheMenu],m.menusAndTheirsContent[IndexOfTheMenu][LineNumOfTheMenu+1]);
    return true;
  }else if(IndexOfTheMenu<m.sizeOfArray&&LineNumOfTheMenu==9){
    showOnLCD(m.menusAndTheirsContent[IndexOfTheMenu][LineNumOfTheMenu],"   ");
    return true;
  }else{
    return false;
  }
}
void LCD1602::selectedEventsOfMenuContentOnLcd(int (*numberChooseFunc)()){

}
void LCD1602::flushMenuContentOnLcd(){

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

//////////////////////////////////////////////////////////////////////

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd1602(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){

}

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd1602(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){

}

/////////////////////////////////////////////////////////////////////
PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):pushBtnsBeTriggeredEvents(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){
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
PushBtns::PushBtns(int up,int down,int quit,int select,int onAndOff,int backLight,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):pushBtnsBeTriggeredEvents(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){
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
  if(digitalRead(select)==triggeredLevelOfSelect){
 
  }
}
void PushBtns::quitEvents(){
  if(digitalRead(quit)==triggeredLevelOfQuit){

  }
}
void PushBtns::onAndOffEvents(){
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==1) delay(100);//去按钮抖动
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff && onAndOffFlag==0) delay(100);
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
int PushBtns::numberChooseFunc(){

}
/////////////////////////////////////////////////////////////////////







