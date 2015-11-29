#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define MAX_CCTV 50
#define MAX_TYPE 10
#define MAX_ARTIFACT 50
#define MAX_OBSTACLE 50
#define POPULATION_SIZE 20
#define ELITISM 0.5
//#define CROSSINGOVER_RATE 0.2
#define MUTATION_RATE 0.05
#define BASE_UNMONITORED_PENALTY 0.50

typedef struct {
    int type[MAX_CCTV];
    double dir[MAX_CCTV];
    double fitness;
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

void evaluateCostFitness(GENE_T * individual) {
    double visible[MAX_CCTV][MAX_ARTIFACT];
    double min_vrange, max_vrange, deg;
    double total_cost, penalty_ratio;
    double temp;
    int i, j, in_cvr, monitored, p_count;

    // Evaluate each pair of cctv and artifact; whether cctv[i] can see artifact[j]
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
                    in_cvr = (min_vrange <= max_vrange) ? (deg > min_vrange && deg < max_vrange) : (deg < min_vrange || deg > min_vrange);
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
    (*individual).fitness = 0.5 + ((max_cc * c_count) - total_cost) / (2 * max_cc * c_count);
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
        penalty_ratio = BASE_UNMONITORED_PENALTY + (1 - BASE_UNMONITORED_PENALTY) * (p_count - 1.0) / (a_count - 1.0);
        temp = (*individual).fitness;
        (*individual).fitness = temp * (1 - penalty_ratio);
    }
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Formats: ./program 'inputfile.txt' 'outputfile.txt'\n" );
        exit(0);
    }
    else {
        int i,j,k;
        GENE_T solution;
        GENE_T selectSolution;
        double bestfit=0;
        int duplicateCounter=0;
        unsigned long hist[101]={0};
        int dummy;
        int msec;
        int cummulative=0;
        clock_t start, diff;
        FILE *fp;

        loadData(argv[1]);
        printf("Read file from %s\n", argv[2] );
        testData();
        start = clock();

        for (i=0;i<c_count;i++){
            solution.dir[i]=-135;
            solution.type[i]=0;
            solution.fitness=-1;
        }

        unsigned long allSolution=pow(t_count*8,c_count);
        printf("all solution: %lu \n", allSolution);
        for (i=0;i<allSolution;i++){
            //evaluate
            evaluateCostFitness(&solution);
            //printf("solution.fitness: %lf\n",solution.fitness );
            dummy= (int) floor(solution.fitness*100);
            hist[dummy]+=1;

            if (solution.fitness > bestfit){
                selectSolution=solution;
                bestfit=selectSolution.fitness;
                //printf("bestfit: %lf\n",bestfit );
                duplicateCounter=0;
            }
            else if (solution.fitness == bestfit){
                duplicateCounter++;
            }

            j=0;
            solution.dir[j]+=45;
            while (j<c_count && solution.dir[j]>180) {
                solution.dir[j]=-135;
                j++;
                if (j!=c_count) solution.dir[j]+=45;
                else solution.type[0]+=1;
            }
            j=0;
            while ((j<c_count) && (solution.type[j]>(t_count-1))) {
                solution.type[j]=0;
                j++;
                if (j!=c_count) {
                    solution.type[j]+=1;
                }
            }
            /*
            for (k=0;k<c_count;k++){
                printf("Type: %d, Dir: %lf\n", solution.type[k], solution.dir[k]);
            }
            printf("----------------------------------------------\n" );
            */
        }
        printf("END brute calculation\n" );
        printf("fitness: %lf, dup: %d\n",selectSolution.fitness, duplicateCounter );

        printf("HISTOGRAM\n" );
        for (i=0;i<101;i++){
            cummulative=cummulative+hist[i];
            printf("Value: %d, Freq: %ld, Cummulative: %d\n", i, hist[i], cummulative);
        }
        diff = clock() - start;
        msec = (diff*1000)/CLOCKS_PER_SEC;
        printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

        fp = fopen(argv[2],"w");
        if (fp==NULL){
            printf("Error opening file! %s\n", argv[2] );
            exit(0);
        }
        fprintf(fp, "%d seconds %d milliseconds\n",msec/1000, msec%1000 );
        for (i=0;i<101;i++){
            fprintf(fp, "%lu\n",hist[i] );
        }
        fclose(fp);
    }
}
