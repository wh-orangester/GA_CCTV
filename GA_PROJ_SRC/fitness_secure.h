#ifndef FITNESS_SECURE_H
#define FITNESS_SECURE_H

#include "basic.h"

#define BASE_UNMONITORED_PENALTY 0.25

void evaluateFitness(GENE_T * individual) {
    double secureness[MAX_CCTV][MAX_ARTIFACT], a_secureness;
    double min_vrange, max_vrange, deg, penalty_ratio;
    int i, j, p_count;
    int visible;

    for(i = 0; i < c_count; i++) {
        if((*individual).type[i] == 0) {
            for(j = 0; j < a_count; j++) secureness[i][j] = 0;
        }
        else {
            min_vrange = (*individual).dir[i] - (cva[(*individual).type[i]] / 2);
            max_vrange = (*individual).dir[i] + (cva[(*individual).type[i]] / 2);
            max_vrange -= (max_vrange > 180) ? 360 : 0;
            for(j = 0; j < a_count; j++) {
                if(ca_dis[i][j] == -1) secureness[i][j] = 0;
                else {
                    deg = ca_deg[i][j];
                    visible = (min_vrange <= max_vrange) ? (deg > min_vrange && deg < max_vrange) : (deg > min_vrange || deg < max_vrange);
                    if(!visible || (ca_dis[i][j] < ci[(*individual).type[i]])) secureness[i][j] = 0;
                    else if(ca_dis[i][j] < cr[(*individual).type[i]]) secureness[i][j] = (ca_dis[i][j] - ci[(*individual).type[i]]) / (cr[(*individual).type[i]] - ci[(*individual).type[i]]);
                    else secureness[i][j] = pow(cr[(*individual).type[i]] / ca_dis[i][j], 2);
                }
            }
        }
    }

    (*individual).fitness = 0;
    p_count = 0;
    for(j = 0; j < a_count; j++) {
        a_secureness = 1;
        for(i = 0; i < c_count; i++) a_secureness = a_secureness * (1 - secureness[i][j]);
        if((1 - a_secureness) == 0) p_count++;
        (*individual).fitness += (1.0 / a_count) * (1 - a_secureness);
    }
    //Apply Penalty If Some Artifacts are not monitored
    if(p_count > 0) {
        penalty_ratio = BASE_UNMONITORED_PENALTY + (1 - BASE_UNMONITORED_PENALTY) * ((double) p_count - 1) / ((double) a_count - 1);
        (*individual).fitness = (*individual).fitness * (1 - penalty_ratio);
    }
}

#endif
