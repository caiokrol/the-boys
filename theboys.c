// programa principal do projeto "The Boys - 2024/2"
// Autor: Caio Mendonça Krol, GRR 20245217

// seus #includes vão aqui
#include <stdio.h>
#include <math.h>>
#include <stdlib.h>
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

#define MORTO 0
#define VIVO 1

#define EMABERTO 0
#define CUMPRIDA 1

#define E_CHEGA    1
#define E_ESPERA   2
#define E_DESISTE  3
#define E_AVISA    4
#define E_ENTRA    5
#define E_SAI      6
#define E_VIAJA    7
#define E_MORRE    8
#define E_MISSAO   9
#define E_FIM      10

#define CRIAR_EVENTO(mundo, lef, tipo, tempo, heroi_id, base_id) \
    do { \
        struct s_evento *novo_evento = cria_evento((tempo), (tipo), (heroi_id), (base_id)); \
        if (novo_evento) { \
            fprio_insere((lef), novo_evento, novo_evento->tipo_evento, novo_evento->tempo); \
        } \
    } while (0);

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
  int status;
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
  int status;
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

int chega(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef){
    struct s_base *b = &mundo->bases[base];
    struct s_heroi *h = &mundo->herois[heroi];
    h->base = base;

  if((cjto_card(b->presentes) < b->lotacao) && (lista_tamanho(b->espera) == 0)){
    CRIAR_EVENTO(mundo, lef, E_ESPERA, tempo, heroi, base);
    return E_ESPERA;
  }
  
  if(h->paciencia > (10 * lista_tamanho(b->espera))){
    CRIAR_EVENTO(mundo, lef, E_ESPERA, tempo, heroi, base);
    return E_ESPERA;
  }

  CRIAR_EVENTO(mundo, lef, E_DESISTE, tempo, heroi, base);
  return E_DESISTE;
}

int espera(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    struct s_base *b = &mundo->bases[base];
    
    // Adiciona o herói ao fim da fila de espera da base
    lista_insere(b->espera, heroi, -1);

    // Cria e insere o evento AVISA na LEF
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, -1, base);  // heroi_id não é necessário para AVISA
    return E_AVISA;
}

int desiste(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    // Escolhe uma base destino aleatória
    int nova_base = aleat(0, N_BASES);  // Considerando que mundo->num_bases é o número total de bases

    // Cria e insere o evento VIAJA na LEF
    CRIAR_EVENTO(mundo, lef, E_VIAJA, tempo, heroi, nova_base);
    return E_VIAJA;
}

int avisa(struct s_mundo *mundo, int tempo, int base, struct fprio_t *lef) {
    struct s_base *b = &mundo->bases[base];

    // Enquanto houver vaga na base e heróis esperando na fila
    while ((cjto_card(b->presentes) < b->lotacao) && (lista_tamanho(b->espera) > 0)) {
        // Retira o primeiro herói da fila de espera
        int heroi_id = lista_remove_inicio(b->espera);  // Assume função para remover do início da lista
        
        // Adiciona o herói ao conjunto de heróis presentes na base
        cjto_adiciona(b->presentes, heroi_id);

        // Cria e insere o evento ENTRA na LEF
        CRIAR_EVENTO(mundo, lef, E_ENTRA, tempo, heroi_id, base);
    }
    return E_ENTRA;
}

int entra(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    struct s_heroi *h = &mundo->herois[heroi];
    int TPB;
    TPB = (15 + h->paciencia) * aleat(1, 20);

    CRIAR_EVENTO(mundo, lef, E_SAI, tempo + TPB, heroi, base);
}

int sai(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    struct s_base *b = &mundo->bases[base];
    struct s_heroi *h = &mundo->herois[heroi];

    // Remove o herói do conjunto de presentes na base
    cjto_remove(b->presentes, heroi);

    // Escolhe uma base destino aleatória diferente da base atual
    int destino;
    do {
        destino = rand() % mundo->NBases;  // Assume que o número total de bases é num_bases
    } while (destino == base);  // Garante que a base destino é diferente da atual

    // Cria e insere o evento VIAJA na LEF
    CRIAR_EVENTO(mundo, lef, E_VIAJA, tempo, heroi, destino);

    // Cria e insere o evento AVISA para notificar o porteiro sobre a vaga liberada
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, -1, base);

    return E_VIAJA;
}

int viaja(struct s_mundo *mundo, int tempo, int heroi, int destino, struct fprio_t *lef) {
    struct s_heroi *h = &mundo->herois[heroi];
    struct s_base *b_atual = &mundo->bases[h->base];
    struct s_base *b_destino = &mundo->bases[destino];

    // Calcula a distância entre a base atual e a base de destino usando a posição (x, y)
    float distancia = calcula_distancia(b_atual->local.x, b_atual->local.y, b_destino->local.x, b_destino->local.y);
    
    // Calcula a duração da viagem com base na velocidade do herói
    int duracao = distancia / h->velocidade;  // Assume que 'velocidade' está na struct s_heroi

    // Cria e insere o evento CHEGA na LEF
    CRIAR_EVENTO(mundo, lef, E_CHEGA, tempo + duracao, heroi, destino);

    return E_CHEGA;
}

int morre(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    struct s_base *b = &mundo->bases[base];
    struct s_heroi *h = &mundo->herois[heroi];

    // Remove o herói do conjunto de presentes na base
    cjto_remove(b->presentes, heroi);

    // Marca o herói como morto
    h->status = MORTO;  // Assume que existe um campo 'status' e uma constante MORTO

    // Cria e insere o evento AVISA na LEF para notificar o porteiro
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, -1, base);

    return E_AVISA;
}

int missao(struct s_mundo *mundo, int tempo, int missao_id, struct fprio_t *lef) {
    struct s_missao *m = &mundo->missoes[missao_id];
    struct s_base *bmp = NULL;
    float menor_distancia = INFINITY;

    // Calcula a distância de cada base até o local da missão e verifica se está apta
    for (int i = 0; i < mundo->NBases; i++) {
        struct s_base *b = &mundo->bases[i];
        if (base_esta_apta(b, m)) { // Supondo função base_esta_apta() já implementada
            float distancia = calcula_distancia(b->local.x, b->local.y, m->local.x, m->local.y);
            if (distancia < menor_distancia) {
                menor_distancia = distancia;
                bmp = b;
            }
        }
    }

    // Se há uma base apta (BMP)
    if (bmp) {
        m->status = CUMPRIDA;
        for (int i = 0; i < cjto_card(bmp->presentes); i++) {
            struct s_heroi *h = cjto_obtem_heroi(bmp->presentes, i);  // Supondo função que retorna o herói na base
            float risco = m->n_perigo / (h->paciencia + h->experiencia + 1.0);
            if (risco > aleatorio(0, 30)) {  // Supondo função aleatorio()
                CRIAR_EVENTO(mundo, lef, E_MORRE, tempo, h->id, bmp->id);
                return E_MORRE;
            } else {
                h->experiencia++;
            }
        }
        return E_MISSAO;
    }

    // Caso não haja uma base apta, adia a missão
    CRIAR_EVENTO(mundo, lef, E_MISSAO, tempo + (24 * 60), missao_id, -1); // Adia para o dia seguinte
    return E_MISSAO;
}

// Retorna um número inteiro aleatório entre min e max
int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

float calcula_distancia(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
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
  heroi->status = VIVO;

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

