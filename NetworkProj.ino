#include "VasMok_Car.h"                                   // Авторская библиотека имитации машины
#include <UTFT.h>

void DrawGraph(car_info*);
void  Draw_Warnin();
void Draw_Fuel(float);
void Init_Fuel();

extern uint8_t BigFont[];

// Обработка дребезга первого прерывания
volatile unsigned long gaer_itter_time = 0;    
volatile unsigned int gear_count = 0;    

int Prev_coord_Arrow_X = 0;
int Prev_coord_Arrow_y = 0;
unsigned int Prev_speed = 10;
unsigned int Prev_transmission = 10;

unsigned long LastGraphicsMillis = 0;                     // Время с последней отрисовки
unsigned long LastCarCalc = 0;                            // Время с последней обработки параметров машины
unsigned long CalcFreq = 1000;                            // Частота вызова программы обработки параметров машины
car_info carINF;                                          // Структура с информацией о мащине                       
UTFT myGLCD(ITDB24E_8,A5,A4,A3,A2);



void setup() 
{
  //Serial.begin(9600);
  pinMode(20, INPUT_PULLUP);
  pinMode(24, OUTPUT);
  // Прерывание по переключению передачи pin20
  attachInterrupt(3, Mode_interr, FALLING);

  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.clrScr();
  Init_Fuel();
}



void loop()
{
  // Машина
  if (0 >= carINF.fuel)
  {
    carINF.motorSpeed = 0;
  }
  else
  {
    carINF.motorSpeed = analogRead(7) *10;             // Текущие обороты двигателя (Max == 4*1024 об/мин)
  }
  //Serial.println(carINF.motorSpeed);
  carINF.transmission = gear_count;
  if (millis() - LastCarCalc >= CalcFreq)
  {
    Car(&carINF, CalcFreq);                                   // Обработка параметров машины
    LastCarCalc = millis();
    gear_count = carINF.transmission;
    analogWrite(44, carINF.speed/14);                            // Выдаём управления на моторчик / 3 / 14
  }


  // Графика
   if (millis() - LastGraphicsMillis >= 500 )               // Отрисовка графики 20 раз в секунду
  {
    //myGLCD.clrScr();
    DrawGraph(&carINF);
    LastGraphicsMillis = millis();
  }
}




void DrawGraph(car_info* carINFptr)
{
  //Рисуем спидометр 
  myGLCD.setColor(0,255,255);
  myGLCD.drawCircle(160,120,10);
  myGLCD.drawCircle(160,120,80);
  myGLCD.drawCircle(160,120,103);
  // Цифры дисплея
  myGLCD.printNumI(1, 92, 180, 0);
  myGLCD.printNumI(2, 63, 130, 0);
  myGLCD.printNumI(3, 74, 66, 0);
  myGLCD.printNumI(4, 117, 28, 0);
  myGLCD.printNumI(5, 187, 28, 0);
  myGLCD.printNumI(6, 232, 66, 0);
  myGLCD.printNumI(7, 241, 130, 0);
  myGLCD.printNumI(8, 213, 180, 0);
  // Рисуем стрелку для спидометра
  float speed_to_deg = 0.045; // Коэффициент перевода скорости в угол
  float Rho = 105;
  float Deg = 0;
  int x_arrow = 0;
  int y_arrow = 0;
  int offset_x = 160;
  int offset_y = 120;
  Deg = carINFptr->motorSpeed/1406 + 1.57;
  x_arrow = (int)Rho*cos(Deg) + offset_x;
  y_arrow = (int)Rho*sin(Deg) + offset_y;

  myGLCD.setColor(0,0,0);
  myGLCD.drawLine(160,120, Prev_coord_Arrow_X, Prev_coord_Arrow_y);
  myGLCD.setColor(255,0,0);
  myGLCD.drawLine(160,120, x_arrow, y_arrow);

  Prev_coord_Arrow_X = x_arrow;
  Prev_coord_Arrow_y = y_arrow;

  // Максимальное значение скорости в итоге составляет 140 (полный круг), пересчитать коэффициенты или...
  // Диплей часто обновлять не получится, не успеваает

  // Рисуем скорость
  myGLCD.setColor(0,255,255);
  myGLCD.drawRect(125,60,125 + 72,60 + 30);
  if (fabs(Prev_speed - carINFptr->speed) >= 2)
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(127,62,125 + 70,60 + 28);
    myGLCD.setColor(0,255,255);
    myGLCD.printNumI((int)carINFptr->speed, 138, 68, 0);
    Prev_speed = carINFptr->speed;
  }
  

  // Передача
  myGLCD.setColor(0,255,255);
  myGLCD.drawRect(145, 145,145 + 30,145 + 40);
  if (fabs(Prev_transmission - carINFptr -> transmission) >= 2)
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(147, 147, 145 + 28, 145 + 38);
    myGLCD.setColor(0,255,255);
    myGLCD.printNumI(carINFptr -> transmission, 155, 160, 0);
    Prev_transmission = carINFptr -> transmission;
  }
  
  

  

  // Предупреждение по превышению скорости
  if (carINFptr -> speed > 120)
  {
    Draw_Warnin();
  }
  else
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(255,  80, 320,  0);
  }

  // Рисуем бак
  Draw_Fuel(carINFptr -> fuel);
  

  // Окно
myGLCD.setColor(0,255,255);
myGLCD.drawRect(240, 200, 240 + 65, 180 + 37);

// Текст
myGLCD.printNumI((int)carINF.mileage, 241, 201, 0); 

// ODO надпись 
myGLCD.setColor(255,0,0);
char text[4] = "ODO";
int i;
int x = 250;
int y = 220;
// Циклическая отрисовка текста
for (i=0; i < 3; i++)
  {
    myGLCD.printChar(text[i], x,  y);
    x+=15;
  }
}

void  Draw_Warnin()
{
  myGLCD.setColor(255,0,0);
  int i = 0;
  int x = 260;
  int y = 15;
  char text[10] = "SLOW";                                            // Текст для дисплея
  for (i=0; i < 5; i++)
  {
    myGLCD.printChar(text[i], x,  y);
    x+=15;
  }

myGLCD.printChar('!', 285,  35);


  text[0] = 'D';
  text[1] = 'O';
  text[2] = 'W';
  text[3] = 'N';
  x = 260;
  y = 55;
  for (i=0; i < 5; i++)
  {
    myGLCD.printChar(text[i], x,  y);
    x+=15;
  }

}
  
void Draw_Fuel(float fuel)
{
  if (fuel < 38) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  131, 11+13,  131 + 8);
  }
  if (fuel < 34) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  141, 11+13,  141 + 8);
  }
  if (fuel < 30) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  151, 11+13,  151 + 8);
  }
  if (fuel < 26) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  161, 11+13,  161 + 8);
  }
  if (fuel < 22) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  171, 11+13,  171 + 8);
  }
  if (fuel < 18) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  181, 11+13,  181 + 8);
  }
  if (fuel < 14) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  191, 11+13,  191 + 8);
  }
   if (fuel < 10) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  201, 11+13,  201 + 8);
  }
   if (fuel < 6) 
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(11,  211, 11+13,  211 + 8);
  }
}


void Init_Fuel(void)
{
  int i = 0;
  int x = 10;
  int y = 220;
  int x2 = x + 15;
  int y2 = y;
  int x3 = x + 12;
  int y3 = y + 8;
  int delta = -10;
/*
  if ( fuel > 2 )
  {
     myGLCD.setColor(255,0,0);
     myGLCD.fillRect(10, 220, 10 + 15, 220 - 20);
  }

if (fuel > 10)
{
  myGLCD.setColor(255,168,0);
    myGLCD.fillRect(10, 200, 10 + 15, 200 - 20);
}

if (fuel > 20)
{
  myGLCD.setColor(85,255,0);
    myGLCD.fillRect(10, 180, 10 + 15, 180 - 50);
}
*/
  myGLCD.setColor(255,0,0);
  myGLCD.fillRect(10, 220, 10 + 15, 220 - 20);
  myGLCD.setColor(255,168,0);
  myGLCD.fillRect(10, 200, 10 + 15, 200 - 20);
  myGLCD.setColor(85,255,0);
  myGLCD.fillRect(10, 180, 10 + 15, 180 - 50);
  myGLCD.setColor(0,255,255);
  myGLCD.drawRect(x, y - 90, x2, y2);
   myGLCD.drawLine(x, y, x2, y2);
   y+=delta;
   y2+=delta;
    myGLCD.drawLine(x, y, x2, y2);
       y+=delta;
       y2+=delta;
     myGLCD.drawLine(x, y, x2, y2);
        y+=delta;
        y2+=delta;
      myGLCD.drawLine(x, y, x2, y2);
         y+=delta;
         y2+=delta;
       myGLCD.drawLine(x, y, x2, y2);
         y+=delta;
         y2+=delta;
        myGLCD.drawLine(x, y, x2, y2);
           y+=delta;
           y2+=delta;
         myGLCD.drawLine(x, y, x2, y2);
            y+=delta;
            y2+=delta;
          myGLCD.drawLine(x, y, x2, y2);
             y+=delta;
             y2+=delta;
           myGLCD.drawLine(x, y, x2, y2);
             y+=delta;
             y2+=delta;
   myGLCD.setColor(255,0,0);

  // Fuel
  char text [4] = "FUEL";
  int xtext = 30;
  int ytext = 140;
  for (i=0; i < 4; i++)
  {
    myGLCD.printChar(text[i], xtext,  ytext);
    ytext+=17;
  }
}

void Mode_interr ()
{
   if (millis() - gaer_itter_time >= 1000)
 {
  if (!digitalRead(2))
  {
    gear_count++;
  }
  
  gaer_itter_time = millis();
 }
}