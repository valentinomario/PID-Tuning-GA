#include "Chromosome.h"

void Chromosome::init_rand() {
    for(int i = 0; i<CHROMOSOME_LENGTH; i++){
        Genetic_Material.push_back(rand_float(min, max));
    }
}

void Chromosome::set_min(const float m) {
    min = m;
}

void Chromosome::set_max(const float m) {
    max = m;
}

vector<float> Chromosome::get_genes() const {
    return Genetic_Material;
}

float Chromosome::rand_float(const float &min, const float &max) {
    static thread_local std::mt19937 generator(time(nullptr) + clock());
    std::uniform_real_distribution<float> distribution(min,max);
    return distribution(generator);
}

void Chromosome::mutation(int point) {
    Genetic_Material[point] = rand_float(min,max);
}

