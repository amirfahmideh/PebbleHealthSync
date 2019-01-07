#include <pebble.h>

static Window *s_window;
static Window *s_window_goals;


static TextLayer *s_text_layer;
static TextLayer *s_text_GoalNumber;

static void watchface_update_proc(Layer *layer, GContext *ctx) {
  int32_t angle_start = DEG_TO_TRIGANGLE(0);
  int32_t angle_end = DEG_TO_TRIGANGLE(340);
  GRect rect_bounds = layer_get_bounds(layer);
  uint16_t inset_thickness = 10;
  graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness, angle_start, angle_end);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 40));
  text_layer_set_text(s_text_layer, "Press a button");
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  layer_set_update_proc(window_layer, watchface_update_proc);
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}
static void update_watchface(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_text_layer, s_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_watchface();
}
// static void update_time() {
//   text_layer_set_text(s_text_layer, "some text");
//
//   // time_t temp = time(NULL);
//   // struct tm *tick_time = localtime(&temp);
//   // static char s_buffer[8];
//   // strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
//   //                                         "%H:%M" : "%I:%M", tick_time);
//   // text_layer_set_text(s_text_layer, s_buffer);
// }



static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = false;
  window_stack_push(s_window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);
  app_event_loop();
  prv_deinit();
}
