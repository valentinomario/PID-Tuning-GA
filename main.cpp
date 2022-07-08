#include <iostream>
#include "GeneticAlgorithm.h"
#include "MatlabEngine.hpp"

using std::string;
using std::cout;
using std::endl;
using namespace matlab::engine;

std::unique_ptr<MATLABEngine> matlabPtr;
matlab::data::ArrayFactory factory;

float fitness_function(const Chromosome& c){
    matlab::data::Array testing_parameters = factory.createArray({1,3}, {c.get_genes()[0],c.get_genes()[1],c.get_genes()[2]});
    matlab::data::Array cost = matlabPtr->feval(u"pidtest",
                                                {matlabPtr->getVariable(u"G"),
                                                 matlabPtr->getVariable(u"dt"),
                                                 testing_parameters}    );
    return cost[0];
}

int main() {

    cout<<"Starting Matlab...\n";
    matlabPtr = connectMATLAB();

    matlabPtr->eval(u"cd(getenv('appdata'));"
                    " cd('GA PID tuning');");
    matlabPtr->eval(u"current_path = convertCharsToStrings(pwd);");
    matlab::data::Array current_path = matlabPtr->getVariable(u"current_path");
    std::string path = current_path[0];
    cout<<"Working directory: " << path << endl;

    matlabPtr->eval(u"G = tf(1,[1 1 1 0]);");
    matlabPtr->eval(u"dt = 0.01;");


    Params p;
    p.GENE_MIN = -5.0;
    p.GENE_MAX = 5.0;
    p.MUTATION_RATE = 0.2;
    p.SELECTION_METHOD = TRUNCATION_SELECTION;
    p.CROSSOVER_METHOD = SINGLE_POINT_CROSSOVER;
    p.MAX_GENERATIONS = 25;
    p.TARGET_FITNESS = 5;
    p.ELITISM = 10;
    p.TRUNCATE = 70;
    p.VERBOSE = true;
    p.POPULATION_SIZE = 100;
    p.REPEAT_PARENTS_SELECTION = false;
    p.FITNESS_FUNCTION = std::move(fitness_function);
    p.OUTPUT_FILE = true;

    GeneticAlgorithm GA(p);
    GA.run();
    return 0;
}

