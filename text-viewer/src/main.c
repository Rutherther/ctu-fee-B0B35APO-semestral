#include "font.h"
#include "mzapo_led_strip.h"
#include "nonblocking_io.h"
#include "serialize_lock.h"
#include <stdio.h>
#include <unistd.h>
#include "logger.h"
#include "display_utils.h"
#include "mzapo_pheripherals.h"
#include "text_viewer.h"

typedef enum {
  SUCCESS,
} error_t;

int main(int argc, char *argv[]) {
  struct termios oldstdin;

  logger_t logger =
      logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");
    logger_warn(&logger, __FILE__, __FUNCTION__, __LINE__, "System is occupied");

    if (1) {
      /* Wait till application holding lock releases IT or exits */
      logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
                  "Waiting");
      serialize_lock(0);
    }
  }

  if (argc < 2) {
    logger_info(&logger, __FILE__, __FUNCTION__, __LINE__, "Not enough arguments");
  }

  display_t display = mzapo_create_display();
  mzapo_rgb_led_t rgb_leds = mzapo_create_rgb_led();
  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();
  void* knobs = mzapo_get_knobs_address();

  if (!mzapo_check_pheripherals(&ledstrip, &rgb_leds, &display, &knobs)) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Could not initialize some of the pheripherals.");
  }

  mzapo_pheripherals_t pheripherals = mzapo_pheripherals_create(&ledstrip, &rgb_leds, &display, &knobs);

  font_t font = font_create(font_rom8x16);
  font.char_spacing = 2;

  text_viewer_t text_viewer = text_viewer_create(argv[1], pheripherals, &logger, font);

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Loading file");
  file_error_t error = text_viewer_load_file(&text_viewer);
  if (error != FILER_SUCCESS) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Could not load file.");
    return 1;
  }

  file_set_nonblocking(STDIN_FILENO, &oldstdin);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Starting text viewer");
  text_viewer_start_loop(&text_viewer);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Closing application");

  text_viewer_destroy(&text_viewer);
  display_deinit(&display);

  file_set_blocking(STDIN_FILENO, &oldstdin);

  serialize_unlock();
  
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Application quit");

  return SUCCESS;
}
