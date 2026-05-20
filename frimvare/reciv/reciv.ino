#include <Servo.h>

// ==========================
// Пины
// ==========================
#define ESC_PIN   10
#define SERVO_PIN 9

Servo esc;
Servo steering;

// ==========================
// Ограничения ESC
// ==========================
// 1500 - стоп
// 1650 - максимум вперед
// 1450 - максимум назад

const int ESC_STOP = 1500;
const int ESC_FORWARD_MAX = 1650;
const int ESC_BACKWARD_MAX = 1450;

// ==========================
// Серво
// ==========================
const int SERVO_CENTER = 90;

int speedValue = ESC_STOP;
int angleValue = SERVO_CENTER;

String inputString = "";

// ==========================
void setup()
{
    Serial.begin(115200);

    esc.attach(ESC_PIN);
    steering.attach(SERVO_PIN);

    steering.write(SERVO_CENTER);

    // Нейтраль ESC
    esc.writeMicroseconds(ESC_STOP);

    delay(3000);

    Serial.println("READY");
}

// ==========================
void loop()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            parseData(inputString);
            inputString = "";
        }
        else
        {
            inputString += c;
        }
    }

    esc.writeMicroseconds(speedValue);
    steering.write(angleValue);
}

// ==========================
// Формат:
// speed,angle
//
// speed:
// -100 ... 100
//
// angle:
// 45 ... 135
// ==========================
void parseData(String data)
{
    int commaIndex = data.indexOf(',');

    if (commaIndex == -1)
        return;

    int speedPercent = data.substring(0, commaIndex).toInt();
    int angle = data.substring(commaIndex + 1).toInt();

    speedPercent = constrain(speedPercent, -100, 100);
    angle = constrain(angle, 45, 135);

    // ==========================
    // ESC
    // ==========================
    if (speedPercent > 0)
    {
        speedValue = map(
            speedPercent,
            0,
            100,
            ESC_STOP,
            ESC_FORWARD_MAX
        );
    }
    else
    {
        speedValue = map(
            speedPercent,
            -100,
            0,
            ESC_BACKWARD_MAX,
            ESC_STOP
        );
    }

    // ==========================
    // Руль
    // ==========================
    angleValue = angle;

    // Debug
    Serial.print("PWM: ");
    Serial.print(speedValue);

    Serial.print("  ANGLE: ");
    Serial.println(angleValue);
}
