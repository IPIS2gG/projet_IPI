#include <stdlib.h>
#include <stdio.h>

//Cette fonction concatène deux chaines de caractères en renvoyant le résultat dans une nouvelle chaîne
char* concat_string(const char* s1, const char* s2)
{
  int i = 0, j = 0, taille_s1, taille_s2, taille_res;
  char* res;
  while (*(s1 + i)) i++;
  while (*(s2 + j)) j++;
  taille_s1 = i;
  taille_s2 = j;
  taille_res = (i + j + 1);
  res = malloc(taille_res * sizeof(char));
  for(i=0; i < taille_s1; i++) res[i] = s1[i];
  for(j=0; j < taille_s2; j++) res[i + j] = s2[j];
  res[i + j] = '\0';
  return res;
}



//Cette fonction concatène deux chaines de caractères en renvoyant le résultat dans une nouvelle chaîne et libère celle de gauche
char* concat_string_gfree(char* s1, const char* s2)
{
  char* res;
  res = concat_string(s1, s2);
  free(s1);
  return res;
}



//Cette fonction concatène deux chaines de caractères en renvoyant le résultat dans une nouvelle chaîne et libère celle de droite
char* concat_string_dfree(const char* s1, char* s2)
{
  char* res;
  res = concat_string(s1, s2);
  free(s2);
  return res;
}



//Cette fonction concatène deux chaines de caractères et les libère en renvoyant le résultat dans une nouvelle chaîne
char* concat_string_bfree(char* s1, char* s2)
{
  char* res;
  res = concat_string(s1, s2);
  free(s1);
  free(s2);
  return res;
}





//Cette fonction converti un entier en chaîne de caractère
char* itoa (int x)
{
  int nb_lettres = 0, y=x, i;
  char* s;

  if (x == 0) return concat_string("0", "");

  while(y)
  {
    nb_lettres++;
    y = y / 10;
  }

  s = malloc((nb_lettres + 1) * sizeof(char));
  for(i=nb_lettres - 1; i>=0; i--)
  {
    s[i] = (x % 10) + 48;
    x = x / 10;
  }
  s[nb_lettres] = '\0';
  return s;
}
