#ifndef MYGA_GENETICALGORITHM_H
#define MYGA_GENETICALGORITHM_H

#include <ostream>
#include <fstream>
#include <functional>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>
#include <ctime>
#include "Chromosome.h"

using std::vector;

typedef float Gene;

enum StopReason{
    MaximumFitnessReached,
    FatalError,
    Continue
};

enum LogClass{
    INFO,
    WARNING,
    ERROR
};

typedef enum{
    ROULETTE_WHEEL_SELECTION,
    RANK_SELECTION,
    TOURNAMENT_SELECTION,
    TRUNCATION_SELECTION
}Selection_Method;

typedef enum {
    SINGLE_POINT_CROSSOVER,
    UNIFORM_CROSSOVER
}Crossover_Method;

struct Params{
    int POPULATION_SIZE;
    Gene GENE_MIN;
    Gene GENE_MAX;
    std::function<double(const Chromosome&)> FITNESS_FUNCTION;
    int MAX_GENERATIONS;
    Selection_Method SELECTION_METHOD;
    Crossover_Method CROSSOVER_METHOD;
    int ELITISM;
    double MUTATION_RATE;
    int TRUNCATE;
    int TOURNAMENT_SIZE;
    double TARGET_FITNESS;
    bool MINIMIZATION_PROBLEM;
    bool VERBOSE;
    bool OUTPUT_FILE;
    Params(){
        //Default values
        POPULATION_SIZE = 0;
        FITNESS_FUNCTION = nullptr;
        MAX_GENERATIONS = 0;
        SELECTION_METHOD = TRUNCATION_SELECTION;
        CROSSOVER_METHOD = SINGLE_POINT_CROSSOVER;
        ELITISM = 0;
        MUTATION_RATE = 0;
        TRUNCATE = 0;
        TARGET_FITNESS = 0;
        TOURNAMENT_SIZE = 0;
        VERBOSE = false;
        OUTPUT_FILE = false;
    }
};

class GeneticAlgorithm{
public:
    typedef struct{
        vector<Chromosome> Individuals;
        bool is_sorted;
    }Population;

    vector<Population> generation_history;

    explicit GeneticAlgorithm(Params &p);

    void run();

    //Visualization functions
    std::ostream& print_population(std::ostream&os) const;

private:
    int current_generation;
    Params params;
    std::ofstream output_file;

    //Core functions
    void new_generation(const Population& elem);
    StopReason step();
    void user_stop_request(int signum);

    //Selection algorithms
    vector<Chromosome> perform_rank_selection(int n = 1);   //TODO
    vector<Chromosome> perform_truncation_selection(int n, int begin, int end);
    vector<Chromosome> perform_roulette_selection(int n = 1);
    vector<Chromosome> perform_tournament_selection(int n = 1,int tournament_size=2);

    //Crossover algorithms
    void perform_single_point_crossover(Chromosome* A, Chromosome* B);
    void perform_uniform_crossover(Chromosome* A, Chromosome* B);

    double rand_double(double min = 0, double max = 1) const;
    static int rand_int(const int & min, const int & max);

    //Logging functions
    void log(std::string message, LogClass logClass = INFO, std::string caller ="", bool printPopulation = false) const;
    void output();
};

#endif //FYGA_GENETICALGORITHM_H
