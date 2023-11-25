#include "TasTronque.hpp"
#include "Chromosome.hpp"
#include "Game.hpp"

TasSelection::TasSelection(int hauteur) {
    int size = pow(2, hauteur) - 1;
    this->tas = (Chromosome **)calloc(2 * size + 1, sizeof(Chromosome *));
    this->virtualSize = size;
    this->selectedInput = 0;
}

void TasSelection::insere(Chromosome *c) {
    int idInsertion = virtualSize + selectedInput;

    tas[idInsertion] = c;
    remonteInsertion(idInsertion);
    selectedInput = (selectedInput + 1) % virtualSize;
}

void TasSelection::remonteInsertion(int id) {
    if (id == 0)
        return;
    int pere = (id - 1) / 2;
    if (tas[pere] == 0) {
        echanger(id, pere);
    } else {
        auto result = play_match(tas[id], tas[pere]);
        nComparaison++;
        if (result.score <= 0) {
            return;
        }
        echanger(id, pere);
    }
    remonteInsertion(pere);
}

Chromosome *TasSelection::extraire() {
    Chromosome *tmp = tas[0];
    tas[0] = 0;
    remonteExtraction(0);
    return tmp;
}

void TasSelection::remonteExtraction(int id) {
    // std::cout << "R : ";
    // afficher();
    int f1 = 2 * id + 1;
    int f2 = 2 * id + 2;
    if (f1 >= virtualSize)
        return;
    if (tas[f2] == 0) {
        echanger(id, f1);
        remonteExtraction(f1);
        return;
    }
    if (tas[f1] == 0) {
        echanger(id, f2);
        remonteExtraction(f2);
        return;
    }
    auto result = play_match(tas[f1], tas[f2]);
    nComparaison++;
    if (result.score > 0) {
        echanger(id, f1);
        remonteExtraction(f1);
        return;
    } else {
        echanger(id, f2);
        remonteExtraction(f2);
        return;
    }
}

void TasSelection::echanger(int id1, int id2) {
    Chromosome *tmp = tas[id1];
    tas[id1] = tas[id2];
    tas[id2] = tmp;
}

void TasSelection::afficher() {
    for (int a = 0, i = 0; a < virtualSize; i++) {
        for (int b = 0; b < pow(2, i); a++, b++) {
            int id = tas[a] ? tas[a]->id : 0;
            std::cout << std::setfill(' ')
                      << std::setw(6 * (ceil(virtualSize / 2.)) / pow(2, i))
                      << id << " ";
        }
        std::cout << std::endl;
    }
    ;
}
