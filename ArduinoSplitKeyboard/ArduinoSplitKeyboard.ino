#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include "config_keymap_left.h"
//#include "config_keymap_right.h"


/** base configuration */
boolean circleBacklight = true;

/* 
 * its quite impossible to debug when the arduino is sending actual keypresses.  
 * So we provide debug modes for
 * Hardware - early state debugging - test your hardware, see how signals are routed
 * Keymap - final state, to test and edit the productive keyboard layout
 * 
 * Note that enabling both at the same time might lead to missleading prints
 */
const boolean debugHardware = false;
const boolean debugKeymap = true;

// dynamic variables
int outPinIndex = 0;
int (*activeMap)[7] = defaultMap;
short pressed[5][7];


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
  }

  if(debugKeymap || debugHardware) {
    Serial.begin(9600);
  }
  Keyboard.begin();

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
  setLEDMode(0);
  
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
    int c = activeMap[row][col];
    //c = handleModifiers(c, true);
    
    if(debugKeymap) {
      serialPrintKeymapDebug(row, col, c);
    }

    if(!debugKeymap && ! debugHardware) {
      // send the keypress to the OS
      Keyboard.press(c);  
    }

    // safe to matrix to register state changes
    pressed[row][col] = 1;  
  }
}

inline int onKeyReleased(int row, int col) {
  
  if(pressed[row][col] == 1) {
    // was pressed -> release it!
    int c = activeMap[row][col];
    //c = handleModifiers(c, false);
    
    if(!debugKeymap && !debugHardware) {
      Keyboard.release(c);
    }

    // safe to matrix to register state changes
    pressed[row][col] = 0;  
  }
}



inline void setMod4(boolean on) {
    if(on) {
      activeMap = keyMod4;
    } else {
      activeMap = defaultMap;
    }
}

inline int handleModifiers(int dec, boolean on) {

  if(dec == LEDOFFKEY && on) {
    ledsOn = !ledsOn;
    return 0;
  }

  boolean isModifierKey = dec == MOD4 || dec == MOD3 || dec == KEY_DELETE;

   if(ledsOn && isModifierKey) {
     setLEDMode(dec);
   }

    // Umlaute
  if(dec == KEY_OE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(112);
    } else {
      Keyboard.release(112);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(dec == KEY_AE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(113);
    } else {
      Keyboard.release(113);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(dec == KEY_UE) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(121);
    } else {
      Keyboard.release(121);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }
  if(dec == KEY_S) {
    if(on) {
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(115);
    } else {
      Keyboard.release(115);
      Keyboard.release(KEY_RIGHT_ALT);
    }
    return 0;
  }

  if(dec == MOD3) {
    if(on) {
      activeMap = keyMod3;
    } else {
      activeMap = defaultMap;
    }
    return 0;
  }
  if(dec == KEY_RIGHT_ALT) {
    setMod4(on);
  }
  // toggle MOD4 ("Numlock") state
  if(dec == MOD4_LOCK && on) {
    if(!isMod4) {
      setMod4(true);
      isMod4 = true;
    } else {
      setMod4(false);
      isMod4 = false;
    }
    return 0;
  }
  return dec;
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

  short r = 60;
  short g = 60;
  short b = 60;

  if(mode == KEY_RIGHT_SHIFT) {
    g = 230;
  } else if(mode == MOD3) {
    r = 140;
    g = 130;
  } else if(mode == KEY_DELETE) {
    r = 255;
    g = 0;
    b = 0;
  }

  if(isMod4) {
    b = 255;
    g = 100;
    r = 0;
  }
  
  if(!ledsOn) {
    r = 0;
    g = 0;
    b = 0;
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
