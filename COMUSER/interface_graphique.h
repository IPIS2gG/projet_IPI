int initialisation_sdl(int, char**, int, int);

int update_sdl(int, char*, char*);

void a_toi_de_jouer(int*, int*, char*);

void attendre_fermeture_sdl();
void attendre_clic_croix();
void attendre_clic_croix_ou_changement_etat (short*, void (*f) (void*));

void arret_sdl();
