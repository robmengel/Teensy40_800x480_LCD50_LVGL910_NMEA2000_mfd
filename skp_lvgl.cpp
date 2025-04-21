

#include <Arduino.h>
#include <lvgl.h>
#include "skp_lvgl.h"
#include <Wire.h>

#include <GSL1680.h> //touch driver
#include "SSD1963.h" //display driver

#include "ui.h" //project specific
#include "ui_helpers.h" //project specific

#define WAKE 255
#define INTRPT 20

GSL1680 ts = GSL1680();
uint16_t TouchY;
uint16_t TouchX;
bool     TouchZ = false;

int oldTouchX = 0;
int oldTouchY = 0;

/*Set to your screen resolution*/
#define TFT_HOR_RES   800
#define TFT_VER_RES   480

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
DMAMEM uint32_t draw_buf[DRAW_BUF_SIZE / 4]; // Use ram2
//uint32_t draw_buf[DRAW_BUF_SIZE / 4]; // Use ram2

extern SSD1963 lcd;

/* Display flushing */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, lv_color_t *color_p)
{ 
   uint16_t width = (area->x2 - area->x1 +1);
   uint16_t height = (area->y2 - area->y1+1);
 
   lcd.writeRect(area->x1, area->y1, width, height, (uint16_t *)color_p);
   lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
   
}

bool my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
  uint16_t touchX, touchY;

          int NBFinger = ts.dataread();
          // Retrieve a point  
          for(int i=0; i<NBFinger; i++){
                touchX = ts.readFingerX(i);
                touchY = ts.readFingerY(i);
          
                if ((touchX != oldTouchX) || (touchY != oldTouchY))
                {
                      Serial.print("x= ");
                      Serial.print(touchX,DEC);
                      Serial.print(" y= ");
                      Serial.println(touchY,DEC);
                      
                      oldTouchY = touchY;
                      oldTouchX = touchX;
                      data->state = LV_INDEV_STATE_PR; 
                      data->point.x = touchX;
                      data->point.y = touchY;
                    //  String coord;
                     // coord = " X : "+ String(touchX) + " Y : " + String(touchY);
                      //lv_label_set_text(ui_coord,coord.c_str());
                
                }else
                {
                    data->point.x = oldTouchX;
                    data->point.y = oldTouchY;
                    data->state =LV_INDEV_STATE_REL;
                }
          }

  
    return 0;
}
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}

void skp_lvgl_init(void)
{
    Serial.println("lvgl init");
    //ts.begin(WAKE, INTRPT);
    
    lv_init();
    lv_log_register_print_cb(my_print);

    lv_tick_set_cb(millis);

    lv_display_t * disp;
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*Initialize the (dummy) input device driver*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);
   
    ui_init();

    Serial.println("LVGL demo SK Pang Electronics Ltd 12/24");
    String LVGL_Arduino = " ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  //  lv_label_set_text(ui_version,LVGL_Arduino.c_str());
    ts.begin(WAKE, INTRPT);
    Serial.println("lvgl init done");
      
}
