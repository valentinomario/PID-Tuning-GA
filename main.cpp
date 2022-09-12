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
    double cost_ = cost[0];
    return 1.0/(1.0+cost_);
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
    matlabPtr->eval(u"dt = 0.001;");


    Params p;
    p.GENE_MIN = 0;
    p.GENE_MAX = 1.0;
    p.MUTATION_RATE = 0;
    p.SELECTION_METHOD = ROULETTE_WHEEL_SELECTION;
    p.TRUNCATE = 5;
    p.CROSSOVER_METHOD = UNIFORM_CROSSOVER;
    p.MAX_GENERATIONS = 10;
    p.TARGET_FITNESS = 1;
    p.ELITISM = 20;
    p.VERBOSE = true;
    p.POPULATION_SIZE = 25;
    p.FITNESS_FUNCTION = std::move(fitness_function);
    p.OUTPUT_FILE = true;

    GeneticAlgorithm GA(p);
    //GA.run();
    return 0;
}

