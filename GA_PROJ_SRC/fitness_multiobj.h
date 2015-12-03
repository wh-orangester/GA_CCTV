#ifndef FITNESS_MULTIOBJ_H
#define FITNESS_MULTIOBJ_H

#include "basic.h"

#define BASE_UNMONITORED_PENALTY 0.5
#define BIAS_COST 0.125
#define BIAS_NUMBER 0.375
#define BIAS_SECURE 0.5
// COST = 0.125, NUMBER = 0.375, SECURE = 0.5

void evaluateFitness(GENE_T * individual) {
    double secureness[MAX_CCTV][MAX_ARTIFACT], a_secureness;
    double min_vrange, max_vrange, deg;
    double total_cost, penalty_ratio;
    double avg_secureness, cost_fitness, number_fitness;
    int cctv_used, p_count;
    int i, j, in_cvr;

    //Secureness Evaluation
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
                    in_cvr = (min_vrange <= max_vrange) ? (deg > min_vrange && deg < max_vrange) : (deg > min_vrange || deg < max_vrange);
                    if(!in_cvr || (ca_dis[i][j] < ci[(*individual).type[i]])) secureness[i][j] = 0;
                    else if(ca_dis[i][j] < cr[(*individual).type[i]]) secureness[i][j] = (ca_dis[i][j] - ci[(*individual).type[i]]) / (cr[(*individual).type[i]] - ci[(*individual).type[i]]);
                    else secureness[i][j] = pow(cr[(*individual).type[i]] / ca_dis[i][j], 2);
                }
            }
        }
    }
    avg_secureness = 0;
    p_count = 0;
    for(j = 0; j < a_count; j++) {
        a_secureness = 1;
        for(i = 0; i < c_count; i++) a_secureness = a_secureness * (1 - secureness[i][j]);
        if((1 - a_secureness) == 0) p_count++;
        avg_secureness += (1.0 / a_count) * (1 - a_secureness);
    }
    //Cost Evaluation
    total_cost = 0;
    for(i = 0; i < c_count; i++) total_cost += cc[(*individual).type[i]];
    cost_fitness = 0.5 + (max_cc * c_count - total_cost) / (2 * max_cc * c_count);

    //Number Evaluation
    cctv_used = 0;
    for(i = 0; i < c_count; i++) {
        if((*individual).type[i] != 0) cctv_used++;
    }
    number_fitness = 0.5 + (c_count - cctv_used) / (2.0 * c_count);
    //Weighted Sum Fitness
    (*individual).fitness = (BIAS_COST * cost_fitness) + (BIAS_NUMBER * number_fitness) + (BIAS_SECURE * avg_secureness);
    //Apply Penalty If Some Artifacts are not monitored
    if(p_count > 0) {
        penalty_ratio = BASE_UNMONITORED_PENALTY + (1 - BASE_UNMONITORED_PENALTY) * ((double) p_count - 1) / ((double) a_count - 1);
        (*individual).fitness = (*individual).fitness * (1 - penalty_ratio);
    }
}

#endif
