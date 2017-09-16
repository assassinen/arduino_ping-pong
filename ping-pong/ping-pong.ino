 // количество кнопок
 #define COUNT_BUTTONS 2
 // количество светодиодов
 #define COUNT_LEDS 8
 // Аналоговый вход A0 для подключения потенциометра
 const int POT=0;
 // переменная для хранения значения потенциометра
 int valpot = 0;
 // условная единица мсек/1 шкалы
 int valscale=1000;
// Выводы Arduino для подключения
 // список светодиодов
 int pinleds[8]={4,5,6,7,8,9,10,11};
 // переменная для хранения времени нажатия мсек
 unsigned long alltimepressing[2]={0,0};
 // переменная для хранения позиции, мячика
 int posball=0;
 // направление движения мячика
 int dirball=1;
 // список кнопок
 int pinbuttons[2]={2,3};
 // список сохранения предыдущих состояний кнопок
 int lastbuttons[2]={0,0};
 // список сохранения текущих состояний кнопок
 int currentbuttons[2]={0,0};
 // состояние кнопки
 boolean pressing[2]={false,false};

#define TIME_PRESSING 1000
 #define TIME_UNPRESSING 500

// режим программы
 // 0 - ожидание начала игры
 // 1 - движение мячика
 // 2 - индикация ГОЛ - мигание 3 сек
 // 3 - индикация ПОБЕДА - мигание половины победителя
 int res=1;
 // скорость движения мячика
 unsigned long speedball=0;
 unsigned long millisgoball=0;
 #define MIN_SPEEDBALL 1000
 #define MAX_SPEEDBALL 250
 // служебная для мигания гола
 unsigned long millisres2=0;
 #define TIME_RES2 1000
 unsigned long millisblinkres2=0;
 boolean blinkres2=true;
 #define TIME_BLINK_RES2 200
 // счет
 int counts[2]={0,0};
 #define COUNTS_END 5

void setup()
{
  Serial.begin(9600);
  // настроить выводы Arduino для светодиодов как выходы
  for(int i=0;i<COUNT_LEDS;i++)
    {
    pinMode(pinleds[i], OUTPUT);
    digitalWrite(pinleds[i],LOW);
    }
  //Serial.println("start");
  setleds(posball);
}

void loop() 
{
   // проверка нажатия/отжатия/удерживания кнопок
   for(int i=0;i<COUNT_BUTTONS;i++) 
   { 
     // проверка нажатие 
     currentbuttons[i] = debounce(lastbuttons[i],pinbuttons[i]); 
     if (lastbuttons[i] == 0 && currentbuttons[i] == 1) // если нажатие... 
     { 
       // реакция на нажатие 
       //Serial.print("press=");
       //Serial.println(i); 
       if(res==0) 
       {  
         res=1;
         posball=0;
         dirball=1; 
         pressing[0]=false;
         pressing[1]=false; 
       } 
       if((millis()-alltimepressing[i]>=TIME_UNPRESSING) && pressing[i]==false) 
       {
         alltimepressing[i]=millis();
         pressing[i]=true;
       }
     }
     // проверка удерживание нажатой клавиши
     if (lastbuttons[i] == 1 && currentbuttons[i] == 1) // если нажатие... 
     {
       if((millis()-alltimepressing[i]>=TIME_PRESSING) && pressing[i]==true) 
       {
         alltimepressing[i]=millis();
         pressing[i]=false;
       }
     }
     // проверка отжатие клавиши
     if (lastbuttons[i] == 1 && currentbuttons[i] == 0) // если отжатие... 
     {
       // реакция на отжатие
       //Serial.print("un_press=");
       //Serial.println(i);
       alltimepressing[i]=millis();
       pressing[i]=false;
     }
   lastbuttons[i] = currentbuttons[i];
 }
 // движение мячика
 if(res==1) 
 {
   if((millis()-millisgoball)>=speedball)  
   {
     posball=posball+dirball;
     // гол
     if(posball==7 && pressing[1]==false) 
     {
       res=2;
       posball=7;
       dirball=-1;
       millisres2=millis();
       millisblinkres2=millis();
       blinkres2=true;
       counts[0]++;
       //Serial.println(counts[0]);
       //Serial.println(COUNTS_END);
       //Serial.println(res);
       //Serial.println("======");
       if(counts[0]==COUNTS_END) 
       {
         res=3;
       }
     }
     else if(posball==0 && pressing[0]==false) 
     {
       res=2;
       posball=0;
       dirball=1;
       millisres2=millis();
       millisblinkres2=millis();
       blinkres2=true;
       counts[1]++;
       if(counts[1]==COUNTS_END) 
       {
         res=3;
       }
     }
     // столкновение
     else if(posball==0 && pressing[0]==true) 
     {
       dirball=1;
     }
     
     // столкновение
     else if(posball==7 && pressing[1]==true) 
     {
       dirball=-1;
     }
     millisgoball=millis();
     // определение скорости движения мячика
     speedball=map(analogRead(POT),0,1023,MAX_SPEEDBALL,MIN_SPEEDBALL);
   }
  setleds(posball);
  }
  // ГОЛ !!! - blink
  if(res==2) 
  {
    if(millis()-millisblinkres2>=TIME_BLINK_RES2) 
    {
      blinkres2=!blinkres2;
      digitalWrite(pinleds[posball],blinkres2);
      millisblinkres2=millis();
    }

    if(millis()-millisres2>=TIME_RES2) 
    {
      digitalWrite(pinleds[posball],HIGH);
      res=1;
      delay(500);
      //posball=posball+dirball;
    }
  }
  // ПОБЕДА - конец игры
  if(res==3) 
  {
    setledsvictory();
    delay(5000);
    counts[0]=0;counts[1]=0;
    // выключение светодиодов всех
    for(int i=0;i<8;i++) 
    {
      digitalWrite(pinleds[i],LOW);
    }
    res=0;
  }
  //Serial.print("res=");Serial.println(res);
  //Serial.print("posball=");Serial.println(posball);
}


// (включение-выключение светодиодов)
void setleds(int pos) 
{
  // выключение светодиодов всех
  for(int i=0;i<8;i++) 
  {
    digitalWrite(pinleds[i],LOW);
  }
  // включение светодиода позиции мячика
 
  digitalWrite(pinleds[pos],HIGH);
  /*
  if(pressing[0]==true) 
  {
    digitalWrite(pinleds[0],HIGH);
  }
  if(pressing[1]==true) 
  {
    digitalWrite(pinleds[7],HIGH);
  }
  */
}

// ПОБЕДА (включение-выключение светодиодов 0-3 или 4-7)
void setledsvictory() 
{
  // выключение светодиодов всех
  for(int i=0;i<8;i++) 
  { 
    digitalWrite(pinleds[i],LOW); 
  }
  if(counts[0]>counts[1]) 
  {
    // победитель - игрок 1
    for(int i=0;i<4;i++) 
    {
      digitalWrite(pinleds[i],HIGH);
    }
  } 
  else 
  { 
    // победитель - игрок 2
    for(int i=4;i<8;i++) 
    {
      digitalWrite(pinleds[i],HIGH);
    }  
  }
}

/* Функция сглаживания дребезга
* Принимает в качестве аргумента предыдущее состояние кнопки,
* и выдает фактическое.
*/
int debounce(int last,int pin1) 
{
  int current = digitalRead(pin1); // Считать состояние кнопки
  if (last != current) // если изменилось...
  {
    delay(5); // ждем 5мс
    current = digitalRead(pin1); // считываем состояние кнопки
    return current; // возвращаем состояние кнопки
  }
}
