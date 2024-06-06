#pragma once

//On recherche les deux meilleures équipes pour afficher leur match.
void simulate_and_save(const char *filename);

// Simuler les deux meilleurs joueurs n'est pas toujours le plus cohérent car ils peuvent être très similaires. La fonction permet de simuler un match entre deux individus sélectionnés aléatoirement.
void play_random_match(const char *filename);

// Simuler des matchs avec des chromosomes aléatoires jusqu'à ce qu'il y ait un match
void see_goal(const char *filename, int nGoal);

// Simuler des matchs avec des chromosomes aléatoires jusqu'à ce qu'il y ait une collision avec la balle
void see_ball(const char *filename, int n_ball);
