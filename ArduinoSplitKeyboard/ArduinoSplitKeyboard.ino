#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include "config_keymap_left.h"
//#include "config_keymap_right.h"


/** base configuration */
const boolean circleBacklight = true;
// default led setting
short _r = 60;
short _g = 60;
short _b = 60;

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
short pressed[5][7];
boolean ledsOn = true;


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
  
  for(int r=0; r<5; r++) {
    for(int c=0; c<7; c++) {
      pressed[r][c] = 0;
    }
  }
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
  
  if(digitalRead(inTab) == HIGH) {
    onKeyPressed(row, 0);
  } else {
    onKeyReleased(row, 0);
  }
  
  if(digitalRead(inA) == HIGH) {
    onKeyPressed(row, 1);
  } else {
    onKeyReleased(row, 1);
  }
  
  if(digitalRead(inS) == HIGH) {
    onKeyPressed(row, 2);
  } else {
    onKeyReleased(row, 2);
  }
  
  if(digitalRead(inD) == HIGH) {
    onKeyPressed(row, 3);
  } else {
    onKeyReleased(row, 3);
  }
  
  if(digitalRead(inF) == HIGH) {
    onKeyPressed(row, 4);
  } else {
    onKeyReleased(row, 4);
  }
  
  if(digitalRead(inG) == HIGH) {
    onKeyPressed(row, 5);
  } else {
    onKeyReleased(row, 5);
  }
  
  if(digitalRead(inThumb) == HIGH) {
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

    // safe to matrix to register state changes
    pressed[row][col] = 1;

    // function might change assignment
    key = handleModifiers(key, true);
    key = handleShortcuts(key, false);
    
    if(debugKeymap) {
      serialPrintKeymapDebug(row, col, key);
    }

    if(!debugKeymap && ! debugHardware) {
      // send the keypress to the OS
      Keyboard.press(key);  
    }
  }
}

inline int onKeyReleased(int row, int col) {
  
  if(pressed[row][col] == 1) {
    // was pressed -> release it!
    pressed[row][col] = 0;
    
    int key = activeMap[row][col];
    key = handleModifiers(key, false);
    key = handleShortcuts(key, false);
    
    if(!debugKeymap && !debugHardware) {
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
    } else {
      Keyboard.release(112);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_AE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(113);
    } else {
      Keyboard.release(113);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_UE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(121);
    } else {
      Keyboard.release(121);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(key == KEY_S) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(115);
    } else {
      Keyboard.release(115);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  return key;
}

inline int handleModifiers(int key, boolean on) {

  /* LED variations */
  if(key == KEY_TOGGLE_LED && on) {
    ledsOn = !ledsOn;

    // turn off once on toggle
    if(ledsOn) {
      setLEDMode(1);
    } else {
      setLEDMode(-1);
    }
    // reset on toggle
    _r = 60;
    _b = 60;
    _b = 60;
    
    return 0;
  }

  if(ledsOn && on) {
    setLEDMode(key); 
  } else {
    setLEDMode(1); // reset on release
  }

  /* Layer mods -> set active keymap*/
  if(key == KEY_MOD_LAYER3) {
    if(on) {
      activeMap = neo2_layer3;
    } else {
      activeMap = baseLayer;
    }
    return 0;
  }

  if(key == KEY_MOD_LAYER4) {
    if(on) {
      activeMap = neo2_layer4;
    } else {
      activeMap = baseLayer;
    }
    return 0;
  }

  if(key == KEY_MOD_LAYER5) {
    if(on) {
      activeMap = neo2_layer5;
    } else {
      activeMap = baseLayer;
    }
    return 0;
  }
  
  return key;
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
  short vala0 = digitalRead(inTab);
  short vala1 = digitalRead(inA);
  short vala2 = digitalRead(inS);
  short vala3 = digitalRead(inD);
  short vala4 = digitalRead(inF);
  short vala5 = digitalRead(inG);
  short vala6 = digitalRead(inThumb);

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
      r = 100;
      g = 40;
      b = 255;
    break;

    case KEY_MOD_LAYER5:
      r = 255;
      g = 50;
      b = 50;
    break;

    case KEY_LEFT_SHIFT: 
      r=100;
      g=50;
      b=150;
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

    case -1:
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
  
  pixels.setPixelColor(1, pixels.Color(r,g,b));
  pixels.setPixelColor(2, pixels.Color(r,g,b));
  pixels.setPixelColor(3, pixels.Color(r,g,b));
  pixels.setPixelColor(4, pixels.Color(r,g,b));
  pixels.setPixelColor(5, pixels.Color(r,g,b));
  pixels.setPixelColor(6, pixels.Color(r,g,b));
  pixels.setPixelColor(7, pixels.Color(r,g,b));
  pixels.setPixelColor(8, pixels.Color(r,g,b));
  pixels.setPixelColor(9, pixels.Color(r,g,b));
  pixels.setPixelColor(10, pixels.Color(r,g,b));
  pixels.setPixelColor(11, pixels.Color(r,g,b));
  pixels.setPixelColor(12, pixels.Color(r,g,b));
  pixels.show();

}
