#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32_Servo.h>

class Encoder;

////////////////////////////////////////////////////////////////////////
// Pin Definition

#define ENCODER_1_CLK 32
#define ENCODER_1_DT 21
#define ENCODER_1_SW 25

#define ENCODER_2_CLK 33
#define ENCODER_2_DT 19
#define ENCODER_2_SW 26

#define ENCODER_3_CLK 4
#define ENCODER_3_DT 18
#define ENCODER_3_SW 14

#define ENCODER_4_CLK 34
#define ENCODER_4_DT 2
#define ENCODER_4_SW 35

//WiFiServer server(80);

////////////////////////////////////////////////////////////////////////
// LCD Display

LiquidCrystal_I2C lcd(0x27,16,2);//set the LCD address to 0x27 for a 16 chars and 2 line display

void displayStrings(String text1, String text2, LiquidCrystal_I2C& lcd) {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text1);
  lcd.setCursor(0,1);
  lcd.print(text2);
}


////////////////////////////////////////////////////////////////////////
// LED diods

int LED1 = 23;
int LED2 = 27;
int LED3 = 22;
int LED4 = 15 ;
int LED5 = 5;

int freq = 5000;

int led1Channel = 0;
int led2Channel = 1;
int led3Channel = 2;
int led4Channel = 3;
int led5Channel = 4;

int resolution = 8;

int dutyCycle = 0;

////////////////////////////////////////////////////////////////////////
// Movement

// Initial values of X, Y, Z
const int startX = 90;
const int startY = 11;
const int startZ = 0;
const int startAngle = 90;

// Current input positions
int currentInputX = startX;
int currentInputY = startY;
int currentInputZ = startZ;
int currentInputAngle = startAngle;
bool currentlyPumpEnabled = false;

// Timestamps of X, Y, Z updates
long currentInputXUpdate = 0;
long currentInputYUpdate = 0;
long currentInputZUpdate = 0;
long currentInputAngleUpdate = 0;

// Last input positions
int lastInputX = 90;
int lastInputY = 90;
int lastInputZ = 90;
int lastInputAngle = 90;
bool beforePumpEnabled = false;

// Limits for input
const int minInputX = 0;
const int maxInputX = 180;
const int minInputY = 0;
const int maxInputY = 200;  // 16
const int minInputZ = 0;   // 6
const int maxInputZ = 200;  // 20
const int minInputAngle = 0;
const int maxInputAngle = 180;

// Real float position
float realX = startX;
float realY = startY;
float realZ = startZ;
float realAngle = startAngle;

// Limits for real movement
const float minRealX = 0;
const float maxRealX = 180;
const float minRealY = 3;
const float maxRealY = 25;  // 16
const float minRealZ = 10;   // 6
const float maxRealZ = 30;  // 20
const float minRealAngle = 0;
const float maxRealAngle = 180;


////////////////////////////////////////////////////////////////////////
// Servo Angles

float servo1Angle = 90;
float servo2Angle = 90;
float servo3Angle = 90;
float servo4Angle = 90;

// Immediate results of conversion
float convertedServo1Angle = 90;
float convertedServo2Angle = 90;
float convertedServo3Angle = 90;
float convertedServo4Angle = 90;

// Previous angles for movement
float lastServo1Angle = 90;
float lastServo2Angle = 90;
float lastServo3Angle = 90;
float lastServo4Angle = 90;

float nextServo1Angle = 90;
float nextServo2Angle = 90;
float nextServo3Angle = 90;
float nextServo4Angle = 90;

const float minServo1Angle = 0;
const float maxServo1Angle = 180;
const float minServo2Angle = 0;
const float maxServo2Angle = 110;  // 16
const float minServo3Angle = 0;   // 6
const float maxServo3Angle = 112;  // 20
const float minServo4Angle = 0;
const float maxServo4Angle = 180;

// PWM Related
const int minPulseWidth = 500;
const int maxPulseWidth = 2500;

const int minEpsilon = 0;
const int maxEpsilon = 90;
const int minGama = 0;
const int maxGama = 90;
const int minDelta = 0;
const int maxDelta = 90;

const double baseHeight = 11;         // vyska zakladny 11 cm
const double armSegmentLength = 20;   // delka casti ramena 20 cm


////////////////////////////////////////////////////////////////////////
// Servos

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;


////////////////////////////////////////////////////////////////////////
// State

#define ST_INITIAL          0
#define ST_MAIN_MENU        1

#define ST_RESET_POSITION   10
#define ST_PLAY_PROGRAM     20
#define ST_CREATE_PROGRAM   30
#define ST_CREATE_PROGRAM_CONFIRM_STEP   31
#define ST_MANUAL_MODE      40
#define ST_BLUETOOTH_MODE   50
#define ST_DEMO             60

#define MENU_RESET_POSITION 0
#define MENU_PLAY_PROGRAM   1
#define MENU_CREATE_PROGRAM 2
#define MENU_MANUAL_MODE    3
#define MENU_BLUETOOTH_MODE 4
#define MENU_DEMO           5

int currentState = ST_INITIAL;
int selectedMenuItem = 0;
int menuOffset = 0;

bool refreshDisplay = true;

////////////////////////////////////////////////////////////////////////
// Program


#define STEP_INITIAL 0
#define STEP_PAUSE_BEFORE 1
#define STEP_MOVEMENT 2
#define STEP_PAUSE_AFTER 3

#define MOVE_BEGIN 0
#define MOVE_IN_PROGRESS 1
#define MOVE_FINISHED 2

int movePhase = MOVE_FINISHED;
const int moveStepDuration = 1500;

const int maxStepCount = 20;

struct ProgramStep {
  int x;
  int y;
  int z;
  int angle;
  bool pump;
  int duration;
  int pauseBefore;
  int pauseAfter;
};

struct ProgramStep program[maxStepCount];

int programStepCount = 0;
int currentStep = -1;
int currentStepPhase = STEP_INITIAL;
unsigned long currentStepBegin = 0;

//String stepNumber = "";
