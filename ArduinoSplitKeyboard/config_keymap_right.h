/** WS2812 LED backlight config */
// If a LED circle backlight is present, configure this
#define PIN 10 // The Control (I-Pin) of the WS2812 LED / NeoPixel
#define NUMPIXELS 12 // Number od WS2812 LEDs / NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// input pins / columns from left to right
const short col1 = 16; // col with z
const short col2 = 15; // h
const short col3 = 14; // j
const short col4 = A0; // k
const short col5 = A1; // l
const short col6 = A2; // :
const short col7 = A3; // "

// output pins / rows (the pins scanning rows)
const int numOutPins = 5;
short outPins[numOutPins] = { 5, 6, 7, 8, 9 };

/**
 * Physical output pins are, unfortunately, not 
 * directly related to array layout
 * Remap it here to assign the hardware wired row to the resp keymap row
 */
inline int getKeymapRowFor(int outPin) {
  // the right side was not soldered in order, so this had to be adjusted
  if(outPin == 5) {
    return 4; // 0 is connected to space / baseline
  }
  if(outPin == 6) {
    return 3; //1 -> n
  }
  if(outPin == 7) {
    return 2; // h-line
  }
  if(outPin == 8) {
    return 1; // z-line
  }
  if(outPin == 9) {
    return 0; // numbers
  }
}

// custom assignments which will be handled in the handle_modifiers function
// int is outside the asci definition
const int KEY_TOGGLE_LED = 999;

// switch between keymaps (programmed neo2 layout)
const int KEY_MOD_LAYER3 = 1000;
const int KEY_MOD_LAYER4 = 1001;
const int KEY_MOD_LAYER5 = 1002; //custom layer to add some keys missing in neo2

const int KEY_TOGGLE_LAYER3 = 1100;

// custom shortcuts
const int KEY_WORD_FORWARD = 1203;
const int KEY_WORD_BACK = 1204;
const int INCREASE_LED = 1208;
const int DECREASE_LED = 1209;
const int UNDO = 1200;
const int REDO = 1201;

/* Special keys involving shortcuts
 * Some chars are best to be created with the international layout and some alt_gr combos
 * https://theasciicode.com.ar/ascii-printable-characters/double-quotes-quotation-mark-speech-marks-ascii-code-34.html
 */
const int KEY_AE = 1005;
const int KEY_UE = 1008;
const int KEY_OE = 1006;
const int KEY_S = 1007;
const int KEY_PARANTHESIS = 1009;
const int KEY_SINGLE_PS = 1010;
const int KEY_EURO = 1011;
const int KEY_PRINT= 1012;



/*
 * NEO2 Quertz Layout
 *
 * Helpful references:
 * https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/ 
 * asciitable.com
 * https://dry.sailingissues.com/us-international-keyboard-layout.html
 * 
 * Important: layer modifiers should be the same on all keymaps, or onRelease will not be correctly identified!
 */
int baseLayer[5][7] = {
   {KEY_TOGGLE_LAYER3,            55,             56,   57,           48,           KEY_S,               KEY_BACKSPACE},
   {KEY_RETURN,   122,            117,  105,          111,          112,              KEY_UE},
   {KEY_DELETE,    104,            106,  107,          108,          KEY_OE,              KEY_AE},
   {KEY_MOD_LAYER5,  110,         109,  44,           46,           47,        KEY_RIGHT_SHIFT}, // 47
   {KEY_MOD_LAYER4,     32,  KEY_MOD_LAYER3,  KEY_MOD_LAYER4, KEY_RIGHT_ALT, KEY_RIGHT_CTRL,     KEY_RETURN} // KEY_END 61 KEY_HOME
};

int neo2_layer3[5][7] = {
   {KEY_TOGGLE_LAYER3,              55,     56,       57,             47,             42,       KEY_BACKSPACE},
   {KEY_RETURN,        61,    55,       56,             57,             43,       45},
   {KEY_DELETE,       104,    52,       53,             54,           44,       46},
   {KEY_MOD_LAYER5, 58,     49,       50,             51,            59,      KEY_RIGHT_SHIFT}, // 47
   {KEY_MOD_LAYER4, 48,    KEY_MOD_LAYER3, KEY_MOD_LAYER4, KEY_RIGHT_ALT, KEY_RIGHT_CTRL,     KEY_RETURN} // KEY_END 61 KEY_HOME
};

int neo2_layer4[5][7] = {
   {KEY_TOGGLE_LAYER3,                 KEY_EURO,             94,   126,           96,           0,               KEY_BACKSPACE},
   {KEY_RETURN,         33,            60,    62,          61,          38,              34},
   {KEY_DELETE,         63,            40,     41,          45,          58,              64},
   {0,   43,            37,  34, 39,           59,     KEY_RIGHT_SHIFT}, // 47
   {KEY_MOD_LAYER4,         32,  KEY_MOD_LAYER3,  KEY_MOD_LAYER4, KEY_RIGHT_ALT,    KEY_RIGHT_CTRL,  KEY_RETURN} // KEY_END 61 KEY_HOME
}; // bug in right side: if mod4 / mod5 key pressed, flickers between layers. set mod5 null in this layer

// custom layer currently same as base
int neo2_layer5[5][7] = {
   {KEY_TOGGLE_LAYER3,            55,             56,   57,           48,           KEY_S,               KEY_BACKSPACE},
   {KEY_TOGGLE_LED,   INCREASE_LED,            117,  KEY_UP_ARROW,          111,          112,              KEY_UE},
   {KEY_DELETE,    DECREASE_LED,            KEY_LEFT_ARROW,  KEY_DOWN_ARROW,          KEY_RIGHT_ARROW,          KEY_OE,              KEY_AE},
   {KEY_MOD_LAYER5,  0,         0,  44,           46,           47,        KEY_RIGHT_SHIFT}, // 47
  {KEY_MOD_LAYER4,     32,  KEY_MOD_LAYER3,  KEY_MOD_LAYER4,  KEY_RIGHT_ALT, KEY_RIGHT_CTRL,     KEY_RETURN} // KEY_END 61 KEY_HOME
};
