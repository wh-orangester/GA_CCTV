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
    for (i=eliteMembers;i<c_count;i++){
        parent1=parentSelection(population,-1);
        parent2=parentSelection(population,parent1);
        evoPop[i]=crossingOver(population,parent1,parent2);
        evaluateAverageSecureness(&evoPop[i]);
    }
    sortPopulation(evoPop);
    return evoPop;
}
