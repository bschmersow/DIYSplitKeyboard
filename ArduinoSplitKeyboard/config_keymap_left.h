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

// custom assignments which will be handled in the handle_modifiers function
// int is outside the asci definition
const int KEY_TOGGLE_LED = 999;

// switch between keymaps (programmed neo2 layout)
const int KEY_MOD_LAYER3 = 1000;
const int KEY_MOD_LAYER4 = 1001;
const int KEY_MOD_LAYER5 = 1002; //custom layer to add some keys missing in neo2


// Special keys involving shortcuts
const int KEY_WORD_FORWARD = 1003;
const int KEY_WORD_BACK = 1004;

const short KEY_AE = 1005;
const short KEY_UE = 1006;
const short KEY_OE = 1006;
const short KEY_S = 1007;

const short INCREASE_LED = 1008;
const short DECREASE_LED = 1009;

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
   {KEY_ESC,         49,             50,        51,         52,           53,           54},
   {KEY_TAB,        113,             109,       101,        114,          116,           KEY_TOGGLE_LED},
   {KEY_MOD_LAYER3,   97,            115,      100,        102,          103,           0 },
   {KEY_LEFT_SHIFT,  121,            120,       99,        118,           98,           0 },
   {KEY_LEFT_CTRL,  KEY_LEFT_GUI, KEY_MOD_LAYER5, KEY_LEFT_ALT,32, KEY_MOD_LAYER3,         KEY_MOD_LAYER4}
};

int neo2_layer3[5][7] = {
   {KEY_ESC,                 49,                50,             51,         52,           53,           54},
   {KEY_TAB,         KEY_PAGE_UP,       KEY_BACKSPACE,      KEY_UP_ARROW,KEY_DELETE, KEY_PAGE_DOWN, KEY_TOGGLE_LED},
   {KEY_MOD_LAYER3,               KEY_HOME,      KEY_LEFT_ARROW,    KEY_DOWN_ARROW,KEY_RIGHT_ARROW,   KEY_END,   0 },
   {KEY_LEFT_SHIFT,  121,                      120,             99,             118,           98,  0 },
   {KEY_LEFT_CTRL,  0   ,    KEY_MOD_LAYER5,        0,              32, KEY_MOD_LAYER3,         KEY_MOD_LAYER4}
};

int neo2_layer4[5][7] = {
   {KEY_ESC,         49,             50,        51,         52,           53,           54},
   {KEY_TAB,        45,             95,       91,        93,          94,           KEY_TOGGLE_LED},
   {KEY_MOD_LAYER3,               92,            47,      123,        125,          42,           0},
   {KEY_LEFT_SHIFT,  35,            36,       124,        126,           39,           0},
   {KEY_LEFT_CTRL,    0,   KEY_MOD_LAYER5,         0,         32,            KEY_MOD_LAYER3,         KEY_MOD_LAYER4}
};

int neo2_layer5[5][7] = {
   {KEY_ESC,         49,             50,        51,         52,           53,           INCREASE_LED},
   {KEY_TAB,        45,             95,       91,        93,          94,           DECREASE_LED},
   {KEY_MOD_LAYER3,   92,            47,      123,        125,          42,           KEY_CAPS_LOCK}, // if you ever need caps lock, here it is
   {KEY_LEFT_SHIFT,  35,            36,       124,        126,           39,           0},
   {KEY_LEFT_CTRL,    0,  KEY_MOD_LAYER5,         0,         32,            KEY_MOD_LAYER3,         KEY_MOD_LAYER4}
};
