/**
 * @file KnightRiderAdventure.ino
 * @brief Arduino code for an adventurous Knight Rider-inspired smart car.
 * @author Md. Faisal Islam <faisal.islam70@gmail.com>
 * @date 2023-09-04
 * @license MIT
 *
 * @details
 * This Arduino sketch brings the motor car an intelligent car
 * equipped with ultrasonic sensors, motors, and a narrative code structure.
 *
 * @hardware
 * - Arduino board (compatible with AFMotor and NewPing libraries)
 * - Ultrasonic sensors (HC-SR04 or similar)
 * - Motor controller (Adafruit Motor Shield | L293D Motor Shield)
 * - LiquidCrystal_I2C display (for debugging and storytelling)
 *
 * @dependencies
 * - AFMotor library: https://github.com/adafruit/Adafruit-Motor-Shield-library
 * - NewPing library: https://bitbucket.org/teckel12/arduino-new-ping
 * - LiquidCrystal_I2C library (for the LCD display): https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
 */

/**----------------------------------------------------------
| Include necessary libraries and define pins and constants
------------------------------------------------------------*/
#include <NewPing.h>
#include <AFMotor.h>
#include <LiquidCrystal_I2C.h>

#define FRONT_TRIGGER_PIN A1
#define FRONT_ECHO_PIN A0
#define LEFT_TRIGGER_PIN A4
#define LEFT_ECHO_PIN A5
#define RIGHT_TRIGGER_PIN A2
#define RIGHT_ECHO_PIN A3

#define MAX_DISTANCE 300


/**----------------------------------------------------------
| Initialize objects and setup functions
------------------------------------------------------------*/

NewPing frontSensor(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN, MAX_DISTANCE);
NewPing leftSensor(LEFT_TRIGGER_PIN, LEFT_ECHO_PIN, MAX_DISTANCE);
NewPing rightSensor(RIGHT_TRIGGER_PIN, RIGHT_ECHO_PIN, MAX_DISTANCE);

AF_DCMotor leftMotor(2, MOTOR12_64KHZ);
AF_DCMotor rightMotor(4, MOTOR34_64KHZ);

LiquidCrystal_I2C lcd(0x27,20,4);
int debugMode = 0;

class Car {
private:
    int averageSpeed = 150;
    int maxSpeed = 250;
    int minSpeed = 125;

    static void drive(int speed) {
        leftMotor.setSpeed(speed);
        rightMotor.setSpeed(speed);
        leftMotor.run(FORWARD);
        rightMotor.run(FORWARD);
    }

    void setTopSpeed() {
        leftMotor.setSpeed(maxSpeed);
        rightMotor.setSpeed(maxSpeed);
    }

    void setOptimalSpeed() {
        leftMotor.setSpeed(averageSpeed);
        rightMotor.setSpeed(averageSpeed);
    }

public:
    Car() {
        leftMotor.setSpeed(minSpeed);
        rightMotor.setSpeed(minSpeed);
    }

    void changeGear(int gear) {
        switch (gear) {
            default:
            case 1:
                averageSpeed = 125;
                break;
            case 2:
                averageSpeed = 150;
                break;
            case 3:
                averageSpeed = 175;
                break;
            case 4:
                averageSpeed = 200;
                break;
        }

        drive(averageSpeed);
    }

    static void reverse() {
        leftMotor.run(BACKWARD);
        rightMotor.run(BACKWARD);
    }

    void brake() const {
        for (int i = averageSpeed; i >= 0; i -= 10) {
            drive(i);
            delay(1);
        }
    }

    static void neutral() {
        leftMotor.run(RELEASE);
        rightMotor.run(RELEASE);
    }

    void leftTurn() {
        neutral();
        setTopSpeed();
        leftMotor.run(BACKWARD);
        rightMotor.run(FORWARD);
        delay(500);
        neutral();
        delay(500);
    }

    void rightTurn() {
        neutral();
        setTopSpeed();
        leftMotor.run(FORWARD);
        rightMotor.run(BACKWARD);
        delay(500);
        neutral();
        delay(500);
    }

    void leftUTurn() {
        neutral();
        setTopSpeed();
        leftMotor.run(BACKWARD);
        rightMotor.run(FORWARD);
        delay(1250);
        neutral();
        delay(1000);
    }

    void rightUTurn() {
        neutral();
        setTopSpeed();
        leftMotor.run(FORWARD);
        rightMotor.run(BACKWARD);
        delay(1250);
        neutral();
        delay(1000);
    }

    void leftLaneOvertake() {
        leftMotor.setSpeed(0);
        rightMotor.setSpeed(200);
        leftMotor.run(BACKWARD);
        rightMotor.run(FORWARD);
        delay(250);
        rightMotor.setSpeed(0);
        leftMotor.setSpeed(200);
        delay(250);
        setOptimalSpeed();
        changeGear(3);
    }

    void rightLaneOvertake() {
        leftMotor.setSpeed(200);
        rightMotor.setSpeed(0);
        leftMotor.run(FORWARD);
        rightMotor.run(BACKWARD);
        delay(250);
        rightMotor.setSpeed(200);
        leftMotor.setSpeed(0);
        delay(250);
        setOptimalSpeed();
        changeGear(3);
    }
};

class Eyes {
private:
    int front{};
    int left{};
    int right{};

    void openEyesWide() {
        measureFrontDistance();
        measureLeftDistance();
        measureRightDistance();

        if (debugMode) {
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0,1);
            lcd.print(left);
            lcd.setCursor(6, 1);
            lcd.print(front);
            lcd.setCursor(13, 1);
            lcd.print(right);
        }

    }

public:
    static const int CLEAR_PATH_AHEAD = 1;
    static const int PATH_CLOSING_UP = 2;
    static const int PATH_BLOCKED = 3;
    static const int NARROW_ROAD_LEFT_RIGHT = 30;
    static const int CLEAR_PATH_LEFT = 40;
    static const int CLEAR_PATH_RIGHT = 50;
    static const int SEEK_ALTERNATE_ROUTE = 60;
    Eyes() = default;

    int measureFrontDistance() {
        int sum = 0;
        int count = random(3,6);
        for(int i = 0; i < count; i++) {
            sum += frontSensor.ping_cm();
            delay(5);
        }

        if(front = sum/count) {
            return front;
        }

        return measureFrontDistance();
    }

    int measureLeftDistance() {
        int sum = 0;
        int count = random(3,6);
        for(int i = 0; i < count; i++) {
            sum += leftSensor.ping_cm();
            delay(5);
        }

        return left = sum/count;
    }

    int measureRightDistance() {
        int sum = 0;
        int count = random(3,6);
        for(int i = 0; i < count; i++) {
            sum += rightSensor.ping_cm();
            delay(5);
        }

        return right = sum/count;
    }

    int suggestGear() {

        int distance = measureFrontDistance();

        if (distance >= 275) {
            return 4;
        } else if (distance >= 200) {
            return 3;
        } else if (distance >= 100) {
            return 2;
        } else {
            return 1;
        }
    }

    // This method calculates the best path for next drive.
    int commonSense() {
        openEyesWide();

        if (front > 50) {
            // Front path open enough for drive
            return CLEAR_PATH_AHEAD;
        } else if (front > 40) {
            // Find alternative route as soon as possible
            return SEEK_ALTERNATE_ROUTE;
        }
        else if (front > 30) {
            // Front path closing for drive
            return PATH_CLOSING_UP;
        } else {
            // Front path blocked need to move left or right

            // @todo - Overtaking functionality coming soon; for now, let's explore turns

            if (left < 8 && right < 8) {
                // Car enters inside a narrow road.
                return NARROW_ROAD_LEFT_RIGHT;
            } else if (left >= 8 && left > right) {
                // Left side has some space to drive
                return CLEAR_PATH_LEFT;
            } else if (right >= 8 && right > left) {
                // Right side has some space to drive
                return CLEAR_PATH_RIGHT;
            }
        }

    }

    int seekAlternateRoute() {
        openEyesWide();
        if (front <= 40 && (left > 80 || right > 80)) {
            if(left > right) {
                return CLEAR_PATH_LEFT;
            } else {
                return CLEAR_PATH_RIGHT;
            }
        }

        return NARROW_ROAD_LEFT_RIGHT;
    }
};

Car car;
Eyes eyes;

unsigned long nonce;
int beforeEyes[3];
int longVision;
int previousSense;


/**----------------------------------------------------------
| Helper functions and rescue mission
------------------------------------------------------------*/
void csf() {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
}

void rescueMission() {
    if (millis() - nonce > 5000) {
        int currentEyes[3] = {
                eyes.measureFrontDistance(),
                eyes.measureLeftDistance(),
                eyes.measureRightDistance()
        };

        int frontTh = 10;
        int longVisionTh = 5;
        csf();
        lcd.print("TH=");
        lcd.setCursor(3, 0);
        int df = (int) abs(beforeEyes[0] - currentEyes[0]);
        lcd.print(df);

        if( df <= frontTh ) {
            if (currentEyes[0] > 100 && df <= longVisionTh) {
                longVision++;
            }
            // Most probably car is stuck
            if (longVision > 2 || currentEyes[0] < 100) {
                delay(50);
                car.changeGear(1);
                Car::reverse();
                delay(1000);
                if(eyes.measureLeftDistance() > eyes.measureRightDistance() ) {
                    car.leftUTurn();
                } else {
                    car.rightUTurn();
                }
                longVision = 0;
            }
        }

        nonce = millis();
        beforeEyes[0] = currentEyes[0];
        beforeEyes[1] = currentEyes[1];
        beforeEyes[2] = currentEyes[2];
    }
}


/**----------------------------------------------------------
| Program Initialization
------------------------------------------------------------*/
void setup() {
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Knight Rider!");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("opening eyes");
    delay(3000);

    Car::neutral();

    beforeEyes[0] = eyes.measureFrontDistance();
    beforeEyes[1] = eyes.measureLeftDistance();
    beforeEyes[2] = eyes.measureRightDistance();

    lcd.setCursor(0, 1);
    lcd.print("                ");
    debugMode = 1;

    nonce = millis();
    longVision = 0;
}

/**----------------------------------------------------------
| Main loop for the Knight Rider adventure
------------------------------------------------------------*/
void loop() {
    // Periodic rescue mission of the Knight Rider
    rescueMission();

    int commonSense = eyes.commonSense();
    char commonSenseText[2];
    switch(commonSense) {
        case Eyes::CLEAR_PATH_AHEAD:
            commonSenseText[0] = 'F';
            commonSenseText[1] = 'S';
            break;
        case Eyes::PATH_CLOSING_UP:
            commonSenseText[0] = 'S';
            commonSenseText[1] = 'L';
            break;
        case Eyes::SEEK_ALTERNATE_ROUTE:
            commonSenseText[0] = 'A';
            commonSenseText[1] = 'R';
            break;
        case Eyes::CLEAR_PATH_LEFT:
            commonSenseText[0] = 'L';
            commonSenseText[1] = 'T';
            break;
        case Eyes::CLEAR_PATH_RIGHT:
            commonSenseText[0] = 'R';
            commonSenseText[1] = 'T';
            break;
        case Eyes::NARROW_ROAD_LEFT_RIGHT:
            commonSenseText[0] = 'U';
            commonSenseText[1] = 'T';
            break;
    }

    if (previousSense != commonSense) {
        csf();
        lcd.print("Knight Rider!");
        lcd.setCursor(14, 0);
        lcd.print(commonSenseText);
    } else {
        lcd.setCursor(14, 0);
        lcd.print(commonSenseText);
    }
    previousSense = commonSense;

    switch(commonSense) {
        case Eyes::CLEAR_PATH_AHEAD:
            car.changeGear(eyes.suggestGear());
            break;
        case Eyes::PATH_CLOSING_UP:
            car.changeGear(1);
            break;
        case Eyes::SEEK_ALTERNATE_ROUTE:
            Car::neutral();
            delay(500);
            switch(eyes.seekAlternateRoute()) {
                default:
                    car.changeGear(1);
                    break;
                case Eyes::CLEAR_PATH_LEFT:
                    car.changeGear(1);
                    Car::reverse();
                    delay(500);
                    car.leftTurn();
                    car.changeGear(1);
                    delay(250);
                    break;
                case Eyes::CLEAR_PATH_RIGHT:
                    car.changeGear(1);
                    Car::reverse();
                    delay(500);
                    car.rightTurn();
                    car.changeGear(1);
                    delay(250);
                    break;
            }
            break;
        case Eyes::NARROW_ROAD_LEFT_RIGHT:
            Car::neutral();
            Car::reverse();
            delay(750);
            car.leftUTurn();
            break;
        case Eyes::CLEAR_PATH_LEFT:
            car.changeGear(1);
            Car::reverse();
            delay(250);
            car.leftTurn();
            break;
        case Eyes::CLEAR_PATH_RIGHT:
            car.changeGear(1);
            Car::reverse();
            delay(250);
            car.rightTurn();
            break;
        default:
            Car::neutral();
            delay(100);
            car.rightUTurn();
            delay(200);
            break;
    }
}
