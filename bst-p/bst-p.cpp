#include <iostream>
#include "AvlTree.h"
#include "Pokedex.h"
#include <format>
#include "cpp11-strfmt.h"
#include <cstdlib>

/*
 * Need to rewrite Node::FindHeight(SubtreeHeightMap)
 * Need to rewrite Rotate
 * Need to rewrite Remove
 *
 * FindHeight is the only of these which MIGHT be okay in its current form, but I am definitely not using it properly at the moment.
 * Rotate has a BIG fundamental problem - the subtree height map is WRONG if it does not account for new parent-child relationships.
 *      Technically I should just wipe that stupid map every time I modify the tree in any way, because if the tree has any changes at all the map is completely useless.
 *      ...Maybe I'm being too dramatic. MOST of the entries should still be meaningful, but the entries regarding the precise nodes involved with the rotation (child, parent, grandparent) can not be relied upon.
 *      Try just wiping those entries first and see if the map still works. By the way, I need to have MUCH better test cases which really interrogate the subtree height map and rotation logic in general.
 * Even supposing Rotate works completely correctly (which it doesn't), Remove similarly has problems with its balance factor assumptions.
 *      I think the best way to go here will be to rely on a fixed Rotate function and a now-properly-populated subtree height map.
 *      No more balance factor shortcuts, I should be using the subtree height to determine if I still need to propogate balance factor adjustments after a rotation.
 *      I suppose the non-rotation logic during Remove is still valid. After a rotation, though, I can't make any assumptions. There's just too many possibilities.
 */

void PrintPokemon(BST_P::Pokemon const & pokemon) {
    BST_P::PokemonBaseStat highestStat = pokemon.GetHighestBaseStat();
    BST_P::PokemonBaseStat secondHighestStat = pokemon.GetSecondHighestBaseStat();
    BST_P::PokemonBaseStat secondLowestStat = pokemon.GetSecondLowestBaseStat();
    BST_P::PokemonBaseStat lowestStat = pokemon.GetLowestBaseStat();

    std::cout << string_format(
        "#%+03u %s\n"
        "    Base Stat Total:\n"
        "        %u\n"
        "    Highest Stat:\n"
        "        %-7u%s\n"
        "    Second Highest Stat:\n"
        "        %-7u%s\n"
        "    Second Lowest Stat:\n"
        "        %-7u%s\n"
        "    Lowest Stat:\n"
        "        %-7u%s\n"
        "    Evo Stage, Evos Left\n"
        "        %-7u%u\n"
        , pokemon.GetId(), pokemon.GetName()
            , pokemon.GetBaseStatTotal()
            , highestStat.GetValue(), highestStat.GetName()
            , secondHighestStat.GetValue(), secondHighestStat.GetName()
            , secondLowestStat.GetValue(), secondLowestStat.GetName()
            , lowestStat.GetValue(), lowestStat.GetName()
            , pokemon.GetEvolutionaryStage(), pokemon.GetRemainingEvolutionaryStages()
    );
}

void PrintTwoPokemon(BST_P::Pokemon const & A, BST_P::Pokemon const & B) {
    BST_P::PokemonBaseStat highestStatA = A.GetHighestBaseStat();
    BST_P::PokemonBaseStat secondHighestStatA = A.GetSecondHighestBaseStat();
    BST_P::PokemonBaseStat secondLowestStatA = A.GetSecondLowestBaseStat();
    BST_P::PokemonBaseStat lowestStatA = A.GetLowestBaseStat();
    BST_P::PokemonBaseStat highestStatB = B.GetHighestBaseStat();
    BST_P::PokemonBaseStat secondHighestStatB = B.GetSecondHighestBaseStat();
    BST_P::PokemonBaseStat secondLowestStatB = B.GetSecondLowestBaseStat();
    BST_P::PokemonBaseStat lowestStatB = B.GetLowestBaseStat();

    std::cout << string_format(
        "#%+03u %-25s#%+03u %s\n"
        "    Base Stat Total:              Base Stat Total:\n"
        "        %-30u%u\n"
        "    Highest Stat:                 Highest Stat:\n"
        "        %-7u%-23s%-7u%s\n"
        "    Second Highest Stat:          Second Highest Stat:\n"
        "        %-7u%-23s%-7u%s\n"
        "    Second Lowest Stat:           Second Lowest Stat:\n"
        "        %-7u%-23s%-7u%s\n"
        "    Lowest Stat:                  Lowest Stat:\n"
        "        %-7u%-23s%-7u%s\n"
        "    Evo Stage, Evos Left:         Evo Stage, Evos Left:\n"
        "        %-7u%-23u%-7u%u\n"
        , A.GetId(), A.GetName(), B.GetId(), B.GetName()
            , A.GetBaseStatTotal(), B.GetBaseStatTotal()
            , highestStatA.GetValue(), highestStatA.GetName(), highestStatB.GetValue(), highestStatB.GetName()
            , secondHighestStatA.GetValue(), secondHighestStatA.GetName(), secondHighestStatB.GetValue(), secondHighestStatB.GetName()
            , secondLowestStatA.GetValue(), secondLowestStatA.GetName(), secondLowestStatB.GetValue(), secondLowestStatB.GetName()
            , lowestStatA.GetValue(), lowestStatA.GetName(), lowestStatB.GetValue(), lowestStatB.GetName()
            , A.GetEvolutionaryStage(), A.GetRemainingEvolutionaryStages(), B.GetEvolutionaryStage(), B.GetRemainingEvolutionaryStages()
    );
}

void BubbleSortRelativePokemonRankings(BST_P::AvlTree<BST_P::Pokemon const *> const & tree, int * relativePokemonRankings) {
    typedef BST_P::AvlTree<BST_P::Pokemon const *>::const_iterator itr_type;

    for (itr_type end = tree.cend(); end != tree.cbegin(); --end) {
        bool isDone = true;

        itr_type next = tree.cbegin();
        itr_type prev = next++;

        while (next != end) {
            BST_P::PokemonId nextId = (*next)->GetId();
            BST_P::PokemonId prevId = (*prev)->GetId();
            int nextRank = relativePokemonRankings[nextId];
            int prevRank = relativePokemonRankings[prevId];

            if (nextRank < prevRank) {
                isDone = false;
                relativePokemonRankings[nextId] = prevRank;
                relativePokemonRankings[prevId] = nextRank;
            }

            prev = next++;
        }

        if (isDone) {
            break;
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    BST_P::Pokedex pokedex("pokedata.txt");
    size_t pokemonCount = pokedex.GetNumberOfPokemon();
    size_t unsortedPokemonCount = pokemonCount;
    BST_P::PokemonId * availablePokemonToSort = new BST_P::PokemonId [pokemonCount];
    int * relativePokemonRankings = new int [pokemonCount + 1];

    for (size_t i = 0U; i < pokemonCount; ++i) {
        availablePokemonToSort[i] = static_cast<BST_P::PokemonId>(i + 1U);
        relativePokemonRankings[i] = static_cast<int>(i);
    }
    relativePokemonRankings[pokemonCount] = static_cast<int>(pokemonCount);

    BST_P::AvlTree<BST_P::Pokemon const *> sortedPokemonTree{
        [relativePokemonRankings](BST_P::Pokemon const * A, BST_P::Pokemon const * B) {
            if (A == B) {
                return 0;
            }

            if (A == nullptr) {
                return -1;
            }

            if (B == nullptr) {
                return 1;
            }

            return (relativePokemonRankings[A->GetId()] > relativePokemonRankings[B->GetId()]) ? 1 : -1;
        }
    };

    while (unsortedPokemonCount > 0U) {
        size_t index = rand() % unsortedPokemonCount;
        bool isSkippingPokemon = false;
        bool isViewingList = false;

        auto inserted = sortedPokemonTree.Insert(
            &(pokedex.FindPokemon(availablePokemonToSort[index])),
            [&isSkippingPokemon, &isViewingList](BST_P::Pokemon const * A, BST_P::Pokemon const * B) {
                if (isSkippingPokemon) {
                    return 1;
                }

                if (A == B) {
                    return 0;
                }

                if (A == nullptr) {
                    return -1;
                }

                if (B == nullptr) {
                    return 1;
                }

                std::cout << "Should " << A->GetName() << " award more experience than " << B->GetName() << "?\n\n";
                PrintTwoPokemon(*A, *B);
                std::cout << "\n";

                char input = 0;
                int result = 0;

                while (result == 0) {
                    if (isViewingList) {
                        std::cout << "!!! After finishing with " << A->GetName() << ", the sorted list will be printed. !!!\n";
                    }

                    std::cout << "These inputs [m, M, >, +, q, Q, 1, 2, y, Y] all mean: " << A->GetName() << " should award MORE experience than " << B->GetName() << "\n";
                    std::cout << "These inputs [l, L, <, -, p, P, 0, 9, n, N] all mean: " << A->GetName() << " should award LESS experience than " << B->GetName() << "\n";
                    std::cout << "These inputs [k, K, $, ^, w, W, s, S] all mean: Skip " << A->GetName() << " and come back to it later.\n";
                    std::cout << "These inputs [u, U, !, ?, z, Z, t, T] all mean: After finishing with " << A->GetName() << ", I'd" << (isViewingList ? " no longer " : " ") << "like to view the sorted list.\n";

                    std::cin >> input;

                    switch (input) {

                    case 'm':
                    case 'M':
                    case '>':
                    case '+':
                    case 'q':
                    case 'Q':
                    case '1':
                    case '2':
                    case 'y':
                    case 'Y':
                        result = 1;
                        break;

                    case 'l':
                    case 'L':
                    case '<':
                    case '-':
                    case 'p':
                    case 'P':
                    case '0':
                    case '9':
                    case 'n':
                    case 'N':
                        result = -1;
                        break;

                    case 'k':
                    case 'K':
                    case '$':
                    case '^':
                    case 'w':
                    case 'W':
                    case 's':
                    case 'S':
                        isSkippingPokemon = true;
                        result = 1;
                        break;

                    case 'u':
                    case 'U':
                    case '!':
                    case '?':
                    case 'z':
                    case 'Z':
                    case 't':
                    case 'T':
                        isViewingList = !isViewingList;
                        break;

                    default:
                        std::cout << "Please provide a valid input.\n\n";
                        break;

                    }
                }

                return result;
            }
        );

        std::cout << "\n";

        if (isSkippingPokemon) {
            std::cout << (*inserted.second)->GetName() << " will be skipped for now.\n";
            sortedPokemonTree.Remove(std::move(inserted.second));
        } else {
            --unsortedPokemonCount;
            BST_P::PokemonId swap = availablePokemonToSort[unsortedPokemonCount];
            availablePokemonToSort[unsortedPokemonCount] = availablePokemonToSort[index];
            availablePokemonToSort[index] = swap;
        }

        if (!isViewingList) {
            continue;
        }

        std::cout << "\nHere is the current list of sorted entries:\n\n";

        for (decltype(sortedPokemonTree)::const_iterator itr = sortedPokemonTree.cbegin(); itr != sortedPokemonTree.cend(); ++itr) {
            std::cout << string_format("%03u: %s\n", (*itr)->GetId(), (*itr)->GetName());
        }

        std::cout << "\nWould you like to remove an entry from this list? It will be placed back into the sortable pool and you can re-evaluate it later.\n";

        size_t pokemonToRemove = pokemonCount;

        while (pokemonToRemove >= pokemonCount) {
            std::cout << "Enter 0 if you do not want to remove anything. Otherwise, please enter an ID from the above list to remove: ";
            std::cin >> pokemonToRemove;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                pokemonToRemove = pokemonCount;
            }

            if (pokemonToRemove < pokemonCount) {
                if (pokemonToRemove == 0U) {
                    std::cout << "\n";
                    break;
                }

                BubbleSortRelativePokemonRankings(sortedPokemonTree, relativePokemonRankings);

                BST_P::PokemonId pokemonToRemoveId = static_cast<BST_P::PokemonId>(pokemonToRemove);
                std::unique_ptr<decltype(sortedPokemonTree)::value_type> removed;

                if (sortedPokemonTree.Remove(&(pokedex.FindPokemon(pokemonToRemoveId)), removed)) {
                    std::cout << "\n" << (*removed)->GetName() << " was removed from the list. It will be placed back into the sortable pool to be re-evaluated later.\n\n";
                    assert(unsortedPokemonCount < pokemonCount);

                    size_t j;
                    for (j = unsortedPokemonCount; j < pokemonCount; ++j) {
                        if (availablePokemonToSort[j] == pokemonToRemoveId) {
                            availablePokemonToSort[j] = availablePokemonToSort[unsortedPokemonCount];
                            availablePokemonToSort[unsortedPokemonCount] = pokemonToRemoveId;
                            break;
                        }
                    }

                    assert(j < pokemonCount);
                    ++unsortedPokemonCount;
                } else {
                    pokemonToRemove = pokemonCount;
                }
            }

            if (pokemonToRemove >= pokemonCount) {
                std::cout << "Invalid input.\n";
            }
        }

        isViewingList = false;
    }

    BubbleSortRelativePokemonRankings(sortedPokemonTree, relativePokemonRankings);
    std::cout << "\n\nResults:\n\n";

    for (decltype(sortedPokemonTree)::const_iterator itr = sortedPokemonTree.cbegin(); itr != sortedPokemonTree.cend(); ++itr) {
        std::cout << (*itr)->GetName() << "\n";
    }

    delete [] relativePokemonRankings;
    delete [] availablePokemonToSort;
}
