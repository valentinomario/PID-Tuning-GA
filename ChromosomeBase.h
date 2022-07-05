#ifndef MYGA_CHROMOSOMEBASE_H
#define MYGA_CHROMOSOMEBASE_H


#include <ostream>
#include <vector>

using std::vector;

template<typename Gene>
class ChromosomeBase{
public:
    vector<Gene> Genetic_Material;

    static const int CHROMOSOME_LENGTH = 3;

    ChromosomeBase(Gene min, Gene max);

    inline void set_fitness(const double& c) { solution_fitness = c;}
    inline double get_fitness() const {return solution_fitness;}

    void set_accumulated_fitness(double accumulatedFitness);
    double get_accumulated_fitness() const;


    //Operator overloading for sorting purpose
    bool operator<(const ChromosomeBase &rhs) const;
    bool operator>(const ChromosomeBase &rhs) const;
    bool operator<=(const ChromosomeBase &rhs) const;
    bool operator>=(const ChromosomeBase &rhs) const;

    double get_norm_fitness() const;
    void set_norm_fitness(double normalizedSolutionFitness);

protected:
    //Gene genes[CHROMOSOME_LENGTH];
    Gene min;
    Gene max;

private:
    double solution_fitness;
    double normalized_solution_fitness;

    double accumulated_fitness;

    virtual void init_rand() = 0;
    virtual void mutation(int point) = 0;
    template<typename G>
    friend std::ostream &operator<<(std::ostream &os, const ChromosomeBase<G> &base);

    virtual std::ostream& print(std::ostream&) const;

    friend class GeneticAlgorithm;
};


template<typename Gene>
std::ostream& ChromosomeBase<Gene>::print(std::ostream& os) const{
    os<<"{";
    for(int i = 0; i<(CHROMOSOME_LENGTH); i++){
        os << "\"" << Genetic_Material[i] << "\", ";
    }
    os << solution_fitness <<"}";
    return os;
}

template<typename Gene>
std::ostream &operator<<(std::ostream &os, const ChromosomeBase<Gene> &base) {
    return base.print(os);
}

template<typename Gene>
ChromosomeBase<Gene>::ChromosomeBase(Gene min, Gene max) {
    accumulated_fitness = 0;
    normalized_solution_fitness = 0;
    this->min = min;
    this->max = max;
}

template<typename Gene>
bool ChromosomeBase<Gene>::operator<(const ChromosomeBase &rhs) const {
    return solution_fitness < rhs.solution_fitness;
}

template<typename Gene>
bool ChromosomeBase<Gene>::operator>(const ChromosomeBase &rhs) const {
    return rhs < *this;
}

template<typename Gene>
bool ChromosomeBase<Gene>::operator<=(const ChromosomeBase &rhs) const {
    return rhs >= *this;
}

template<typename Gene>
bool ChromosomeBase<Gene>::operator>=(const ChromosomeBase &rhs) const {
    return *this >= rhs;
}

template<typename Gene>
double ChromosomeBase<Gene>::get_accumulated_fitness() const {
    return accumulated_fitness;
}

template<typename Gene>
void ChromosomeBase<Gene>::set_accumulated_fitness(double accumulatedFitness) {
    accumulated_fitness = accumulatedFitness;
}

template<typename Gene>
double ChromosomeBase<Gene>::get_norm_fitness() const {
    return normalized_solution_fitness;
}

template<typename Gene>
void ChromosomeBase<Gene>::set_norm_fitness(double normalizedSolutionFitness) {
    normalized_solution_fitness = normalizedSolutionFitness;
}

#endif //MYGA_CHROMOSOMEBASE_H
