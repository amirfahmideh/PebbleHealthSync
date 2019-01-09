#include <pebble.h>
#include "windows/dialog_message_window.h"
#include "goalwindow.h"
#include "local_storage.h"
#include "persiancalendar.h"

static Window *s_window;
static NumberWindow *s_window_goals;


static TextLayer *s_text_layer;
static TextLayer *s_text_persianDate;
static TextLayer *s_text_goalNumber;
static TextLayer *s_text_todayStepNumber;
static TextLayer *s_text_ofLabel;

static void callOnGoalChanges(int newgoal){
  static char str_goalStep[8];
  snprintf(str_goalStep,sizeof(str_goalStep), "%d",ReadGoalSteps(8000));
  text_layer_set_text(s_text_goalNumber, str_goalStep);
}

void goal_selected(struct NumberWindow *numberWindow, void *context){
  vibes_short_pulse();
  window_stack_pop(true);
  int number = number_window_get_value(s_window_goals);
  SaveGoalSteps(number);
  callOnGoalChanges(number);
  window_stack_push((Window*)s_window, true);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_window_goals = number_window_create("Daily Step Goal!",
                     (NumberWindowCallbacks){
                      .decremented = NULL,
                      .incremented = NULL,
                      .selected = (NumberWindowCallback) goal_selected
                      },
                      context);
  int32_t savedGoalValue = ReadGoalSteps(8000);
  number_window_set_value(s_window_goals, savedGoalValue);
  number_window_set_max(s_window_goals, 18000);
  number_window_set_min(s_window_goals, 8000);
  number_window_set_step_size(s_window_goals,250);
  window_stack_push((Window*)s_window_goals, true);

}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(s_text_layer, "Up");

}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 60));
  s_text_persianDate = text_layer_create(GRect(0, 60, bounds.size.w, 26));

  s_text_todayStepNumber = text_layer_create(GRect(0, 102, bounds.size.w, 28));
  s_text_ofLabel = text_layer_create(GRect(0, 130, bounds.size.w, 18));
  s_text_goalNumber = text_layer_create(GRect(0, 148, bounds.size.w, 18));

  text_layer_set_background_color(s_text_layer,GColorBlack);
  text_layer_set_text_color(s_text_layer, GColorWhite);
  text_layer_set_text(s_text_layer, "00:00");
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

  text_layer_set_text(s_text_persianDate, "1400/01/03");
  text_layer_set_font(s_text_persianDate, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_text_persianDate, GTextAlignmentCenter);

  // text_layer_set_background_color(s_text_todayStepNumber,GColorBlack);
  text_layer_set_text(s_text_todayStepNumber, "4679");
  // text_layer_set_text_color(s_text_todayStepNumber, GColorWhite);
  text_layer_set_font(s_text_todayStepNumber, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_text_todayStepNumber, GTextAlignmentCenter);

  text_layer_set_text(s_text_ofLabel, "of");
  text_layer_set_font(s_text_ofLabel, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_text_ofLabel, GTextAlignmentCenter);

  static char str_goalStep[8];
  snprintf(str_goalStep,sizeof(str_goalStep), "%d",ReadGoalSteps(8000));
  text_layer_set_text(s_text_goalNumber, str_goalStep);
  text_layer_set_font(s_text_goalNumber, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_text_goalNumber, GTextAlignmentCenter);
    // text_layer_set_background_color(s_text_goalNumber,GColorBlack);


  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_text_persianDate));

  layer_add_child(window_layer, text_layer_get_layer(s_text_todayStepNumber));
  layer_add_child(window_layer, text_layer_get_layer(s_text_ofLabel));
  layer_add_child(window_layer, text_layer_get_layer(s_text_goalNumber));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_text_persianDate);

  text_layer_destroy(s_text_goalNumber);
  text_layer_destroy(s_text_ofLabel);
  text_layer_destroy(s_text_todayStepNumber);
}
static void update_watchface(){
  //update time
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_text_layer, s_buffer);
  //update datetime
  int y, m, d;
  gregorian_to_jalali(&y, &m, &d,1900+tick_time->tm_year,1+tick_time->tm_mon,tick_time->tm_mday);
  static char str[20];
  snprintf(str,sizeof(str), "%d/%d/%d",y,m,d);
  text_layer_set_text(s_text_persianDate, str);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_watchface();
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {
  window_destroy(s_window);
  number_window_destroy(s_window_goals);
}

int main(void) {
  prv_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);
  app_event_loop();
  prv_deinit();
}
