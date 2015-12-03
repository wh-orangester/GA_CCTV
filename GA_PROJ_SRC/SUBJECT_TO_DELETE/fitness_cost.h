#ifndef FITNESS_COST_H
#define FITNESS_COST_H

#include "basic.h"

#define BASE_UNMONITORED_PENALTY 0.5

void evaluateFitness(GENE_T * individual) {
    double visible[MAX_CCTV][MAX_ARTIFACT];
    double min_vrange, max_vrange, deg;
    double total_cost, penalty_ratio;
    int i, j, in_cvr, monitored, p_count;

    // Evaluate each pair of cctv and artifact; whether cctv[i] can see aartifact[j]
    for(i = 0; i < c_count; i++) {
        if((*individual).type[i] == 0) {
            for(j = 0; j < a_count; j++) visible[i][j] = 0;
        }
        else {
            min_vrange = (*individual).dir[i] - (cva[(*individual).type[i]] / 2);
            max_vrange = (*individual).dir[i] + (cva[(*individual).type[i]] / 2);
            max_vrange -= (max_vrange > 180) ? 360 : 0;
            for(j = 0; j < a_count; j++) {
                if(ca_dis[i][j] == -1) visible[i][j] = 0;
                else {
                    deg = ca_deg[i][j];
                    in_cvr = (min_vrange <= max_vrange) ? (deg > min_vrange && deg < max_vrange) : (deg > min_vrange || deg < min_vrange);
                    if(!in_cvr || (ca_dis[i][j] < ci[(*individual).type[i]])) visible[i][j] = 0;
                    else visible[i][j] = 1;
                }
            }
        }
    }
    // Find total cost
    total_cost = 0;
    for(i = 0; i < c_count; i++) total_cost += cc[(*individual).type[i]];
    // Calculate base fitness
    (*individual).fitness = 0.5 + (max_cc * c_count - total_cost) / (2.0 * max_cc * c_count);
    // Apply penalty if some artifact is not monitored
    p_count = 0;
    for(j = 0; j < a_count; j++) {
        monitored = 0;
        for(i = 0; i < c_count && !monitored; i++) {
            monitored = visible[i][j];
        }
        if(!monitored) p_count++;
    }
    // If penalty should be applied
    if(p_count > 0) {
        penalty_ratio = BASE_UNMONITORED_PENALTY + (1 - BASE_UNMONITORED_PENALTY) * (double) (p_count - 1) / (double) (a_count - 1);
        (*individual).fitness = (*individual).fitness * (1 - penalty_ratio);
        //printf("%lf\n", penalty_ratio);
    }
}

#endif
