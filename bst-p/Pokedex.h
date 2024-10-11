#pragma once
#include <unordered_map>
#include "Pokemon.h"

namespace BST_P {

class Pokedex {
public:
    Pokedex(char const * filename);

    Pokemon const & FindPokemon(PokemonId const id) const;
    inline size_t GetNumberOfPokemon() const { return _data.size(); }

private:
    bool InsertPokemon(Pokemon && pokemon);

    std::unordered_map<PokemonId, Pokemon const> _data;
};

}
