//
// Created by valen on 26/03/2022.
//

#include <chrono>
#include <cstring>
#include "GeneticAlgorithm.h"


GeneticAlgorithm::GeneticAlgorithm(Params &p) : params(std::move(p)){
    if(params.OUTPUT_FILE){
        std::string filename = "";

        std::time_t rawtime;
        std::tm* timeinfo;


        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);
        char buffer [80];
        std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
        std::puts(buffer);

        filename.append(buffer);
        filename.append(".gaoutput");

        output_file.open(filename,std::fstream::out);

    }

    current_generation = -1;
    if(params.ELITISM>params.POPULATION_SIZE){
        log("Elitism greater than population size",ERROR, "GA Constructor");
        return;
    }
    Population first_population;
    for(int i = 0;i <params.POPULATION_SIZE; i++){
        first_population.Individuals.emplace_back(params.GENE_MIN,params.GENE_MAX);
        first_population.Individuals[i].init_rand();
    }

    new_generation(first_population);
}

std::ostream& GeneticAlgorithm::print_population(std::ostream&os) const {
    for(int i = 0;i < params.POPULATION_SIZE; i++){
        os<<generation_history[current_generation].Individuals[i]<<std::endl;
    }
    return os;
}
void GeneticAlgorithm::new_generation(const Population& elem) {
    generation_history.push_back(elem);
    current_generation++;
}

void GeneticAlgorithm::log(std::string message, LogClass logClass, std::string caller, bool printPopulation) const {
    if(params.VERBOSE) {
        switch(logClass){
            case INFO:
                std::cout<<"INFO: ";
                break;
            case WARNING:
                std::cout<<"WARNING: ";
                break;
            case ERROR:
                std::cout<<"ERROR: ";
                break;
        }
        std::cout<<message;
        if(!caller.empty())
            std::cout << ", CALLER: "<< caller;
        if(printPopulation){
            std::cout<<std::endl;
            //print_population(std::cout);
            std::cout<<generation_history[current_generation].Individuals[0]<<std::endl;
        }
        std::cout<<std::endl;
    }
}

void GeneticAlgorithm::output() {
    if(output_file.is_open()){
        for(int i = 0; i<=current_generation;i++) {
            output_file<<"Sim{"<<i+1<<"} = {\n";
            for(int j = 0; j<params.POPULATION_SIZE;j++) {
                output_file << generation_history[i].Individuals[j];
                output_file<<";"<<std::endl;
            }
            output_file<<"};\n";
        }

    }else{
        log("Error, file is not open!",ERROR,"output");
    }
    output_file.close();
}

void GeneticAlgorithm::run() {
    StopReason stopReason;
    for(int i = 0; i<params.MAX_GENERATIONS;i++){
        stopReason = step();
        if(stopReason != StopReason::Continue) break;
    }

    switch (stopReason) {
        case (MaximumFitnessReached):
            log("Problem optimized, target fitness reached!", INFO, "run", true);
            break;
        case (FatalError):
            log("Fatal error during iteration, aborting...", ERROR);
            break;
        case(Continue):
            std::sort(generation_history[current_generation].Individuals.begin(),
                      generation_history[current_generation].Individuals.end(), std::less<>());
            generation_history[current_generation].is_sorted = true;
            log("Maximum number of generations reached",WARNING,"run",true);
            break;
    }
    if(params.OUTPUT_FILE){
        log("Printing population...");
        output();
    }
}

StopReason GeneticAlgorithm::step() {

    Population& current_population = generation_history[current_generation];

    //Evaluate fitness
    double fit = 0;
    bool optimized = false;
    for(int i = 0; i<params.POPULATION_SIZE; i++){
        fit = params.FITNESS_FUNCTION(current_population.Individuals[i]);
        if(fit <= params.TARGET_FITNESS) optimized |= true;
        current_population.Individuals[i].set_fitness(fit);
    }
    std::sort(current_population.Individuals.begin(),
              current_population.Individuals.end(), std::less<>());
    current_population.is_sorted = true;

    if(optimized)
        return MaximumFitnessReached;

    log(std::string("Generation ") + std::to_string(current_generation), INFO, "step", true);

    //Offspring
    Population new_population = Population();

    //Perform elitism
    for(int i = 0; i<params.ELITISM; i++){
        new_population.Individuals.insert(new_population.Individuals.begin(),generation_history[current_generation].Individuals[i]);
    }

    //Select parents and add to the new generation
    vector<Chromosome> rand_selected;
    int children = params.ELITISM;
    int trials;
    //TODO necessario? io lo toglierei anche perchè metto false a repeat parents selection
    while(children < params.POPULATION_SIZE) {
        trials = 0;
        do{

            if (params.REPEAT_PARENTS_SELECTION && (trials > params.MAX_REPETITIONS)) {
                params.REPEAT_PARENTS_SELECTION = false;
                log("No more repeating of parent's selection");
            }
            if (params.REPEAT_PARENTS_SELECTION) {
                trials++;
            }


            switch(params.SELECTION_METHOD) {
                case ROULETTE_WHEEL_SELECTION:
                    rand_selected = perform_roulette_selection(2);
                    break;
                case RANK_SELECTION:
                    rand_selected = perform_rank_selection(2);
                    break;
                case TRUNCATION_SELECTION:
                    rand_selected = perform_truncation_selection(2,params.ELITISM, params.TRUNCATE-1);
                    break;
                default:
                    log("Selection method not found",ERROR,"step");
                    return FatalError;
            }
        }while((rand_selected[0].Genetic_Material == rand_selected[1].Genetic_Material) &&
                params.REPEAT_PARENTS_SELECTION);

        //Crossover
        switch(params.CROSSOVER_METHOD){
            case SINGLE_POINT_CROSSOVER:
                perform_single_point_crossover(&rand_selected[0], &rand_selected[1]);
                break;
            case UNIFORM_CROSSOVER:
                //perform_uniform_crossover(&rand_selected[0], &rand_selected[1]);
                break;
            default:
                log("Crossover method not found",ERROR,"step");


        }

        if((params.POPULATION_SIZE - children) % 2 == 0){
            new_population.Individuals.push_back(rand_selected[0]);
            new_population.Individuals.push_back(rand_selected[1]);
            children+=2;
        }else{
            new_population.Individuals.push_back(rand_selected[0]);
            children+=1;
        }
    }

    //Mutation TODO:Inefficiency goes VRRRUUUUUUUUM
    if(params.MUTATION_RATE>0){
        int N = Chromosome::CHROMOSOME_LENGTH;
        for(int i=params.ELITISM;i<params.POPULATION_SIZE;i++){
            for(int j=0;j<N;j++){
                if(rand_double(0,1)<params.MUTATION_RATE){
                    new_population.Individuals[i].mutation(j);
                }
            }
        }
    }

    new_generation(new_population);
    return Continue;

}

vector<Chromosome> GeneticAlgorithm::perform_roulette_selection(int n) {
    //https://stackoverflow.com/questions/10531565/how-should-roulette-wheel-selection-be-organized-for-non-sorted-population-in-g

    //This works only for positive fitness values!!!!!

    vector<Chromosome> selected_individuals;
    Population& current_population = generation_history[current_generation];

    //Get total fitness
    double total_fitness = 0;
    for(int i = 0; i<params.POPULATION_SIZE; i++){
        total_fitness += current_population.Individuals[i].get_fitness();
    }

    int new_members = 0;
    double R = 0;
    int i;
    while(new_members < n){
        R = rand_double(0,total_fitness);
        i = 0;
        while(R>=0){
            R = R - (current_population.Individuals[i%params.POPULATION_SIZE].get_fitness());
            i++;
        }
        //Found the individual:
        selected_individuals.push_back(current_population.Individuals[(i-1)%params.POPULATION_SIZE]);
        new_members++;
    }
    return selected_individuals;
}

//TODO
vector<Chromosome> GeneticAlgorithm::perform_rank_selection(int n) {
    //Probability of being selected is proportional to the RANK
    vector<Chromosome> selected_individuals;
    Population& current_population = generation_history[current_generation];
    if(!current_population.is_sorted){
        //Sort
        std::sort(current_population.Individuals.begin(),
                  current_population.Individuals.end(), std::greater<>());
        current_population.is_sorted = true;
    }

    return selected_individuals;
}

void GeneticAlgorithm::perform_single_point_crossover(Chromosome *A, Chromosome *B) {
    if(A->Genetic_Material == B->Genetic_Material) return;
    int crossover_point = rand_int(1, Chromosome::CHROMOSOME_LENGTH-2);
    //std::cout<<"Crossover point: "<<crossover_point<<std::endl;
    vector<Gene> tmp;
    tmp.reserve(crossover_point);
    for(int i = 0; i<crossover_point; i++){
        tmp[i] = A->Genetic_Material[i];
        A->Genetic_Material[i] = B->Genetic_Material[i];
        B->Genetic_Material[i] =  tmp[i];
    }
}

double GeneticAlgorithm::rand_double(double min, double max) const {
    static thread_local std::mt19937 generator(time(nullptr) + clock());
    std::uniform_real_distribution<double> distribution(min,max);
    return distribution(generator);
}

int GeneticAlgorithm::rand_int(const int &min, const int &max) {
    static thread_local std::mt19937 generator(time(nullptr) + clock());
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

vector<Chromosome> GeneticAlgorithm::perform_truncation_selection(int n, int begin,  int end) {
    Population& current_population = generation_history[current_generation];
    int* selected_indeces = new int[n];
    for(int i = 0; i<n; i++) selected_indeces[i] = -1;

    vector<Chromosome> selected_individuals;

    if(!current_population.is_sorted) {
        std::sort(current_population.Individuals.begin(),
                  current_population.Individuals.end(), std::greater<>());
        current_population.is_sorted = true;
    }
    int p;
    for(int i = 0; i<n; i++){
        do {
            p = rand_int(begin, end);
        }while((std::find(selected_indeces, selected_indeces+n, p)) != (selected_indeces+n));

        selected_indeces[i] = p;
        selected_individuals.push_back(current_population.Individuals[p]);
    }
    return selected_individuals;
}

int GeneticAlgorithm::binomial_coefficient(int n, int k) const {
    return std::tgamma(n)/(std::tgamma(k)*std::tgamma(n-k));
}


