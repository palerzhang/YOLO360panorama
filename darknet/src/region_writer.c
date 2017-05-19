#include "region_writer.h"
#include <string.h>

// create for 360 panorama by paler

void write_regions_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes)
{
	int i;

	FILE *fp = fopen(filename, "w+");
	if (fp == 0)
	{
		printf("cannot open file.\n");
		return 0;
	}

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh){

            //int width = im.h * .012;

            //if(0){
            //    width = pow(prob, 1./2.)*10+1;
            //    alphabet = 0;
            //}

            //printf("%s: %.0f%%\n", names[class], prob*100);

            //int left  = (b.x-b.w/2.)*im.w;
            //int right = (b.x+b.w/2.)*im.w;
            //int top   = (b.y-b.h/2.)*im.h;
            //int bot   = (b.y+b.h/2.)*im.h;

            //if(left < 0) left = 0;
            //if(right > im.w-1) right = im.w-1;
            //if(top < 0) top = 0;
            //if(bot > im.h-1) bot = im.h-1;
            box b = boxes[i];
            char buf[64];
            sprintf(buf,"%d %.8f %.8f %.8f %.8f %.2f\n", class, b.x, b.y, b.w, b.h, prob);
            fwrite(buf, strlen(buf), 1, fp);
        }
    }
    fclose(fp);
    return;
}

void write_normalized_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes, int gap, int size)
{
	int i;

	FILE *fp = fopen(filename, "w+");
	if (fp == 0)
	{
		printf("cannot open file.\n");
		return 0;
	}

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh)
        {
            box b = boxes[i];

            float x,y,w,h;
            int orign = 1;
            if (b.y > 0.5)
       		{
            	x = b.x + 0.5;
            	x = x > 1 ? x - 1 : x;
            	y = (2.0*size*b.y-size-gap) / (size-gap);
            	w = b.w;
            	h = 2.0*size*b.h / (size-gap);
            	orign = 0;
            }
            else
            {
            	x = b.x;
            	y = 2.0*size*b.y / (size-gap);
            	w = b.w;
            	h = 2.0*size*b.h / (size-gap);
            }

            char buf[64];
            sprintf(buf,"%d %d %.8f %.8f %.8f %.8f %.2f\n", class, orign, x, y, w, h, prob);
            fwrite(buf, strlen(buf), 1, fp);
        }
    }
    fclose(fp);
    return;
}

void swap_float(float * v1, float * v2)
{
    float tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

void swap_b4p(box4panorama * b1, box4panorama * b2)
{
    SWAPVAL(b1->c, b2->c);
    SWAPVAL(b1->u, b2->u);
    swap_float(&b1->l, &b2->l);
    swap_float(&b1->r, &b2->r);
    swap_float(&b1->t, &b2->t);
    swap_float(&b1->b, &b2->b);
    swap_float(&b1->p, &b2->p);
}

void cal_lrtb(box4panorama * b, float x, float y, float w, float h)
{
    b->l = x - w / 2.0;
    b->r = x + w / 2.0;
    b->t = y - h / 2.0;
    b->b = y + h / 2.0;
    if (b->u)
    {
        if (b->l < 0) b->l = 0;
        if (b->r > 1) b->r = 1;
    }
    else
    {
        if (b->l < 0.5) b->l = 0.5;
        if (b->r > 0.5) b->r = 0.5;
    }
    if (b->t < 0) b->t = 0;
    if (b->b > 1) b->b = 1;
}

float b4p_iou(box4panorama * b1, box4panorama * b2)
{
    float _and = VALNONNEG(VALMIN(b1->r, b2->r) - VALMAX(b1->l,b2->l)) * XNNEG(VALMIN(b1->b, b2->b) - VALMAX(b1->t, b2->t));
    float _or = VALNONNEG(VALMAX(b1->r, b2->r) - VALMIN(b1->l,b2->l)) * XNNEG(VALMAX(b1->b, b2->b) - VALMIN(b1->t, b2->t));
    return _or == 0 ? 0 : _and / _or;
}

void box_2_box4panorama(box4panorama * tb4ps, int &tnum, box4panorama * bb4ps, int &bnum, box *boxes, int num, float **probs, int classes, int gap, int size)
{
    // tb4ps and bb4ps does not malloced outside function
    tb4ps = (box4panorama *)malloc(num * sizeof(box4panorama));
    bb4ps = (box4panorama *)malloc(num * sizeof(box4panorama));
    int tidx = 0;
    int bidx = 0;
    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh)
        {
            box b = boxes[i];
            box4panorama b4p;
            b4p.c = class;
            b4p.p = prob;
            if (b.y > 0.5)
            {
                x = b.x + 0.5;
                x = x > 1 ? x - 1 : x;
                y = (2.0*size*b.y-size-gap) / (size-gap);
                w = b.w;
                h = 2.0*size*b.h / (size-gap);
                cal_lrtb(b4p, x, y, w, h);
                b4p.u = 0;
                bb4ps[bidx] = b4p;
                bidx++;
            }
            else
            {
                x = b.x;
                y = 2.0*size*b.y / (size-gap);
                w = b.w;
                h = 2.0*size*b.h / (size-gap);
                cal_lrtb(b4p, x, y, w, h);
                b4p.u = 1;
                tb4ps[tidx] = b4p;
                tidx++;
            }
        }
    }
    tb4ps = (box4panorama *)realloc(tb4ps, tidx * sizeof(box4panorama));
    bb4ps = (box4panorama *)realloc(bb4ps, bidx * sizeof(box4panorama));
    tnum = tidx;
    bnum = bidx;
}

void handle_clip(box4panorama * r1, int &s1, box4panorama * r2, int &s2, float threshold)
{
    for (int i=0;i<s1;i++)
    {
        // find region that was cut
        if (r1[i].l < 0 && r1[i].r > 0)
        {
            box4panorama left, right;
            left.l = r1[i].l + 1;
            left.r = 1;
            left.t = r1[i].t;
            left.b = r1[i].b;

            right.l = 0;
            right.r = r1[i].r;
            right.t = r1[i].t;
            right.b = r1[i].b;

            int j1 = -1;
            int j2 = -1;
            float maxiou1 = 0;
            float maxiou2 = 0;

            for (int j=r2-1;j>=0;j--)
            {
                if (r1[i].c == r2[j].c)
                {
                    float iou1 = b4p_iou(left, r2[j]);

                    float iou2 = b4p_iou(right, r2[j]);

                    if (iou1 > threshold && iou1 > maxiou1)
                    {
                        maxiou1 = iou1;
                        j1 = j;
                    }
                    else if (iou2 > threshold && iou2 > maxiou2)
                    {
                        maxiou2 = iou2;
                        j2 = j;
                    }
                }
            }
            if (j1 >= 0)
            {
                float s = r1[i].p + r2[j1].p;
                r1[i].l = (left.l*r1[i].p + r2[j1].l*r2[j1].p) / s - 1;
                r1[i].t = (left.t*r1[i].p + r2[j1].t*r2[j1].p) / s;
                r1[i].b = (left.b*r1[i].p + r2[j1].b*r2[j1].p) / s;
                swap_b4p(r2[j1], r2[s2-1]);
                s2 --;
                //r2.erase(r2.begin() + j1);
            }
            if (j2 >= 0)
            {
                float s = r1[i].p + r2[j2].p;
                r1[i].r = (right.r*r1[i].p + r2[j2].r*r2[j2].p) / s;
                r1[i].t = (right.t*r1[i].p + r2[j2].t*r2[j2].p) / s;
                r1[i].b = (right.b*r1[i].p + r2[j2].b*r2[j2].p) / s;
                swap_b4p(r2[j2], r2[s2-1]);
                s2 --;
                //r2.erase(r2.begin() + j2);
            }
        }
        else if (r1[i].l < 1 && r1[i].r > 1)
        {
            box4panorama left, right;
            left.l = r1[i].l;
            left.r = 1;
            left.t = r1[i].t;
            left.b = r1[i].b;

            right.l = 0;
            right.r = r1[i].r - 1;
            right.t = r1[i].t;
            right.b = r1[i].b;

            int j1 = -1;
            int j2 = -1;
            float maxiou1 = 0;
            float maxiou2 = 0;

            for (int j=s2-1;j>=0;j--)
            {
                if (r1[i].c == r2[j].c)
                {
                    float iou1 = b4p_iou(left, r2[j]);

                    float iou2 = b4p_iou(right, r2[j]);

                    if (iou1 > threshold && iou1 > maxiou1)
                    {
                        maxiou1 = iou1;
                        j1 = j;
                    }
                    else if (iou2 > threshold && iou2 > maxiou2)
                    {
                        maxiou2 = iou2;
                        j2 = j;
                    }
                }
            }


            if (j1 >= 0)
            {
                float s = r1[i].p + r2[j1].p;
                r1[i].l = (left.l*r1[i].p + r2[j1].l*r2[j1].p) / s;
                r1[i].t = (left.t*r1[i].p + r2[j1].t*r2[j1].p) / s;
                r1[i].b = (left.b*r1[i].p + r2[j1].b*r2[j1].p) / s;
                swap_b4p(r2[j1], r2[s2-1]);
                s2 --;
            }
            if (j2 >= 0)
            {
                float s = r1[i].p + r2[j2].p;
                r1[i].r = (right.r*r1[i].p + r2[j2].r*r2[j2].p) / s + 1;
                r1[i].t = (right.t*r1[i].p + r2[j2].t*r2[j2].p) / s;
                r1[i].b = (right.b*r1[i].p + r2[j2].b*r2[j2].p) / s;
                swap_b4p(r2[j2], r2[s2-1]);
                s2 --;
            }
        }
    }

    for (int i=0;i<s2;i++)
    {
        // find region that was cut
        if (r2[i].l < 0.5 && r2[i].r > 0.5)
        {
            box4panorama left, right;
            left.l = r2[i].l;
            left.r = 0.5;
            left.t = r2[i].t;
            left.b = r2[i].b;

            right.l = 0.5;
            right.r = r2[i].r;
            right.t = r2[i].t;
            right.b = r2[i].b;

            int j1 = -1;
            int j2 = -1;
            float maxiou1 = 0;
            float maxiou2 = 0;

            for (int j=s1-1;j>=0;j--)
            {
                if (r2[i].c == r1[j].c)
                {
                    float iou1 = b4p_iou(left, r1[j]);

                    float iou2 = b4p_iou(right, r1[j]);

                    if (iou1 > threshold && iou1 > maxiou1 && abs(0.5 - r1[j].r) < 0.02)
                    {
                        maxiou1 = iou1;
                        j1 = j;
                    }
                    else if (iou2 > threshold && iou2 > maxiou2 && abs(r1[j].l - 0.5) < 0.02)
                    {
                        maxiou2 = iou2;
                        j2 = j;
                    }
                }
            }
            if (j1 >= 0)
            {
                float s = r2[i].p + r1[j1].p;
                r2[i].l = (left.l*r2[i].p + r1[j1].l*r1[j1].p) / s;
                r2[i].t = (left.t*r2[i].p + r1[j1].t*r1[j1].p) / s;
                r2[i].b = (left.b*r2[i].p + r1[j1].b*r1[j1].p) / s;
                swap_b4p(r1[j1], r2[s1-1]);
                s1 --;
                //r1.erase(r1.begin() + j1);
            }
            if (j2 >= 0)
            {
                float s = r2[i].p + r1[j2].p;
                r2[i].r = (right.r*r2[i].p + r1[j2].r*r1[j2].p) / s;
                r2[i].t = (right.t*r2[i].p + r1[j2].t*r1[j2].p) / s;
                r2[i].b = (right.b*r2[i].p + r1[j2].b*r1[j2].p) / s;
                swap_b4p(r1[j2], r2[s1-1]);
                s1 --;
                //r1.erase(r1.begin() + j2);
            }
        }
    }
    r1 = (box4panorama *)realloc(r1, s1 * sizeof(box4panorama));
    r2 = (box4panorama *)realloc(r2, s2 * sizeof(box4panorama));
}

box4panorama * finetune_both(box4panorama * r1, int &s1, box4panorama * r2, int &s2, float threshold)
{
    int **related_list = (int **)malloc(s1 * sizeof(int*));
    char **tags = (char **)malloc(s1 * sizeof(char*));
    bool * used = (bool *)malloc(s2 * sizeof(bool));
    bool * done = (bool *)malloc(s1 * sizeof(bool));

    // init
    for (int i=0;i<s1;i++)
    {
        related_list[i] = (int *)malloc(s2 * sizeof(int));

    }
}