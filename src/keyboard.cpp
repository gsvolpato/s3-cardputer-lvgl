#include "keyboard.h"

Cardputer_Keyboard Keyboard;

// Define static members
const std::vector<int> Cardputer_Keyboard::output_list = {KB_ROW_0, KB_ROW_1, KB_ROW_2};
const std::vector<int> Cardputer_Keyboard::input_list = {KB_COL_0, KB_COL_1, KB_COL_2, KB_COL_3, KB_COL_4, KB_COL_5, KB_COL_6};
const Chart_t Cardputer_Keyboard::X_map_chart[7] = {
    {1, 0, 1},
    {2, 2, 3},
    {4, 4, 5},
    {8, 6, 7},
    {16, 8, 9},
    {32, 10, 11},
    {64, 12, 13}
};

// Keyboard layout mapping based on the M5Stack Cardputer
const KeyValue_t Cardputer_Keyboard::_key_value_map[4][14] = {{
    // Row 0: ESC 1234567890-=DEL
    {"esc", KEY_ESC, "esc", KEY_ESC},
    {"1", KEY_1, "!", KEY_1},
    {"2", KEY_2, "@", KEY_2},
    {"3", KEY_3, "#", KEY_3},
    {"4", KEY_4, "$", KEY_4},
    {"5", KEY_5, "%", KEY_5},
    {"6", KEY_6, "^", KEY_6},
    {"7", KEY_7, "&", KEY_7},
    {"8", KEY_8, "*", KEY_8},
    {"9", KEY_9, "(", KEY_9},
    {"0", KEY_0, ")", KEY_0},
    {"-", KEY_MINUS, "_", KEY_MINUS},
    {"=", KEY_EQUAL, "+", KEY_EQUAL},
    {"del", KEY_BACKSPACE, "del", KEY_BACKSPACE}
}, {
    // Row 1: TAB QWERTYUIOP[]\\
    {"tab", KEY_TAB, "tab", KEY_TAB},
    {"q", KEY_Q, "Q", KEY_Q},
    {"w", KEY_W, "W", KEY_W},
    {"e", KEY_E, "E", KEY_E},
    {"r", KEY_R, "R", KEY_R},
    {"t", KEY_T, "T", KEY_T},
    {"y", KEY_Y, "Y", KEY_Y},
    {"u", KEY_U, "U", KEY_U},
    {"i", KEY_I, "I", KEY_I},
    {"o", KEY_O, "O", KEY_O},
    {"p", KEY_P, "P", KEY_P},
    {"[", KEY_LEFTBRACE, "{", KEY_LEFTBRACE},
    {"]", KEY_RIGHTBRACE, "}", KEY_RIGHTBRACE},
    {"\\", KEY_BACKSLASH, "|", KEY_BACKSLASH}
}, {
    // Row 2: FN CAPS ASDFGHJKL;'"ENTER
    {"fn", 0, "fn", 0},
    {"caps", KEY_CAPSLOCK, "caps", KEY_CAPSLOCK},
    {"a", KEY_A, "A", KEY_A},
    {"s", KEY_S, "S", KEY_S},
    {"d", KEY_D, "D", KEY_D},
    {"f", KEY_F, "F", KEY_F},
    {"g", KEY_G, "G", KEY_G},
    {"h", KEY_H, "H", KEY_H},
    {"j", KEY_J, "J", KEY_J},
    {"k", KEY_K, "K", KEY_K},
    {"l", KEY_L, "L", KEY_L},
    {";", KEY_SEMICOLON, ":", KEY_SEMICOLON},
    {"'", KEY_APOSTROPHE, "\"", KEY_APOSTROPHE},
    {"enter", KEY_ENTER, "enter", KEY_ENTER}
}, {
    // Row 3: CTRL OPT ALT ZXCVBNM,./SPACE
    {"ctrl", KEY_LEFTCTRL, "ctrl", KEY_LEFTCTRL},
    {"opt", 0, "opt", 0},
    {"alt", KEY_LEFTALT, "alt", KEY_LEFTALT},
    {"z", KEY_Z, "Z", KEY_Z},
    {"x", KEY_X, "X", KEY_X},
    {"c", KEY_C, "C", KEY_C},
    {"v", KEY_V, "V", KEY_V},
    {"b", KEY_B, "B", KEY_B},
    {"n", KEY_N, "N", KEY_N},
    {"m", KEY_M, "M", KEY_M},
    {",", KEY_COMMA, "<", KEY_COMMA},
    {".", KEY_DOT, ">", KEY_DOT},
    {"/", KEY_SLASH, "?", KEY_SLASH},
    {"space", KEY_SPACE, "space", KEY_SPACE}
}};

Cardputer_Keyboard::Cardputer_Keyboard() : 
    _isInit(false), 
    _keyPressStartTime(0),
    _lastRepeatTime(0),
    _isRepeating(false) {
    clearState();
    _lastKey.x = -1;
    _lastKey.y = -1;
}

void Cardputer_Keyboard::begin() {
    if (_isInit) return;
    
    Serial.println("Initializing keyboard with GPIO matrix...");
    
    // Initialize output pins
    for (auto pin : output_list) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    
    // Initialize input pins
    for (auto pin : input_list) {
        pinMode(pin, INPUT_PULLUP);
    }
    
    _isInit = true;
    Serial.println("Keyboard GPIO matrix initialized");
}

void Cardputer_Keyboard::setOutput(const std::vector<int>& pinList, uint8_t output) {
    output = output & 0B00000111;
    digitalWrite(pinList[0], (output & 0B00000001));
    digitalWrite(pinList[1], (output & 0B00000010));
    digitalWrite(pinList[2], (output & 0B00000100));
}

uint8_t Cardputer_Keyboard::getInput(const std::vector<int>& pinList) {
    uint8_t buffer = 0x00;
    uint8_t pin_value = 0x00;
    
    for (int i = 0; i < 7; i++) {
        pin_value = (digitalRead(pinList[i]) == 1) ? 0x00 : 0x01;
        pin_value = pin_value << i;
        buffer = buffer | pin_value;
    }
    
    return buffer;
}

Point2D_t Cardputer_Keyboard::getKey() {
    Point2D_t coor;
    coor.x = -1;
    coor.y = -1;
    
    uint8_t input_value = 0;
    
    for (int i = 0; i < 8; i++) {
        setOutput(output_list, i);
        input_value = getInput(input_list);
        
        if (input_value) {
            // Get X coordinate
            for (int j = 0; j < 7; j++) {
                if (input_value == X_map_chart[j].value) {
                    coor.x = (i > 3) ? X_map_chart[j].x_1 : X_map_chart[j].x_2;
                    break;
                }
            }
            
            // Get Y coordinate
            coor.y = (i > 3) ? (i - 4) : i;
            coor.y = -coor.y + 3;  // Transform to match keymap
            
            break;
        }
    }
    
    return coor;
}

bool Cardputer_Keyboard::handleKeyRepeat(const Point2D_t& currentKey) {
    unsigned long currentTime = millis();
    
    // If no key is pressed
    if (currentKey.x == -1 || currentKey.y == -1) {
        // Reset state if key was released before repeat delay
        if (!_isRepeating) {
            _lastKey.x = -1;
            _lastKey.y = -1;
        }
        _keyPressStartTime = 0;
        _isRepeating = false;
        return false;
    }
    
    // If it's a new key press
    if (_lastKey.x != currentKey.x || _lastKey.y != currentKey.y) {
        _lastKey = currentKey;
        _keyPressStartTime = currentTime;
        _isRepeating = false;
        return true;  // Process new key press
    }
    
    // If key is being held
    if (_keyPressStartTime > 0) {
        // Check if we should start repeating
        if (!_isRepeating && (currentTime - _keyPressStartTime >= REPEAT_DELAY_MS)) {
            _isRepeating = true;
            _lastRepeatTime = currentTime;
            return true;  // Process first repeat
        }
        // If we're repeating, check if it's time for another repeat
        else if (_isRepeating && (currentTime - _lastRepeatTime >= REPEAT_RATE_MS)) {
            _lastRepeatTime = currentTime;
            return true;  // Process repeat
        }
    }
    
    return false;  // Don't process key
}

void Cardputer_Keyboard::update() {
    if (!_isInit) {
        Serial.println("Keyboard not initialized!");
        return;
    }
    
    clearState();
    Point2D_t currentKey = getKey();
    
    if (handleKeyRepeat(currentKey)) {
        if (currentKey.y >= 0 && currentKey.y < 4 && currentKey.x >= 0 && currentKey.x < 14) {
            const KeyValue_t& keyValue = _key_value_map[currentKey.y][currentKey.x];
            
            // Handle special keys
            if (currentKey.y == 2 && currentKey.x == 0) { // FN key
                _currentState.fn = true;
            } else if (currentKey.y == 3 && currentKey.x == 0) { // CTRL key
                _currentState.modifier_keys.push_back(KEY_LEFTCTRL);
            } else if (currentKey.y == 3 && currentKey.x == 2) { // ALT key
                _currentState.modifier_keys.push_back(KEY_LEFTALT);
            } else if (keyValue.value_num_first == KEY_BACKSPACE) {
                _currentState.del = true;
                _currentState.hid_keys.push_back(KEY_BACKSPACE);
            } else if (keyValue.value_num_first == KEY_ENTER) {
                _currentState.enter = true;
                _currentState.hid_keys.push_back(KEY_ENTER);
            } else if (keyValue.value_num_first != 0) {
                _currentState.hid_keys.push_back(keyValue.value_num_first);
                if (keyValue.value_first[0] >= 'a' && keyValue.value_first[0] <= 'z') {
                    _currentState.word += keyValue.value_first;
                }
            }
            
            // Debug output
            Serial.printf("Key pressed at (%d,%d) - Value: %s, HID: 0x%02X%s\n", 
                currentKey.x, currentKey.y, 
                keyValue.value_first, 
                keyValue.value_num_first,
                _isRepeating ? " (repeat)" : "");
        }
    }
}

bool Cardputer_Keyboard::isPressed() {
    return _currentState.word.length() > 0 || 
           !_currentState.hid_keys.empty() || 
           _currentState.del || 
           _currentState.enter;
}

bool Cardputer_Keyboard::isKeyPressed(char key) {
    return _currentState.word.indexOf(key) != -1;
}

Cardputer_Keyboard::KeysState Cardputer_Keyboard::keysState() {
    return _currentState;
}

void Cardputer_Keyboard::clearState() {
    _currentState.fn = false;
    _currentState.del = false;
    _currentState.enter = false;
    _currentState.word = "";
    _currentState.hid_keys.clear();
    _currentState.modifier_keys.clear();
}   