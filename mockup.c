//testbench file
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int x;
    int y;
} point;
typedef struct {
    int type;
    double dir;
} gene;

const double cva[4] = {0, 60, 90, 120};
const double cc[4] = {0, 3, 6, 12};
const point a[5] = {{10, 10}, {15, 2}, {4, 6}, {7, 9}, {28, 12}};
const point c[5] = {{20, 20}, {10, 0}, {0, 0}, {15, 10}, {30, 25}};

double distance(point a, point c) {
    return pow(((a.x - c.x) * (a.x - c.x)) + ((a.y - c.y) * (a.y - c.y)), 0.5);
}
double * visiblerange(gene g) {
    double * vrange = (double *) calloc(2, sizeof(double));
    vrange[0] = g.dir - (cva[g.type] / 2);
    vrange[1] = g.dir + (cva[g.type] / 2);
    vrange[1] -= (vrange[1] > 180) ? 360 : 0;
    return vrange;
}
int visible(point c, point a, double * crange) {
    double theta = atan2(a.y - c.y, a.x - c.x) * 180 / M_PI;
    return (crange[0] <= crange[1]) ? (theta > crange[0] && theta < crange[1]) : (theta < crange[0] || theta > crange[1]);
}
int main() {
    int i, j;
    double * vrange;
    gene g[5] = {{0, 0}, {0, 45}, {1, 180}, {2, -135}, {3, -90}};

    for(i = 0; i < 5; i++) {
        vrange = visiblerange(g[i]);
        for(j = 0; j < 5; j++) {
            if(visible(c[i], a[j], vrange))
                printf("C%d-A%d: %lf\n", i, j, distance(a[j], c[i]));
        }
    }
}
