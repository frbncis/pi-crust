#include <map>
#include <string>

#pragma once

class Mouse {
public:
    Mouse(std::string hid_device);

    void update_position(int x, int y);

    void button_pressed_handler(int button);
    void button_released_handler(int button);

    void send_mouse_report();

private:
    std::string hid_device;

    bool updated_once;

    int x_previous;
    int y_previous;
    
    int8_t x_delta;
    int8_t y_delta;

    std::map<char, bool> pressed_buttons;
};
