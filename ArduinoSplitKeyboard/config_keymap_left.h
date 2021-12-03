/** WS2812 LED backlight config */
// If a LED circle backlight is present, configure this
#define PIN 9 // The Control (I-Pin) of the WS2812 LED / NeoPixel
#define NUMPIXELS 12 // Number od WS2812 LEDs / NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// input pins / columns
const short inTab = 7;
const short inA = A3;
const short inS = A1;
const short inD = 10;
const short inF = 15;
const short inG = 14;
const short inThumb = 16;

// output pins / rows (the pins scanning rows)
const int numOutPins = 5;
short outPins[numOutPins] = { 2, 3, 4, 5, 6 };

/**
 * Physical output pins are, unfortunately, not 
 * directly related to array layout
 * Remap it here to assign the hardware wired row to the resp keymap row
 */
inline int getKeymapRowFor(int outPin) {
  // in this case, it's soldered in ascending order, which makes it easy
  return outPin - 2;
}


// asciitable.com  
// 5 rows, 7 cols
// special chars (marked with §) must still be hardcoded in doPress()
/*const char keyC[5][7] = {
   {'6', '7', '8', '9', '0', '?', '§'},
   {'a', 'z', 'u', 'i', 'o', 'p', '+'},ip
   {'6', 'h', 'j', 'k', 'l', '\r', '-'},
   {'a', 'n', 'm', ',', '.', '§', '#'},
   {'§', ' ', 'd', 'f', 'g', 'y', '<'}
};*/
const int LEDOFFKEY = 557;

boolean ledsOn = false;

boolean isMod4 = false; // Mod4: hold keyRightAlt, lock: NUM
int MOD3 = KEY_CAPS_LOCK; //
int MOD4 = KEY_RIGHT_ALT; //
int MOD4_LOCK = 556; // NUM

const int KEY_OE = 148;
const int KEY_AE = 132;
const int KEY_UE = 129;
const int KEY_S = 225;

const int MOD_RIGHT_MID = 0;
const int MOD_CAPS_LOCK = 0;
const int MOD_QUAD_TL = 0;
const int MOD_QUAD_TR = 0;
const int MOD_QUAD_BL = 0;
const int MOD_QUAD_BR = 0;
const int MOD_WIN = KEY_LEFT_GUI;
const int MOD_FN = 0;
const int MOD_ALT = KEY_LEFT_ALT;


/*
 *https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/ 
 *asciitable.com
 *https://dry.sailingissues.com/us-international-keyboard-layout.html
 */
int defaultMap[5][7] = {
   {KEY_ESC,         49,             50,      51,           52,           53,           54},
   {KEY_TAB,        113,             77,     101,          114,          116,           MOD_RIGHT_MID},
   {MOD_CAPS_LOCK,   97,            115,     100,          102,          103,           MOD_QUAD_TR},
   {KEY_LEFT_SHIFT, 121,            120,      99,          118,           98,           MOD_QUAD_TL},
   {KEY_LEFT_CTRL,MOD_WIN,       MOD_FN, MOD_ALT,           32,  MOD_QUAD_BL,           MOD_QUAD_BR}
};

int keyMod3[5][7] = {
   {LEDOFFKEY,           55,             94,   126,           96,           0,               KEY_BACKSPACE},
   {KEY_DELETE,   33,            60,    62,          61,          38,              35},
   {KEY_HOME,    63,            40,     41,          45,          58,              64},
   {KEY_END,43,            37,  34,           39,           59,     KEY_RIGHT_SHIFT}, // 47
   {MOD3,         32,  KEY_RETURN,  MOD4_LOCK,    MOD4,    KEY_RIGHT_CTRL,  KEY_RETURN} // KEY_END 61 KEY_HOME
};

//NUM
// curr used with mod4 (right alt) -> almost exactly as in neo2 layer 4
int keyMod4[5][7] = {
   {LEDOFFKEY,       55,     56, 57, 47, 42, KEY_BACKSPACE},
   {KEY_TAB,  121,    55, 56, 57, 43, 45},
   {KEY_HOME, 104,   52, 53, 54, 44, 46},
   {KEY_END,    58, 49, 50, 51, 59, KEY_RIGHT_SHIFT}, // 47
   {MOD3,     48,     KEY_RETURN, MOD4_LOCK, MOD4_LOCK, KEY_RIGHT_CTRL, KEY_RETURN} // KEY_END 61 KEY_HOME
};
