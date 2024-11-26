// programa principal do projeto "The Boys - 2024/2"
// Autor: Caio Mendonça Krol, GRR 20245217

// seus #includes vão aqui
#include "conjunto.h"
#include "fprio.h"
#include "lista.h"
// seus #defines vão aqui

// minimize o uso de variáveis globais

struct s_coord {
  int x;
  int y;
};

struct s_heroi {
  int id;
  struct cjto_t *habilidades;
  int paciencia;
  int velocidade;
  int experiencia;
  int base;
};

struct s_base {
  int id;
  int lotacao;
  struct cjto_t *presentes;
  struct lista_t *espera;
  struct s_coord local;
};

struct s_missao {
  int id;
  struct cjto_t *habilidades;
  int n_perigo;
  struct s_coord local;
};

struct s_mundo {
  int NHerois;
  struct s_heroi herois[];
  
};
// programa principal
int main ()
{
  // iniciar o mundo

  // executar o laço de simulação

  // destruir o mundo

  return (0) ;
}

