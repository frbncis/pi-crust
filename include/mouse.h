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
    
    int x_delta;
    int y_delta;

    bool left_button_pressed;
    bool right_button_pressed;
};