#include <random>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
using namespace std;



// representasi genetik
string create_gen(int panjang_gen) {
    // Create a random number generator object
    std::random_device rd;
    // Create a uniform integer distribution object with the desired range
    std::uniform_int_distribution<int> dist(32, 126);
    // The result string variable
    string result;
    for (int i = 0; i < panjang_gen; ++i) {
        int random_number = dist(rd);
        result += static_cast<char>(random_number);
    }
    return result;
}
// ---------------------------------------------------------------------------------

// calculate_fitness.m
// fitness function
float calculate_fitness(string gen_baru, string target) {
	// ???
   	vector<float> fitness(gen_baru.length());
	// Iterate through each char in `gen_baru` and compare it with the `target`
    for (int i = 0; i < gen_baru.length(); i++) {
		fitness[i] = (gen_baru[i] == target[i]); // Compare characters directly
    }

	// return the percentage of similarity between `gen_baru` and `target`
   	return ( count(fitness.begin(), fitness.end(), true)/static_cast<float>(target.length()) )*100;
}
// ---------------------------------------------------------------------------------

// create_population.m
// population Struct
struct PopulasiStruct {
	string gen;
    float fitness;
};
// create population function that will return an array of Population Struct
vector<PopulasiStruct> create_population(string target, int besar_populasi) {
    vector<PopulasiStruct> population(besar_populasi);
    
    for (int i=0; i<besar_populasi; i++) {
        string gen = create_gen(target.length());
    	population[i].gen = gen;
        population[i].fitness = calculate_fitness(gen, target);

        // cout << "\nGen:     " << population[i].gen << endl;
        // cout << "Fitness: " << population[i].fitness << endl;
    
    }
    
    return population;
}
// ---------------------------------------------------------------------------------

// selection.m
pair<PopulasiStruct,PopulasiStruct> selection(vector<PopulasiStruct> populasi) {
    vector<PopulasiStruct> best_parents(2);
    
    float greatest1 = 0;
    float greatest2 = 0;
    int indexG1 = 0;
    int indexG2 = 0;
    // Iterate through the array
    for (int i=0; i < populasi.size(); i++) {
        // If the current number is greater than both greatest1 and greatest2
        if (populasi[i].fitness > greatest1 && populasi[i].fitness > greatest2) {
            // Update greatest2 to the previous greatest1
            greatest2 = greatest1;
            // Update greatest1 to the current number
            greatest1 = populasi[i].fitness;
            // Update the indexG1 to the current index
            indexG1 = i;
        } else if (populasi[i].fitness > greatest2 && populasi[i].fitness <= greatest1) {
            // Update greatest2 to the current number
            greatest2 = populasi[i].fitness;
            // Update the indexG1 to the current index
            indexG2 = i;
        }
    }

    best_parents[0].gen = populasi[indexG1].gen;
    best_parents[0].fitness = populasi[indexG1].fitness;
    best_parents[1].gen = populasi[indexG2].gen;
    best_parents[1].fitness = populasi[indexG2].fitness;

    return make_pair(best_parents[0], best_parents[1]);
}
// ---------------------------------------------------------------------------------

// crossover.m
pair<PopulasiStruct,PopulasiStruct> crossover_strings(PopulasiStruct str1, PopulasiStruct str2) {
    vector<PopulasiStruct> crossover(2);

    // Generate random point at the middle letter
    int crossover_point = round(str1.gen.length()/2); 

    // Create offspring strings
    std::string offspring1 = str1.gen.substr(0, crossover_point) + str2.gen.substr(crossover_point);
    std::string offspring2 = str2.gen.substr(0, crossover_point) + str1.gen.substr(crossover_point);

    str1.gen = offspring1;
    str2.gen = offspring2;

    return std::make_pair(str1, str2);
}
// ---------------------------------------------------------------------------------

// mutation.m
PopulasiStruct mutation(PopulasiStruct child, float laju_mutasi) {

    for (int i=0; i<child.gen.size(); i++) {
        std::random_device rd; // Create a random number generator object
        std::uniform_real_distribution<float> dist(0.0, 1.0); // Create a uniform real distribution object with the desired range
        float random_number = dist(rd); // Generate a random number within the range
        if (random_number <= laju_mutasi) {
            std::uniform_int_distribution<int> dist(32, 126);
            child.gen[i] = static_cast<char>( dist(rd) );
        }
    }

    return child;
}
// ---------------------------------------------------------------------------------

// regeneration.m
vector<PopulasiStruct> regeneration(vector<PopulasiStruct> mutants, vector<PopulasiStruct> populasi) {
    vector<PopulasiStruct> populasi_baru = populasi;

    for (int m=0; m<mutants.size(); m++) {
        int smallest_index = 0;
        float smallest_fitness = populasi_baru[0].fitness; // Initialize with first element
        for (int i = 1; i < populasi_baru.size(); ++i) {
            if (populasi_baru[i].fitness < smallest_fitness) {
                smallest_index = i;
                smallest_fitness = populasi_baru[i].fitness;
            }
        }
        if (mutants[m].fitness >= populasi_baru[smallest_index].fitness) {
            populasi_baru[smallest_index] = mutants[m];
        }
    }

    return populasi_baru;
}
// ---------------------------------------------------------------------------------

// Genetic-Algorithm
void genetic_algorithm(string target, int besar_populasi, float laju_mutasi){
    // populasi
    vector<PopulasiStruct> population = create_population(target,besar_populasi);
    // selection
    pair<PopulasiStruct, PopulasiStruct> selected;
    // crossover
    pair<PopulasiStruct, PopulasiStruct> crossover;
    // mutation
    vector<PopulasiStruct> mutants;


    bool isLooping = true;
    int generation = 0;
    while (isLooping)    
    {
        // selection
        selected = selection(population);
        PopulasiStruct parent1 = selected.first, parent2 = selected.second;

        // crossover
        crossover = crossover_strings(parent1,parent2);
        PopulasiStruct child1 = crossover.first, child2 = crossover.second;

        // mutasi
        PopulasiStruct mutant1 = mutation(child1, laju_mutasi);
        PopulasiStruct mutant2 = mutation(child2, laju_mutasi);

        // fitness
        mutant1.fitness = calculate_fitness(mutant1.gen, target);
        mutant2.fitness = calculate_fitness(mutant2.gen, target);

        // regenerate
        mutants = {mutant1,mutant2};
        population = regeneration(mutants, population);

        generation++;

        for (auto vectorit = population.begin(); vectorit != population.end(); ++vectorit)
        {
            system("clear");
            cout << "Target:     " << target << endl;
            cout << "Mutant:     " << (*vectorit).gen << endl;
            cout << "Generation: " << generation << endl;
            
            if ((*vectorit).fitness == 100) {
                isLooping = false;
                // cout << "Target:     " << target << endl;
                // cout << "Mutant:     " << (*vectorit).gen << endl;
                // cout << "Generation: " << generation << endl;
                break;
            }
        }
    }
}








int main() {

    string target = "Semangka";    
    int besar_populasi = 20;
    float laju_mutasi = 0.45;

    genetic_algorithm(target, besar_populasi, laju_mutasi);

    return 0;
}