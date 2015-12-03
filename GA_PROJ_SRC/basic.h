#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_CCTV 50
#define MAX_TYPE 10
#define MAX_ARTIFACT 50
#define MAX_OBSTACLE 50

typedef struct {
    int type[MAX_CCTV];
    double dir[MAX_CCTV];
    double fitness;
    double total_cost;
    int total_number;
    double base_secureness;
    int unmonitored_count;
} GENE_T;

int c_count; // Number of point for CCTV installment
int t_count; // Number of CCTV type including no CCTV
int a_count; // Number of artifact needs observation
double cva[MAX_TYPE]; // CCTV visual angle
double cc[MAX_TYPE]; // CCTV cost
double ci[MAX_TYPE]; // Distance from CCTV which an artifact start to be observable
double cr[MAX_TYPE]; // Distance from CCTV which an artifact can be fully monitor; cr > ci
double ca_dis[MAX_CCTV][MAX_ARTIFACT]; // Distance between CCTVs to artifacts
double ca_deg[MAX_CCTV][MAX_ARTIFACT]; // An angle to artifacts with CCTVs as references
double max_cc = 0; // CCTV max price

int ccw(double * a, double * b, double * c) {
    double area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    if(area < 0) return -1;
    else if(area > 0) return 1;
    else return 0;
}
int isIntersect(double * c, double * a, double * o) {
    int t1 = ccw(&c[0], &a[0], &o[0]) * ccw(&c[0], &a[0], &o[2]);
    int t2 = ccw(&o[0], &o[2], &c[0]) * ccw(&o[0], &o[2], &a[0]);
    return (t1 <= 0) && (t2 <= 0);
}
void loadData(char * infile) {
    FILE * fp;
    double c[MAX_CCTV][2];
    double a[MAX_ARTIFACT][2];
    double o[MAX_OBSTACLE][4];
    int i, j, k, visible_flag;
    int o_count;
    char type;

    fp = fopen(infile, "r");
    c_count = 0;
    t_count = 1;
    a_count = 0;
    o_count = 0;
    while(fscanf(fp, "%c", &type) != EOF){
        if(type == 'c') {
            fscanf(fp, "%lf %lf", &c[c_count][0], &c[c_count][1]);
            c_count++;
        }
        else if(type == 't') {
            fscanf(fp, "%lf %lf %lf %lf", &cva[t_count], &cc[t_count], &ci[t_count], &cr[t_count]);
            if(cc[t_count] > max_cc) max_cc = cc[t_count];
            t_count++;
        }
        else if(type == 'a') {
            fscanf(fp, "%lf %lf", &a[a_count][0], &a[a_count][1]);
            a_count++;
        }
        else if(type == 'o') {
            fscanf(fp, "%lf %lf %lf %lf", &o[o_count][0], &o[o_count][1], &o[o_count][2], &o[o_count][3]);
            o_count++;
        }
    }
    fclose(fp);
    for(i = 0; i < c_count; i++) {
        for(j = 0; j < a_count; j++) {
            visible_flag = 1;
            for(k = 0; k < o_count && visible_flag; k++)
                visible_flag = !isIntersect(&c[i][0], &a[j][0], &o[k][0]);
            if(visible_flag) {
                ca_dis[i][j] = sqrt(pow(a[j][0] - c[i][0], 2) + pow(a[j][1] - c[i][1], 2));
                ca_deg[i][j] = atan2(a[j][1] - c[i][1], a[j][0] - c[i][0]) * 180 / M_PI;
            } else {
                ca_dis[i][j] = -1;
                ca_deg[i][j] = 0;
            }
        }
    }
}
void testData() {
    int i, j;

    for(i = 0; i < c_count; i++) {
        for(j = 0; j < a_count; j++) {
            printf("C%02d - A%02d: %7.2lfDIS %7.2lfDEG\n", i, j, ca_dis[i][j], ca_deg[i][j]);
        }
    }
}

#endif
