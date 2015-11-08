#include <stdio.h>
#include <math.h>

int main() {
    int a1_coordinate[2] = {10, 10};
    int c1_coordinate[2] = {20, 20};
    double c1_direction = -140;
    double c1_visual_angle = 60;
    double c1_min_angle;
    double c1_max_angle;
    double t11;
    int v11;

    c1_min_angle = c1_direction - (c1_visual_angle / 2);
    c1_max_angle = c1_direction + (c1_visual_angle / 2);
    c1_max_angle -= (c1_max_angle > 180) ? 360 : 0;
    t11 = atan2(a1_coordinate[1] - c1_coordinate[1], a1_coordinate[0] - c1_coordinate[0]) * 180 / M_PI;
    v11 = (c1_min_angle < c1_max_angle) ? (t11 > c1_min_angle && t11 < c1_max_angle) : (t11 < c1_min_angle || t11 > c1_max_angle);

    printf("Visible range of CCTV, c1: [%lf, %lf]\n", c1_min_angle, c1_max_angle);
    printf("Theta 1-1: %lf\n", t11);
    printf("Visibility 1-1: %d\n", v11);
}
