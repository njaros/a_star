# a_star

algo un peu gros,

il permet de trouver le plus court chemin d'un labyrinthe 2D avec une heuristique "a vol d'oiseau".
chaque coordonnée du chemin a suivre est située dans les variables i (x) et j (y) situées dans la boucle de la ligne 220.
La fonction retourne le nombre de pas minimum.

! L'algo ne fonctionne pas s'il n'y a pas de solution !

# paththis (prononced "pastis" in french)

Paththis in an easy to module aStar algorithm (because that's a Coplien class). I needed to implement this for adventOfCode2022 and codingame fall challenge 2022.  

I still have to improve this by using map<pair<int, coord>, Node *> unstead of multimap<int, Node *> for the open list.  
