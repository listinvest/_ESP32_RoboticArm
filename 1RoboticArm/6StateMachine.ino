#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

bool bluetoothModeSelection = false;
bool bluetoothModeEnabled = false;

String menuItems[] = {"Reset Position", "Play Program", "Create Program", "Manual Mode", "Bluetooth Mode", "Self-Test", "Compactify"};
String testMenuItems[] = {"Servo1 Slow", "Servo2 Slow", "Servo3 Slow", "Servo4 Slow", "Servo1 Fast", "Servo2 Fast", "Servo3 Fast", "Servo4 Fast", "Random coords"};

int stepSize(long before, long after) {
  long difference = after - before;
  if (difference > 500) {
    return 1;
  }
  if (difference > 300) {
    return 3;
  }
  if (difference > 250) {
    return 6;
  }
  if (difference > 180) {
    return 10;
  }
  return 15;
}


void initialState() {

  //("Initial state");

  if (refreshDisplay) {
    displayStrings("FrankArm JVDA-01", "  Macej & Cech", lcd);
    refreshDisplay = false;
  }
 
  if (firstEncoder.buttonPressed) {
    currentState = ST_MAIN_MENU;
    refreshDisplay = true;
    ("Button Pressed");
    delay(200);
  }
}


void mainMenu() {
//  ("Main menu");

  if (firstEncoder.buttonPressed) {
    switch (selectedMenuItem) {
      case MENU_RESET_POSITION:
        currentState = ST_RESET_POSITION;
        refreshDisplay = true;
        delay(200);
        return;
        
      case MENU_PLAY_PROGRAM:
        currentState = ST_PLAY_PROGRAM;
        refreshDisplay = true;
        delay(200);
        return;
        

      case MENU_CREATE_PROGRAM:
        localProgramStepCount = 0;
        localProgramCurrentStep = -1;
        currentState = ST_CREATE_PROGRAM;
        refreshDisplay = true;
        delay(200);
        return;

      case MENU_MANUAL_MODE:
        currentState = ST_MANUAL_MODE;
        refreshDisplay = true;
        delay(200);
        return;

       case MENU_BLUETOOTH_MODE:
        currentState = ST_BLUETOOTH_MODE;
        refreshDisplay = true;
        delay(200);
        return;

      case MENU_SELF_TEST:
        currentState = ST_SELF_TEST;
        refreshDisplay = true;
        delay(200);
        return;

      case MENU_COMPACT_POSITION:
        currentState = ST_COMPACT_POSITION;
        refreshDisplay = true;
        delay(200);
        return;
    }
  }

  if (secondEncoder.buttonPressed) {
    currentState = ST_INITIAL;
    refreshDisplay = true;
    return;
  }
  
  if (refreshDisplay) {
    String first = ((selectedMenuItem == menuOffset) ? "> ": "  ") + menuItems[menuOffset];
    String second = ((selectedMenuItem == menuOffset + 1) ? "> ": "  ") + menuItems[menuOffset + 1];
    displayStrings(first, second, lcd);
    refreshDisplay = false;
  }

  if (firstEncoder.direction < 0) {
    selectedMenuItem = MAX(selectedMenuItem - 1, 0);
    refreshDisplay = true;
  }
  if (firstEncoder.direction > 0) {
    selectedMenuItem = MIN(selectedMenuItem + 1, 6);
    refreshDisplay = true;
  }

  if (selectedMenuItem > menuOffset + 1) {
    menuOffset = selectedMenuItem - 1;
    refreshDisplay = true;
  }
  else if (selectedMenuItem < menuOffset) {
    menuOffset = selectedMenuItem;
    refreshDisplay = true;
  }
}

void resetPosition() {
  selectedInputX = startX;
  selectedInputY = startY;
  selectedInputZ = startZ;
  selectedInputAngle = startAngle;
  currentlyPumpEnabled = false;
  beforePumpEnabled = false;

  startManualMovement(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle, MV_LOCAL_MANUAL);
  currentState = ST_MAIN_MENU;
}


void manualMode() {

  if (refreshDisplay) {
    char firstLine[20];
    sprintf(firstLine, "XYZ:%3d,%3d,%3d", selectedInputX, selectedInputY, selectedInputZ);
    
    char secondLine[20];
    sprintf(secondLine, "A:%-3d Pump:%s #%d", selectedInputAngle, currentlyPumpEnabled ? "1" : "0", localProgramCurrentStep + 1 );
    
    displayStrings(String(firstLine), String(secondLine), lcd);
    refreshDisplay = false;
  }

  if (firstEncoder.direction < 0) {
    int step = stepSize(selectedInputXUpdate, millis());
    selectedInputX = MAX(selectedInputX - step, minInputX);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputXUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }
  if (firstEncoder.direction > 0) {
    int step = stepSize(selectedInputXUpdate, millis());
    selectedInputX = MIN(selectedInputX + step, maxInputX);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputXUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }

  if (secondEncoder.direction < 0) {
    int step = stepSize(selectedInputYUpdate, millis());
    selectedInputY = MAX(selectedInputY - step, minInputY);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputYUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }
  if (secondEncoder.direction > 0) {
    int step = stepSize(selectedInputYUpdate, millis());
    selectedInputY = MIN(selectedInputY + step, maxInputY);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputYUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }

  if (thirdEncoder.direction < 0) {
    int step = stepSize(selectedInputZUpdate, millis());
    selectedInputZ = MAX(selectedInputZ - step, minInputZ);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputZUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }
  if (thirdEncoder.direction > 0) {
    int step = stepSize(selectedInputZUpdate, millis());
    selectedInputZ = MIN(selectedInputZ + step, maxInputZ);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputZUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }

  if (fourthEncoder.direction < 0) {
    int step = stepSize(selectedInputAngleUpdate, millis());
    selectedInputAngle = MAX(selectedInputAngle - step, minInputAngle);
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputZUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }
  if (fourthEncoder.direction > 0) {
    int step = stepSize(selectedInputAngleUpdate, millis());
    if (encoder4normalDirection) {
      selectedInputAngle = MIN(selectedInputAngle + step, maxInputAngle);  
    }
    else {
      selectedInputAngle = MAX(selectedInputAngle - step, 0);
    }
    checkInputCoordinateLimits(selectedInputX, selectedInputY, selectedInputZ, selectedInputAngle);
    selectedInputAngleUpdate = millis();
    lastMovementSource = MV_LOCAL_MANUAL;
    refreshDisplay = true;
    return;
  }

  if (firstEncoder.buttonPressed && currentState == ST_CREATE_PROGRAM) {
    struct ProgramStep newStep;
    newStep.x = selectedInputX;
    newStep.y = selectedInputY;
    newStep.z = selectedInputZ;
    newStep.angle = selectedInputAngle;
    newStep.pump = currentlyPumpEnabled;
    newStep.duration = 1000;
    newStep.pauseBefore = 300;
    newStep.pauseAfter = 300;
    localProgramCurrentStep++;
    localProgramStepCount++;
    localProgram[localProgramCurrentStep] = newStep;

    refreshDisplay = true;
    delay(200);
    return;
  }

  // Return to main menu
  if (secondEncoder.buttonPressed) {
    localProgramCurrentStep = -1;
    currentState = ST_MAIN_MENU;
    refreshDisplay = true;
    delay(200);
    return;
  }

  if (thirdEncoder.buttonPressed) {
    currentlyPumpEnabled = !currentlyPumpEnabled;
    refreshDisplay = true;
    lastMovementSource = MV_LOCAL_MANUAL;
    delay(200);
    return;
  }

  if (fourthEncoder.buttonPressed) {
    encoder4normalDirection = !encoder4normalDirection;
  }
}

void bluetoothMode() {
//  Serial.println("Bluetooth mode");
  String line = bluetoothModeSelection ? " [On]     Off" : "  On     [Off]";

  if (refreshDisplay) {
    displayStrings("Bluetooth Mode", line, lcd);
    refreshDisplay = false;
  }

  // Return to main menu
  if (secondEncoder.buttonPressed) {
    currentState = ST_MAIN_MENU;
    refreshDisplay = true;
    delay(200);
    return;
  }

  if (firstEncoder.buttonPressed) {
    bluetoothModeEnabled = bluetoothModeSelection;

    if (bluetoothModeEnabled) {
      enableBluetooth();
    }
    currentState = ST_MAIN_MENU;
    
    refreshDisplay = true;
    delay(200);
    return;
    
  }

  if (firstEncoder.direction != 0) {
    bluetoothModeSelection = !bluetoothModeSelection;
    refreshDisplay = true;
  }
}

void confirmProgramStep() {
  String line = "Step stored";

  if (refreshDisplay) {
    displayStrings("Create Program:", line, lcd);
    refreshDisplay = false;
  }
}

void selfTest() {
  if (firstEncoder.buttonPressed) {
    currentState = ST_SELF_TEST_PROGRESS;
    testType = selectedTestMenuItem;
    testState = 0;
    refreshDisplay = true;
    delay(200);
    return;
  }

  if (secondEncoder.buttonPressed) {
    currentState = ST_MAIN_MENU;
    refreshDisplay = true;
    return;
  }
  
  if (refreshDisplay) {
    String first = ((selectedTestMenuItem == testMenuOffset) ? "> ": "  ") + testMenuItems[testMenuOffset];
    String second = ((selectedTestMenuItem == testMenuOffset + 1) ? "> ": "  ") + testMenuItems[testMenuOffset + 1];
    displayStrings(first, second, lcd);
    refreshDisplay = false;
  }

  if (firstEncoder.direction < 0) {
    selectedTestMenuItem = MAX(selectedTestMenuItem - 1, 0);
    refreshDisplay = true;
  }
  if (firstEncoder.direction > 0) {
    selectedTestMenuItem = MIN(selectedTestMenuItem + 1, 8);
    refreshDisplay = true;
  }

  if (selectedTestMenuItem > testMenuOffset + 1) {
    testMenuOffset = selectedTestMenuItem - 1;
    refreshDisplay = true;
  }
  else if (selectedTestMenuItem < testMenuOffset) {
    testMenuOffset = selectedTestMenuItem;
    refreshDisplay = true;
  }
}

void compactPosition() {
  servo1Angle = 90;
  servo2Angle = 90;
  servo3Angle = 90;
  servo4Angle = 90;

  currentState = ST_MAIN_MENU;
}

void processState() {

  switch (currentState) {
    
  case ST_INITIAL: {
      initialState();
      return;
    }

  case ST_MAIN_MENU: {
      mainMenu();
      return;
    }

   case ST_RESET_POSITION: {
      resetPosition();
      return;
   }

  case ST_PLAY_PROGRAM: {
      playLocalProgram();
      return;
    }
    
  case ST_CREATE_PROGRAM: {
      manualMode();
      return;
    }
    
  case ST_MANUAL_MODE: {
      manualMode();
      return;
    }
    
  case ST_BLUETOOTH_MODE: {
      bluetoothMode();
      return;
    }

  case ST_SELF_TEST: {
      selfTest();
      return;
    }

  case ST_SELF_TEST_PROGRESS: {
      selfTestProgress();
      return;
    }


  case ST_COMPACT_POSITION: {
      compactPosition();
      return;
    }
          
  }    
}


