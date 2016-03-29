/*
  Робот складчик на Arduino
  Created by Rostislav Varzar
*/
// Определения для портов
#define US1_trigPin A1
#define US1_echoPin A0
#define US2_trigPin A2
#define US2_echoPin A3
#define US3_trigPin A4
#define US3_echoPin A5
#define US4_trigPin 2
#define US4_echoPin 3
#define US5_trigPin 8
#define US5_echoPin 9
#define US6_trigPin 10
#define US6_echoPin 11
#define minimumRange 0
#define maximumRange 200

#define PWMA 5
#define DIRA 4
#define PWMB 6
#define DIRB 7

// Параметры ПИД регулятора
#define KPID 0.75

// Параметры дистанций
#define DIST1 10
#define DIST2 30
#define DIST4 2

// Параметры моторов
#define MPWR 80

// Параметры заедания
#define DIST3 2

// Задержка между считываниями датчиков
#define USDELAY 25

// Задержка между столкновениями
#define CDELAY 250

// Максимальное количество заеданий
#define COLMAX 5

int flag = 0;
long dist = 0;
long left = 0;
long right = 0;
long olddist = 0;
long oldleft = 0;
long oldright = 0;
long colcounter = 0;
float u = 0;

void setup()
{
  // Инициализация последовательного порта
  Serial.begin(9600);

  // Инициализация выводов для работы с УЗ датчиком
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);
  pinMode(US3_trigPin, OUTPUT);
  pinMode(US3_echoPin, INPUT);
  pinMode(US4_trigPin, OUTPUT);
  pinMode(US4_echoPin, INPUT);
  pinMode(US5_trigPin, OUTPUT);
  pinMode(US5_echoPin, INPUT);
  pinMode(US6_trigPin, OUTPUT);
  pinMode(US6_echoPin, INPUT);

  // Инициализация выходов для управления моторами
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
}


void loop()
{
  /*
    // Тестирование датчиков
    while (true)
    {
      Serial.print(readUS1_distance());
      Serial.print("\t");
      delay(15);
      Serial.print(readUS2_distance());
      Serial.print("\t");
      delay(15);
      Serial.print(readUS3_distance());
      Serial.print("\t");
      delay(15);
      Serial.print(readUS4_distance());
      Serial.print("\t");
      delay(15);
      Serial.print(readUS5_distance());
      Serial.print("\t");
      delay(15);
      Serial.print(readUS6_distance());
      Serial.print("\t");
      delay(15);
      Serial.println("");
    }
  */

  delay(5000);

  while (true)
  {
    // Вперед
    delay(CDELAY);
    colcounter = 0;
    motorA_setpower(MPWR, false);
    motorB_setpower(MPWR, true);
    while (flag == 0)
    {
      Serial.print(flag);
      Serial.print("\t");
      dist = readUS4_distance(); delay(USDELAY);
      left = readUS5_distance(); delay(USDELAY);
      right = readUS3_distance(); delay(USDELAY);
      // Основное движение
      if ((left != (-1)) && (right != (-1)))
      {
        if ((left < DIST2) || (right < DIST2))
        {
          u = KPID * float(left - right);
          motorA_setpower(MPWR + u, false);
          motorB_setpower(MPWR - u, true);
        }
        else
        {
          motorA_setpower(MPWR, false);
          motorB_setpower(MPWR, true);
        }
      }
      else
      {
        motorA_setpower(MPWR, false);
        motorB_setpower(MPWR, true);
      }
      // Проверка на столкновение
      if ((dist < DIST1) && (dist != (-1)))
      {
        flag = 1;
        Serial.print("E1\t");
      }
      if ((left < DIST4) && (left != (-1)))
      {
        flag = 1;
        Serial.print("E2\t");
      }
      if ((right < DIST4) && (right != (-1)))
      {
        flag = 1;
        Serial.print("E3\t");
      }
      // Прверка на заедание
      if ((abs(olddist - dist) < DIST3) && (abs(oldleft - left) < DIST3) && (abs(oldright - right) < DIST3))
      {
        //flag = 1;
        colcounter = colcounter + 1;
        Serial.print("E4\t");
      }
      if (colcounter > COLMAX)
      {
        flag = 1;
        Serial.print("E5\t");
      }
      olddist = dist;
      oldleft = left;
      oldright = right;
      Serial.print(dist);
      Serial.print("\t");
      Serial.print(left);
      Serial.print("\t");
      Serial.print(right);
      Serial.print("\t");
      Serial.print(u);
      Serial.print("\t");
      Serial.print(olddist);
      Serial.print("\t");
      Serial.print(oldleft);
      Serial.print("\t");
      Serial.print(oldright);
      Serial.print("\t");
      Serial.println("");
    }
    // Назад
    delay(CDELAY);
    colcounter = 0;
    motorA_setpower(MPWR, true);
    motorB_setpower(MPWR, false);
    while (flag == 1)
    {
      Serial.print(flag);
      Serial.print("\t");
      dist = readUS1_distance(); delay(USDELAY);
      left = readUS2_distance(); delay(USDELAY);
      right = readUS6_distance(); delay(USDELAY);
      // Основное движение
      if ((left != (-1)) && (right != (-1)))
      {
        if ((left < DIST2) || (right < DIST2))
        {
          u = KPID * float(right - left);
          motorA_setpower(MPWR + u, true);
          motorB_setpower(MPWR - u, false);
        }
        else
        {
          motorA_setpower(MPWR, true);
          motorB_setpower(MPWR, false);
        }
      }
      else
      {
        motorA_setpower(MPWR, true);
        motorB_setpower(MPWR, false);
      }
      // Проверка на столкновение
      if ((dist < DIST1) && (dist != (-1)))
      {
        flag = 0;
        Serial.print("E1\t");
      }
      if ((left < DIST4) && (left != (-1)))
      {
        flag = 0;
        Serial.print("E2\t");
      }
      if ((right < DIST4) && (right != (-1)))
      {
        flag = 0;
        Serial.print("E3\t");
      }
      // Прверка на заедание
      if ((abs(olddist - dist) < DIST3) && (abs(oldleft - left) < DIST3) && (abs(oldright - right) < DIST3))
      {
        //flag = 0;
        colcounter = colcounter + 1;
        Serial.print("E4\t");
      }
      if (colcounter > COLMAX)
      {
        flag = 0;
        Serial.print("E5\t");
      }
      olddist = dist;
      oldleft = left;
      oldright = right;
      Serial.print(dist);
      Serial.print("\t");
      Serial.print(left);
      Serial.print("\t");
      Serial.print(right);
      Serial.print("\t");
      Serial.print(u);
      Serial.print("\t");
      Serial.print(olddist);
      Serial.print("\t");
      Serial.print(oldleft);
      Serial.print("\t");
      Serial.print(oldright);
      Serial.print("\t");
      Serial.println("");
    }
  }
}

// УЗ датчик 1
long readUS1_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US1_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_trigPin, LOW);
  duration = pulseIn(US1_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 2
long readUS2_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US2_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US2_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_trigPin, LOW);
  duration = pulseIn(US2_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 3
long readUS3_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US3_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US3_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US3_trigPin, LOW);
  duration = pulseIn(US3_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 4
long readUS4_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US4_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US4_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US4_trigPin, LOW);
  duration = pulseIn(US4_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 5
long readUS5_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US5_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US5_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US5_trigPin, LOW);
  duration = pulseIn(US5_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 6
long readUS6_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US6_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US6_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US6_trigPin, LOW);
  duration = pulseIn(US6_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// Мощность мотора "A" от -100% до +100% (от знака зависит направление вращения)
void motorA_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRA, LOW);
  }
  else
  {
    digitalWrite(DIRA, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMA, pwmvalue);
}

// Мощность мотора "B" от -100% до +100% (от знака зависит направление вращения)
void motorB_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRB, LOW);
  }
  else
  {
    digitalWrite(DIRB, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMB, pwmvalue);
}

