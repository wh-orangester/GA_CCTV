#include <stdio.h>
#include <math.h>

double distance(int ax, int ay, int cx, int cy) {
    return pow(((ax - cx) * (ax - cx)) + ((ay - cy) * (ay - cy)), 0.5);
}
int visibility(int ax, int ay, int cx, int cy, double cmin, double cmax) {
    double theta;
    theta = atan2(ay - cy, ax - cx) * 180 / M_PI;
    return (cmin < cmax) ? (theta > cmin && theta < cmax) : (theta < cmin || theta > cmax);
}
int main() {
    int i, j;
    int ax[5] = {10, 15, 4, 7, 28};
    int ay[5] = {10, 2, 6, 9, 12};
    int cx[5] = {20, 10, 0, 15, 30};
    int cy[5] = {20, 0, 0, 10, 25};
    double cd[5] = {0, 45, 180, -135, -90};
    double cva[5] = {60, 60, 60, 90, 120};
    double cmin;
    double cmax;

    for(i = 0; i < 5; i++) {
        cmin = cd[i] - (cva[i] / 2);
        cmax = cd[i] + (cva[i] / 2);
        cmax -= (cmax > 180) ? 360 : 0;
        for(j = 0; j < 5; j++) {
            if(visibility(ax[j], ay[j], cx[i], cy[i], cmin, cmax))
                printf("C%d-A%d: %lf\n", i, j, 48 / pow(distance(ax[j], ay[j], cx[i], cy[i]), 2));
        }
    }
}
