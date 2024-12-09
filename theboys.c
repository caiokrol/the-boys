// programa principal do projeto "The Boys - 2024/2"
// Autor: Caio Mendonça Krol, GRR 20245217

// seus #includes vão aqui
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
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
        cria_evento(tempo, tipo, heroi_id, base_id, lef);\
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
  int contador_missoes;
  int fila_max;
};

struct s_missao {
  int id;
  struct cjto_t *habilidades;
  int n_perigo;
  struct s_coord local;
  int status;
  int tentativas;
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
  int eventos_tratados;
  int missoes_cumpridas;
};

struct s_evento {
    int tempo;        // Quando o evento ocorre
    int heroi_id;     // Identificador do herói
    int base_id;      // Identificador da base (se necessário)
    int tipo_evento;  // Tipo do evento (CHEGA, SAI, etc.)
};

// Retorna um número inteiro aleatório entre min e max
int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

int calcula_distancia(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

struct s_evento *cria_evento(int tempo, int tipo, int heroi_id, int base_id, struct fprio_t *lef) {
    // Aloca memória para o novo evento
    struct s_evento *novo = malloc(sizeof(struct s_evento));
    if (!novo) {
        printf("Erro ao alocar memória para o novo evento.\n");
        return NULL;
    }

    // Inicializa os campos do evento
    novo->tempo = tempo;
    novo->tipo_evento = tipo;
    novo->heroi_id = heroi_id;
    novo->base_id = base_id;

    // Insere o evento na fila de prioridade
    if (fprio_insere(lef, novo, tipo, tempo) == -1) {
        printf("Erro ao inserir evento na fila de prioridade.\n");
        free(novo); // Libera a memória se a inserção falhar
        return NULL;
    }

    return novo;
}


int chega(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef){

  if((cjto_card(mundo->bases[base].presentes) < mundo->bases[base].lotacao) && (lista_tamanho(mundo->bases[base].espera) == 0)){
    CRIAR_EVENTO(mundo, lef, E_ESPERA, tempo, heroi, base);
    return E_ESPERA;
  }
  
  if(mundo->herois[heroi].paciencia > (10 * lista_tamanho(mundo->bases[base].espera))){
    CRIAR_EVENTO(mundo, lef, E_ESPERA, tempo, heroi, base);
    return E_ESPERA;
  }

  CRIAR_EVENTO(mundo, lef, E_DESISTE, tempo, heroi, base);
  return E_DESISTE;
}

int espera(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {

    // Adiciona o herói ao fim da fila de espera da base
    lista_insere(mundo->bases[base].espera, heroi, (-1));


    // Cria e insere o evento AVISA na LEF
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, heroi, base);  // heroi_id não é necessário para AVISA
    return E_AVISA;
}

int desiste(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    (void)mundo; //Suprime o erro do compilador de que o parametro não está sendo utilizado
    // Escolhe uma base destino aleatória

    int nova_base = aleat(0, N_BASES - 1);  // Considerando que mundo->num_bases é o número total de bases

    // Cria e insere o evento VIAJA na LEF
    CRIAR_EVENTO(mundo, lef, E_VIAJA, tempo, heroi, nova_base);
    return E_VIAJA;
}

void avisa(struct s_mundo *mundo, int tempo, int base, struct fprio_t *lef){
    int heroi_na_vez;
    if(lista_tamanho(mundo->bases[base].espera) > mundo->bases[base].fila_max){
        mundo->bases[base].fila_max = lista_tamanho(mundo->bases[base].espera);
    };
    while (cjto_card(mundo->bases[base].presentes) < mundo->bases[base].lotacao && lista_tamanho(mundo->bases[base].espera))
    {
        lista_retira(mundo->bases[base].espera, &heroi_na_vez, 0);
        cjto_insere(mundo->bases[base].presentes, heroi_na_vez);
        printf("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", tempo, base, heroi_na_vez);
        CRIAR_EVENTO(mundo, lef, E_ENTRA, tempo, heroi_na_vez, base);
    }
}

int entra(struct s_mundo *mundo, int tempo_entrada, int heroi, int base, struct fprio_t *lef) {
    int TPB;

    TPB = (15 + mundo->herois[heroi].paciencia) * aleat(1, 20);
    int tempo = tempo_entrada + TPB;
    CRIAR_EVENTO(mundo, lef, E_SAI, tempo, heroi, base);

    return tempo;
}

int sai(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {

    // Remove o herói do conjunto de presentes na base
    cjto_retira(mundo->bases[base].presentes, heroi);
    // Escolhe uma base destino aleatória diferente da base atual
    int destino;
    do {
        destino = rand() % mundo->NBases;  // Assume que o número total de bases é num_bases
    } while (destino == base);  // Garante que a base destino é diferente da atual
    // Cria e insere o evento VIAJA na LEF
    CRIAR_EVENTO(mundo, lef, E_VIAJA, tempo, heroi, destino);
    // Cria e insere o evento AVISA para notificar o porteiro sobre a vaga liberada
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, heroi, base);

    return E_VIAJA;
}

int viaja(struct s_mundo *mundo, int tempo, int heroi, int destino, struct fprio_t *lef) {
  int b_atual = mundo->herois[heroi].base;
    // Calcula a distância entre a base atual e a base de destino usando a posição (x, y)
    int distancia = calcula_distancia(mundo->bases[b_atual].local.x, mundo->bases[b_atual].local.y, mundo->bases[destino].local.x, mundo->bases[destino].local.y);
    // Calcula a duração da viagem com base na velocidade do herói
    int duracao = distancia / mundo->herois[heroi].velocidade;  // Assume que 'velocidade' está na struct s_heroi
    // Cria e insere o evento CHEGA na LEF
    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", tempo, heroi, b_atual, destino, distancia, mundo->herois[heroi].velocidade, (tempo + duracao));
    CRIAR_EVENTO(mundo, lef, E_CHEGA, (tempo + duracao), heroi, destino);

    return E_CHEGA;
}
int morre(struct s_mundo *mundo, int tempo, int heroi, int missao_id, struct fprio_t *lef) {

    // Remove o herói do conjunto de presentes na base
    cjto_retira(mundo->bases[mundo->herois[heroi].base].presentes, heroi);

    // Marca o herói como morto
    mundo->herois[heroi].status = MORTO;  // Assume que existe um campo 'status' e uma constante MORTO
    // Cria e insere o evento AVISA na LEF para notificar o porteiro

    printf("%6d: MORRE  HEROI %2d MISSAO %d\n", tempo, heroi, missao_id);
    CRIAR_EVENTO(mundo, lef, E_AVISA, tempo, (-1), mundo->herois[heroi].base);

    return E_AVISA;
}

struct cjto_t *uniao_habilidades(struct s_mundo *mundo, struct s_base *b) {
    if (!mundo || !b) return NULL;

    // Cria um conjunto vazio para armazenar a união
    struct cjto_t *uniao = cjto_cria(N_HEROIS); // Aloca com capacidade inicial suficiente
    if (!uniao) return NULL;
    /*printf("Presentes:\n");
    cjto_imprime(b->presentes);
    printf("\n");*/
    // Itera sobre todos os heróis para verificar os presentes
    for (int i = 0; i < N_HEROIS; i++) {
          /*printf("Base: %d\n", b->id);
          printf("I: %d\n", i);*/
        if (cjto_pertence(b->presentes, i) == 1){

          /*printf("ENtrou aqui no União habilidades\n");*/
            // Realiza a união do conjunto de habilidades do herói com o acumulado
            struct cjto_t *temp = cjto_uniao(uniao, mundo->herois[i].habilidades);
            if (!temp) {
                cjto_destroi(uniao); // Libera memória em caso de erro
                return NULL;
            }

            // Libera o conjunto antigo e atualiza para o novo conjunto unido
            cjto_destroi(uniao);
            uniao = temp;
        }
    }

    return uniao;
}

bool base_esta_apta(struct s_mundo *mundo, struct s_base *b, struct s_missao *m){
  if(cjto_contem((uniao_habilidades(mundo, b)), m->habilidades)){
    return true;
  }

  return false;
}

int missao(struct s_mundo *mundo, int tempo, int missao_id, struct fprio_t *lef) {
    struct s_base *bmp = NULL;
    int menor_distancia = (N_TAMANHO_MUNDO * N_TAMANHO_MUNDO) + 1; 
    mundo->missoes[missao_id].tentativas = mundo->missoes[missao_id].tentativas + 1;
    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", tempo, missao_id, mundo->missoes[missao_id].tentativas);
    cjto_imprime(mundo->missoes[missao_id].habilidades);
    printf(" ]\n");
    // Calcula a distância de cada base até o local da missão e verifica se está apta
    for (int i = 0; i < mundo->NBases; i++) {
        struct s_base *b = &mundo->bases[i];
        if (base_esta_apta(mundo, b, &mundo->missoes[missao_id])) { // Supondo função base_esta_apta() já implementada 
            int distancia = calcula_distancia(mundo->bases[i].local.x, mundo->bases[i].local.y, mundo->missoes[missao_id].local.x, mundo->missoes[missao_id].local.y);
            if (distancia < menor_distancia) {
                menor_distancia = distancia;
                bmp = b;
            }
        }
    }
    // Se há uma base apta (BMP)
    if (bmp) {
        mundo->missoes[missao_id].status = CUMPRIDA;
        mundo->missoes_cumpridas++;
        mundo->bases[bmp->id].contador_missoes++;
        printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", tempo, missao_id, bmp->id);
        cjto_imprime(uniao_habilidades(mundo, bmp));
        printf(" ]\n");
        for (int i = 0; i < cjto_card(bmp->presentes); i++) {
            struct s_heroi *h = &mundo->herois[i];
            float risco = mundo->missoes[missao_id].n_perigo / (h->paciencia + h->experiencia + 1.0);
            if (risco > aleat(0, 30)) { 
                CRIAR_EVENTO(mundo, lef, E_MORRE, tempo, h->id, missao_id);
                return E_MORRE;
            } else {
                h->experiencia++;
            }
        }
        return E_MISSAO;
    }
    printf("%6d: MISSAO %d IMPOSSIVEL\n", tempo, missao_id);
    // Caso não haja uma base apta, adia a missão
    CRIAR_EVENTO(mundo, lef, E_MISSAO, tempo + (24 * 60), missao_id, (-1)); // Adia para o dia seguinte
    return E_MISSAO;
}

int procura_min_tentativas(struct s_mundo *mundo){
  int min = mundo->missoes[0].tentativas;
  for (int i = 1; i < N_MISSOES; i++){
    if(mundo->missoes[i].tentativas < min){
      min = mundo->missoes[i].tentativas;
    }
  }
  return min;
}
int procura_max_tentativas(struct s_mundo *mundo){
  int max = mundo->missoes[0].tentativas;
  for (int i = 1; i < N_MISSOES; i++){
    if(mundo->missoes[i].tentativas > max){
      max = mundo->missoes[i].tentativas;
    }
  }
  return max;
}
float media_tentativas(struct  s_mundo *mundo){
  int soma = 0;
  for (int i = 0; i < N_MISSOES; i++){
    soma = soma + mundo->missoes[i].tentativas;
  }
  return ((float)soma/N_MISSOES);
};

int fim(struct s_mundo *mundo, int tempo) {
    // 1. Exibe as informações dos heróis
    printf("%6d: FIM\n\n", tempo);

    printf("HEROIS:\n");
    for (int i = 0; i < N_HEROIS; i++) {
        struct s_heroi *heroi = &mundo->herois[i];
        printf(
            "HEROI %2d %s PAC %3d VEL %4d EXP %4d HABS [ ",
            heroi->id,
            heroi->status == MORTO ? "MORTO" : "VIVO ",
            heroi->paciencia,
            heroi->velocidade,
            heroi->experiencia
        );

        // Imprime as habilidades do herói
        cjto_imprime(mundo->herois[i].habilidades);
        printf(" ]\n");
    }

    // 2. Exibe as informações das bases
    printf("\nBASES:\n");
    for (int i = 0; i < N_BASES; i++) {
        struct s_base *base = &mundo->bases[i];
        printf(
            "BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n",
            base->id,
            base->lotacao,
            base->fila_max,
            base->contador_missoes
        );
    }

    // 3. Calcula e exibe as estatísticas gerais
    int total_eventos = mundo->eventos_tratados;
    int missoes_sucesso = mundo->missoes_cumpridas;
    int total_missoes = N_MISSOES;

    float sucesso_perc = (missoes_sucesso / (float)total_missoes) * 100.0;
    int tentativas_min = procura_min_tentativas(mundo);
    int tentativas_max = procura_max_tentativas(mundo);
    float tentativas_media = media_tentativas(mundo);

    int total_herois = N_HEROIS;
    int herois_mortos = 0;
    for (int i = 0; i < N_HEROIS; i++) {
        if (mundo->herois[i].status == MORTO) {
            herois_mortos++;
        }
    }
    float taxa_mortalidade = (herois_mortos / (float)total_herois) * 100.0;

    // Exibe estatísticas gerais
    printf("\nEVENTOS TRATADOS: %d\n", total_eventos);
    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", missoes_sucesso, total_missoes, sucesso_perc);
    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", tentativas_min, tentativas_max, tentativas_media);
    printf("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);

    // 4. Libera os recursos alocados
    for (int i = 0; i < N_BASES; i++) {
        cjto_destroi(mundo->bases[i].presentes);
        lista_destroi(mundo->bases[i].espera);
    }
    for (int i = 0; i < N_HEROIS; i++) {
        cjto_destroi(mundo->herois[i].habilidades);
    }
    for (int i = 0; i < N_MISSOES; i++) {
        cjto_destroi(mundo->missoes[i].habilidades);
    }
    free(mundo);

    // 5. Encerra a simulação
    printf("\nSimulação encerrada.\n");
    return -1;  // Retorna o código de encerramento
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
  mundo->eventos_tratados = 0;
  mundo->missoes_cumpridas = 0;
  return mundo;
};

int inicializa_heroi(struct s_heroi *heroi, int *id) {

  if (heroi == NULL) {  // Verificação de ponteiro inválido
    printf("Erro: ponteiro heroi inválido!\n");
    return -1;
  }

  heroi->id = *id;
  heroi->experiencia = 0;
  heroi->paciencia = aleat(0, 100);
  heroi->velocidade = aleat(50, 5000);
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

int inicializar_herois(struct s_mundo *mundo, int *id, struct fprio_t *lef) {

  for (int i = 0; i < N_HEROIS; i++) {
    if (inicializa_heroi(&(mundo->herois[i]), id) == -1) {
      return -1;
    }
    int base_aleatoria = aleat(0, N_BASES - 1);       // Base aleatória entre 0 e (N_BASES - 1)
    int tempo = aleat(0, 4320);       // Tempo aleatório entre 0 e 4320 minutos

        // Cria e insere o evento CHEGA na LEF
        CRIAR_EVENTO(mundo, lef, E_CHEGA, tempo, i, base_aleatoria);
    (*id)++;
  }
  return 1;
}

int inicializa_base(struct s_base *base, int *id) {
  base->id = *id;
  base->lotacao = aleat(3, 10);
  base->local.x = aleat(0, N_TAMANHO_MUNDO - 1);
  base->local.y = aleat(0, N_TAMANHO_MUNDO - 1);
  base->contador_missoes = 0;

  if (!(base->presentes = cjto_cria(N_HEROIS))) {
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
  missao->status = EMABERTO;
  missao->tentativas = 0;
  int num_Hab = aleat(6, 10);
  if (!(missao->habilidades = cjto_cria(N_HABILIDADES))) {
    printf("Erro ao criar conjunto de habilidades da missão\n");
    return -1;
  }
  int flag;
  for (int i = 1; i < num_Hab; i++) {
    flag = cjto_insere(missao->habilidades, aleat(0, num_Hab));
    if (flag < 0) {
      printf("Erro ao inserir habilidade na missão %d\n", i);
      cjto_destroi(missao->habilidades);  // Libera memória em caso de falha
      return -1;
    }
  }
  missao->n_perigo = aleat(0, 100);


  return 1;
}

int inicializar_missoes(struct s_mundo *mundo, int *id, struct fprio_t *lef) {
  for (int i = 0; i < N_MISSOES; i++) {
    if (inicializa_missao(&(mundo->missoes[i]), id) == -1) {
      return -1;
    }
    int tempo = aleat(0, T_FIM_DO_MUNDO);
    CRIAR_EVENTO(mundo, lef, E_MISSAO, tempo, i, -1);
    (*id)++;
  }
  return 1;
}

void executar_simulacao(struct s_mundo *mundo, struct fprio_t *lef) {
    struct s_evento *evento_atual = malloc(sizeof(struct s_evento));
    if (evento_atual == NULL) {
        printf("Erro ao alocar memória para evento_atual!\n");
        return;
    }
      bool flag = true;
    while (flag) {
        evento_atual = fprio_retira(lef, &evento_atual->tipo_evento, &evento_atual->tempo);
        mundo->eventos_tratados++;
        if (evento_atual == NULL) {
            printf("Erro: Não há mais eventos para processar\n");
            return;
        }
        // Processa o evento com base no tipo
        switch (evento_atual->tipo_evento) {
            case E_CHEGA:
                //printf("Entrou no E_CHEGA\n");
                int retorno = chega(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                if(retorno == E_ESPERA){
                  printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ESPERA\n",evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                }
                if(retorno == E_DESISTE){
                  printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) DESISTE\n",evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                }
                //printf("Saiu do E_CHEGA\n");
                break;
            case E_ESPERA:
                //printf("Entrou no E_ESPERA\n");
                espera(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lista_tamanho(mundo->bases[evento_atual->base_id].espera));
                //printf("Saiu do E_ESPERA\n");
                break;
            case E_ENTRA:
                //printf("Entrou no E_ENTRA\n");
                int t_saida = entra(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao, t_saida);
               // printf("Saiu do E_ENTRA\n");
                break;
            case E_DESISTE:
                //printf("Entrou no E_DESISTE\n");
                desiste(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                printf("%6d: DESIST HEROI %2d BASE %d\n",evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id);
                //printf("Saiu do E_DESISTE\n");
                break;
            case E_AVISA:
               // printf("Entrou no E_AVISA\n");
               printf("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA [ ", evento_atual->tempo, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
               lista_imprime(mundo->bases[evento_atual->base_id].espera);
               printf(" ]\n");
                avisa(mundo, evento_atual->tempo, evento_atual->base_id, lef);
               // printf("Saiu do E_AVISA\n");
                break;
            case E_MISSAO:
                //printf("Entrou no E_MISSAO\n");
                missao(mundo, evento_atual->tempo, evento_atual->heroi_id, lef); // Adapte se missao_id existir
               // printf("Saiu do E_MISSAO\n");
                break;
            case E_SAI:
               // printf("Entrou no E_SAI\n");
                printf("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                sai(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
               // printf("Saiu do E_SAI\n");
                break;
            case E_MORRE:
               // printf("Entrou no E_MORRE\n");
               
                morre(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
               // printf("Saiu do E_MORRE\n");
                break;
            case E_VIAJA:
               // printf("Entrou no E_VIAJA\n");
                //printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", evento_atual->tempo, evento_atual->heroi_id, mundo->herois[evento_atual->heroi_id].base, evento_atual->base_id, distancia, h->velocidade, duracao);
                viaja(mundo, evento_atual->tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
               // printf("Saiu do E_VIAJA\n");
                break;
            case E_FIM:
                fim(mundo, evento_atual->tempo);
                free(evento_atual);
                flag = false;
                return;
            default:
                printf("Evento desconhecido: %d\n", evento_atual->tipo_evento);
        }

        // Libera a memória do evento processado
    }
}

void imprimirDetMissoes(struct s_mundo *mundo){
  for(int i = 0; i<N_MISSOES; i++){
printf("Missao: %d, id: %d\n", i, mundo->missoes[i].id);
printf("habilidades necessarias: ");
cjto_imprime((mundo->missoes[i].habilidades));
printf("\n");
printf("Nivel de perigo: %d\n", mundo->missoes[i].n_perigo);
printf("Coordenadas: x: %d, y: %d\n",mundo->missoes[i].local.x, mundo->missoes[i].local.y);
printf("Status: %d\n", mundo->missoes[i].status);
printf("\n");
}
}
void imprimirDetHerois(struct s_mundo *mundo){
for(int i=0; i < N_HEROIS; i++){
  printf("Heroi: %d, id: %d\n", i, mundo->herois[i].id);
  printf("habilidades: ");
  cjto_imprime((mundo->herois[i].habilidades));
  printf("\n");
  printf("Paciencia: %d\n", mundo->herois[i].paciencia);
  printf("Velocidade: %d\n", mundo->herois[i].velocidade);
  printf("Experiencia: %d\n", mundo->herois[i].experiencia);
  printf("Base: %d\n", mundo->herois[i].base);
  printf("Status: %d\n", mundo->herois[i].status);
  printf("\n");
}
}
void imprimirDetBases(struct s_mundo *mundo){
for(int i = 0; i <N_BASES; i++){
  printf("Base: %d, id: %d\n", i, mundo->bases[i].id);
  printf("Lotacao: %d\n", mundo->bases[i].lotacao);
  printf("Presentes: ");
  cjto_imprime(mundo->bases[i].presentes);
  printf("\n");
  printf("Espera: ");
  lista_imprime(mundo->bases[i].espera);
  printf("\n");
  printf("Coordenadas: x: %d, y: %d\n",mundo->bases[i].local.x, mundo->bases[i].local.y);
  printf("\n");
}
}
// programa principal
int main ()
{
int ultimo_id_herois = 0;
int ultimo_id_bases = 0;
int ultimo_id_missoes = 0;


struct s_mundo *mundo = inicializa_mundo();
struct fprio_t *lef = fprio_cria();

srand(time(NULL));

inicializar_bases(mundo, &ultimo_id_bases);
printf("Passou inicializações Bases\n");
inicializar_herois(mundo, &ultimo_id_herois, lef);
printf("Passou inicializações Herois\n");
inicializar_missoes(mundo, &ultimo_id_missoes, lef);
printf("Passou inicializações Missoes\n");

cria_evento((T_FIM_DO_MUNDO), (E_FIM), 0, 0, lef); 

executar_simulacao(mundo, lef);

  return (0) ;
}