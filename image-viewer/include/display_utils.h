#ifndef __DISPLAY_UTILS_H__
#define __DISPLAY_UTILS_H__

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_MAX_RED 31u
#define DISPLAY_MAX_GREEN 63u
#define DISPLAY_MAX_BLUE 31u

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

typedef union {
  struct {
    uint8_t b : 5;
    uint8_t g : 6;
    uint8_t r : 5;
  } __attribute__((__packed__)) fields;
  uint16_t bits;
} __attribute__((__packed__)) display_pixel_t;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} __attribute__((__packed__)) raw_pixel_onebit_t;

typedef struct {
  uint16_t red;
  uint16_t green;
  uint16_t blue;
} __attribute__((__packed__)) raw_pixel_t;

typedef struct {
  void* base_address;
} display_data_t;

typedef struct {
  display_pixel_t pixels[DISPLAY_HEIGHT * DISPLAY_WIDTH];
  display_data_t data;
} display_t;

extern const display_pixel_t BLACK_PIXEL;
extern const display_pixel_t WHITE_PIXEL;

extern const raw_pixel_t DISPLAY_PIXEL_MAX;

display_pixel_t raw_pixel_onebit_convert_to_display(raw_pixel_onebit_t pixel,
                                             raw_pixel_onebit_t max);
display_pixel_t raw_pixel_convert_to_display(raw_pixel_t pixel, raw_pixel_t max);

display_t display_init(display_data_t data);
void display_deinit(display_t *display);

void display_render(display_t *display);
void display_clear(display_t *display, bool render);

display_pixel_t display_get_pixel(display_t *display, uint16_t x, uint16_t y);
void display_set_pixel(display_t *display, uint16_t x, uint16_t y,
                       display_pixel_t pixel);

#endif // __DISPLAY_UTILS_H__
