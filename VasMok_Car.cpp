#include "VasMok_Car.h"
/*

Код АЦП: [130; 300]
Обороты двигателя: [0; 8000]
Скорость мотора [0; 330]: 

speed = kx + b; // k = 47, b = -130

ШИМ: [0;100]
shim = speed / 80;
*/


void Speed(car_info*);
void MotorTemperCalc(car_info*);

// Основная функция работы с машиной
void Car(car_info* carINFptr, unsigned int CalcFreq)
{
  //Передачи машины
  carINFptr -> transmission > 2 ? carINFptr -> transmission = 0: carINFptr -> transmission = carINFptr -> transmission; 
  // Скорость машины
  Speed(carINFptr);
  // Бензин в баке
  carINFptr->fuel -= carINFptr->motorSpeed/10000;
  // Температура мотора
  //MotorTemperCalc(carINFptr);
  // Пробег
  carINFptr->mileage += carINFptr->speed / (200);      


  // Статусы
  //if (carINFptr->fuel <= 1)
  //{
   // carINFptr -> status |= 0b00000010;
  //}

  //if (carINFptr->speed >= 120)
  //{
   // carINFptr -> status |= 0b00000100;
  //}
  
  //if (carINFptr -> status & 0b00000010)
  //{
    //carINFptr -> status |= 0b00000001;
  //}
  
}




// Вычисляем скорость машины
void Speed(car_info* carINFptr)
{
  unsigned int temp_speed = carINFptr->motorSpeed / 24;
  // Нейтральная передача 
  if (0 == carINFptr -> transmission)
  {
    carINFptr->speed = 0;
  }
  

  // Первая передача
  if ( (1 == carINFptr -> transmission) &&  (carINFptr -> motorSpeed <= 4000))      // Проверяем бензин в баке + зона нечувствительности моторчика + Предел скорости на передаче
  {
    carINFptr->speed = temp_speed;            // Вычисляем скорость машины
  }

  // Вторая передачааа
  if (2 == carINFptr -> transmission)      // Проверяем бензин в баке + зона нечувствительности моторчика
  {
    carINFptr->speed = temp_speed;            // Вычисляем скорость машины
  }
}

/*
void MotorTemperCalc(car_info* carINFptr)
{
  if (carINFptr->motorSpeed > 500)                           // Начинает греться
  {
    carINFptr->MotorTempr += carINFptr->motorSpeed * carINFptr->motorSpeed / 10000000;
  }
  else if (carINFptr->MotorTempr > 24)                       // Остывает
  {
    carINFptr->MotorTempr -= carINFptr->MotorTempr * carINFptr->MotorTempr / 500;
  }
}
*/