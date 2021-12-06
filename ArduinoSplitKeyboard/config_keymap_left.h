/** WS2812 LED backlight config */
// If a LED circle backlight is present, configure this
#define PIN 9 // The Control (I-Pin) of the WS2812 LED / NeoPixel
#define NUMPIXELS 12 // Number od WS2812 LEDs / NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// input pins / columns
const short col1 = 7;
const short col2 = A3;
const short col3 = A1;
const short col4 = 10;
const short col5 = 15;
const short col6 = 14;
const short col7 = 16;

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

/*
 * NEO2 Quertz Layout
 *
 * Helpful references:
 * https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/ 
 * asciitable.com
 * https://dry.sailingissues.com/us-international-keyboard-layout.html
 * 
 * Important: layer modifiers should be the same on all keymaps (resp. at least on the one that has been activated, to deactivate) 
 * or onRelease will not be correctly identified!
 */
int baseLayer[5][7] = {
   {KEY_ESC,         49,             50,        51,         52,           53,           54},
   {KEY_TAB,        113,             119,       101,        114,          116,           KEY_RETURN},
   {KEY_MOD_LAYER3,   97,            115,      100,        102,          103,           KEY_TOGGLE_LAYER3 },
   {KEY_LEFT_SHIFT,  121,            120,       99,        118,           98,            39},
   {KEY_LEFT_CTRL, KEY_LEFT_GUI , KEY_MOD_LAYER5 , KEY_LEFT_ALT,KEY_MOD_LAYER4,32,          KEY_MOD_LAYER3}
};

int neo2_layer3[5][7] = {
   {KEY_ESC,                 49,                50,             51,         52,           53,           KEY_PAGE_UP},
   {KEY_TAB,         KEY_HOME,       KEY_BACKSPACE,      KEY_UP_ARROW,KEY_DELETE, KEY_END, KEY_PAGE_DOWN},
   {KEY_MOD_LAYER3,               KEY_WORD_BACK,      KEY_LEFT_ARROW,    KEY_DOWN_ARROW,KEY_RIGHT_ARROW,   KEY_WORD_FORWARD,   KEY_TOGGLE_LAYER3 },
   {KEY_LEFT_SHIFT,  UNDO,              REDO,             99,             118,           98,  0 },
   {KEY_LEFT_CTRL,  0   ,    KEY_MOD_LAYER5,        0,      KEY_MOD_LAYER4,        32,          KEY_MOD_LAYER3}
};

int neo2_layer4[5][7] = {
   {KEY_ESC,         49,             50,        51,         52,           53,           54},
   {KEY_TAB,        45,             95,       91,        93,          94,           KEY_RETURN},
   {KEY_MOD_LAYER3,               92,            47,      123,        125,          42,       KEY_TOGGLE_LAYER3},
   {KEY_LEFT_SHIFT,  35,            36,       124,        126,           39,         0  }, 
   {KEY_LEFT_CTRL,    0,   KEY_MOD_LAYER5,         0,      KEY_MOD_LAYER4,   32,                     KEY_MOD_LAYER3}
};

int neo2_layer5[5][7] = {
   {KEY_ESC,         KEY_F1,             KEY_F2,        KEY_F3,         KEY_F4,           KEY_F5,           KEY_F6},
   {KEY_TAB,        45,             95,       91,        93,          94,           KEY_TOGGLE_LED},
   {KEY_MOD_LAYER3,   92,            47,      123,        125,          42,           DECREASE_LED}, 
   {KEY_CAPS_LOCK,  35,            36,       124,        126,           39,           INCREASE_LED}, // if you ever need caps lock, here it is
   {KEY_LEFT_CTRL,    KEY_LEFT_GUI,  KEY_MOD_LAYER5,         0,    KEY_MOD_LAYER4,     32,                     KEY_MOD_LAYER3}
};
