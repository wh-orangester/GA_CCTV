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
#define MUTATION_RATE 0.9

typedef struct {
    int type[MAX_CCTV];
    double dir[MAX_CCTV];
    double fitness;
} GENE_T;

int c_count=5;
int t_count=3;
int a_count;
double cva[MAX_TYPE];
double cc[MAX_TYPE];
double cr[MAX_TYPE];
double ca_dis[MAX_CCTV][MAX_ARTIFACT];
double ca_deg[MAX_CCTV][MAX_ARTIFACT];

void mutation(GENE_T* child, int i){
    (*child).type[i] = rand() % t_count;;
    (*child).dir[i]  = (double) ((rand()%8)-3)*45;
    printf("Mutation occur! type %d dir %lf\n", (*child).type[i],(*child).dir[i] );
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
    srand(time(NULL));

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
    printf("point1: %d point2: %d\n",point1,point2 );
    for (i=0;i<point1;i++){
        muChance= (double)((rand()%100)+1)/100;
        printf("muchance %lf\n", muChance);
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
int main(){
    GENE_T outgene;
    GENE_T testGENE[2];
    int i,j;
    srand(time(NULL));
    for (j=0;j<2;j++){
        for (i=0;i<c_count;i++){
            testGENE[j].type[i] = rand() % t_count;
            testGENE[j].dir[i]  = (double) ((rand()%8)-3)*45;
        }
    }

    for (i=0;i<c_count;i++){
        printf("Par1: type = %d dir = %lf, \n", testGENE[0].type[i],testGENE[0].dir[i]);
    }

    for (i=0;i<c_count;i++){
        printf("Par2: type = %d dir = %lf, \n", testGENE[1].type[i],testGENE[1].dir[i]);
    }

    outgene=crossingOver(testGENE,0,1);
    for (i=0;i<c_count;i++){
        printf("Outgene: type = %d dir = %lf, \n", outgene.type[i],outgene.dir[i]);
    }
}
