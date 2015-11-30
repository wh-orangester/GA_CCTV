#include "basic.h"
#ifdef PROB1
#include "fitness_secure.h"
#endif
#ifdef PROB2
#include "fitness_cost.h"
#endif
#ifdef PROB3
#include "fitness_number.h"
#endif
#ifdef PROB4
#include "fitness_multiobj.h"
#endif

#define HIST_SIZE 1000

int main(int argc, char *argv[]) {
    int i,j,k;
    GENE_T solution;
    GENE_T selectSolution;
    double bestfit=0;
    int duplicateCounter=0;
    unsigned long hist[HIST_SIZE + 1]={0};
    int dummy;
    int msec;
    int cummulative=0;
    clock_t start, diff;
    FILE *fp;

    if (argc != 3) {
        printf("Formats: ./program 'inputfile.txt' 'outputfile.txt'\n" );
        exit(0);
    }
    else {
        loadData(argv[1]);
        printf("Read file from %s\n", argv[2] );
        //testData();
        printf("END BruteForce Calculation\n" );
        start = clock();

        for (i=0;i<c_count;i++){
            solution.dir[i]=-135;
            solution.type[i]=0;
            solution.fitness=-1;
        }

        unsigned long allSolution=pow(t_count*8,c_count);
        printf("All Possible Solutions: %lu \n", allSolution);
        for (i=0;i<allSolution;i++){
            //evaluate
            evaluateFitness(&solution);
            //printf("solution.fitness: %lf\n",solution.fitness );
            dummy= (int) floor(solution.fitness*HIST_SIZE);
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
        diff = clock() - start;
        msec = (diff*1000)/CLOCKS_PER_SEC;
        printf("END BruteForce Calculation\n" );
        printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
        printf("Global Optimum: %lf, Duplication: %d\n",selectSolution.fitness, duplicateCounter );
        printf("------------------------------------------------------------\n" );
        printf("HISTOGRAM\n" );
        for (i=0;i<HIST_SIZE+1;i++){
            cummulative=cummulative+hist[i];
            printf("Value: %4d, Freq: %10ld, Cummulative: %10d\n", i, hist[i], cummulative);
        }
        printf("------------------------------------------------------------\n" );
        fp = fopen(argv[2],"w");
        if (fp==NULL){
            printf("Error opening file! %s\n", argv[2] );
            exit(0);
        }
        fprintf(fp, "%d seconds %d milliseconds\n",msec/1000, msec%1000 );
        for (i=0;i<HIST_SIZE+1;i++){
            fprintf(fp, "%lu\n",hist[i] );
        }
        fclose(fp);
    }
}
