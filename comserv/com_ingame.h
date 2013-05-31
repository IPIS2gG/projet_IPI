#ifndef HEADER_COMINGAME
#define HEADER_COMINGAME

struct param_partie
{
	int w;
	int h;
	int nb_coups;
	int nb_max;
	std::vector<int> tab_stream;
	std::vector<char*> tab_pseudo;
};

void launch_game(struct param_partie* infos);

#endif
