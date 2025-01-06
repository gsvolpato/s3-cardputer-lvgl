#include "menu.h"
#include <Arduino.h>
#include <cstdio>
#include "User_Setup.h"
#include <Wire.h>
#include "keyboard.h"
#include "SD.h"
#include "WiFi.h"

static lv_obj_t *container;
static lv_obj_t *menu_items[10];
static lv_obj_t *status_bar;
static int current_item = 0;
static lv_obj_t *wifi_list = nullptr;
static int wifi_selected_network = 0;
static bool in_wifi_screen = false;

static void scroll_to_item(int index) {
    if (index >= 0 && index < 10) {
        current_item = index;
        lv_obj_scroll_to_view(menu_items[current_item], LV_ANIM_ON);
        Serial.printf("Scrolling to menu item: %d\n", index);
    }
}

static void show_sd_contents() {
    // Clear current screen
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    
    // Create a list container
    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, TFT_HEIGHT, TFT_WIDTH);
    lv_obj_set_style_bg_color(list, lv_color_make(0, 0, 64), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    
    // Initialize SD card
    if (!SD.begin()) {
        lv_obj_t *error_label = lv_label_create(list);
        lv_label_set_text(error_label, "Error: SD card not found");
        lv_obj_set_style_text_color(error_label, lv_color_white(), 0);
        Serial.println("SD Card Mount Failed");
        return;
    }
    
    // Open root directory
    File root = SD.open("/");
    if (!root) {
        lv_obj_t *error_label = lv_label_create(list);
        lv_label_set_text(error_label, "Error: Failed to open root directory");
        lv_obj_set_style_text_color(error_label, lv_color_white(), 0);
        Serial.println("Failed to open directory");
        return;
    }
    
    // Read directory contents
    File file = root.openNextFile();
    while (file) {
        char name_buf[128];
        if (file.isDirectory()) {
            snprintf(name_buf, sizeof(name_buf), "[DIR] %s", file.name());
        } else {
            snprintf(name_buf, sizeof(name_buf), "%s (%d bytes)", file.name(), file.size());
        }
        
        lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_FILE, name_buf);
        lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
        lv_obj_set_style_text_color(btn, lv_color_black(), 0);
        
        file = root.openNextFile();
    }
    
    root.close();
}

static void show_wifi_networks() {
    in_wifi_screen = true;
    wifi_selected_network = 0;
    
    // Clear current screen
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    
    // Create a list container
    wifi_list = lv_list_create(scr);
    lv_obj_set_size(wifi_list, TFT_HEIGHT, TFT_WIDTH);
    lv_obj_set_style_bg_color(wifi_list, lv_color_make(64, 0, 0), 0);  // Dark blue (BGR)
    lv_obj_set_style_bg_opa(wifi_list, LV_OPA_COVER, 0);
    
    // Start WiFi in station mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Scan for networks
    int n = WiFi.scanNetworks();
    Serial.printf("Found %d networks\n", n);
    
    if (n == 0) {
        lv_obj_t *error_label = lv_label_create(wifi_list);
        lv_label_set_text(error_label, "No networks found");
        lv_obj_set_style_text_color(error_label, lv_color_white(), 0);
    } else {
        for (int i = 0; i < n; ++i) {
            char ssid_buf[128];
            snprintf(ssid_buf, sizeof(ssid_buf), "%s (%d dBm)", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            
            lv_obj_t *btn = lv_list_add_btn(wifi_list, LV_SYMBOL_WIFI, ssid_buf);
            lv_obj_set_style_bg_color(btn, i == 0 ? lv_color_make(255, 128, 0) : lv_color_white(), 0);  // Light blue highlight (BGR)
            lv_obj_set_style_text_color(btn, i == 0 ? lv_color_white() : lv_color_black(), 0);
        }
    }
}

static void navigate_wifi_list(bool up) {
    if (!in_wifi_screen) return;
    
    int n = WiFi.scanNetworks();
    if (n == 0) return;
    
    // Update selection
    lv_obj_t *old_btn = lv_obj_get_child(wifi_list, wifi_selected_network + 1);  // +1 for list header
    if (old_btn) {
        lv_obj_set_style_bg_color(old_btn, lv_color_white(), 0);
        lv_obj_set_style_text_color(old_btn, lv_color_black(), 0);
    }
    
    if (up) {
        wifi_selected_network = (wifi_selected_network - 1 + n) % n;
    } else {
        wifi_selected_network = (wifi_selected_network + 1) % n;
    }
    
    lv_obj_t *new_btn = lv_obj_get_child(wifi_list, wifi_selected_network + 1);  // +1 for list header
    if (new_btn) {
        lv_obj_set_style_bg_color(new_btn, lv_color_make(255, 128, 0), 0);  // Light blue highlight (BGR)
        lv_obj_set_style_text_color(new_btn, lv_color_white(), 0);
        lv_obj_scroll_to_view(new_btn, LV_ANIM_ON);
    }
}

void check_keyboard() {
    Keyboard.update();
    
    if (Keyboard.isPressed()) {
        auto state = Keyboard.keysState();
        
        // Check for navigation keys
        for (auto key : state.hid_keys) {
            if (in_wifi_screen) {
                if (key == KEY_ESC) {  // ESC to return to main menu
                    in_wifi_screen = false;
                    createMenu();  // Recreate the main menu
                    delay(150);
                    break;
                } else if (key == 0x33) {  // Semicolon key for up
                    navigate_wifi_list(true);
                    delay(150);
                    break;
                } else if (key == 0x37) {  // Period key for down
                    navigate_wifi_list(false);
                    delay(150);
                    break;
                } else if (key == KEY_ENTER) {  // Enter to select network
                    String ssid = WiFi.SSID(wifi_selected_network);
                    Serial.printf("Selected network: %s\n", ssid.c_str());
                    delay(150);
                    break;
                }
            } else {
                if (key == KEY_LEFT || key == KEY_COMMA) {
                    scroll_to_item(current_item - 1);
                    Serial.printf("Selected menu item: %d (moved left)\n", current_item);
                    delay(150);
                    break;
                } else if (key == KEY_RIGHT || key == KEY_SLASH) {
                    scroll_to_item(current_item + 1);
                    Serial.printf("Selected menu item: %d (moved right)\n", current_item);
                    delay(150);
                    break;
                } else if (key >= 0x1E && key <= 0x26) {
                    int menu_index = key - 0x1E;
                    scroll_to_item(menu_index);
                    Serial.printf("Selected menu item: %d (direct selection)\n", menu_index);
                    delay(150);
                    break;
                } else if (key == 0x27) {
                    scroll_to_item(9);
                    Serial.printf("Selected menu item: 9 (direct selection)\n");
                    delay(150);
                    break;
                } else if (key == KEY_ENTER) {
                    if (current_item == 0) {
                        show_sd_contents();
                    } else if (current_item == 1) {
                        show_wifi_networks();
                    }
                    delay(150);
                    break;
                }
            }
        }
    }
}

static void menu_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        uint32_t index = (uint32_t)lv_obj_get_user_data(obj);
        Serial.printf("Menu item %d activated\n", index);
    }
}

void createMenu() {
    Serial.println("Initializing keyboard...");
    Keyboard.begin();
    
    Serial.println("Creating menu interface...");
    lv_obj_t *scr = lv_scr_act();
    
    // Create status bar
    status_bar = lv_obj_create(scr);
    lv_obj_set_size(status_bar, TFT_HEIGHT, 20);  // 20 pixels high status bar
    lv_obj_set_pos(status_bar, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_make(0, 0, 32), 0);  // Dark blue status bar (BGR)
    lv_obj_set_style_bg_opa(status_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_bo                                                                 rder_width(status_bar, 0, 0);
    lv_obj_set_style_radius(status_bar, 0, 0);  // Remove rounded corners
    
    // Add battery percentage text (placeholder)
    lv_obj_t *battery_label = lv_label_create(status_bar);
    lv_label_set_text(battery_label, "100%");
    lv_obj_align(battery_label, LV_ALIGN_RIGHT_MID, -5, 0);  // 5 pixels from right edge
    
    // Create menu container below status bar
    container = lv_obj_create(scr);
    lv_obj_set_size(container, TFT_HEIGHT, TFT_WIDTH - 20);  // Subtract status bar height
    lv_obj_set_pos(container, 0, 20);  // Position below status bar
    
    // Set solid background color
    lv_obj_set_style_bg_color(container, lv_color_make(0, 0, 64), 0);  // Dark blue (BGR)
    lv_obj_set_style_bg_opa(container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_radius(container, 0, 0);  // Remove rounded corners
    
    lv_obj_set_style_pad_column(container, 10, 0);
    lv_obj_set_style_pad_all(container, 10, 0);
    
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_dir(container, LV_DIR_HOR);
    lv_obj_set_scroll_snap_x(container, LV_SCROLL_SNAP_CENTER);
    
    // Create menu items in keyboard order (1-9-0)
    const char* labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    
    for(int i = 0; i < 10; i++) {
        menu_items[i] = lv_obj_create(container);
        lv_obj_set_size(menu_items[i], 90, 90);
        
        lv_obj_set_style_radius(menu_items[i], 10, 0);  // 10 pixels rounded corners
        lv_obj_set_style_bg_color(menu_items[i], lv_color_white(), 0);
        lv_obj_set_style_border_width(menu_items[i], 0, 0);
        
        lv_obj_t *label = lv_label_create(menu_items[i]);
        lv_label_set_text(label, labels[i]);
        lv_obj_center(label);
        
        lv_obj_set_user_data(menu_items[i], (void*)i);
        lv_obj_add_event_cb(menu_items[i], menu_event_cb, LV_EVENT_CLICKED, NULL);
    }
    
    scroll_to_item(0);
    Serial.println("Menu creation complete!");
} 