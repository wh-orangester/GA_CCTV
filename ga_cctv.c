#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CCTV 50
#define MAX_TYPE 10
#define MAX_ARTIFACT 50
#define MAX_OBSTACLE 50
#define POPULATION_SIZE 20
#define ELITISM 0.5
#define CROSSINGOVER_RATE 0.2
#define MUTATION_RATE 0.05

typedef struct {
    int type[MAX_CCTV];
    double dir[MAX_CCTV];
    double fitness;
} GENE_T;

int c_count;
int t_count;
int a_count;
double cva[MAX_TYPE];
double cc[MAX_TYPE];
double cr[MAX_TYPE];
double ca_dis[MAX_CCTV][MAX_ARTIFACT];
double ca_deg[MAX_CCTV][MAX_ARTIFACT];

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
            fscanf(fp, "%lf %lf %lf", &cva[t_count], &cc[t_count], &cr[t_count]);
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
                ca_dis[i][j] = pow(a[j][0] - c[i][0], 2) + pow(a[j][1] - c[i][1], 2);
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

GENE_T parentSelection(GENE_T * population) {
    // ** Roulette Wheel Selection **
}
GENE_T crossingOver(GENE_T parent1, GENE_T parent2) {

}
void evaluateAverageSecureness(GENE_T * individual) {
    double secureness[MAX_CCTV][MAX_ARTIFACT], a_secureness;
    double min_vrange, max_vrange, deg;
    int i, j;
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
                    visible = (min_vrange <= max_vrange) ? (deg > min_vrange && deg < max_vrange) : (deg < min_vrange || deg > min_vrange);
                    if(!visible) secureness[i][j] = 0;
                    else secureness[i][j] = 1 / (1 + (ca_dis[i][j] / cr[(*individual).type[i]]));
                }
            }
        }
    }

    (*individual).fitness = 0;
    for(j = 0; j < a_count; j++) {
        a_secureness = 1;
        for(i = 0; i < c_count; i++) a_secureness = a_secureness * (1 - secureness[i][j]);
        (*individual).fitness += (1.0 / a_count) * (1 - a_secureness);
    }
}
void sortPopulation(GENE_T * population) {
    // sort individuals in the population according to their fitness
    int i, j;
    double a;
    for(i = 0; i < POPULATION_SIZE; i++) {
        for(j = i + 1; j < POPULATION_SIZE; j++) {
            if(population[i].fitness < population[j].fitness) {
                a = population[i].fitness;
                population[i].fitness = population[j].fitness;
                population[j].fitness = a;
            }
        }
    }
}
GENE_T * initialize() {
    // generate a random population: array of random-generated gene
    // evaluate each individual in the population: evaluateFitness()
    // sort population according to fitness: sortPopulation()
}
GENE_T * evolve(GENE_T * population) {
    // move elites to new generation
    // geneate the offspring for non-elite individuals replacement: crossingOver()
    // evaluate new individuals: evaluateFitness()
    // sort population according to fitness: sortPopulation()
}

int main() {
    int i;
    GENE_T * current_generation, next_generation;

    loadData("input.txt");
    testData();
    current_generation = initialize();
    for(i = 2; i <= 1000; i++) {
        next_generation = evole(current_generation);
        clear(current_generation);
        current_generation = next_generation;
    }
}
