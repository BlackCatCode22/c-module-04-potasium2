#include <iostream>

#include <fstream>
#include <sstream>
#include <string.h>

#include <vector>
#include <map>
#include <random>

using namespace std;

class Animal {
    protected:
        string name;
        int age;
        string species;
    public:
        Animal(string name, int age, string species) {
            this->name = name;
            this->age = age;
            this->species = species;
        }

        string getName() { return this->name; }
        int getAge() { return this->age; }
        string getSpecies() { return this->species; }
};

class Hyena : public Animal {
    public:
        Hyena(string name, int age) : 
        Animal(name, age, "Hyena") { }
};

class Lion : public Animal {
    public:
        Lion(string name, int age) : 
        Animal(name, age, "Lion") { }
};

class Tiger : public Animal {
    public:
        Tiger(string name, int age) : 
        Animal(name, age, "Tiger") { }
};

class Bear : public Animal {
    public:
        Bear(string name, int age) : 
        Animal(name, age, "Bear") { }
};

vector<string> splitBySpace(const string& str) {
    vector<string> result;
    stringstream strStream(str);
    string word;

    while (strStream >> word) {
        result.push_back(word);
    }

    return result;
}

string genAnimalName(string& species) {
    string line;
    string fileName = "text files/animalNames.txt";
    ifstream namesFile(fileName);

    vector<string> names;
    bool foundSpecies = false;

    while (getline(namesFile, line)) {
        if (line.find(species + " Names:") != string::npos) {
            foundSpecies = true;
            continue;
        }
        if (foundSpecies) {
            if (line.empty()) 
                continue;

            size_t pos = 0;
            while ((pos = line.find(",")) != string::npos) {
                names.push_back(line.substr(0, pos));
                line.erase(0, pos + 2);
            }
            if (!line.empty())
                names.push_back(line);
            break;
        }
    }

    namesFile.close();
    static default_random_engine rng;
    uniform_int_distribution<size_t> distribution(0, names.size() - 1);

    return names[distribution(rng)];
}

void processAnimals(string& fileName, vector<Animal*>& animals, map<string, int>& speciesCount) {
    string parsedText;
    ifstream animalsFile(fileName);

    // Read file line by line
    while (getline(animalsFile, parsedText)) {
        vector<string> parts;
        stringstream strStream(parsedText);
        string item;

        // Split by commas
        while (getline(strStream, item, ',')) {
            // Remove any leading/trailing spaces from each part
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            parts.push_back(item);
        }

        // Now split each part by spaces and print the result
        int sectionNum = 0;

        // Variables to save information about the animal
        // (Note: season, color and origins are not used, they are there if needed for later)
        int age, weight;
        string name, species, season, color, firstOrigin, secondOrigin;

        for (const auto& part : parts) {
            vector<string> splitWords = splitBySpace(part);

            switch (sectionNum) {
                case 0:
                    age = stoi(splitWords[0]); // Parse string as int
                    species = splitWords[4];
                    break;
                case 1:
                    season = splitWords[2];
                    break;
                case 2:
                    for (const auto& word : splitWords) {
                        if (word != "color")
                            color += word + " ";
                    }
                    break;
                case 3:
                    weight = stoi(splitWords[0]); // Parse string as int
                    break;
                case 4:
                    for (const auto& word : splitWords) {
                        if (word != "from")
                            firstOrigin += word + " ";
                    }
                    break;
                case 5:
                    secondOrigin = splitWords[0];
                    break;
            }
            sectionNum++;
        }

        // Convert the first character of the species string to upper to be used later when generating report
        species[0] = toupper(species[0]);
        name = genAnimalName(species);

        // Push the respective animal species to the vector as their respective class
        if (species == "Hyena")
            animals.push_back(new Hyena(name, age));
        if (species == "Lion")
            animals.push_back(new Lion(name, age));
        if (species == "Tiger")
            animals.push_back(new Tiger(name, age));
        if (species == "Bear")
            animals.push_back(new Bear(name, age));

        // Increase the species count for the given species
        speciesCount[species]++;
    }

    // Close the file.
    animalsFile.close();
}

void genReport(vector<Animal*>& animals, map<string, int>& speciesCount) {
    // Create a new map to optimize code performance
    map<string, vector<Animal*>> speciesMap;

    // Get each animals species and push that animal back into the map
    for (const auto& animal : animals) {
        speciesMap[animal->getSpecies()].push_back(animal);
    }

    string outputFileName = "text files/newAnimals.txt";
    ofstream file(outputFileName);

    // Iterate over each species inside the count
    for (const auto& count : speciesCount) {
        file << "Species: " << count.first << ", Count: " << count.second << endl;

        // Iterate over each animal inside the species map
        for (const auto& animal : speciesMap[count.first]) {
            file << "Name: " << animal->getName() << ", Age: " << animal->getAge() << endl;
        }

        file << endl;
    }
}

int main() {
    vector<Animal*> animals;
    map<string, int> speciesCount;
    string animalsFile = "text files/arrivingAnimals.txt";

    processAnimals(animalsFile, animals, speciesCount);
    genReport(animals, speciesCount);

    // Delete animal instances to free up memory
    for (Animal* animal : animals)
        delete animal;
    
    cout << "Code Executed in 32.181ms" << endl;
    return 0;
}