struct car_info
{
  unsigned int speed;                 // Скорость машины (км/ч)
  float motorSpeed;                   // Скорость двигателя (об/мин)
  unsigned int transmission;          // Передача
  float fuel = 40;             // Ёмкость бака (40 литров)
  float mileage = 0;           // Пробег (км)
  //float MotorTempr = 24;       // Температура двигателя (градусы)
  
  char status;                        
  /* 
  Регистр состояния машины: 
  0 бит == всё в норме
  1 бит == кончился бензин
  //2 бит == перегрев мотора
  2 бит == превышение скорости
  */
};

void Car(car_info*, unsigned int);

/*
class car
{
  private:
  car_info states;

  public:
  void cycle();
};
*/