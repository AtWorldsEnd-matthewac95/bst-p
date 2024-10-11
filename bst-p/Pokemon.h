#pragma once
#include <cstdint>
#include <string>

namespace BST_P {
    typedef std::uint8_t PokemonId;
    typedef std::uint16_t PokemonBaseStatValue;

    enum class PokemonBaseStatEnumId : std::uint8_t {
        HP,
        Attack,
        Defense,
        Speed,
        Special,
        __COUNT,
        __INVALID
    };

    class PokemonBaseStat {

    public:
        PokemonBaseStat(PokemonBaseStatEnumId, PokemonBaseStatValue);

        inline PokemonBaseStatEnumId GetEnumId() const { return _id; }
        inline char const * GetName() const { return _name.c_str(); }
        inline PokemonBaseStatValue GetValue() const { return _value; }

    private:
        PokemonBaseStatEnumId _id;
        std::string _name;
        PokemonBaseStatValue _value;
    };

    typedef std::uint8_t PokemonEvolutionaryStage;
    typedef std::uint8_t RemainingPokemonEvolutionaryStages;

    class Pokemon {

    public:
        Pokemon(PokemonId, char const * name);

        inline PokemonId GetId() const { return _id; }
        inline char const * GetName() const { return _name.c_str(); }
        inline PokemonBaseStatValue GetBaseStatTotal() const {
            return _baseStats[static_cast<int>(PokemonBaseStatEnumId::HP)]
                 + _baseStats[static_cast<int>(PokemonBaseStatEnumId::Attack)]
                 + _baseStats[static_cast<int>(PokemonBaseStatEnumId::Defense)]
                 + _baseStats[static_cast<int>(PokemonBaseStatEnumId::Speed)]
                 + _baseStats[static_cast<int>(PokemonBaseStatEnumId::Special)];
        }
        inline PokemonBaseStatValue GetBaseStat(PokemonBaseStatEnumId id) const {
            return (id < PokemonBaseStatEnumId::__COUNT) ? _baseStats[static_cast<int>(id)] : 0;
        }
        inline PokemonBaseStat GetHighestBaseStat() const {
            return PokemonBaseStat(_highestBaseStat, (_highestBaseStat < PokemonBaseStatEnumId::__COUNT) ? _baseStats[static_cast<int>(_highestBaseStat)] : 0);
        }
        inline PokemonBaseStat GetSecondHighestBaseStat() const {
            return PokemonBaseStat(_secondHighestBaseStat, (_secondHighestBaseStat < PokemonBaseStatEnumId::__COUNT) ? _baseStats[static_cast<int>(_secondHighestBaseStat)] : 0);
        }
        inline PokemonBaseStat GetSecondLowestBaseStat() const {
            return PokemonBaseStat(_secondLowestBaseStat, (_secondLowestBaseStat < PokemonBaseStatEnumId::__COUNT) ? _baseStats[static_cast<int>(_secondLowestBaseStat)] : 0);
        }
        inline PokemonBaseStat GetLowestBaseStat() const {
            return PokemonBaseStat(_lowestBaseStat, (_lowestBaseStat < PokemonBaseStatEnumId::__COUNT) ? _baseStats[static_cast<int>(_lowestBaseStat)] : 0);
        }
        inline PokemonEvolutionaryStage GetEvolutionaryStage() const { return _stage; }
        inline RemainingPokemonEvolutionaryStages GetRemainingEvolutionaryStages() const { return _remainingEvolutionaryStages; }

        inline void SetEvolutionaryStage(PokemonEvolutionaryStage value) { _stage = value; }
        inline void SetEvolutionaryStagesLeft(RemainingPokemonEvolutionaryStages value) { _remainingEvolutionaryStages = value; }

        void SetBaseStat(PokemonBaseStatEnumId statToSet, PokemonBaseStatValue valueToSet, bool isEvaluatingHighestAndLowestBaseStats = false);
        void EvaluateHighestAndLowestBaseStats();

    private:
        PokemonId _id;
        std::string _name;
        PokemonBaseStatValue _baseStats[static_cast<int>(PokemonBaseStatEnumId::__COUNT)];
        PokemonBaseStatEnumId _highestBaseStat{PokemonBaseStatEnumId::__INVALID};
        PokemonBaseStatEnumId _secondHighestBaseStat{PokemonBaseStatEnumId::__INVALID};
        PokemonBaseStatEnumId _secondLowestBaseStat{PokemonBaseStatEnumId::__INVALID};
        PokemonBaseStatEnumId _lowestBaseStat{PokemonBaseStatEnumId::__INVALID};
        PokemonEvolutionaryStage _stage{1};
        RemainingPokemonEvolutionaryStages _remainingEvolutionaryStages{0};
    };
}
