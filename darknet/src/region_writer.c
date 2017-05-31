#include "region_writer.h"
#include <string.h>
#include <stdio.h>

// used for 360 panorama

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
    swap_float(&(b1->l), &(b2->l));
    swap_float(&(b1->r), &(b2->r));
    swap_float(&(b1->t), &(b2->t));
    swap_float(&(b1->b), &(b2->b));
    swap_float(&(b1->p), &(b2->p));
}

void cal_lrtb(box4panorama * b, float x, float y, float w, float h)
{
    b->l = x - w / 2.0;
    b->r = x + w / 2.0;
    b->t = y - h / 2.0;
    b->b = y + h / 2.0;
    if (b->u == 1)
    {
        if (b->l < 0) b->l = 0;
        if (b->r > 1) b->r = 1;
    }
    if (b->t < 0) b->t = 0;
    if (b->b > 1) b->b = 1;
}

float b4p_iou(box4panorama * b1, box4panorama * b2)
{
    float _and = VALNONNEG(VALMIN(b1->r, b2->r) - VALMAX(b1->l,b2->l)) * VALNONNEG(VALMIN(b1->b, b2->b) - VALMAX(b1->t, b2->t));
    float _or = VALNONNEG(VALMAX(b1->r, b2->r) - VALMIN(b1->l,b2->l)) * VALNONNEG(VALMAX(b1->b, b2->b) - VALMIN(b1->t, b2->t));
    return _or == 0 ? 0 : _and / _or;
}

void box_2_box4panorama(box4panorama * tb4ps, int * tnum, box4panorama * bb4ps, int *bnum, box *boxes, int num, float thresh, float **probs, int classes, int gap, int size)
{
    // tb4ps and bb4ps does not malloced outside function
    int tidx = 0;
    int bidx = 0;
    int cnt = 0;
    for(int i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh)
        {
            box b = boxes[i];
            box4panorama b4p;
            b4p.c = class;
            b4p.p = prob;
            cnt ++;

            float x,y,w,h;
            if (b.y > 0.5)
            {
                x = b.x + 0.5;
                x = x > 1 ? x - 1 : x;
                y = (2.0*size*b.y-size-gap) / (size-gap);
                w = b.w;
                h = 2.0*size*b.h / (size-gap);
                b4p.u = 0;
                cal_lrtb(&b4p, x, y, w, h);
                //bb4ps[bidx] = b4p;
                b4p_copy(&bb4ps[bidx], &b4p);
                bidx++;
            }
            else
            {
                x = b.x;
                y = 2.0*size*b.y / (size-gap);
                w = b.w;
                h = 2.0*size*b.h / (size-gap);
                b4p.u = 1;
                cal_lrtb(&b4p, x, y, w, h);
                //tb4ps[tidx] = b4p;
                b4p_copy(&tb4ps[tidx], &b4p);
                tidx++;
            }
        }
    }
    tb4ps = (box4panorama *)realloc(tb4ps, tidx * sizeof(box4panorama));
    bb4ps = (box4panorama *)realloc(bb4ps, bidx * sizeof(box4panorama));
    *tnum = tidx;
    *bnum = bidx;
}

void handle_clip(box4panorama * r1, int *size1, box4panorama * r2, int *size2, float threshold)
{
    int s1 = *size1;
    int s2 = *size2;
    
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

            float ll = left.l * r1[i].p;
            float lt = left.t * r1[i].p;
            float lb = left.b * r1[i].p;
            float lcnt = 0;//r1[i].p;

            float rr = right.r * r1[i].p;
            float rt = right.t * r1[i].p;
            float rb = right.b * r1[i].p;
            float rcnt = 0;//r1[i].p;

            //int j1 = -1;
            //int j2 = -1;
            //float maxiou1 = 0;
            //float maxiou2 = 0;

            for (int j=s2-1;j>=0;j--)
            {
                if (r1[i].c == r2[j].c)
                {
                    float iou1 = b4p_iou(&left, &r2[j]);

                    float iou2 = b4p_iou(&right, &r2[j]);

                    if (iou1 > threshold)// && iou1 > maxiou1)
                    {
                        //maxiou1 = iou1;
                        //j1 = j;
                        ll += r2[j].l * r2[j].p;
                        lt += r2[j].t * r2[j].p;
                        lb += r2[j].b * r2[j].p;
                        lcnt += r2[j].p;
                        swap_b4p(&r2[j], &r2[s2-1]);
                        s2 --;
                    }
                    else if (iou2 > threshold)// && iou2 > maxiou2)
                    {
                        //maxiou2 = iou2;
                        //j2 = j;
                        rr += r2[j].r * r2[j].p;
                        rt += r2[j].t * r2[j].p;
                        rb += r2[j].b * r2[j].p;
                        rcnt += r2[j].p;
                        swap_b4p(&r2[j], &r2[s2-1]);
                        s2 --;
                    }
                }
            }


            if (lcnt > 0)
            {
                lcnt += r1[i].p;
                //float s = r1[i].p + r2[j1].p;
                r1[i].l = ll / lcnt;//(left.l*r1[i].p + r2[j1].l*r2[j1].p) / s;
                r1[i].t = lt / lcnt;//(left.t*r1[i].p + r2[j1].t*r2[j1].p) / s;
                r1[i].b = lb / lcnt;//(left.b*r1[i].p + r2[j1].b*r2[j1].p) / s;
                //swap_b4p(&r2[j1], &r2[s2-1]);
                //s2 --;
            }
            if (rcnt > 0)
            {
                rcnt += r1[i].p;
                //float s = r1[i].p + r2[j2].p;
                r1[i].r = rr / rcnt;//(right.r*r1[i].p + r2[j2].r*r2[j2].p) / s + 1;
                r1[i].t = rt / rcnt;//(right.t*r1[i].p + r2[j2].t*r2[j2].p) / s;
                r1[i].b = rb / rcnt;//(right.b*r1[i].p + r2[j2].b*r2[j2].p) / s;
                //swap_b4p(&r2[j2], &r2[s2-1]);
                //s2 --;
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

            float ll = left.l * r1[i].p;
            float lt = left.t * r1[i].p;
            float lb = left.b * r1[i].p;
            float lcnt = 0;//r1[i].p;

            float rr = right.r * r1[i].p;
            float rt = right.t * r1[i].p;
            float rb = right.b * r1[i].p;
            float rcnt = 0;//r1[i].p;

            //int j1 = -1;
            //int j2 = -1;
            //float maxiou1 = 0;
            //float maxiou2 = 0;

            for (int j=s2-1;j>=0;j--)
            {
                if (r1[i].c == r2[j].c)
                {
                    float iou1 = b4p_iou(&left, &r2[j]);

                    float iou2 = b4p_iou(&right, &r2[j]);

                    if (iou1 > threshold)// && iou1 > maxiou1)
                    {
                        //maxiou1 = iou1;
                        //j1 = j;
                        ll += r2[j].l * r2[j].p;
                        lt += r2[j].t * r2[j].p;
                        lb += r2[j].b * r2[j].p;
                        lcnt += r2[j].p;
                        swap_b4p(&r2[j], &r2[s2-1]);
                        s2 --;
                    }
                    else if (iou2 > threshold)// && iou2 > maxiou2)
                    {
                        //maxiou2 = iou2;
                        //j2 = j;
                        rr += r2[j].r * r2[j].p;
                        rt += r2[j].t * r2[j].p;
                        rb += r2[j].b * r2[j].p;
                        rcnt += r2[j].p;
                        swap_b4p(&r2[j], &r2[s2-1]);
                        s2 --;
                    }
                }
            }


            if (lcnt > 0)
            {
                lcnt += r1[i].p;
                //float s = r1[i].p + r2[j1].p;
                r1[i].l = ll / lcnt;//(left.l*r1[i].p + r2[j1].l*r2[j1].p) / s;
                r1[i].t = lt / lcnt;//(left.t*r1[i].p + r2[j1].t*r2[j1].p) / s;
                r1[i].b = lb / lcnt;//(left.b*r1[i].p + r2[j1].b*r2[j1].p) / s;
                //swap_b4p(&r2[j1], &r2[s2-1]);
                //s2 --;
            }
            if (rcnt > 0)
            {
                rcnt += r1[i].p;
                //float s = r1[i].p + r2[j2].p;
                r1[i].r = rr / rcnt;//(right.r*r1[i].p + r2[j2].r*r2[j2].p) / s + 1;
                r1[i].t = rt / rcnt;//(right.t*r1[i].p + r2[j2].t*r2[j2].p) / s;
                r1[i].b = rb / rcnt;//(right.b*r1[i].p + r2[j2].b*r2[j2].p) / s;
                //swap_b4p(&r2[j2], &r2[s2-1]);
                //s2 --;
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

            float ll = left.l * r2[i].p;
            float lt = left.t * r2[i].p;
            float lb = left.b * r2[i].p;
            float lcnt = 0;//r2[i].p;

            float rr = right.r * r2[i].p;
            float rt = right.t * r2[i].p;
            float rb = right.b * r2[i].p;
            float rcnt = 0;//r2[i].p;

            //int j1 = -1;
            //int j2 = -1;
            //float maxiou1 = 0;
            //float maxiou2 = 0;

            for (int j=s1-1;j>=0;j--)
            {
                if (r2[i].c == r1[j].c)
                {
                    float iou1 = b4p_iou(&left, &r1[j]);

                    float iou2 = b4p_iou(&right, &r1[j]);

                    //printf("k:%d c:%d u:%d l:%.8f r:%.8f t:%.8f b:%.8f iou1:%.4f iou2:%.4f\n", j, r1[j].c, r1[j].l, r1[j].u, r1[j].r, r1[j].t, r1[j].b, iou1, iou2);

                    if (iou1 > threshold /*&& iou1 > maxiou1*/ && abs(0.5 - r1[j].r) < 0.02)
                    {
                        //maxiou1 = iou1;
                        //j1 = j;
                        ll += r1[j].l * r1[j].p;
                        lt += r1[j].t * r1[j].p;
                        lb += r1[j].b * r1[j].p;
                        lcnt += r1[j].p;
                        swap_b4p(&r1[j], &r1[s1-1]);
                        s1 --;
                    }
                    else if (iou2 > threshold /*&& iou2 > maxiou2*/ && abs(r1[j].l - 0.5) < 0.02)
                    {
                        //maxiou2 = iou2;
                        //j2 = j;
                        rr += r1[j].r * r1[j].p;
                        rt += r1[j].t * r1[j].p;
                        rb += r1[j].b * r1[j].p;
                        rcnt += r1[j].p;
                        swap_b4p(&r1[j], &r1[s1-1]);
                        s1 --;
                    }
                }
            }
            if (lcnt > 0)
            {
                lcnt += r2[i].p;
                //float s = r2[i].p + r1[j1].p;
                r2[i].l = ll / lcnt;//(left.l*r2[i].p + r1[j1].l*r1[j1].p) / s;
                r2[i].t = lt / lcnt;//(left.t*r2[i].p + r1[j1].t*r1[j1].p) / s;
                r2[i].b = lb / lcnt;//(left.b*r2[i].p + r1[j1].b*r1[j1].p) / s;
                //swap_b4p(&r1[j1], &r1[s1-1]);
                //s1 --;
                //printf("------\n");
                //r1.erase(r1.begin() + j1);
            }
            if (rcnt > 0)
            {
                rcnt += r2[i].p;
                //float s = r2[i].p + r1[j2].p;
                r2[i].r = rr / rcnt;//(right.r*r2[i].p + r1[j2].r*r1[j2].p) / s;
                r2[i].t = rt / rcnt;//(right.t*r2[i].p + r1[j2].t*r1[j2].p) / s;
                r2[i].b = rb / rcnt;//(right.b*r2[i].p + r1[j2].b*r1[j2].p) / s;
                //swap_b4p(&r1[j2], &r1[s1-1]);
                //s1 --;
                //printf("------\n");
                //r1.erase(r1.begin() + j2);
            }
        }
    }
    r1 = (box4panorama *)realloc(r1, s1 * sizeof(box4panorama));
    r2 = (box4panorama *)realloc(r2, s2 * sizeof(box4panorama));
    *size1 = s1;
    *size2 = s2;
    //printf("s1:%d, s2:%d\n", s1, s2);
}

void finetune_both(box4panorama * r1, int *size1, box4panorama * r2, int *size2, float threshold, box4panorama * merged, int * size)
{
    int s1 = *size1;
    int s2 = *size2;

    int **related_list = (int **)malloc(s1 * sizeof(int*));
    char **tags = (char **)malloc(s1 * sizeof(char*));
    short * used = (short *)malloc(s2 * sizeof(short));
    short * done = (short *)malloc(s1 * sizeof(short));

    // init
    for (int j=0;j<s2;j++)
        used[j] = 0;
    for (int i=0;i<s1;i++)
    {
        related_list[i] = (int *)malloc(s2 * sizeof(int));
        tags[i] = (char *)malloc(s2 * ( sizeof(char) + 1) );
        done[i] = 0;
        for (int j=0;j<s2;j++)
        {
            related_list[i][j] = 0;
            tags[i][j] = '0';
            if (r1[i].c == r2[j].c)
            {
                float iou = b4p_iou(&r1[i], &r2[j]);
                if (iou > threshold)
                {
                    related_list[i][j] = 1;
                    tags[i][j] = '1';
                    used[j] = 1;
                }
            }
        }
        tags[i][s2] = '\0';
    }

    int k = 0;

    for (int j=0;j<s2;j++)
    {
        if (!used[j])
        {
            b4p_copy(&merged[k], &r2[j]);
            k++;
        }
    }

    for (int i=0;i<s1;i++)
    {
        if (strequal_char(tags[i], '0'))
        {
            b4p_copy(&merged[k], &r1[i]);
            k++;
            done[i] = 1;
        }
    }

    for (int i=0;i<s1;i++)
    {
        if (done[i] == 1) continue;
        else
        {
            //int c = r1[i].c;
            //int u = r1[i].u;
            float p = r1[i].p;
            float l = r1[i].l * p;
            float r = r1[i].r * p;
            float t = r1[i].t * p;
            float b = r1[i].b * p;            
            float cnt = p;
            p *= p;           

            //box4panorama b4p;

            for (int j=i+1;j<s1;j++)
            {
                if (strcmp(tags[i], tags[j]) == 0)
                {
                    l += r1[j].l * r1[j].p;
                    r += r1[j].r * r1[j].p;
                    t += r1[j].t * r1[j].p;
                    b += r1[j].b * r1[j].p;
                    p += r1[j].p * r1[j].p;
                    cnt += r1[j].p;
                    done[j] = 1;
                }
            }
            merged[k].c = r1[i].c;
            merged[k].u = r1[i].u;
            merged[k].l = l / cnt;
            merged[k].r = r / cnt;
            merged[k].t = t / cnt;
            merged[k].b = b / cnt;
            merged[k].p = p / cnt;
            k++;
            done[i] = 1;
            //b4p_copy(&merged[k], &b4p)
        }
    }

    *size = k;
    merged = (box4panorama *)realloc(merged, k * sizeof(box4panorama));

    //for (int i=0;i<k;i++)
    //{
    //    printf("k:%d c:%d u:%d l:%.8f r:%.8f t:%.8f b:%.8f\n", i, merged[i].c, merged[i].l, merged[i].u, merged[i].r, merged[i].t, merged[i].b);
    //}

    free(used);
    free(done);
    for (int i=0;i<s1;i++)
    {
        free (related_list[i]);
        free (tags[i]);
    }
    free (related_list);
    free (tags);
}

void finetune_self(box4panorama * rg, int * size, float threshold)
{
    int s = *size;
    float cl[3] = {0.5, 1.0, 0.0};
    int st = 0;
    int en = 0;
    for (int i=0;i<s;i++)
    {
        if (rg[i].u == 1)
        {
            st = 0;
            en = 1;
        }
        else
        {
            st = 1;
            en = 3;
        }
        for (int m = st;m<en;m++)
        {
            if (rg[i].l < cl[m] && rg[i].r > cl[m])
            {
                float p = rg[i].p;
                float l = rg[i].l * p;
                float r = rg[i].r * p;
                float t = rg[i].t * p;
                float b = rg[i].b * p;
                float cnt = p;
                p *= p;           
                for (int j=s-1;j>i;j--)
                {
                    if (rg[j].c == rg[i].c && rg[j].l < cl[m] && rg[j].r > cl[m])
                    {
                        float iou = b4p_iou(&rg[i], &rg[j]);
                        if (iou > threshold)
                        {
                            l += rg[j].l * rg[j].p;
                            r += rg[j].r * rg[j].p;
                            t += rg[j].t * rg[j].p;
                            b += rg[j].b * rg[j].p;
                            p += rg[j].p * rg[j].p;
                            cnt += rg[j].p;
                            swap_b4p(&rg[j], &rg[s-1]);
                            s --;
                        }
                    }
                }
                rg[i].l = l / cnt;
                rg[i].r = r / cnt;
                rg[i].t = t / cnt;
                rg[i].b = b / cnt;
                rg[i].p = p / cnt;
            }
        }            
    }
    *size = s;
    rg = (box4panorama *)realloc(rg, s * sizeof(box4panorama));
}

b4p_copy(box4panorama * dst, const box4panorama * src)
{
    dst->c = src->c;
    dst->u = src->u;
    dst->l = src->l;
    dst->r = src->r;
    dst->t = src->t;
    dst->b = src->b;
    dst->p = src->p;
}

int strequal_char(const char * str, char c)
{
    int i=0;
    do{
        if (!(str[i] == c))
            return 0;
        i++;
    } while(str[i] != '\0');
    return 1;
}