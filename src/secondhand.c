//----------------------------------------------------------------------------------
//
// Project 4.1 from "Programming Pebble in C"
// 
// Alessio Roberto, June 2016

#include <pebble.h>

static Window *window;
static Layer *seconds_layer;
static int position_x, position_y;
static int center_point_x, center_point_y;
static int second, radius;
static int direction=1;
static GColor time_color;

static void change_colors() {
    
}

static void write_the_second(GContext *ctx, char * text, GFont const font, int text_x, int text_y) {
    graphics_context_set_text_color(ctx, time_color);
    graphics_draw_text(ctx, text, font, 
                     GRect(text_x,text_y,30,30), 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void mark_the_second(GContext *ctx) {
    int text_x, text_y;
    // Check if second is a multple of 5
    if (second % 5 == 0) {
        char text[3];
        int number = second / 5;
        
        if (number == 0) { strcpy(text, "12"); }
        else if (number > 0 && number < 10) {
          text[0] = 48 + number; // Number ASCII value
          text[1] = 0;
        } 
        else if (number == 10){ strcpy(text, "10"); }
        else { strcpy(text, "11"); }
        
        // Now figure out the best position for drawing the numbers.
        text_x = position_x;
        text_y = position_y;
        
        switch (number) {
            case 1: 
                text_x -= 15;
                text_y -= 30;
                break;
            case 2: 
                text_x -= 7;
                text_y -= 20;
                break;
            case 3: 
               text_y -= 15;
               break;
            case 4: 
                text_x -= 7;
                text_y -= 10;
                break;
            case 5: 
                text_x -= 7;
                break;
            case 6: 
                text_x -= 15;
                break;
            case 7: 
                text_x -= 21;
                break;
            case 8: 
                text_x -= 21;
                text_y -= 10;
                break;
            case 9:
                text_x -= 30;
                text_y -= 15;
                break;
            case 10:
               text_x -= 30;
               text_y -= 20;
               break;
            case 11: 
               text_x -= 30;
               text_y -= 30;
               break;
            case 0:
               text_x -= 15;
               text_y -= 30;
               break;
         }
        if (number == 0) {
          write_the_second(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), text_x, text_y);
        } else {
          write_the_second(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_24), text_x, text_y);
      }
    }
}

static void move_second_hand() {
    second = (second + 1) % 60;
    int dangle = TRIG_MAX_ANGLE * second /  60;
    position_x = center_point_x + (radius*direction*sin_lookup(dangle)/TRIG_MAX_RATIO);
    position_y = center_point_y + (-radius*cos_lookup(dangle)/TRIG_MAX_RATIO);
}
    

static void seconds_layer_update_callback(Layer *me, GContext *ctx) { 
    graphics_draw_line(ctx, GPoint(center_point_x, center_point_y), GPoint(position_x, position_y));
    mark_the_second(ctx);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
    move_second_hand();
    change_colors();
    layer_mark_dirty(seconds_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  seconds_layer = layer_create(frame);
  layer_set_update_proc(seconds_layer, seconds_layer_update_callback);
  layer_add_child(window_layer, seconds_layer);
    
}

static void window_unload(Window *window) {
  layer_destroy(seconds_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
        
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);
    center_point_x = frame.size.w/2;
    center_point_y = frame.size.h/2;
    radius = 40;
    second = 0;
    time_color = GColorBlack;

    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}