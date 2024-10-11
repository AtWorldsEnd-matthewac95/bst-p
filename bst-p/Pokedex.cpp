#include "Pokedex.h"
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

namespace BST_P {

Pokedex::Pokedex(char const * filename) {
    std::ifstream datafile(filename);

    if (datafile.is_open()) {
        std::string token;

        while (datafile) {
            datafile >> token;
            int const id{std::stoi(token)};

            if ((id <= 0) || (id >= static_cast<int>(std::numeric_limits<PokemonId>::max()))) {
                if (id == 0) {
                    break; // ID of 0 indicates end of the list.
                }
                std::cout << "Invalid ID " << id << " given!";
                throw std::exception{"Bad ID given to Pokedex in constructor!"};
            }

            datafile >> token;
            Pokemon pokemon{static_cast<PokemonId>(id), token.c_str()};

            datafile >> token;
            int stat = std::stoi(token);

            if ((stat < 0) || (static_cast<long long>(stat) >= static_cast<long long>(std::numeric_limits<PokemonBaseStatValue>::max()))) {
                std::cout << "Invalid HP value " << stat << "given!";
                throw std::exception{"Bad HP value given to Pokedex in constructor!"};
            }

            pokemon.SetBaseStat(PokemonBaseStatEnumId::HP, static_cast<PokemonBaseStatValue>(stat));

            datafile >> token;
            stat = std::stoi(token);

            if ((stat < 0) || (static_cast<long long>(stat) >= static_cast<long long>(std::numeric_limits<PokemonBaseStatValue>::max()))) {
                std::cout << "Invalid Attack value " << stat << "given!";
                throw std::exception{"Bad Attack value given to Pokedex in constructor!"};
            }

            pokemon.SetBaseStat(PokemonBaseStatEnumId::Attack, static_cast<PokemonBaseStatValue>(stat));

            datafile >> token;
            stat = std::stoi(token);

            if ((stat < 0) || (static_cast<long long>(stat) >= static_cast<long long>(std::numeric_limits<PokemonBaseStatValue>::max()))) {
                std::cout << "Invalid Defense value " << stat << "given!";
                throw std::exception{"Bad Defense value given to Pokedex in constructor!"};
            }

            pokemon.SetBaseStat(PokemonBaseStatEnumId::Defense, static_cast<PokemonBaseStatValue>(stat));

            datafile >> token;
            stat = std::stoi(token);

            if ((stat < 0) || (static_cast<long long>(stat) >= static_cast<long long>(std::numeric_limits<PokemonBaseStatValue>::max()))) {
                std::cout << "Invalid Speed value " << stat << "given!";
                throw std::exception{"Bad Speed value given to Pokedex in constructor!"};
            }

            pokemon.SetBaseStat(PokemonBaseStatEnumId::Speed, static_cast<PokemonBaseStatValue>(stat));

            datafile >> token;
            stat = std::stoi(token);

            if ((stat < 0) || (static_cast<long long>(stat) >= static_cast<long long>(std::numeric_limits<PokemonBaseStatValue>::max()))) {
                std::cout << "Invalid Special value " << stat << "given!";
                throw std::exception{"Bad Special value given to Pokedex in constructor!"};
            }

            pokemon.SetBaseStat(PokemonBaseStatEnumId::Special, static_cast<PokemonBaseStatValue>(stat));
            pokemon.EvaluateHighestAndLowestBaseStats();

            datafile >> token;
            int const evolutionStage = std::stoi(token);

            if ((stat < 0) || (stat >= static_cast<int>(std::numeric_limits<PokemonEvolutionaryStage>::max()))) {
                std::cout << "Invalid evolutionary stage value " << stat << "given!";
                throw std::exception{ "Bad evolutionary stage value given to Pokedex in constructor!" };
            }

            pokemon.SetEvolutionaryStage(evolutionStage);

            datafile >> token;
            int const evolutionsLeft = std::stoi(token);

            if ((stat < 0) || (stat >= static_cast<int>(std::numeric_limits<RemainingPokemonEvolutionaryStages>::max()))) {
                std::cout << "Invalid remaining evolution count " << stat << "given!";
                throw std::exception{ "Bad remaining evolution count given to Pokedex in constructor!" };
            }

            pokemon.SetEvolutionaryStagesLeft(evolutionsLeft);

            InsertPokemon(std::move(pokemon));
        }
    }

    assert(_data.size() == 151U);

    datafile.close();
}

Pokemon const & Pokedex::FindPokemon(PokemonId const id) const {
    auto found = _data.find(id);

    if (found == _data.end()) {
        std::cout << "Cannot find Pokemon [" << id << "] in the Pokedex!\n";
        throw std::exception{"Pokemon not in Pokedex!"};
    }

    return found->second;
}

bool Pokedex::InsertPokemon(Pokemon && pokemon) {
    auto found = _data.find(pokemon.GetId());

    if (found != _data.end()) {
        return false;
    }

    _data.insert(std::make_pair(pokemon.GetId(), std::move(pokemon)));
    return true;
}

}
