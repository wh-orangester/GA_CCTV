#include "basic.h"
#include "ga_basic.h"

int main(int argc, char *argv[]) {
    int i, j;
    GENE_T *current_generation;
    GENE_T *next_generation;
    clock_t start, diff;
    FILE *fp;
    int milestone = 10;
    int msec;

    if (argc != 3) {
        printf("Formats: ./program 'inputfile.txt' 'outputfile.txt'\n" );
        exit(0);
    }
    else {
        loadData(argv[1]); //load data from input file
        printf("Read file from %s \n",argv[1] );
        //testData();

        start = clock();
        srand(time(NULL));

        current_generation = initialize();

        fp = fopen(argv[2],"w");
        if (fp==NULL){
            printf("Error opening file! %s\n",argv[2] );
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
                fp = fopen(argv[2],"a+");
                if (fp==NULL){
                    printf("Error opening file! %s\n",argv[2] );
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
}
