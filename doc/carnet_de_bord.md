# Carnet de Bord : a-license-to-kill - Belghomari Abdelmalek

### 6/12 : 

-   penser à l'implémentatuon des threads pour faire le compteur de jours, ou faire des threads qui durent sur toute la simulation. 

### 24/12: 

-   Reprise du hard-work: Merry Xmas; 

### 25/12 :
- Travail sur spy simulation : gestion de la shared memory
- Voir la fonction de gestion des blessures laisée dans spy_simulation
- Création de nouvelles structures dans citizen manager

### 31/12 :
- J'ai oublié de noter les jours d'avant
- timer fonctionnel avec test des sémaphores ( note du 06/01 : la memoire n'tait pas modifiable à cause du mappage fait en lecture only........)
- j'ai du reprendre toutes les fonctions d'initialisation des citizen pour utiliser citizen_manager proprement
- Haykel a codé un CSP pour la génération de map. (il est chaud)
- bonne compréhension des mutex et barrieres

### 02/01 : 
- Travail sur le astar pour le déplacement des personnages.
- Création de path dans les structures des citizen pour ne faire le astar qu'une fois pour chaque citizen. 

### 03/01 : 
- Travail sur le mouvement des citoyens ainsi que des espions

### 04/01 :
- bug de déplacements et du astar pour certaines map
- nouveau csp pour le creation random de map

### 05/01:
- trouvaille dans l'avion sur le bug des espions à l'etat 1: j'ai pas reussi à debug, j'y reviens cette aprem
- Bug corrigé je pense, il s'agissait d'un buig de déplacement. 

### 06/01 :

- Travail sur les caméras et le reseau de maille.  
- WOOOOOOOOORKING better, i achieved the displaying of states and i noticed an error in the main.c of monitor (now corrected) -> en gros on voit juste bien tous les deplacemenst et les etats. ça m'a quand meme pris beaucoup de temps pour debuger le code.... le mmap a ete fait en lecture only dans le main.c du monitor, tu m'etonnes que ça marchait pas.
- Demain, on améliore le reseau de maille et on fais la baisse de point de vie s'il reste du temps.
