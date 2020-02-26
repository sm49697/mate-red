/* 
 *  Current controller configuration:
 *  Right Joystick                --> Right Servo                       --> (incomplete, need actual servo)
 *  Right Trigger                 --> Right Motor                       --> (incomplete, need actual motor)
 *  Left Joystick                 --> Left Servo
 *  Left Trigger                  --> Left Motor
 *  D-Pad Up                      --> Back Motor Vertical Push          --> (incomplete, need actual motor)
 *  D-Pad Down                    --> Back Motor Vertical Pull          --> (incomplete, need actual motor)
 *  D-Pad Right                   --> Back Servo Clockwise              --> (incomplete, need actual motor)
 *  D-Pad Left                    --> Back Servo Counter-Clockwise      --> (incomplete, need actual motor)
 */

/**
 * TODO: Debug right servo not working with right joystick
 * TODO: Map horizontal movement of joysticks to act as modifier for strength of motors
 */

// LIBRARY IMPORTS
#include <XBOXONE.h>                        // Library to use the XBox Controller
#include <Servo.h>                          // Library to use servos and motors
#include <Wire.h>                           // Library to be able to communicate with I2C / TWI devices

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

// OBJECTS
USB Usb;                                    // Objoct to establish USB connection between Arduino and computer
XBOXONE Xbox(&Usb);                         // Object to use USB connection with XBox controller
Servo RIGHTMOTOR;                           // Object to control the right motor
Servo RIGHTSERVO;                           // Object to control the right servo
Servo LEFTMOTOR;                            // Object to control the left motor
Servo LEFTSERVO;                            // Object to control the left servo
Servo BACKMOTOR;                            // Object to control the rear motor
Servo BACKSERVO;                            // Object to control the rear servo

//Revision 1.3 (DEV-09947)
#define MAX_RESET         7                 // MAX3421E pin 12
#define MAX_GPX           8                 // MAX3421E pin 17
#define WRITEDELAY        21                // Delay between loops in void loop()

// PIN CONSTANTS
#define LEFTMOTORPIN      6                 // Pin the LEFTMOTOR to a PWM pin
#define LEFTSERVOPIN      5                 // Pin the LEFTSERVO to a PWM pin
#define RIGHTMOTORPIN     9                 // Pin the RIGHTMOTOR to a PWM pin
#define RIGHTSERVOPIN     3                 // Pin the RIGHTMOTOR to a PWM pin
#define BACKMOTORPIN      7                 // Pin the RIGHTMOTOR to a PWM pin
#define BACKSERVOPIN      10                // Pin the RIGHTMOTOR to a PWM pin

// CONTROLLER CONSTANTS
#define TRIGGERMAX        1023              // The maximum value a controller trigger returns when fully pressed
#define TRIGGERMIN        0                 // The minimum value a controller trigger returns when fully released
#define JOYSTICKMAX       32767             // The maximum value a controller joystick returns when fully up
#define JOYSTICKMIN      -32768             // The minimum value a controller joystick returns when fully down
#define TRIGGERDEADZONE   10                // The value that needs to be exceeded to start sending power to the motor
#define JOYSTICKDEADZONE  4000              // The value that needs to be exceeded to start sending power to the servo

// MOTOR AND SERVO CONSTANTS
#define ESCMIN            1100              // The minimum value of writeMicroseconds for the motor to be fully powered in reverse
#define ESCMID            1500              // The default value of writeMicroseconds for the motor to remain still
#define ESCMAX            1900              // The maximum value of writeMicroseconds for the motor to be fully powered
#define WPMIN             900               // The minimum value of writeMicroseconds for the servo to be in min position
#define WPMID             1500              // The default value of writeMicroseconds for the servo to be in equilibrium position
#define WPMAX             2100              // The maximum value of writeMicroseconds for the servo to be in max position
<<<<<<< HEAD

=======
>>>>>>> removed depracated functions, motors now push correct direction

// MOTOR AND SERVO DRIVE SETTINGS
#define ESCDRIVEMIN 1300
#define ESCDRIVEMAX 1700
int rear_servo_setting = 0;


// SETUP

void setup() { 
  Serial.begin(115200);
  attachAndPin();
  serialConnect();
  setAllPwm();
  secondAttachAndPin();
  stopTest();
}

/** Attaches motors and servos to their respective pins on the Arduino
 *    This is needed to be able to send power to each motor and servo. This should only
 *    be called once; it is not needed to set the pins repeatedly.
 */
void attachAndPin() {
  LEFTMOTOR.attach(LEFTMOTORPIN);
  LEFTSERVO.attach(LEFTSERVOPIN);
  RIGHTMOTOR.attach(RIGHTMOTORPIN);
  RIGHTSERVO.attach(RIGHTSERVOPIN);
  BACKMOTOR.attach(BACKMOTORPIN);
  BACKSERVO.attach(BACKSERVOPIN);
  pinMode(MAX_GPX, INPUT);
  pinMode(MAX_RESET, OUTPUT); 
  digitalWrite(MAX_RESET, LOW); 
  delay(20); //wait 20ms 
  digitalWrite(MAX_RESET, HIGH);
  delay(20); //wait 20ms
  Serial.begin(115200);
}

/** Connects to the Serial
 *    This enables the Arduino to send information to the computer. This is needed so
 *    that the driver interface can display the information it receives. This should
 *    only be called once; you do not need to setup the serial once it has already been
 *    set up.
 */
void serialConnect() {
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy a/nd other boards with built-in USB CDC serial connection
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.println("Serial connected");
  Serial.println(F("\r\nXBOX USB Library Started"));
}

/** Set the PWM Frequency of all motors/servos
 *    Emulate analog electricity output for digital pins on all of our servos/motors.
 *    Keeps setup() code clean. Forces PWM at 500Hz for all servos and motors.
 */
void setAllPwm() {
    setPwmFrequency(LEFTMOTORPIN, 64);
    setPwmFrequency(LEFTSERVOPIN, 64);
    setPwmFrequency(RIGHTMOTORPIN, 64);
    setPwmFrequency(RIGHTSERVOPIN, 64);
    setPwmFrequency(BACKSERVOPIN, 64);
    setPwmFrequency(BACKSERVOPIN, 64);
}

/** Used on PWM Digital pins to emulate the electricity output of Analog pins
 *    Without enabling this functionality, the motors and servos would be unable to function
 *    correctly. Likewise, this only needs to be called once (when setting up).
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 6) {
      TCCR3B = TCCR3B & 0b11111000 | mode;
    } else {
      TCCR4B = TCCR4B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
  Serial.print("Set pin ");
  Serial.print(pin);
  Serial.print(" to ");
  Serial.println(divisor);
}

/** Pins the motors and servos for validation
 *    It sends output to the pinned servos and motors just to validate that the motors
 *    and servos have been properly attached to pins.
 */
void secondAttachAndPin() {
  motorPin(LEFTMOTOR, LEFTMOTORPIN);
  servoPin(LEFTSERVO, LEFTSERVOPIN);
  motorPin(RIGHTMOTOR, RIGHTMOTORPIN);
  servoPin(RIGHTSERVO, RIGHTSERVOPIN);
  motorPin(BACKMOTOR, BACKMOTORPIN);
  servoPin(BACKSERVO, BACKSERVOPIN);
}

/** Individually pins motors
 *    Used for convenience, rather than having to retype this block of code multiple times
 *    for each motor.
 */
void motorPin(Servo motor, int pin) {
  motor.attach(pin, ESCMIN, ESCMAX);            // (pin, min pulse width, max pulse width in microseconds)
  delay(200);
  motor.writeMicroseconds(1800);                // throttle init
  delay(200);
  motor.writeMicroseconds(ESCMID);              // stop
  Serial.print("Motor setup to pin ");
  Serial.print(pin);
  Serial.println(" complete.");
}

/** Individually pins servos
 *    Used for convenience, rather than having to retype this block of code multiple times
 *    for each servo.
 */
void servoPin(Servo servo, int pin) {
  servo.attach(pin, WPMIN, WPMAX);
  servo.writeMicroseconds(WPMID);
  delay(200);
  servo.writeMicroseconds(WPMID);
  Serial.print("Servo setup to pin ");
  Serial.print(pin);
  Serial.println(" complete.");
}

void stopTest() {
  Serial.println("starting 7.5s stop test       ||");
  for(int i = 0; i < 30; ++i) {
    delay(250);
    Serial.print("#");
  }
  Serial.println(" | program commencing");
  Serial.println("setup completed");
}













// Driving phase

void loop() {
  Usb.Task();
  if (Xbox.XboxOneConnected) {
    leftJoystick();
    leftTrigger();
    rightJoystick();
    rightTrigger();
    directionalPad();
    Serial.println();
  }
  else {
    Serial.println("ERROR: No controller input");
  }
  delay(WRITEDELAY);
}



/** Maps the right joystick to the right servo
 *    The right joystick powers the right servo. This design choice was made because
 *    the right joystick returns an analog value based on its position from its equilibrium,
 *    which allows us to develop precision when driving the robot. Moving the analog stick
 *    up moves the servo to tilt the motor in a way to push the robot down. Likewise,
 *    moving the analog stick down tilts the motor in a way to push the robot up. This
 *    mirrors the centre stick of a pilot.
 */
void rightJoystick() {
  double joystick_input = Xbox.getAnalogHat(RightHatY);
  Serial.print("RightHatY:");
  Serial.print(joystick_input);
  if (joystick_input > JOYSTICKDEADZONE || joystick_input < -1 * JOYSTICKDEADZONE) {
    double power;
    if(joystick_input > 0) power = map(joystick_input, JOYSTICKDEADZONE, JOYSTICKMAX, WPMID, WPMIN);
    else                   power = map(joystick_input, JOYSTICKMIN, -1*JOYSTICKDEADZONE, WPMAX, WPMIN);

    Serial.print("|");
    Serial.print(power);

    RIGHTSERVO.writeMicroseconds(power);
  }
  else {
    Serial.print("|0");
    RIGHTSERVO.writeMicroseconds(WPMID);
  }
  Serial.print(">");
}

/** Maps the left joystick to the left servo
 *    The left joystick controls the left servo. This design choice was made because
 *    the left joystick returns an analog value based on its position from its equilibrium,
 *    which allows us to develop precision when driving the robot. Moving the analog stick
 *    up moves the servo to tilt the motor in a way to push the robot down. Likewise,
 *    moving the analog stick down tilts the motor in a way to push the robot up. This
 *    mirrors the centre stick of a pilot.
 */
void leftJoystick() {
  double joystick_input = Xbox.getAnalogHat(LeftHatY);
  Serial.print("LeftHatY:");
  Serial.print(joystick_input);
  
  if (joystick_input > JOYSTICKDEADZONE || joystick_input < -1 * JOYSTICKDEADZONE) {
    double power;
    if(joystick_input > 0) power = map(joystick_input, JOYSTICKDEADZONE, JOYSTICKMAX, WPMID, WPMAX);
    else                   power = map(joystick_input, JOYSTICKMIN, -1*JOYSTICKDEADZONE, WPMIN, WPMID);

    Serial.print("|");
    Serial.print(power);

    LEFTSERVO.writeMicroseconds(power);
  }
  else {
    Serial.print("|0");
    LEFTSERVO.writeMicroseconds(WPMID);
  }
  Serial.print(">");
}

/** Maps the right trigger to the right motor
 *    The right trigger powers the right motor. This design choice was made because
 *    the right trigger returns an analog value based on how hard it is being pressed,
 *    which allows us to develop precision while driving the robot. Pressing the left
 *    bumper reverses the motor, allowing the robot to move backwards. However, the
 *    driver must be careful, as incorrect use of the bumper may jolt the robot and 
 *    disorient the driver.
 */
void rightTrigger() {
  double trigger_input = Xbox.getButtonPress(R2);
  Serial.print("R2Trigger:");
  Serial.print(trigger_input);
  if(trigger_input > TRIGGERDEADZONE) {
    double power = map(trigger_input, TRIGGERDEADZONE, TRIGGERMAX, ESCMID, ESCDRIVEMIN);

    RIGHTMOTOR.writeMicroseconds(power);

    Serial.print("|");
    Serial.print(power);
  }
  else {
    Serial.print("|0");
    RIGHTMOTOR.writeMicroseconds(ESCMID);
  }
  Serial.print(">");
}

/** Maps the left trigger to the left motor
 *    The left trigger powers the left motor. This design choice was made because
 *    the left trigger returns an analog value based on how hard it is being pressed,
 *    which allows us to develop precision while driving the robot. Pressing the
 *    left bumper reverses the motor, allowing the robot to move backwards. However,
 *    the driver must be careful, as incorrect use of the bumper may jolt the robot
 *    and disorient the driver.
 */
void leftTrigger() {
  double trigger_input = Xbox.getButtonPress(L2);
  Serial.print("L2Trigger:");
  Serial.print(trigger_input);
  if(trigger_input > TRIGGERDEADZONE) {
    double power = map(trigger_input, TRIGGERDEADZONE, TRIGGERMAX, ESCMID, ESCDRIVEMIN);

    LEFTMOTOR.writeMicroseconds(power);
    //TODO: Bumper take make value negative

    Serial.print("|");
    Serial.print(power);
  }
  else {
    Serial.print("|0");
    LEFTMOTOR.writeMicroseconds(ESCMID);
  }
  Serial.print(">");
}

/** Maps the directional pad to the back motor and servo
 *    The reasoning for this is that the back motors and servos do not require as much 
 *    precision compared to the ones located on the sides. 
 */
void directionalPad() {
  dpadMotor();
  dpadServo();
}

/** Maps the up and down buttons of the directional pad to the motor
 * 
 */
void dpadMotor() {
  if(Xbox.getButtonClick(UP)) {
    BACKMOTOR.writeMicroseconds(ESCDRIVEMAX);
    Serial.print("UP");
  }
  else if (Xbox.getButtonClick(DOWN)) {
    BACKMOTOR.writeMicroseconds(ESCDRIVEMIN);
    Serial.print("DOWN");
  }
  else {
    BACKMOTOR.writeMicroseconds(ESCMID);
    Serial.print("NONE");
  }
  Serial.print(">");
}

/** Maps the right and left buttons of the directional pad to the servo
 *    The back servo does not require as much precision as the side motors. There are
 *    only three settings for the servo: rightmost, leftmost, and equilibrium. Furthermore,
 *    the servo will remain in its current setting unless it is told to change by a
 *    different button. To return to equilibrium, the 'X' button on the controller must be
 *    pressed.
 */
void dpadServo() {
  if(Xbox.getButtonClick(RIGHT))        rear_servo_setting = 1;
  else if (Xbox.getButtonClick(LEFT))   rear_servo_setting = 2;
  else                                  rear_servo_setting = 0;

  if(rear_servo_setting == 0) {
      BACKSERVO.writeMicroseconds(WPMIN);
      Serial.print("LEFT");
  }
  else if (rear_servo_setting == 0) {
      BACKSERVO.writeMicroseconds(WPMAX);
      Serial.print("RIGHT");
  }
  else {
      BACKSERVO.writeMicroseconds(WPMID);
      Serial.print("NONE");
  }

  Serial.print(">");
}
