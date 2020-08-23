#include <mouse.h>

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2

Mouse::Mouse(std::string hid_device)
{
    this->hid_device = hid_device;

    this->updated_once = false;

    this->x_previous = 0;
    this->y_previous = 0;
}

void Mouse::button_pressed_handler(int button)
{
    if (button == LEFT_BUTTON)
    {
        this->left_button_pressed = true;
    }
    else
    {
        this->right_button_pressed = true;
    }
}

void Mouse::button_released_handler(int button)
{
    if (button == LEFT_BUTTON)
    {
        this->left_button_pressed = false;
    }
    else
    {
        this->right_button_pressed = false;
    }
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
    // printf("X: %d, Y: %d\n", x_delta, y_delta);

    FILE *hid_pipe = fopen(this->hid_device.c_str(), "w");

   fprintf(
       hid_pipe,
       "%c%c%c",
       '\0',
       x_delta,
       y_delta);

   fclose(hid_pipe);
}
