#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int type[20];
    double dir[20];
    double fitness;
} GENE_T;

void main(){
    GENE_T *pop=malloc(sizeof(GENE_T)*4);
    pop[2].type[1]=8;
    int a=4;
    int i;
    for (i=0;i<4;i++){
        pop[i].type[3]=i;
    }
    printf("a = %d\n",a );
    for (i=0;i<4;i++){
        printf("aoeuaoseucrh %d\n",pop[i].type[3]=i);
    }
}
