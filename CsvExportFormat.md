# Sauvegarde des parties dans un fichier CSV:

Le format de sauvegarde est basé sur un principe d'évènements, ils sont écrits dans la forme
`EVENEMENT; [...paramètres]`

Voici la liste des évènements :

## STARTGAME
Commence une partie en en définissant les paramètres.\
Paramètres: 
 - Nombre de joueurs par équipe (il y a donc en vrai 2 fois plus de joueurs).
 - Hauteur du terrain
 - Longueur du terrain
 - Taille de la balle
 - Taille des joueurs

## ALLSETPOS
Définit les positiones de tous les joueurs. \
Paramètres: 
 - pos x de la balle
 - pos y de la balle
 - \<I FOIS>
   - pos x du joueur i
   - pos y du joueur i

## SETSCORE
Définit le score. \
Paramètres:
 - Score équipe 1
 - Score équipe 2

## TICK
Informe l'interpreteur qu'un tick est passé
