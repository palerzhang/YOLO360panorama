#ifndef REGION_WRITER_H
#define REGION_WRITER_H

// create for 360 panorama by paler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "box.h"

#define VALMAX(x,y) (x>y?x:y)
#define VALMIN(x,y) (x<y?x:y)
#define VALNONNEG(x) (x<0?0:x) 
#define SWAPVAL(x,y) (x ^= y ^= x ^= y)

struct box4panorama
{
	int c; // class
	int u; // is up
	float l; // left
	float r; // right
	float t; // top
	float b; // bottom
	float p; // probability
};

void swap_float(float * v1, float * v2);

void swap_b4p(box4panorama * b1, box4panorama * b2);

void cal_lrtb(box4panorama * b, float x, float y, float w, float h);

float b4p_iou(box4panorama * b1, box4panorama * b2);

void box_2_box4panorama(box4panorama * tb4ps, int &tnum, box4panorama * bb4ps, int &bnum, box *boxes, int num, float **probs, int classes, int gap, int size);

void handle_clip(box4panorama * r1, int &s1, box4panorama * r2, int &s2, float threshold);

box4panorama * finetune_both(box4panorama * r1, int &s1, box4panorama * r2, int &s2, float threshold);

void write_regions_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes);

void write_normalized_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes, int gap, int size);

#endif