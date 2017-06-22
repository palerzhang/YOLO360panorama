#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include "box.h"
#include "region_writer.h"

typedef struct {
    int h;
    int w;
    int c;
    float *data;
} image;

float get_color(int c, int x, int max);
void flip_image(image a);
void draw_box(image a, int x1, int y1, int x2, int y2, float r, float g, float b);
void draw_box_width(image a, int x1, int y1, int x2, int y2, int w, float r, float g, float b);
void draw_bbox(image a, box bbox, int w, float r, float g, float b);
void draw_label(image a, int r, int c, image label, const float *rgb);
void write_label(image a, int r, int c, image *characters, char *string, float *rgb);
void draw_detections(image im, int num, float thresh, box *boxes, float **probs, char **names, image **labels, int classes);
image image_distance(image a, image b);
void scale_image(image m, float s);
image crop_image(image im, int dx, int dy, int w, int h);
image random_crop_image(image im, int w, int h);
image random_augment_image(image im, float angle, float aspect, int low, int high, int size);
void random_distort_image(image im, float hue, float saturation, float exposure);
image resize_image(image im, int w, int h);
image resize_min(image im, int min);
image resize_max(image im, int max);
void translate_image(image m, float s);
void normalize_image(image p);
image rotate_image(image m, float rad);
void rotate_image_cw(image im, int times);
void embed_image(image source, image dest, int dx, int dy);
void saturate_image(image im, float sat);
void exposure_image(image im, float sat);
void distort_image(image im, float hue, float sat, float val);
void saturate_exposure_image(image im, float sat, float exposure);
void hsv_to_rgb(image im);
void rgbgr_image(image im);
void constrain_image(image im);
void composite_3d(char *f1, char *f2, char *out, int delta);
int best_3d_shift_r(image a, image b, int min, int max);

image grayscale_image(image im);
image threshold_image(image im, float thresh);

image collapse_image_layers(image source, int border);
image collapse_images_horz(image *ims, int n);
image collapse_images_vert(image *ims, int n);

void show_image(image p, const char *name);
void show_image_normalized(image im, const char *name);
void save_image_png(image im, const char *name);
void save_image(image p, const char *name);
void show_images(image *ims, int n, char *window);
void show_image_layers(image p, char *name);
void show_image_collapsed(image p, char *name);

void print_image(image m);

image make_image(int w, int h, int c);
image make_random_image(int w, int h, int c);
image make_empty_image(int w, int h, int c);
image float_to_image(int w, int h, int c, float *data);
image copy_image(image p);
image load_image(char *filename, int w, int h, int c);
image load_image_color(char *filename, int w, int h);
image **load_alphabet();

float get_pixel(image m, int x, int y, int c);
float get_pixel_extend(image m, int x, int y, int c);
void set_pixel(image m, int x, int y, int c, float val);
void add_pixel(image m, int x, int y, int c, float val);
float bilinear_interpolate(image im, float x, float y, int c);

image get_image_layer(image m, int l);

void free_image(image m);
void test_resize(char *filename);

// used for 360 panorama
image resize_and_merge(image im, int w, int h);
void draw_detections_panorama(image im, int num, box4panorama * b4ps, char **names, image **alphabet, int classes);
void draw_box_panorama(image img, box4panorama * b4p, int width, float r, float g, float b);
void draw_labeled_box_panorama(image img, box4panorama * b4p, int width, float r, float g, float b, image label);
void draw_label_panorama(image a, int r, int c, image label, const float *rgb);
image crop_panorama_image(image im, int x, int y, int w, int h);
image merge_at_cutline(image im, float cutline, int gap);
//void draw_detections_360(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes);


// convert normal image to part of panorama
#define __PI 3.14159265359
#define __STEP 2
#define __TOTL 8

typedef struct{
    float x;
    float y;
    float z;
} point;

typedef struct{
    float theta;
    float phi;
    float r;
} polar;

typedef struct{
    float A;
    float B;
    float C;
    float D;
} plane;

typedef struct{
    int left;
    int right;
    int top;
    int bottom;
} lrtb_box;

float minimum(float v1, float v2, float v3, float v4);

float maximum(float v1, float v2, float v3, float v4);

float rtoa(float r);

float ator(float a);

float predict_cam_dis(int w, int h);

plane get_plane(float r, float theta);

polar map_from_point(point p, float r, float theta, int w, int h);

point map_from_polar(polar p, plane P, float theta, int w, int h);

void generate_points(point *pts, lrtb_box b, int step);

lrtb_box box_transform(point * pts, int size, float r, float theta, int srcw, int srch, int panw, int panh);

image convert(image img, int w, float theta, float r);

#endif

