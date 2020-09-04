#include <mouse.h>

#define LEFT_BUTTON 1
#define MIDDLE_BUTTON 2
#define RIGHT_BUTTON 3
#define VSCROLL_UP 4
#define VSCROLL_DOWN 5
#define BACK_BUTTON 6
#define FORWARD_BUTTON 7

Mouse::Mouse(std::string hid_device)
{
    this->hid_device = hid_device;

    this->updated_once = false;

    this->x_previous = 0;
    this->y_previous = 0;
}

void Mouse::button_pressed_handler(int button)
{
    this->pressed_buttons[button] = true;
}

void Mouse::button_released_handler(int button)
{
    this->pressed_buttons[button] = false;
}

void Mouse::update_position(int x, int y)
{
    if (updated_once == false)
    {
        this->x_delta = 0;
        this->y_delta = 0;

        updated_once = true;
    }
    else
    {
        this->x_delta = x - this->x_previous;
        this->y_delta = y - this->y_previous;
    }

    this->x_previous = x;
    this->y_previous = y;
}

void Mouse::send_mouse_report()
{
    FILE *hid_pipe = fopen(this->hid_device.c_str(), "w");
    
    uint8_t buttons = 0;
    int8_t scroll = 0;

    // Map from X11 button numbering to USB HID button numbering
    buttons += pressed_buttons[LEFT_BUTTON] << 0;
    buttons += pressed_buttons[RIGHT_BUTTON] << 1;
    buttons += pressed_buttons[MIDDLE_BUTTON] << 2;
    buttons += pressed_buttons[BACK_BUTTON] << 3;
    buttons += pressed_buttons[FORWARD_BUTTON] << 4;

    if (pressed_buttons[VSCROLL_UP])
    {
        scroll += 1;
    }

    if (pressed_buttons[VSCROLL_DOWN])
    {
       scroll -= 1;
    }

    // x_delta and y_delta
    // need to be reported as:
    // [bit 8 - 0] [bit 16 - 9]

    uint8_t x1 = x_delta & 0xFF;
    uint8_t x2 = x_delta >> 8;

    uint8_t y1 = y_delta & 0xFF;
    uint8_t y2 = y_delta >> 8;
    // printf("x_delta: %d (%#04x), y_delta: %d\n", x_delta, x_delta, y_delta);
    // printf("%#02x %#02x \n", x1, x2);

    fprintf(
       hid_pipe,
       "%c%c%c%c%c%c",
       buttons,
       scroll,
       x1,
       x2,
       y1,
       y2);

    // The deltas have been consumed, set to zero
    // in the event that the mouse position has not 
    // changed before we send the next report.
    x_delta = 0;
    y_delta = 0;

   fclose(hid_pipe);
}
