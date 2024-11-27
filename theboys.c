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

#define CHEGA 0
#define ESPERA 1
#define DESISTE 2
#define SAI 3

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

struct s_evento {
    int tempo;        // Quando o evento ocorre
    int heroi_id;     // Identificador do herói
    int base_id;      // Identificador da base (se necessário)
    int tipo_evento;  // Tipo do evento (CHEGA, SAI, etc.)
    // Adicione outros campos conforme necessário para o evento
};

struct s_evento *cria_evento(int tempo, int tipo, int heroi_id, int base_id){
    struct s_evento *novo;
    if (!(novo = malloc(sizeof(struct s_evento)))){
      printf("Erro ao alocar memoria para novo evento");
      return NULL;
    }
    novo->tempo = tempo;
    novo->tipo_evento = tipo;
    novo->heroi_id = heroi_id;
    novo->base_id = base_id;

    return novo;
}

int chega(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef)
{
    struct s_base *b = &mundo->bases[base];
    struct s_heroi *h = &mundo->herois[heroi];
    bool espera;
    h->base = base;

  if((cjto_card(b->presentes) < b->lotacao) && (lista_tamanho(b->espera) == 0)){
    struct s_evento *novo_evento = cria_evento(tempo, ESPERA, heroi, base);
    fprio_insere(lef, &novo_evento, ESPERA, novo_evento->tempo);
    return 1;
  }
  
  if(h->paciencia > (10 * lista_tamanho(b->espera))){
    struct s_evento *novo_evento = cria_evento(tempo, ESPERA, heroi, base);
    fprio_insere(lef, &novo_evento, ESPERA, novo_evento->tempo);
    return 1;
  }

  struct s_evento *novo_evento = cria_evento(tempo, DESISTE, heroi, base);
  fprio_insere(lef, &novo_evento, DESISTE, novo_evento->tempo);
  return 0;
}

// Retorna um número inteiro aleatório entre min e max
int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}


struct s_mundo* inicializa_mundo(){
  struct s_mundo *mundo;

  if(!(mundo = malloc(sizeof(struct s_mundo)))){
    printf("Erro ao alocar espaço para a estrutura mundo");
    return NULL;
  }
  mundo->NHerois = N_HEROIS;
  mundo->NBases = N_BASES;
  mundo->NMissoes = N_MISSOES;
  mundo->NHabilidades = N_HABILIDADES;
  mundo->tamanho_mundo = N_TAMANHO_MUNDO;
  mundo->relogio = T_INICIO;
  
  return mundo;
};

int inicializa_heroi(struct s_heroi *heroi, int *id) {
  heroi->id = *id;
  heroi->experiencia = 0;
  heroi->paciencia = aleat(0, 100);
  heroi->velocidade = aleat(50, 50000);


  heroi->habilidades = cjto_cria(N_HABILIDADES);
  if (!heroi->habilidades) {
    printf("Erro ao alocar habilidades do novo herói\n");
    return -1;
  }

  for (int i = 0; i < aleat(1, 3); i++) {
    if (!cjto_insere(heroi->habilidades, aleat(0, N_HABILIDADES - 1))) {
      printf("Falha ao inserir habilidade!\n");
      cjto_destroi(heroi->habilidades); //Libera memoria em caso de erro;
      return -1;
    }
  }
  return 1;
}

int inicializar_herois(struct s_mundo *mundo, int *id) {

  for (int i = 0; i < N_HEROIS; i++) {
    if (inicializa_heroi(&(mundo->herois[i]), id) == -1) {
      return -1;
    }
    (*id)++;
  }
  return 1;
}

int inicializa_base(struct s_base *base, int *id) {
  base->id = *id;
  base->lotacao = aleat(3, 10);
  base->local.x = aleat(0, N_TAMANHO_MUNDO - 1);
  base->local.y = aleat(0, N_TAMANHO_MUNDO - 1);

  if (!(base->presentes = cjto_cria(base->lotacao))) {
    printf("Erro ao criar conjunto de presentes da base\n");
    return -1;
  }

  base->espera = lista_cria();
  if (!base->espera) {
    printf("Erro ao criar lista de espera da base\n");
    lista_destroi(base->espera);  // Libera memória alocada antes
    return -1;
  }
  return 1;
}

int inicializar_bases(struct s_mundo *mundo, int *id) {
  for (int i = 0; i < N_BASES; i++) {
    if (inicializa_base(&(mundo->bases[i]), id) == -1) {
      return -1;
    }
    (*id)++;
  }
  return 1;
}

int inicializa_missao(struct s_missao *missao, int *id) {
  missao->id = *id;
  missao->local.x = aleat(0, N_TAMANHO_MUNDO - 1);
  missao->local.y = aleat(0, N_TAMANHO_MUNDO - 1);

  int num_Hab = aleat(6, 10);
  if (!(missao->habilidades = cjto_cria(num_Hab))) {
    printf("Erro ao criar conjunto de habilidades da missão\n");
    return -1;
  }

  for (int i = 0; i < num_Hab; i++) {
    if (!cjto_insere(missao->habilidades, aleat(1, N_HABILIDADES))) {
      printf("Erro ao inserir habilidade na missão\n");
      cjto_destroi(missao->habilidades);  // Libera memória em caso de falha
      return -1;
    }
  }

  missao->n_perigo = aleat(0, 100);
  return 1;
}

int inicializar_missoes(struct s_mundo *mundo, int *id) {
  for (int i = 0; i < N_MISSOES; i++) {
    if (inicializa_missao(&(mundo->missoes[i]), id) == -1) {
      return -1;
    }
    (*id)++;
  }
  return 1;
}



// programa principal
int main ()
{
int *ultimo_id_herois = 0;
int *ultimo_id_bases = 0;
int *ultimo_id_missoes = 0;

struct s_mundo *mundo = inicializa_mundo();
inicializar_herois(mundo, ultimo_id_herois);
inicializar_bases(mundo, *ultimo_id_bases);
inicializar_missoes(mundo, ultimo_id_missoes);

for (int i = 0; i < N_HEROIS; i++){
  mundo->herois->base;
}

struct fprio_t *lef = fprio_cria();
  // executar o laço de simulação

  // destruir o mundo

  return (0) ;
}

