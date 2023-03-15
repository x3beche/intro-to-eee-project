#include <Arduino.h>
#include <MeOrion.h>

MeRGBLed led(PORT_3);
MeDCMotor motor1(M1);
MeDCMotor motor2(M2);
unsigned long last_blink;

// digital 14  -> line tracking sensor 1 sol
// digital 15 -> lint tracking sensor 2 sağ

int16_t previous_speed_limit = 0;
int16_t speed_limit = 0;
unsigned long round_time = 0;

void setup()
{
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  Serial.begin(115200);

  led.setColorAt(0, 0, 0, 0);
  led.setColorAt(1, 0, 0, 0);
  led.show();

  delay(7000);
  for (int i; i < 10000; i++)
  {
    while (Serial.available() > 0)
    {
      Serial.read();
    }
  }
}

void setLeds()
{
  if (speed_limit == 0)
  {
    led.setColorAt(0, 5, 0, 0);
    led.setColorAt(1, 5, 0, 0);
  }
  else if (speed_limit == 5)
  {
    led.setColorAt(0, 0, 5, 0);
    led.setColorAt(1, 0, 5, 0);
  }
  else if (speed_limit == 11)
  {
    led.setColorAt(0, 0, 0, 5);
    led.setColorAt(1, 0, 0, 5);
  }
  else if (speed_limit == 20)
  {
    led.setColorAt(0, 5, 5, 5);
    led.setColorAt(1, 5, 5, 5);
  }
  led.show();
}
void setMotorLeft(int value)
{
  motor2.run(-value);
}
void setMotorRight(int value)
{
  motor1.run(value);
}
bool readRightLineTrackingSensor()
{
  return digitalRead(15);
}
bool readLeftLineTrackingSensor()
{
  return digitalRead(14);
}
double calculateSpeedChange(double amountOfChange, unsigned long round_time)
{
  double speedChange;
  unsigned long t = millis() - round_time;

  if (amountOfChange > 0)
  {
    double increasingFunction = 1 / (1 + exp(-5 * (double)t / 1000 + 5));
    speedChange = -(increasingFunction * amountOfChange);
  }
  else
  {
    amountOfChange *= -1;
    double decreasingFunction = -1 / (1 + exp(-5 * (double)t / 1000 + 5)) + 1;
    speedChange = amountOfChange - (decreasingFunction * amountOfChange);
  }
  return speedChange;
}
int16_t speedToMotorSignal(double speed)
{
  return (int16_t)map(speed, 0, 30, 75, 255);
}

void loop()
{
  int right_motor;
  int left_motor;

  while (Serial.available() > 0)
  {
    previous_speed_limit = speed_limit;
    uint16_t readed_speed_limit = Serial.read();
    if (readed_speed_limit != speed_limit)
    {
      if (readed_speed_limit == 1)
      {
        speed_limit = 0;
      }
      else
      {
        speed_limit = readed_speed_limit;
      }
      round_time = millis();
      Serial.println(speed_limit);
    }
  }

  double speed = calculateSpeedChange(speed_limit - previous_speed_limit, round_time);
  int16_t motor_signal = speedToMotorSignal((double)previous_speed_limit - speed);

  if (motor_signal <= 75)
  {
    motor_signal = 0;
  }
  else if (motor_signal >= 255)
  {
    motor_signal = 255;
  }

  right_motor = motor_signal;
  left_motor = motor_signal;

  bool right_sensor = readRightLineTrackingSensor();
  bool left_sensor = readLeftLineTrackingSensor();

  // Line Tracking Sensors
  if (right_sensor && left_sensor)
  {
    right_motor = -120;
    left_motor = -120;
  }
  else if (right_sensor)
  {
    left_motor = 0;
  }

  else if (left_sensor)
  {
    right_motor = 0;
  }

  setMotorLeft(left_motor);
  setMotorRight(right_motor);
  setLeds();
}