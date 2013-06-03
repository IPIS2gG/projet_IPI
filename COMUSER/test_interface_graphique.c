#include <stdlib.h>
#include <stdio.h>
#include "interface_graphique.h"

#define true 1
#define false 0
typedef short bool;

int main(int argc, char** argv)
{
  //Initialisation des variables.
  int nb_joueurs_recu;
  char* pseudos[9] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  int nb_case_col_map_recu;
  int nb_case_lig_map_recu;
  int nb_case_tot_map;
  int i;
  char* desc_map;
  int x, y;





  //TODO A supprimer
  //Exemple pour les tests
  nb_joueurs_recu = 9;

  pseudos[0] = "Bidouille";
  pseudos[1] = "Tamaman";
  pseudos[2] = "Natsouille";
  pseudos[3] = "Seip'";
  pseudos[4] = "Unidouille";
  pseudos[5] = "Nocta";
  pseudos[6] = "";
  pseudos[7] = "éééééé";
  pseudos[8] = "YOPYOPYOP";

  /*
  *pseudos = malloc(10 * sizeof(char));
  *(*pseudos) = 'B';
  *(*pseudos + 1) = 'i';
  *(*pseudos + 2) = 'd';
  *(*pseudos + 3) = 'o';
  *(*pseudos + 4) = 'u';
  *(*pseudos + 5) = 'i';
  *(*pseudos + 6) = 'l';
  *(*pseudos + 7) = 'l';
  *(*pseudos + 8) = 'e';
  *(*pseudos + 9) = '\0';


  *(pseudos + 1) = malloc(8 * sizeof(char));
  *(*(pseudos + 1)) = 'T';
  *(*(pseudos + 1) + 1) = 'a';
  *(*(pseudos + 1) + 2) = 'm';
  *(*(pseudos + 1) + 3) = 'a';
  *(*(pseudos + 1) + 4) = 'm';
  *(*(pseudos + 1) + 5) = 'a';
  *(*(pseudos + 1) + 6) = 'n';
  *(*(pseudos + 1) + 7) = '\0';

  *(pseudos + 2) = malloc(11 * sizeof(char));
  *(*(pseudos + 2)) = 'N';
  *(*(pseudos + 2) + 1) = 'a';
  *(*(pseudos + 2) + 2) = 't';
  *(*(pseudos + 2) + 3) = 's';
  *(*(pseudos + 2) + 4) = 'o';
  *(*(pseudos + 2) + 5) = 'u';
  *(*(pseudos + 2) + 6) = 'i';
  *(*(pseudos + 2) + 7) = 'l';
  *(*(pseudos + 2) + 8) = 'l';
  *(*(pseudos + 2) + 9) = 'e';
  *(*(pseudos + 2) + 10) = '\0';

  *(pseudos + 3) = malloc(6 * sizeof(char));
  *(*(pseudos + 3)) = 'S';
  *(*(pseudos + 3) + 1) = 'e';
  *(*(pseudos + 3) + 2) = 'i';
  *(*(pseudos + 3) + 3) = 'p';
  *(*(pseudos + 3) + 4) = '\'';
  *(*(pseudos + 3) + 5) = '\0';

  *(pseudos + 4) = malloc(11 * sizeof(char));
  *(*(pseudos + 4)) = 'U';
  *(*(pseudos + 4) + 1) = 'n';
  *(*(pseudos + 4) + 2) = 'i';
  *(*(pseudos + 4) + 3) = 'd';
  *(*(pseudos + 4) + 4) = 'o';
  *(*(pseudos + 4) + 5) = 'u';
  *(*(pseudos + 4) + 6) = 'i';
  *(*(pseudos + 4) + 7) = 'l';
  *(*(pseudos + 4) + 8) = 'l';
  *(*(pseudos + 4) + 9) = 'e';
  *(*(pseudos + 4) + 10) = '\0';
  */

  nb_case_col_map_recu = 20;
  nb_case_lig_map_recu = 15;

  nb_case_tot_map = nb_case_col_map_recu * nb_case_lig_map_recu;
  desc_map = malloc ((nb_case_tot_map + 1) * sizeof(char));

  for (i=0; i < nb_case_tot_map; i++) desc_map[i] = 'O';
  desc_map[nb_case_tot_map] = '\0';
  desc_map[10] = '1';
  desc_map[13] = '2';
  desc_map[23] = '3';
  desc_map[45] = '4';
  desc_map[52] = '5';
  desc_map[53] = '6';
  desc_map[63] = '7';
  desc_map[68] = '8';
  desc_map[70] = '9';
  desc_map[72] = 'X';
  desc_map[83] = 'D';




  //Initialisation (chargement de la SDL, chargement des surfaces et de la police de référence, calcul des tailles de référence, création des zones et de la fenêtre, affichage de la fenêtre)
  initialisation_sdl(nb_joueurs_recu, pseudos, nb_case_col_map_recu, nb_case_lig_map_recu);



  attendre_clic_croix();








  //Mise à jour avec la map d'exemple
  update_sdl(2, 1, "15", desc_map);
  update_sdl(0, 8, "5", desc_map);

  a_toi_de_jouer(&x, &y, desc_map);

  desc_map[((y * nb_case_col_map_recu) + x)] = 'D';

  update_sdl(3, 3, "15", desc_map);

  a_toi_de_jouer(&x, &y, desc_map);

  desc_map[((y * nb_case_col_map_recu) + x)] = 'X';

  update_sdl(3, 4, "6", desc_map);

  attendre_clic_croix();

  a_toi_de_jouer(&x, &y, desc_map);

  desc_map[((y * nb_case_col_map_recu) + x)] = 'D';

  update_sdl(9, 5, "79", desc_map);

  attendre_clic_croix();

  victoire(3);

  attendre_fermeture_sdl();




  //Libération de la mémoire des surfaces de référence et arrêt de la SDL
  //for(i=0; i<9; i++) free(pseudos[i]);
  free(desc_map);
  arret_sdl();


  return EXIT_SUCCESS;
}
