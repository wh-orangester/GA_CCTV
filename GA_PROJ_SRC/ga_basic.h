#ifndef GA_BASIC_H
#define GA_BASIC_H

#include "fitness.h"

#define POPULATION_SIZE 20
#define ELITISM 0.5
#define MUTATION_RATE 0.05

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
        if((j == 0 && 0 < r && r < garray[j]) || (r > garray[j - 1] && r <= garray[j]))
            break;
    }
    return j;
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
        evaluateFitness(&population[i]);

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
        evaluateFitness(&evoPop[i]);
    }
    sortPopulation(evoPop);
    return evoPop;
}

#endif
