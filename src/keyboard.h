#pragma once

#include <Arduino.h>
#include <vector>
#include "cardputer_gpios.h"
#include "keymap.h"

struct Point2D_t {
    int x;
    int y;
};

struct Chart_t {
    uint8_t value;
    uint8_t x_1;
    uint8_t x_2;
};

struct KeyValue_t {
    const char* value_first;
    const uint8_t value_num_first;
    const char* value_second;
    const uint8_t value_num_second;
};

class Cardputer_Keyboard {
public:
    struct KeysState {
        bool fn;
        bool del;
        bool enter;
        String word;
        std::vector<uint8_t> hid_keys;
        std::vector<uint8_t> modifier_keys;
    };

    Cardputer_Keyboard();
    void begin();
    void update();
    bool isPressed();
    bool isKeyPressed(char key);
    KeysState keysState();

private:
    static const std::vector<int> output_list;
    static const std::vector<int> input_list;
    static const Chart_t X_map_chart[7];
    static const KeyValue_t _key_value_map[4][14];
    
    KeysState _currentState;
    bool _isInit;
    
    // Key repeat handling
    Point2D_t _lastKey;
    unsigned long _keyPressStartTime;
    unsigned long _lastRepeatTime;
    static const unsigned long REPEAT_DELAY_MS = 500;  // 0.5 seconds before repeat starts
    static const unsigned long REPEAT_RATE_MS = 500;   // Repeat every 0.5 seconds
    bool _isRepeating;
    
    void readKeyboard();
    void clearState();
    void setOutput(const std::vector<int>& pinList, uint8_t output);
    uint8_t getInput(const std::vector<int>& pinList);
    Point2D_t getKey();
    bool handleKeyRepeat(const Point2D_t& currentKey);
};

extern Cardputer_Keyboard Keyboard;