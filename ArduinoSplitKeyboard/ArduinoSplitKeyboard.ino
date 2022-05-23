#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
//#include "config_keymap_left.h"
#include "config_keymap_right.h"

/** base configuration */
const boolean circleBacklight = true;
// default led setting
short _r = 30;
short _g = 50;
short _b = 80;

int minLatency = 20;

/* 
 * its quite impossible to debug when the arduino is sending actual keypresses.  
 * So we provide debug modes for
 * Hardware - early state debugging - test your hardware, see how signals are routed
 * Keymap - final state, to test and edit the productive keyboard layout
 * 
 * Note that enabling both at the same time might lead to missleading prints
 */
const boolean debugHardware = false;
const boolean debugKeymap = false;

// dynamic variables
int outPinIndex = 0;
int (*activeMap)[7] = baseLayer;
short activeLayer = 0;
short pressed[5][7];
boolean ledsOn = true;
boolean isLayer3Locked = false; // layer 3 (arrows left / num right) has proven useful with a lock function


/**
 * INIT
 */
void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  // others in by default
  
  // output pins
  for(int i=0; i<numOutPins; i++) {
    pinMode(outPins[i], OUTPUT);
  }

  if(circleBacklight) {
    pixels.begin(); // Init NeoPixel
    setLEDMode(1);
  }

  if(debugKeymap || debugHardware) {
    Serial.begin(9600);
  } else {
    Keyboard.begin();  
  }
  changeKeymap(baseLayer);
}

/**
 * MAIN LOOP
 */
void loop() {

  if(debugHardware) {
    delay(400); // slow it down for hardware debugging
  }
  
  scanLine(outPins[outPinIndex]);
  
  if(outPinIndex >= numOutPins-1) {
    outPinIndex = 0;
  } else {
    outPinIndex++;
  }
}

inline void scanLine(int outPin) { 
  // BEGIN SCAN
  digitalWrite(outPin, HIGH);
  scanRow(outPin);
  digitalWrite(outPin, LOW);
  //END SCAN
}

//
inline int scanRow(int outPin) {

  if(debugHardware) {
    serialPrintHardwareDebug(outPin);
  }

  int row = getKeymapRowFor(outPin);
  
  if(digitalRead(col1) == HIGH) {
    onKeyPressed(row, 0);
  } else {
    onKeyReleased(row, 0);
  }
  
  if(digitalRead(col2) == HIGH) {
    onKeyPressed(row, 1);
  } else {
    onKeyReleased(row, 1);
  }
  
  if(digitalRead(col3) == HIGH) {
    onKeyPressed(row, 2);
  } else {
    onKeyReleased(row, 2);
  }
  
  if(digitalRead(col4) == HIGH) {
    onKeyPressed(row, 3);
  } else {
    onKeyReleased(row, 3);
  }
  
  if(digitalRead(col5) == HIGH) {
    onKeyPressed(row, 4);
  } else {
    onKeyReleased(row, 4);
  }
  
  if(digitalRead(col6) == HIGH) {
    onKeyPressed(row, 5);
  } else {
    onKeyReleased(row, 5);
  }
  
  if(digitalRead(col7) == HIGH) {
    onKeyPressed(row, 6);
  } else {
    onKeyReleased(row, 6);
  }
}

/*
 * Wires the hardware to the logically assigned mapped key in config_map
 * Note that we dont want to press on each cycle, but register press once - then, 
 * if the state changes, send a "released" signal to the os. 
 * So the state is stored in a matrix and checked for changes
 */
inline int onKeyPressed(int row, int col) {

  if(pressed[row][col] == 0) {
              
    // was not yet pressed -> press it!
    int key = activeMap[row][col];

    // function might change assignment
    key = handleModifiers(key, true);
    key = handleShortcuts(key, true);

    // safe to matrix to register state changes
    // on keymap change (resets pressed states), doing this here (afterwards) will register 
    // a pressed modifier key
    pressed[row][col] = millis();
    
    if(debugKeymap) {
      serialPrintKeymapDebug(row, col, key);
    }

    if( key != 0 && !debugKeymap && !debugHardware) {
      // send the keypress to the OS
      Keyboard.press(key);
    }
  }
}

inline int onKeyReleased(int row, int col) {
  
  if(pressed[row][col] != 0) {
    
    int key = activeMap[row][col];
    key = handleModifiers(key, false);
    key = handleShortcuts(key, false);

    // was pressed -> release it!
    pressed[row][col] = 0;

    if(key != 0) {
      Keyboard.release(key);
    }
  }      
}

inline int handleShortcuts(int key, boolean on) {
  // Umlaute with us international Keyboard layout
  if(key == KEY_OE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(112);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(112);
      //Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_AE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(113);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(113);
      //Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_UE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(121);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(121);
      //Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_S) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(115);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(115);
      //Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_WORD_FORWARD) {
     if(on) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_RIGHT_ARROW);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(KEY_LEFT_CTRL);
      //Keyboard.release(KEY_RIGHT_ARROW);
    }
    return 0;
  }
  if(key == KEY_WORD_BACK) {
     if(on) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ARROW);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(KEY_LEFT_CTRL);
      //Keyboard.release(KEY_LEFT_ARROW);
    }
    return 0;
  }
  if(key == UNDO) {
     if(on) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(122);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(KEY_LEFT_CTRL);
      //Keyboard.release(122);
    }
    return 0;
  }
  if(key == REDO) {
     if(on) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(122);
      delay(10);
      Keyboard.releaseAll();
    } else {
      //Keyboard.release(KEY_LEFT_CTRL);
      //Keyboard.release(KEY_LEFT_SHIFT);
      //Keyboard.release(122);
    }
    return 0;
  }
  return key;
}

inline int handleModifiers(int key, boolean on) {

  /* LED variations */
  if(key == KEY_TOGGLE_LED) {
    if(on) {
      setLEDMode(-1);
      ledsOn = !ledsOn;
    }
    return 0;
  }

  if(key == INCREASE_LED || key == DECREASE_LED) {
    if(on) {
      setLEDMode(key);
    }
    return 0;
  }

  if(key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT) {
    if(on) {
      setLEDMode(key);
    } else {
      setLEDMode(1);
    }
    return key;
  }

  /* Layer mods -> set active keymap*/
  if(key == KEY_MOD_LAYER3) {
    if(on && activeLayer != 3) {
      changeKeymap(neo2_layer3);
      setLEDMode(key);
      activeLayer = 3;
    } else {
      setLEDMode(1);
      changeKeymap(baseLayer);
      activeLayer = 0;
    }
    return 0; // return 0 both on on/off to prevent ambigous keypress
  }

  if(key == KEY_TOGGLE_LAYER3) {
    if(on && activeLayer != 3) {
      if(!isLayer3Locked) {
        changeKeymap(neo2_layer3);
        setLEDMode(KEY_MOD_LAYER3);
        activeLayer = 3;
      } else {
        setLEDMode(1);
        changeKeymap(baseLayer);
        activeLayer = 0;
      }
      isLayer3Locked = !isLayer3Locked;
    }
    return 0; // return 0 both on on/off to prevent ambigous keypress
  }

  if(key == KEY_MOD_LAYER4) {
    if(on && activeLayer != 4) {
      changeKeymap(neo2_layer4);
      setLEDMode(key);
      activeLayer = 4;
    } else {
      setLEDMode(1);
      changeKeymap(baseLayer);
      activeLayer = 0;
    }
    return 0;
  }

  if(key == KEY_MOD_LAYER5) {
    if(on && activeLayer != 5) {
      changeKeymap(neo2_layer5);
      setLEDMode(key);
      activeLayer = 5;
    } else {
      setLEDMode(1);
      changeKeymap(baseLayer);
      activeLayer = 0;
    }
    return 0;
  }
  
  return key;
}

/**
 * Release all pressed keys, reset pressed matrix and set new layer
 */
void changeKeymap(int (*nextMap)[7]) {
  for(int r=0; r<5; r++) {
    for(int c=0; c<7; c++) {
      pressed[r][c] = 0;
    }
  }
  Keyboard.releaseAll();
  activeMap = nextMap;
}

/**
 * print the active row / col and the resolved char (short format)
 */
void serialPrintKeymapDebug(int row, int col, int assignedChar) {
  Serial.print("row:");
  Serial.print(row);
  Serial.print("|");
  Serial.print("col:");
  Serial.print(col);
  Serial.print("|char:");
  Serial.println(assignedChar);
}

/**
 * Print in pin states in a matrix format, to test the hardware, eg to assign the wiring correctly 
 * or test for dead keys
 */
void serialPrintHardwareDebug(int outPin) {
  short vala0 = digitalRead(col1);
  short vala1 = digitalRead(col2);
  short vala2 = digitalRead(col3);
  short vala3 = digitalRead(col4);
  short vala4 = digitalRead(col5);
  short vala5 = digitalRead(col6);
  short vala6 = digitalRead(col7);

  Serial.print(outPin);
  Serial.print("|");

  Serial.print(vala0);
  Serial.print(vala1);
  Serial.print(vala2);
  Serial.print(vala3);
  Serial.print(vala4);
  Serial.print(vala5);
  Serial.print(vala6);
  Serial.println();
}

/**
 * UTIL FUNCTIONS
 */
void setLEDMode(int mode) {

  if(!ledsOn) {
    return;
  }

  //local settings based on defaults
  short r = _r;
  short g = _g;
  short b = _b;

  switch(mode) {
    case KEY_MOD_LAYER3: 
      r = 0;
      g = 200;
      b = 255;
    break;

    case KEY_MOD_LAYER4:
      r = 190;
      g = 40;
      b = 255;
    break;

    case KEY_MOD_LAYER5:
      r = 255;
      g = 50;
      b = 50;
    break;

    case KEY_LEFT_SHIFT: 
      r=40;
      g=100;
      b=70;
    break;
    case KEY_RIGHT_SHIFT: 
      r=40;
      g=100;
      b=70;
    break;

    case INCREASE_LED:
      _r+=20;
      _g+=20;
      _b+=20;
      if(_r > 255) {
        _r = 255;
        _g = 255;
        _b = 255;
      }
    break;

    case DECREASE_LED:
      _r-=20;
      _g-=20;
      _b-=20;
      if(_r < 0) {
        _r = 0;
        _g = 0;
        _b = 0;
      }
    break;

    case -1: //disable
      r=0;
      g=0;
      b=0;
    break;

    default:
    break;
  }

  if(debugKeymap) {
    Serial.print("led:");
    Serial.print("r g b");
    Serial.print(r);
    Serial.print(" ");
    Serial.print(g);
    Serial.print(" ");
    Serial.println(b);
  }
  pixels.fill(pixels.Color(r,g,b));
  pixels.show();
}
