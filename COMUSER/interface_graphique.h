/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*              interface_graphique.h               */
/*        Header pour  l'utilisation externe        */
/*             de l'interface graphique             */
/*           (créé par Métraud Alexandre)           */
/****************************************************/

//Remarque :
//  Numeros de 1 à 9
//  Indices de 0 à 8

//Fonction d'initialisation de la SDL
int initialisation_sdl(int nombre_de_joueurs, char** pseudos, int nombre_de_cases_en_colonne, int nombre_de_cases_en_ligne);

//Fonction d'update de la fenêtre si arrivée de nouvelles infos
int update_sdl(int indice_joueur_dont_cest_le_tour, int indice_joueur_dont_on_doit_updater_le_score, char* nouveau_score, char* descripteur_de_la_nouvelle_map);

//Fonction d'affichage de victoire
void victoire(int indice_winner);

//Fonction de tour de jeu
//  La case choisie sera placée dans res_x et res_y
void a_toi_de_jouer(int* res_x, int* res_y, char* descripteur_de_map);

//Attente de clic sur croix...
//  ...pour fermer le programme
void attendre_fermeture_sdl();
//  ...pour quitter la fonction
void attendre_clic_croix();
//  ...ou de clic sur bouton valider pour quitter la fonction
void attendre_clic_croix_ou_bouton_valider();
//  ...pour quitter la fonction, et exécution de f si executer passe à true.
void attendre_clic_croix_ou_changement_etat (short* executer, void (*f) (void*));
//  ...pour fermer le programme, et exécution de f si executer passe à true.
void attendre_fermeture_sdl_ou_changement_etat (short* executer, void (*f) (void*));

//Arrêt propre de la SDL
void arret_sdl();
