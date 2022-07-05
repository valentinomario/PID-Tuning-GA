#ifndef MYGA_CHROMOSOME_H
#define MYGA_CHROMOSOME_H

#include <random>
#include <ctime>
#include <iostream>
#include "ChromosomeBase.h"

class Chromosome : public ChromosomeBase<float>{
public:
    explicit Chromosome(float min = 0, float max = 0) : ChromosomeBase(min, max){}

    void set_min(float);
    void set_max(float);

    void init_rand() override;
    void mutation(int point) override;
    static float rand_float(const float & min, const float & max);

    vector<float> get_genes() const;

};



#endif //MYGA_CHROMOSOME_H
