//
// Created by Sprite on 2023/4/3.
//

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "testHal.h"
#include "lvgl.h"

#include "math.h"

/*
    Linux frame buffer like /dev/fb0
*/

/*******************
*       DEFINE
********************/

#define DEMO_MAX 9
#define LV_HOR_RES_MAX 1280
#define LV_VER_RES_MAX 800

static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;

static void draw_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                      LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                               obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_40;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

        /*Remove the masks*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_free_param(&fade_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
        /*Hook the division lines too*/
    else if(dsc->part == LV_PART_MAIN) {
        if(dsc->line_dsc == NULL || dsc->p1 == NULL || dsc->p2 == NULL) return;

        /*Vertical line*/
        if(dsc->p1->x == dsc->p2->x) {
            dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            if(dsc->id == 3) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 1;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }
        }
            /*Horizontal line*/
        else {
            if(dsc->id == 2) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 2;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }

            if(dsc->id == 1  || dsc->id == 3) {
                dsc->line_dsc->color  = lv_palette_main(LV_PALETTE_GREEN);
            }
            else {
                dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            }
        }
    }
}
static void add_data(lv_timer_t * timer)
{
//    LV_UNUSED(timer);
//    static uint32_t cnt = 0;
////    lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
//
//    if(cnt % 4 == 0) lv_chart_set_next_value(chart1, ser2, lv_rand(40, 60));
//
//    cnt++;
}

/**
 * Add a faded area effect to the line chart and make some division lines ticker
 */
void lv_example_chart_2(void)
{
    /*Create a chart1*/
    chart1 = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart1, 480, 320);
    lv_obj_set_style_bg_color(chart1, lv_color_black(), LV_PART_MAIN);
    lv_obj_center(chart1);
    lv_chart_set_type(chart1, LV_CHART_TYPE_SCATTER);   /*Show lines and points too*/

    lv_chart_set_div_line_count(chart1, 8, 8);

    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 800);
    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_X, 0, 480);
    lv_chart_set_point_count(chart1, 480);

//    lv_table_set_col_cnt( chart , 2400);
    lv_obj_add_event_cb(chart1, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_CIRCULAR);

//    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
//    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, points);

    /*Add two data series*/
//    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

    uint32_t i;
    for(i = 0; i < 480; i++) {

        int yMax = lv_bezier3(i, 0, 600, 220, 570);
        lv_chart_set_value_by_id2(chart1, ser2, i, i, yMax);

//        int yMax = lv_bezier3(i, 0, 160, 22, 390);
//        lv_chart_set_next_value(chart1, ser2, yMax);

//        lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
//        lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
    }

//    lv_timer_create(add_data, 200, NULL);
}

int fx(int x)
{
    float t = (float )x/10.0f;
    int y = (int)(80.0f*(sin((float )x/10.0f)+1));
    return y+250;
}


int main(){

    lv_init();

    testHalInit();

    printf("START CURVE DEMO\n");


    if(0){

        lv_example_chart_2();
    }else {

        int pts[16] = {0, 90, 300, 500, 550, 600, 321, 400, 200, 115, 113, 115, 0, 660, 45, 50};

/*Create an array for the points of the line*/
//    lv_point_t line_points[15] = {0};

//    for (int idx=0;idx<15;idx++) {
//        line_points[idx].x = idx * 60;
//        line_points[idx].y = pts[idx];
//    }

        size_t points = 800;
//    int firstP = 0;
//
//    lv_point_t * pointList = (lv_point_t * )malloc( points * sizeof (struct lv_point_t *));
//
//    // 3阶
//    for (int i = 0; i < points; i++) {
//
//        if (i>=742 && i <=746) {
//            firstP = 12;
//
//        }else if ( i < ( points/15 ) ){
//
//            firstP = 0;
//
//        }else if( i >= ( points - points/15 ) ){
//
//            firstP = 15-1-3;
//
//        }else{
//
//            firstP = i/(points/15) - 1;
//        }
//
//        pointList[i].x = i;
//
//        pointList[i].y = lv_bezier3(i, pts[firstP], pts[firstP+1], pts[firstP+2], LV_BEZIER_VAL_MAX);
//        printf("P %d, %4d, %4d, %4d    --  %d \n ",i, pts[firstP], pts[firstP+1], pts[firstP+2],pointList[i].y);
//
//    }

        lv_obj_t *chart = lv_chart_create(lv_scr_act());
//    lv_obj_align(chart,LV_ALIGN_DEFAULT,50,90);
        lv_obj_set_size(chart, 800, 480);
        lv_obj_set_style_line_color(chart, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
        lv_obj_set_style_line_opa(chart, LV_OPA_80, LV_PART_MAIN);
        lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);
//    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(chart, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
        lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
        lv_chart_set_div_line_count(chart, 16, 12);
        lv_chart_series_t *series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_AMBER),
                                                        LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, points);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, points);
        lv_chart_set_point_count(chart, 800);

        lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

        /*Add a faded are effect*/

        for (int i = 0; i <= points; i++) {
            int yMax = fx(i);
//            int yMax = lv_bezier3(i, 300, 1600, 120, LV_BEZIER_VAL_MAX);
            lv_chart_set_value_by_id2(chart, series, i, i, yMax);
        }

        lv_chart_refresh(chart);


//    /*Create style*/
//    static lv_style_t style_line;
//    lv_style_init(&style_line);
//    lv_style_set_line_width(&style_line, 8);
//    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
//    lv_style_set_line_rounded(&style_line, true);
//
//    /*Create a line and apply the new style*/
//    lv_obj_t * line1;
//    line1 = lv_line_create(lv_scr_act());
//    lv_line_set_points(line1, pointList, points);     /*Set the points*/
//    lv_obj_add_style(line1, &style_line, 0);
//    lv_obj_center(line1);

        printf("touchSliderInit\n");
    }

    testHalLoop();
}
