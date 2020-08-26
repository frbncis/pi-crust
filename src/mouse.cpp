#include <mouse.h>

#define LEFT_BUTTON 1
#define MIDDLE_BUTTON 2
#define RIGHT_BUTTON 3
#define VSCROLL_UP 4
#define VSCROLL_DOWN 5
#define HSCROLL_LEFT 6
#define HSCROLL_RIGHT 7

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

    // Buttons 4, 5, 6, 7, 8 not mapped yet.
    for (int i = 4; i <= 9; i++)
    {
	buttons += (0 << i);
    }

    if (pressed_buttons[VSCROLL_UP])
    {
        scroll += 1;
    }

    if (pressed_buttons[VSCROLL_DOWN])
    {
       scroll -= 1;
    }

    fprintf(
       hid_pipe,
       "%c%c%c%c",
       buttons,
       x_delta,
       y_delta,
       scroll);

    // The deltas have been consumed, set to zero
    // in the event that the mouse position has not 
    // changed before we send the next report.
    x_delta = 0;
    y_delta = 0;

   fclose(hid_pipe);
}
