#include <Servo.h>
Servo wheel;
Servo ESC;
int counter = 0;
void setup() {
  Serial.begin(115200);
  wheel.attach(9);
  ESC.attach(10);
  attachInterrupt(0, SpeedTest, FALLING);
  

}

void loop() {
  wheel.write(90);
  ESC.writeMicroseconds(1500); 
  delay(3000);
  wheel.write(90);
  ESC.writeMicroseconds(1600); 
  delay(3000);
  Serial.println(counter);

}
void SpeedTest()
{ 
  counter++;
}
