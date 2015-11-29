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

int parentSelection(GENE_T * population, int taboo_index) {
    double garray[POPULATION_SIZE], r, d;
    int i, j;

    for(i = 0; i <= POPULATION_SIZE - 1; i++){
        if(i == 0){
            if(taboo_index == 0) garray[i] = 0;
            else garray[i] = population[i].fitness;
        }
        else{
            if(taboo_index == i) garray[i] = garray[i - 1];
            else garray[i] = population[i].fitness + garray[i - 1];
        }
        if(i == POPULATION_SIZE - 1){
            d = RAND_MAX / garray[POPULATION_SIZE - 1];
            //srand(time(NULL));
            r = (rand() / d);
        }
    }
    for(j = 0; j <= POPULATION_SIZE - 1; j++){
        if(j == 0 && 0 < r && r < garray[j]){
            return j;
        }
        else {
            if(r > garray[j - 1] && r <= garray[j])
                return j;
        }
    }
}
//there's chance that gene will mutate
void mutation(GENE_T* child, int i){
    (*child).type[i] = rand() % t_count;;
    (*child).dir[i]  = (double) ((rand()%8)-3)*45;
    //printf("Mutation occur! type %d dir %lf\n", (*child).type[i],(*child).dir[i] );
}
// crossingOver function
GENE_T crossingOver(GENE_T* pop, int p1, int p2){
    int i;
    int j;
    int temp1=0, temp2 = 0;
    int point1, point2; //crossing over point
    double muChance; //mutation chance if less than MUTATION_RATE, it mutate
    GENE_T child;
    int region = 3; //region of crossing over. may be array of int. decide it later.

    while (temp1==temp2){
        temp1=rand()%c_count;
        temp2=rand()%c_count;

    }
    if (temp1<temp2){
        point1=temp1;
        point2=temp2;
    }
    else{
        point1=temp2;
        point2=temp1;
    }
    //printf("point1: %d point2: %d\n",point1,point2 );
    for (i=0;i<point1;i++){
        muChance= (double)((rand()%100)+1)/100;
        //printf("muchance %lf\n", muChance);
        if (muChance<MUTATION_RATE)
            mutation(&child,i);
        else {
            child.type[i]= pop[p1].type[i];
            child.dir[i] = pop[p1].dir[i];
        }
    }

    for (i=point1;i<point2;i++){
        muChance= (double)((rand()%100)+1)/100;
        if (muChance<MUTATION_RATE)
            mutation(&child,i);
        else {
            child.type[i]= pop[p2].type[i];
            child.dir[i] = pop[p2].dir[i];
        }
    }

    for (i=point2;i<c_count;i++){
        muChance= (double)((rand()%100)+1)/100;
        if (muChance<MUTATION_RATE)
            mutation(&child,i);
        else {
            child.type[i]= pop[p1].type[i];
            child.dir[i] = pop[p1].dir[i];
        }
    }
    return child;
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
void sortPopulation(GENE_T * population) {
    // sort individuals in the population according to their fitness in descending order
    int i, j;
    GENE_T a;
    for(i = 0; i < POPULATION_SIZE; i++) {
        for(j = i + 1; j < POPULATION_SIZE; j++) {
            if(population[i].fitness < population[j].fitness) {
                a = population[i];
                population[i] = population[j];
                population[j] = a;
            }
        }
    }
}
GENE_T * initialize() {
    // by Peter
    // generate a random population: array of random-generated gene
    // evaluate each individual in the population: evaluateFitness()
    // sort population according to fitness: sortPopulation()

    //allocate array of population
    int i, j; //counter
    //srand(time(NULL));
    GENE_T *population=(GENE_T*) malloc(sizeof(GENE_T)*POPULATION_SIZE);
    if (population==NULL){
        printf("Error: Cannot allocate memory!\n Exiting\n");
        exit(0);
    }
    //generate random value for types and directions in each population
    for (i=0;i<POPULATION_SIZE;i++){
        for (j=0;j<c_count;j++){
            population[i].type[j]=rand() % t_count;
            population[i].dir[j] = (double) ((rand()%8)-3)*45;
        }
    }
    //evaluate fitness/secureness of each pop
    for(i = 0; i < POPULATION_SIZE; i++)
        evaluateCostFitness(&population[i]);

    //sort pop by fitness in descending
    sortPopulation(population);

    return population;
}

GENE_T * evolve(GENE_T * population) {
    // move elites to new generation
    // geneate the offspring for non-elite individuals replacement: crossingOver()
    // evaluate new individuals: evaluateFitness()
    // sort population according to fitness: sortPopulation()
    int eliteMembers;
    int i;
    int parent1, parent2;
    GENE_T *evoPop=(GENE_T*) malloc(sizeof(GENE_T)*POPULATION_SIZE);
    if (evoPop==NULL){
        printf("Error: Cannot allocate memory!\n Exiting\n");
        exit(0);
    }

    //use ELITISM: copy higher fitness value from highest to eliteMembers.
    //population is already sorted highest fitness descending
    eliteMembers=ELITISM*POPULATION_SIZE;
    for (i=0;i<eliteMembers;i++){
        evoPop[i]=population[i];
    }
    for (i=eliteMembers;i<POPULATION_SIZE;i++){
        parent1=parentSelection(population,-1);
        parent2=parentSelection(population,parent1);
        evoPop[i]=crossingOver(population,parent1,parent2);
        evaluateCostFitness(&evoPop[i]);
    }
    sortPopulation(evoPop);
    return evoPop;
}


int main() {
    int i,j;
    GENE_T *current_generation;
    GENE_T *next_generation;
    clock_t start, diff;
    FILE *fp;
    int milestone=10;
    int msec;

    loadData("input1.txt");
    printf("Read file from input1.txt \n" );
    testData();

    start = clock();
    srand(time(NULL));

    current_generation = initialize();

    fp = fopen("GA_set1prob2.txt","a+");
    if (fp==NULL){
        printf("Error opeining file!\n" );
        exit(0);
    }
    fprintf(fp, "---1\n" );
    diff = clock() - start;
    msec = (diff*1000)/CLOCKS_PER_SEC;
    fprintf(fp, "%d seconds %d milliseconds\n",msec/1000, msec%1000);

    for (i=0;i<c_count;i++){
        fprintf(fp,"dir: %lf, type: %d\n", current_generation[0].dir[i],current_generation[0].type[i]);
    }
    
    for (i=0;i<POPULATION_SIZE;i++){
        fprintf(fp,"%lf\n",current_generation[i].fitness );
    }
    fclose(fp);

    for(i = 2; i <= 1000000; i++) {
        //GA process
        next_generation = evolve(current_generation);
        free(current_generation);
        current_generation = next_generation;

        //print fitness to file
        if (milestone==i){
            fp = fopen("GA_set1prob2.txt","a+");
            if (fp==NULL){
                printf("Error opening file!\n" );
                exit(0);
            }
            fprintf(fp, "---%d\n",milestone );

            diff = clock() - start;
            msec = (diff*1000)/CLOCKS_PER_SEC;
            fprintf(fp, "%d seconds %d milliseconds\n",msec/1000, msec%1000);
            for (i=0;i<c_count;i++){
                fprintf(fp,"dir: %lf, type: %d\n", current_generation[0].dir[i],current_generation[0].type[i]);
            }
            for (i=0;i<POPULATION_SIZE;i++){
                fprintf(fp,"%lf\n",current_generation[i].fitness );

            }
            fclose(fp);
            milestone*=10;
        }

        //print debugging
        /*
        printf("generation %d\n", i);
        for (j=0;j<c_count;j++){
            printf("cctv number: %d type: %d, dir: %lf\n", j, current_generation[0].type[j],current_generation[0].dir[j]);
        }
        printf("GEN%04d: %6.2lf, %6.2lf, %6.2lf \n", i, current_generation[0].fitness, current_generation[1].fitness, current_generation[2].fitness );
        */
    }




    diff = clock() - start;
    msec = (diff*1000)/CLOCKS_PER_SEC;
    printf("Total time: time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}
