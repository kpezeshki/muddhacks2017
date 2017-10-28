#include "ILI9163.h"
#include "glcdfont.cpp"
#include <stdint.h>
#include "Particle.h"

#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }

// Private
void ILI9163::write_command(uint8_t cmd) {
    digitalWrite(a0, LOW);
    digitalWrite(cs, LOW);
    SPI.transfer(cmd);
    digitalWrite(cs, HIGH);
}

void ILI9163::write_data(uint8_t data) {
    digitalWrite(a0, HIGH);
    digitalWrite(cs, LOW);
    SPI.transfer(data);
    digitalWrite(cs, HIGH);
}

void ILI9163::write_data16(uint16_t data) {
    digitalWrite(a0, HIGH);
    digitalWrite(cs, LOW);
    SPI.transfer((data >> 8) & 0xff);
    SPI.transfer(data & 0xff);
    digitalWrite(cs, HIGH);
}

void ILI9163::set_address(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    write_command(ILI9163_CMD_SET_COLUMN_ADDRESS);
    write_data16(x1);
    write_data16(x2);

    write_command(ILI9163_CMD_SET_PAGE_ADDRESS);
    write_data16(y1 + 32);
    write_data16(y2 + 32);
    write_command(ILI9163_CMD_WRITE_MEMORY_START);
}

// Public
ILI9163::ILI9163(int _cs, int _rst, int _a0) {
    cs = _cs;
    rst = _rst;
    a0 = _a0;

    cursor_x = 0;
    cursor_y = 0;

    fg_color = rgb(255, 255, 255);
    bg_color = fg_color;

    SPI.begin(cs); // Start SPI
    SPI.setBitOrder(MSBFIRST);

    // Setup pins
    pinMode(rst, OUTPUT);
    pinMode(a0, OUTPUT);

    // Reset display
    digitalWrite(rst, HIGH);
    delay(5);
    digitalWrite(rst, LOW);
    delay(20);
    digitalWrite(rst, HIGH);
    delay(150);

    // Init sequence
    write_command(ILI9163_CMD_EXIT_SLEEP_MODE);
    delay(120); // Wait for the screen to wake up

    for (size_t i = 0; i < sizeof(init_sequence)/sizeof(uint8_t); i++) {
        if (init_sequence[i] == 0xff) {
            i++;
            write_command(init_sequence[i]);
        } else {
            write_data(init_sequence[i]);
        }
    }
}

void ILI9163::fill(uint16_t color) {
    for (int i = 0; i < ILI9163_WIDTH * ILI9163_HEIGHT * 2; i += 2) {
        buffer[i] = (color >> 8) & 0xff;
        buffer[i + 1] = color & 0xff;
    }
}

void ILI9163::draw_pixel(uint8_t x, uint8_t y, uint16_t color) {
    if ((x < 0) || (x >= ILI9163_WIDTH) || (y < 0) || (y >= ILI9163_HEIGHT)) return;
    buffer[(x + y*ILI9163_WIDTH)*2] = (color >> 8) & 0xff;
    buffer[(x + y*ILI9163_WIDTH)*2 + 1] = color & 0xff;
}

void ILI9163::draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    draw_pixel(x0  , y0+r, color);
    draw_pixel(x0  , y0-r, color);
    draw_pixel(x0+r, y0  , color);
    draw_pixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        draw_pixel(x0 + x, y0 + y, color);
        draw_pixel(x0 - x, y0 + y, color);
        draw_pixel(x0 + x, y0 - y, color);
        draw_pixel(x0 - x, y0 - y, color);
        draw_pixel(x0 + y, y0 + x, color);
        draw_pixel(x0 - y, y0 + x, color);
        draw_pixel(x0 + y, y0 - x, color);
        draw_pixel(x0 - y, y0 - x, color);
    }
}

void ILI9163::draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            draw_pixel(y0, x0, color);
        } else {
            draw_pixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ILI9163::clear() {
    fill(rgb(0, 0, 0));
    set_cursor(0, 0);
}

void ILI9163::draw_char(uint8_t x, uint8_t y, char c, uint16_t col, uint16_t bg) {
    if(x >= ILI9163_WIDTH || y >= ILI9163_HEIGHT || (x + 5) < 0 || (y + 7) < 0)
        return;

    for(int8_t i = 0; i < 6; i++ ) {
        uint8_t line;
        if(i < 5) line = pgm_read_byte(font+(c*5)+i);
        else      line = 0x0;
        for(int8_t j = 0; j < 8; j++, line >>= 1) {
            if(line & 0x1) {
                draw_pixel(x+i, y+j, col);
            } else if(bg != col) {
                draw_pixel(x+i, y+j, bg);
            }
        }
    }
}

size_t ILI9163::write(uint8_t c) {
    if (c == '\n') {
        cursor_y += 8;
        cursor_x = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        if((cursor_x + 6) >= ILI9163_WIDTH) { // Heading off edge?
            cursor_x = 0;            // Reset x to zero
            cursor_y += 8; // Advance y one line
        }
        draw_char(cursor_x, cursor_y, c, fg_color, bg_color);
        cursor_x += 6;
    }

    return 1;
}

void ILI9163::set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}

void ILI9163::set_color(uint16_t fg, uint16_t bg) {
    fg_color = fg;
    bg_color = bg;
}

void ILI9163::copy_buffer() {
    set_address(0, 0, ILI9163_WIDTH - 1, ILI9163_HEIGHT - 1);
    digitalWrite(a0, HIGH);
    digitalWrite(cs, LOW);
    SPI.transfer(buffer, NULL, sizeof(buffer)/sizeof(uint8_t), NULL);
    digitalWrite(cs, HIGH);
}
