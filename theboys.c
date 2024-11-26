// programa principal do projeto "The Boys - 2024/2"
// Autor: Caio Mendonça Krol, GRR 20245217

// seus #includes vão aqui
#include <stdio.h>
#include "conjunto.h"
#include "fprio.h"
#include "lista.h"
// seus #defines vão aqui
#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)
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
  struct s_heroi herois[N_HEROIS];
  int NBases;
  struct s_base bases[N_BASES];
  int NMissoes;
  struct s_missao missoes[N_MISSOES];
  int NHabilidades;
  int tamanho_mundo;
  int relogio;
};
// Retorna um número inteiro aleatório entre min e max
int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

struct s_mundo inicializa_mundo()
{
  /* data */
};


struct s_heroi inicializa_heroi(int id){
  struct s_heroi novo_heroi;
  novo_heroi.id = id;
  novo_heroi.experiencia = 0;
  novo_heroi.paciencia = aleat(0, 100);
  novo_heroi.velocidade = aleat(50, 50000);

  if (!(novo_heroi.habilidades = cjto_cria(N_HABILIDADES))){
    printf("Erro ao alocar habilidades do novo heroi");
    return;
  }
  for (int i = 0; i <= aleat(1, 3); i++){
    if (!(cjto_insere(novo_heroi.habilidades, aleat(1, N_HABILIDADES)))){
      printf("Falha ao inserir habilidade!\n");
      return;
    }
  }
    return novo_heroi;
}


// programa principal
int main ()
{

  
  // executar o laço de simulação

  // destruir o mundo

  return (0) ;
}

