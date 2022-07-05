#include <iostream>
#include "GeneticAlgorithm.h"
#include "MatlabEngine.hpp"

float fitness_function(const Chromosome& c){
    return 0.0;
}

int main() {



    system("PAUSE");

    Params p;
    p.GENE_MIN = -5.0;
    p.GENE_MAX = 5.0;
    p.MUTATION_RATE = 0.05;
    p.SELECTION_METHOD = TRUNCATION_SELECTION;
    p.CROSSOVER_METHOD = SINGLE_POINT_CROSSOVER;
    p.MAX_GENERATIONS = 400;
    p.TARGET_FITNESS = 0;
    p.ELITISM = 3;
    p.TRUNCATE = 10;
    p.VERBOSE = true;
    p.POPULATION_SIZE = 20;
    p.REPEAT_PARENTS_SELECTION = false;
    p.FITNESS_FUNCTION = std::move(fitness_function);
    p.OUTPUT_FILE = true;

    GeneticAlgorithm GA(p);
    GA.run();
    return 0;
}

