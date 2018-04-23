class Encoder {
public:
  int encoderIndex;
  
  int clkState = 1;
  int clkPreviousState = 1;
  int dtState = 1;
  int buttonState = 1;
  int buttonPreviousState = 1;

  bool buttonPressed = false;
  int direction = 0;


  Encoder(int index) {
    encoderIndex = index;
  }

  void readEncoder(int clk, int dt, int sw) {
    clkState = digitalRead(clk);
    dtState = digitalRead(dt);
    buttonState = digitalRead(sw);

    if (buttonState && !buttonPreviousState) {
      Serial.printf("Encoder %d: button", encoderIndex);
      Serial.println("");
      buttonPressed = true;
    }

    if (clkState < clkPreviousState) {
      if (clkState < dtState) {
        Serial.printf("Encoder %d: down", encoderIndex);
        Serial.println("");
        direction = -1;
      }
      else{
        Serial.printf("Encoder %d: up", encoderIndex);
        Serial.println("");
        direction = 1;
      }  
    }
    else {
      direction = 0;
    }

    clkPreviousState = clkState;
    buttonPreviousState = buttonState;
  }  
};

