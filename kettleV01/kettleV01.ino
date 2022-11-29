#include <LiquidCrystal.h>
#include <ArxContainer.h>

/*
class AllMenus {
public:
  arx::map<arx::map<int, arx::vector< String>>, arx::vector<int>> menus;
  void creatAMenu(arx::vector<String> menucontent, int menuIndex, int rowsPointeddIndex);
  arx::vector<String> & getAMenuByIndex(int menuIndex);
};
void AllMenus::creatAMenu(arx::vector<String> menucontent, int menuIndex, arx::vector<int> rowsPointedIndex){
  //arx::pair<int, arx::vector<String>> p(arx::make_pair(menuIndex,menucontent));arx::map<int, arx::vector< String>> m;m.insert(p);Also worked,but not necessary to creat a p
  arx::map<int, arx::vector< String>> m;
  m.insert(arx::make_pair(menuIndex,menucontent));
  menus.insert(arx::make_pair(m,rowsPointedIndex));
};

arx::vector<String> & AllMenus::getAMenuByIndex(int menuIndex){
  for(auto p = menus.begin();p!=menus.end();p++){
    if(p->first.begin()->first==menuIndex){
      return p->first.begin()->second;
    }
  }
}*/
//上面是一个已废置的数据结构及其方法
////////////////////////////////////////////////////////////////////
class AllMenus {
public:

  /*struct menuIndexAndContent{
    int menuIndex;
    arx::vector<String> menuContent;
    friend bool operator==(const menuIndexAndContent & c,const menuIndexAndContent & b);//C++可在一个结构体里声明或定义函数。这里为该结构体重载一个==运算符，这是为了它可以在map容器中被设置为键。因为键是唯一的，所以键必须要定义这样的一个==运算符在编译前来判断它是否唯一。这里所以声明一个该结构体的友元函数。
  };*/
  //arx::map<arx::pair<int, arx::vector<String>>, arx::vector<int>> menus;//太过复杂，可以通过编译，但是UNO开发板会宕机，这是因为mega2560这款单片机的算力难以支持
  //arx::map<arx::pair<int, String[4]>, arx::vector<int>> menus;//仍有一些复杂，当String数组的长度大于等于5时，UNO板的算力就不足以使其在运行程序的时候驱动LCD显示正确的字符了
  //arx::map<menuIndexAndContent, arx::vector<int>> menus;//结构体代替Pair。这是可行的，因为该结构体重载了==运算符。然而这更加复杂了，UNO板依然宕机。
  //当出现map<x,vector<String>>时，UNO板就会宕机
  //最终只好选择下面这种有限菜单空间的方式。上面所罗列的数据储存结构，在理论上可以储存无限个菜单内容及索引。而下面的最多只能创建10个菜单，每个菜单只能存入10行。
  enum {sizeOfArray=10,aaa=10};
  //static const int sizeOfArray = 10;//同样可行
  int eachMenuRowsPointedIndex[sizeOfArray][sizeOfArray];
  String menusAndTheirsContent[sizeOfArray][sizeOfArray];
  //-----------------------------------------------↑data  ↓func-------------------------------------------------
  void creatAMenu(int menuIndex,String menucontent[], int rowsPointedIndex[]);
  //arx::vector<String> & getAMenuByIndex(int menuIndex);//C++的函数不能将数幅当作返回值。所以这个方法无法适配二维字符串数组的储存结构。

};
/*bool operator==(const AllMenus::menuIndexAndContent & c,const AllMenus::menuIndexAndContent & b)//将对结构体的==运算符重载函数定义在其嵌套其中的类外。定义在类内是不行的。因为那样会被视为该类对于运算符==的重载函数。
{
  return c.menuIndex==b.menuIndex&&c.menuContent==b.menuContent;
}*/

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
  int up,down,quit,select,onAndOff;//储存各按钮所对应的开发板的引脚号的变量
  int triggeredLevelOfUp,triggeredLevelOfDown,triggeredLevelOfQuit,triggeredLevelOfSelect,triggeredLevelOfOnAndOff;
  PushBtnsBeTriggeredEvents pushBtnsBeTriggeredEvents;
  //-------------------------------------------------//
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);
  PushBtns(int up,int down,int quit,int select,int OnAndOff,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0);  
  void upEvents();
  void downEvents();
  void selectEvents();
  void quitEvents();
  void onAndOffEvents();
};
////////////////////////////////////////////////////////////////////
class Kettle{

};



////////////////////++++++++++++++++++++++++++++++++/////////////////////////

//Uno会将动态存储持续性的数据对象（比如用new声明的匿名对象）以及自动存储持续性的数据对象（比如在函数中定义的变量）与程序储存在它32256字节的程序存储空间（硬件对应着一个闪存）中；将静态存储持续性的数据对象（比如在函数外定义的全局变量或对象和用static定义的变量或对象）储存在它2048字节的动态内存（硬件对应着一个SRAM）中。这样的设计是因为可以较为快速地调用全局变量。
//所以，在创建大型数据对象时，须将其置于程序储存空间中，否则动态内存将无法容纳这么大的数据。所以，下述创建的对象将占满并溢出UNO的动态内存：
//static PushBtns pushBtns2(A0,A1,A2,A3,A4,5,4,3,2,6,7,0);
//PushBtns pushBtns2(A0,A1,A2,A3,A4,5,4,3,2,6,7,0);

  //const int rs=7,rw=10,en=6,d4=5,d5=4,d6=3,d7=2;
  //LiquidCrystal lcd(rs,rw,en,d4,d5,d6,d7);
  PushBtns *pushBtns;

void setup() {
  pushBtns = new PushBtns(A0,A1,A2,A3,A4,5,4,3,2,6,7,0);
  Serial.begin(9600);
  // put your setup code here, to run once:
  //static PushBtns pushBtns2(A0,A1,A2,A3,A4,5,4,3,2,6,7,0);//这也会溢出UNO的动态内存
  //PushBtns pushBtns2(A0,A1,A2,A3,A4,5,4,3,2,6,7,0);//但这样就不会，因为这样创建的对象会被储存在UNO的程序储存空间中
  pushBtns->triggeredLevelOfUp=LOW;
  pushBtns->triggeredLevelOfDown=LOW;
  pushBtns->triggeredLevelOfQuit=LOW;
  pushBtns->triggeredLevelOfSelect=LOW;
  pushBtns->triggeredLevelOfOnAndOff=LOW;

  String a[5]={"eeeeee","wwww","aaaaaa","cccccc"};
  int b[5] = {1,2,3,4,5};
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.creatAMenu(0, a, b);
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.lcd.begin(16, 2);
  pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(0, 3);
  Serial.print(pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.menusAndTheirsContent[0][0]);
  Serial.print(pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.menusAndTheirsContent[0][1]);
  Serial.print(pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.menusAndTheirsContent[0][2]);
  Serial.print(pushBtns->pushBtnsBeTriggeredEvents.lcd1602.m.menusAndTheirsContent[0][3]);
  
  
  
}

void loop() {
  //pushBtns->pushBtnsBeTriggeredEvents.lcd1602.showMenuContentOnLcd(int IndexOfTheMenu, int LineNumOfTheMenu);
  
  // put your main code here, to run repeatedly:
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
/*
void AllMenus::creatAMenu(arx::vector<String> menucontent, int menuIndex, arx::vector<int> rowsPointedIndex) {
  arx::pair<int, arx::vector<int>> p(arx::make_pair(menuIndex,rowsPointedIndex));
  menus.insert(arx::make_pair(p, rowsPointedIndex));
}

arx::vector<String>& AllMenus::getAMenuByIndex(int menuIndex) {
  for(auto p=menus.begin();p!=menus.end();p++){
    if(p->first.first==menuIndex){
      return p->first.second;      
    }
  }
}
*/
////////////////////////////////////////////////////////////////////

LCD1602::LCD1602(int pinDB4=2,int pinDB5=3,int pinDB6=4,int pinDB7=5,int pinE=6,int pinRS=7,int pinRW=0):lcd(pinRS,pinRW,pinE,pinDB4,pinDB5,pinDB6,pinDB7){
  
  //lcd.begin(16,2);//这一句也会导致异常。在setup与loop函数外创建的全局变量的构造函数内，这样的语句是不安全的。
  //lcd.cursor();
  menuIndexOfCurrentScreen=0;
  firstLineNumOfCurrentScreen=0;
  //showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);//此句无效并且会导致白屏
}

LCD1602::LCD1602(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd(pinRS,pinRW,pinE,pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7){
  //lcd.begin(16, 2);
  //lcd.cursor();
  //menuIndexOfCurrentScreen=0;
  //firstLineNumOfCurrentScreen=0;
  //ShowMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
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

void LCD1602::openLCD(){
  lcd.display();
}
void LCD1602::closeLCD(){
  lcd.noDisplay();
}
void LCD1602::menuScollUp(){
  if(firstLineNumOfCurrentScreen==0){
      //firstLineNumOfCurrentScreen=m.getAMenuByIndex(menuIndexOfCurrentScreen).size()-1;
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }else{
      firstLineNumOfCurrentScreen--;
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }
}
void LCD1602::menuScollDown(){
  if(firstLineNumOfCurrentScreen==m.sizeOfArray-1){
      firstLineNumOfCurrentScreen=0;
      lcd.clear();
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }else{
      firstLineNumOfCurrentScreen++;
      lcd.clear();
      showMenuContentOnLcd(menuIndexOfCurrentScreen,firstLineNumOfCurrentScreen);
  }  
}

//////////////////////////////////////////////////////////////////////

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd1602(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){

}

PushBtnsBeTriggeredEvents::PushBtnsBeTriggeredEvents(int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):lcd1602(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){

}

/////////////////////////////////////////////////////////////////////
PushBtns::PushBtns(int up,int down,int quit,int select,int OnAndOff,int pinDB0,int pinDB1,int pinDB2,int pinDB3,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):pushBtnsBeTriggeredEvents(pinDB0,pinDB1,pinDB2,pinDB3,pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){
  this->up=up;
  this->down=down;
  this->quit=quit;
  this->select=select;
  this->onAndOff=onAndOff;
  pinMode(up,INPUT_PULLUP);//设置up按钮的引脚，并将引脚模式设置为读取引脚电平且使用内部上拉电阻，使用内部上拉电阻将使得引脚在与地断开时电平为高，接地时电平为低
  pinMode(down,INPUT_PULLUP);
  pinMode(quit,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(onAndOff,INPUT_PULLUP);
}
PushBtns::PushBtns(int up,int down,int quit,int select,int OnAndOff,int pinDB4,int pinDB5,int pinDB6,int pinDB7,int pinE,int pinRS,int pinRW=0):pushBtnsBeTriggeredEvents(pinDB4,pinDB5,pinDB6,pinDB7,pinE,pinRS,pinRW=0){
  this->up=up;
  this->down=down;
  this->quit=quit;
  this->select=select;
  this->onAndOff=onAndOff;
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
  pinMode(quit,INPUT_PULLUP);
  pinMode(select,INPUT_PULLUP);
  pinMode(onAndOff,INPUT_PULLUP);
}
void PushBtns::upEvents(){
  if(digitalRead(up)==triggeredLevelOfUp){
    pushBtnsBeTriggeredEvents.lcd1602.menuScollUp();
  }
  
}
void PushBtns::downEvents(){
  if(digitalRead(down)==triggeredLevelOfDown){
    pushBtnsBeTriggeredEvents.lcd1602.menuScollDown();
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
  if(digitalRead(onAndOff)==triggeredLevelOfOnAndOff){

  }
}
/////////////////////////////////////////////////////////////////////







