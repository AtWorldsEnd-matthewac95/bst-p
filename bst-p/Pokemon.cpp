#include "Pokemon.h"
#include <cassert>
#include <climits>

namespace BST_P {
    PokemonBaseStat::PokemonBaseStat(PokemonBaseStatEnumId id, PokemonBaseStatValue value) : _id{id}, _value{value} {
        assert(id < PokemonBaseStatEnumId::__COUNT);

        switch (id) {

        case PokemonBaseStatEnumId::HP: {
                _name = "HP";
                break;
            }
        case PokemonBaseStatEnumId::Attack: {
                _name = "Attack";
                break;
            }
        case PokemonBaseStatEnumId::Defense: {
                _name = "Defense";
                break;
            }
        case PokemonBaseStatEnumId::Speed: {
                _name = "Speed";
                break;
            }
        case PokemonBaseStatEnumId::Special: {
                _name = "Special";
                break;
            }
        default: {
                _name = "[INVALID]";
                break;
            }

        }
    }

    Pokemon::Pokemon(PokemonId id, char const * name) : _id{id}, _name{name} {
        for (int i = 0; i < static_cast<int>(PokemonBaseStatEnumId::__COUNT); ++i) {
            _baseStats[i] = 0;
        }
    }

    void Pokemon::SetBaseStat(PokemonBaseStatEnumId statToSet, PokemonBaseStatValue valueToSet, bool isEvaluatingHighestAndLowestBaseStats) {
        if (statToSet >= PokemonBaseStatEnumId::__COUNT) {
            assert(true);
            return;
        }

        _baseStats[static_cast<int>(statToSet)] = valueToSet;

        if (isEvaluatingHighestAndLowestBaseStats) {
            EvaluateHighestAndLowestBaseStats();
        }
    }

    void Pokemon::EvaluateHighestAndLowestBaseStats() {
        PokemonBaseStatValue highestValue = std::numeric_limits<PokemonBaseStatValue>::min();
        PokemonBaseStatValue secondHighestValue = std::numeric_limits<PokemonBaseStatValue>::min();
        PokemonBaseStatValue secondLowestValue = std::numeric_limits<PokemonBaseStatValue>::max();
        PokemonBaseStatValue lowestValue = std::numeric_limits<PokemonBaseStatValue>::max();

        for (int i = 0; i < static_cast<int>(PokemonBaseStatEnumId::__COUNT); ++i) {
            PokemonBaseStatValue baseStat = _baseStats[i];

            if (baseStat > highestValue) {
                secondHighestValue = highestValue;
                _secondHighestBaseStat = _highestBaseStat;

                highestValue = baseStat;
                _highestBaseStat = static_cast<PokemonBaseStatEnumId>(i);
            } else if (baseStat > secondHighestValue) {
                secondHighestValue = baseStat;
                _secondHighestBaseStat = static_cast<PokemonBaseStatEnumId>(i);
            }

            if (baseStat < lowestValue) {
                secondLowestValue = lowestValue;
                _secondLowestBaseStat = _lowestBaseStat;

                lowestValue = baseStat;
                _lowestBaseStat = static_cast<PokemonBaseStatEnumId>(i);
            }
            else if (baseStat < secondLowestValue) {
                secondLowestValue = baseStat;
                _secondLowestBaseStat = static_cast<PokemonBaseStatEnumId>(i);
            }
        }

        assert(_highestBaseStat < PokemonBaseStatEnumId::__COUNT);
        assert(_secondHighestBaseStat < PokemonBaseStatEnumId::__COUNT);
        assert(_secondLowestBaseStat < PokemonBaseStatEnumId::__COUNT);
        assert(_lowestBaseStat < PokemonBaseStatEnumId::__COUNT);
    }
}
