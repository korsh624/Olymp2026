#include <Servo.h>
Servo wheel;
Servo ESC;
int counter = 0;
void setup() {
  Serial.begin(115200);
  
  wheel.attach(9);
  ESC.attach(10);
  attachInterrupt(0, SpeedTest, FALLING);
  wheel.write(90);
  Serial.println("Testing wheel");
  delay(1000);
  for (int i = 90; i < 120; i++) {
    wheel.write(i);
    delay(100);
  }
  for (int i = 120; i > 70; i--) {
    wheel.write(i);
    delay(100);
  }
  for (int i = 70; i < 90; i++) {
    wheel.write(i);
    delay(100);
  }
  Serial.print("Wheel testing complete");
  delay(1000);
  Serial.println("Testing ESC");
  delay(500);
  Serial.println("Forvard 3 s");
  ESC.writeMicroseconds(1600);
  delay(3000);
  Serial.println("Stop 3 s");
  ESC.writeMicroseconds(1500);
  delay(3000);
  Serial.println("Resvard 3 s");
  ESC.writeMicroseconds(1400);
  delay(3000);
  Serial.println("Stop");
  ESC.writeMicroseconds(1500);
  delay(3000);
  Serial.print("ESC testing complete");
  Serial.print("Encoder state=");
  Serial.print(counter);



}

void loop() {

}
void SpeedTest()
{
  counter++;
}
