// programa principal do projeto "The Boys - 2024/2"
// Autor: Caio Mendonça Krol, GRR 20245217

// seus #includes vão aqui
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "conjunto.h"
#include "fprio.h"
#include "lista.h"


//Parametros do mundo
#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)

//Parametros para heróis fora das bases
#define CEMITERIO -1
#define VOID -2

//Status do heróis
#define MORTO 0
#define VIVO 1

//Status da missao
#define EMABERTO 0
#define CUMPRIDA 1

//identificadores de eventos
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

//Struct que armazena as coordenadas x e y dentro do mundo
struct s_coord {
  int x;
  int y;
};
//Struct que armazena stats do herói
struct s_heroi {
  int id; //id herói
  struct cjto_t *habilidades; //conjunto de habilidades do herói
  int paciencia; //paciencia do herói
  int velocidade; //velocidade do herói
  int experiencia; //experiencia do herói
  int base; //id da base atual do herói
  int status; //status de vida do herói
};
//Struct que armazena stats da base
struct s_base {
  int id; //id base
  int lotacao; //numero máximo de presentes na base
  struct cjto_t *presentes; //conjunto que armazena os heróis presentes na base
  struct lista_t *espera; //lista que armazena a fila de espera da base
  struct s_coord local; //localização da base
  int contador_missoes; //armazena o numero de missoes cumpridas pela base
  int fila_max; //armazena o recorde de maior fila de espera da base
  struct cjto_t *habilidades; //conjunto união das habilidades dos heróis na base
};
//Struct que armazena stats da missão
struct s_missao {
  int id; //id da missão
  struct cjto_t *habilidades; //conjunto de habilidades necessárias para completar a missão
  int n_perigo; //nível de perigo da missão
  struct s_coord local; //localização da missão
  int status; //armazena o status de completude da missão
  int tentativas; //armazena o número de tentativas para completar a missão
};
//Struct que armazena stats do mundo
struct s_mundo {
  int NHerois; //Armazena o numero de heróis do mundo
  struct s_heroi herois[N_HEROIS]; //Vetor de heróis do mundo
  int NBases; //Armazena o numero de bases do mundo
  struct s_base bases[N_BASES]; //Vetor de bases do mundo
  int NMissoes; //Armazena o numero de missoes do mundo
  struct s_missao missoes[N_MISSOES]; //Vetor de missões do mundo
  int NHabilidades; //Armazena o numero de habilidades do mundo
  int tamanho_mundo; //Armazena o tamanho do mundo
  int eventos_tratados; //Armazena o número de eventos tratados durante a execução
  int missoes_cumpridas; //Armazena o número de missões cumpridas no mundo
};
//Struct que armazena as infos do evento
struct s_evento {
    int tempo;        // Quando o evento ocorre
    int heroi_id;     // Identificador do herói (Tambem usado como missao_id, para o evento E_MISSAO especificamente)
    int base_id;      // Identificador da base (se necessário)
    int tipo_evento;  // Tipo do evento (CHEGA, SAI, etc.)
};

// Retorna um número inteiro aleatório entre min e max
int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}
//Calcula a distancia entre os pontos (x1,y1) e (x2,y2) (distancia euclidiana). 
//Retorno é essa distancia em um int
int calcula_distancia(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
//Recebe os parametros do evento: tempo, tipo, heroi_id (ou missão_id), base_id e a lef
//insere o evento na lef
//Retorna 1 em caso de sucesso e 0 em falha
int cria_evento(int tempo, int tipo, int heroi_id, int base_id, struct fprio_t *lef) {
    // Aloca memória para o novo evento
    struct s_evento *novo;
    if (!(novo = malloc(sizeof(struct s_evento)))) {
        printf("Erro ao alocar memória para o novo evento.\n");
        return 0;
    }

    // Inicializa os campos do evento
    novo->tempo = tempo;
    novo->tipo_evento = tipo;
    novo->heroi_id = heroi_id;
    novo->base_id = base_id;

    // Insere o evento na fila de prioridade
    if (!(fprio_insere(lef, novo, tipo, tempo))) {
        printf("Erro ao inserir evento na fila de prioridade.\n"); // Libera a memória se a inserção falhar
        return 0;
    }
    return 1;
}
/*Função que inicializa o mundo e suas stats
Retorna o ponteiro para o mundo inicializado
e NULL em caso de falha*/
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
  mundo->eventos_tratados = 0;
  mundo->missoes_cumpridas = 0;
  return mundo;
};
/*Inicializa um herói e suas stats
Retorna -1 em caso de falha e 1 em caso de sucesso*/
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
  heroi->base = VOID;

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
/*Cria o número de heróis existentes no início do mundo
Retorna -1 em caso de falha e 1 em caso de sucesso*/
int inicializar_herois(struct s_mundo *mundo, int *id, struct fprio_t *lef) {
  for (int i = 0; i < N_HEROIS; i++) {
    if (inicializa_heroi(&mundo->herois[i], id) == -1) {
      return -1;
    }
    int base_aleatoria = aleat(0, N_BASES - 1);       // Base aleatória entre 0 e (N_BASES - 1)
    int tempo = aleat(0, 4320);       // Tempo aleatório entre 0 e 4320 minutos

        // Cria e insere o evento CHEGA na LEF
        cria_evento(tempo, E_CHEGA, i, base_aleatoria, lef);
    (*id)++;
  }
  return 1;
}
/*Inicializa uma base e suas stats
Retorna -1 em caso de falha e 1 em caso de sucesso*/
int inicializa_base(struct s_base *base, int *id) {
  base->id = *id;
  base->lotacao = aleat(3, 10);
  base->local.x = aleat(0, N_TAMANHO_MUNDO - 1);
  base->local.y = aleat(0, N_TAMANHO_MUNDO - 1);
  base->contador_missoes = 0;
  base->fila_max = 0;

  if (!(base->presentes = cjto_cria(N_HEROIS))) {
    printf("Erro ao criar conjunto de presentes da base\n");
    cjto_destroi(base->presentes);
    return -1;
  }

  base->espera = lista_cria();
  if (!base->espera) {
    printf("Erro ao criar lista de espera da base\n");
    lista_destroi(base->espera);  // Libera memória alocada antes
    return -1;
  }

  base->habilidades = cjto_cria(N_HABILIDADES);
  if(!base->habilidades){
    printf("Erro ao criar conjunto de habilidades da base");
    cjto_destroi(base->habilidades);
    return -1;
  }
  return 1;
}
/*Cria o número de bases existentes no início do mundo
Retorna -1 em caso de falha e 1 em caso de sucesso*/
int inicializar_bases(struct s_mundo *mundo, int *id) {
  for (int i = 0; i < N_BASES; i++) {
    if (inicializa_base(&mundo->bases[i], id) == -1) {
      return -1;
    }
    (*id)++;
  }
  return 1;
}
/*Inicializa uma missão e suas stats
Retorna -1 em caso de falha e 1 em caso de sucesso*/
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
/*Cria o número de missões existentes no início do mundo
Retorna -1 em caso de falha e 1 em caso de sucesso*/
int inicializar_missoes(struct s_mundo *mundo, int *id, struct fprio_t *lef) {
  for (int i = 0; i < N_MISSOES; i++) {
    if (inicializa_missao(&(mundo->missoes[i]), id) == -1) {
      return -1;
    }
    int tempo = aleat(0, T_FIM_DO_MUNDO);
    cria_evento(tempo, E_MISSAO, i, -1, lef);
    (*id)++;
  }
  return 1;
}
/*Atualiza o conjunto união de habilidades dos presentes na base*/
void unir_habilidades(struct s_mundo* mundo, struct s_base* b) {
    if (mundo == NULL || b == NULL) {
        return;
    }
    // Criar um novo conjunto para armazenar a união de habilidades
    struct cjto_t* habilidades_unidas = cjto_cria(N_HABILIDADES);
    if (habilidades_unidas == NULL) { //Checa se o cjto foi criado com sucesso
        return;
    }
    // Itera sobre os IDs dos heróis presentes na base
    for (int i = 0; i < N_HEROIS - 1; i++) {
        int heroi_id = i;
        if(cjto_pertence(b->presentes, i)){
        // Garantir que o ID do herói é válido
          if (heroi_id >= 0 && heroi_id < N_HEROIS) {
            // Realiza a união com as habilidades do herói
            struct cjto_t* habilidades_heroi = mundo->herois[heroi_id].habilidades;
            struct cjto_t* nova_uniao = cjto_uniao(habilidades_unidas, habilidades_heroi);

            // Liberar o conjunto antigo e atualizar para a nova união
            cjto_destroi(habilidades_unidas);
            habilidades_unidas = nova_uniao;
        }
    }
}
    // Destruir o conjunto de habilidades anterior da base (se existir)
    if (b->habilidades != NULL) {
        cjto_destroi(b->habilidades);
    }
    // Atribuir o novo conjunto à base
    b->habilidades = habilidades_unidas;
}
/*Evento de chegada*/
int chega(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef){
  //checa se o heroi está vivo
  if(mundo->herois[heroi].status == MORTO){ 
    return -1;
  };
  //Atualiza a base do heroi
  mundo->herois[heroi].base = base;
  //checa se tem vaga na base, e se a fila está vazia
  if((cjto_card(mundo->bases[base].presentes) < mundo->bases[base].lotacao) && (lista_tamanho(mundo->bases[base].espera) == 0)){
    cria_evento(tempo, E_ESPERA, heroi, base, lef); //Insere o evento de espera na lef
    return E_ESPERA;
  }
  //checa se a paciencia do herói é suficiente para aguardar na fila
  if(mundo->herois[heroi].paciencia > (10 * lista_tamanho(mundo->bases[base].espera))){
    cria_evento(tempo, E_ESPERA, heroi, base, lef); //insere o evento de espera na lef
    return E_ESPERA;
  }
  //caso a paciencia nao seja suficiente, adiciona desiste a lef
  cria_evento(tempo, E_DESISTE, heroi, base, lef);
  return E_DESISTE;
}
/*Evento de espera*/
int espera(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    // Adiciona o herói ao fim da fila de espera da base
    lista_insere(mundo->bases[base].espera, heroi, (-1));
    // Cria e insere o evento AVISA na LEF
    cria_evento(tempo, E_AVISA, heroi, base, lef);
    return E_AVISA;
}
/*Evento de desistencia*/
int desiste(int tempo, int heroi, struct fprio_t *lef) {
    // Escolhe uma base destino aleatória
    int nova_base = aleat(0, N_BASES - 1);

    // Cria e insere o evento VIAJA na LEF
    cria_evento(tempo, E_VIAJA, heroi, nova_base, lef);
    return E_VIAJA;
}
/*Evento de aviso para o porteiro*/
void avisa(struct s_mundo *mundo, int tempo, int base, struct fprio_t *lef){
    int heroi_na_vez; //armazena qual o primeiro heroi da fila
    struct s_base *b = &mundo->bases[base];
    //Guarda o recorde no tamanho da fila
    if(lista_tamanho(b->espera) > b->fila_max){
        b->fila_max = lista_tamanho(b->espera);
    };
    //Admite heróis enquando houverem vagas na base
    while (cjto_card(b->presentes) < b->lotacao && lista_tamanho(b->espera))
    {
        lista_retira(b->espera, &heroi_na_vez, 0);
        cjto_insere(b->presentes, heroi_na_vez);
        printf("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", tempo, base, heroi_na_vez);
        cria_evento(tempo, E_ENTRA, heroi_na_vez, base, lef); //Adiciona o evento de entrada na lef
    }
}
/*Evento de entrada na base*/
int entra(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    int TPB; //Armazena o tempo de permanencia na base

    //Atualiza o conjunto união de habilidades da base
    unir_habilidades(mundo, &mundo->bases[base]);
    
    if(mundo->herois[heroi].status == MORTO){//checa se o herói está vivo
    return -1;
  };
    //Calcula o tempo de permanencia na base a adiciona o evento SAI na lef
    TPB = 15 + mundo->herois[heroi].paciencia * aleat(1, 20);
    cria_evento((tempo + TPB), E_SAI, heroi, base, lef); //Insere o evento de Saida na lef
    return tempo + TPB;
}
/*Evento de saída na base*/
int sai(struct s_mundo *mundo, int tempo, int heroi, int base, struct fprio_t *lef) {
    if(mundo->herois[heroi].status == MORTO){ //checa se o heroi está vivo
    return -1;
  };
    // Remove o herói do conjunto de presentes na base
    cjto_retira(mundo->bases[base].presentes, heroi);
    //Atualiza o conjunto união de habilidades da base
    unir_habilidades(mundo, &mundo->bases[base]);

    int destino; //Armazena a base destino do heroi
    do {
        destino = rand() % mundo->NBases; // Escolhe uma base destino aleatória diferente da base atual
    } while (destino == base);  // Garante que a base destino é diferente da atual
    // Cria e insere o evento VIAJA na LEF
    cria_evento(tempo, E_VIAJA, heroi, destino, lef);
    // Cria e insere o evento AVISA para notificar o porteiro sobre a vaga liberada
    cria_evento(tempo, E_AVISA, heroi, base, lef);
    return E_VIAJA;
}
/*Evento de viagem entre bases*/
int viaja(struct s_mundo *mundo, int tempo, int heroi, int destino, struct fprio_t *lef) {
  int b_atual = mundo->herois[heroi].base;
    // Calcula a distância entre a base atual e a base de destino usando as coordenas x e y;
    int distancia = calcula_distancia(mundo->bases[b_atual].local.x, mundo->bases[b_atual].local.y, mundo->bases[destino].local.x, mundo->bases[destino].local.y);
    // Calcula a duração da viagem com base na velocidade do herói
    int duracao = distancia / mundo->herois[heroi].velocidade;
    // Cria e insere o evento CHEGA na LEF
    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", tempo, heroi, b_atual, destino, distancia, mundo->herois[heroi].velocidade, (tempo + duracao));
    cria_evento((tempo+duracao), E_CHEGA, heroi, destino, lef);
    return E_CHEGA;
}
/*Evento de morte do herói*/
int morre(struct s_mundo *mundo, int tempo, int heroi, int missao_id, struct fprio_t *lef) {
    // Remove o herói do conjunto de presentes na base
    cjto_retira(mundo->bases[mundo->herois[heroi].base].presentes, heroi);
    //Atualiza o conjunto união de habilidades da base
    unir_habilidades(mundo, &mundo->bases[mundo->herois[heroi].base]);
    // Marca o herói como morto
    mundo->herois[heroi].status = MORTO;  // Assume que existe um campo 'status' e uma constante MORTO
    // Cria e insere o evento AVISA na LEF para notificar o porteiro
    printf("%6d: MORRE  HEROI %2d MISSAO %d\n", tempo, heroi, missao_id);
    cria_evento(tempo, E_AVISA, (-1), mundo->herois[heroi].base, lef);
    mundo->herois[heroi].base = CEMITERIO; //Define a base do heroi como cemitério;
    return E_AVISA;
}
/*Retorna um booleano se a base está apta para a missão*/
bool base_esta_apta(struct s_base *b, struct s_missao *m){
  if(cjto_contem(b->habilidades, m->habilidades)){ //Checa se o conjunto de habilidades da base contem todas as habilidades necessárias para a missão
    return true;
  }
  return false;
}
/*Evento de missão*/
int missao(struct s_mundo *mundo, int tempo, int missao_id, struct fprio_t *lef) {

    int bmp = VOID; //Variavel para armazenar o ID da base apta mais próxima (Inicia como uma base inválida)
    int menor_distancia = ((N_TAMANHO_MUNDO + 1) * sqrt(2)); //Define a menor_distancia como uma distancia não possivel de atingir (lado + 1)*sqrt(2)
    mundo->missoes[missao_id].tentativas++; //Incrementa o numero de tentativas da missao

    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", tempo, missao_id, mundo->missoes[missao_id].tentativas);
    cjto_imprime(mundo->missoes[missao_id].habilidades);
    printf(" ]\n");

    // Calcula a distância de cada base até o local da missão e verifica se está apta
    for (int i = 0; i < mundo->NBases; i++) {
        if (base_esta_apta(&mundo->bases[i], &mundo->missoes[missao_id])) {
            int distancia = calcula_distancia(mundo->bases[i].local.x, mundo->bases[i].local.y, mundo->missoes[missao_id].local.x, mundo->missoes[missao_id].local.y);
            if (distancia < menor_distancia) {
                menor_distancia = distancia;
                bmp = i; //Define bmp como o ID da base mais próxima apta
            }
        }
    }

    // Se há uma base apta (BMP)
    if (bmp != VOID) {
        mundo->missoes[missao_id].status = CUMPRIDA; //Define a missao como CUMPRIDA
        mundo->missoes_cumpridas++; //Incrementa quantas missões foram cumpridas no mundo
        mundo->bases[bmp].contador_missoes++; //Incrementa o contador de missões concluidas pela base
        printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", tempo, missao_id, bmp);
        cjto_imprime(mundo->bases[bmp].habilidades);
        printf(" ]\n");

        //Checa se algum herói morre durante a missão
        for (int i = 0; i < N_HEROIS; i++) {
            struct s_heroi *h = &mundo->herois[i];
          if(h->base == bmp){ //Calcula a chance de morte
            int risco = mundo->missoes[missao_id].n_perigo / (h->paciencia + h->experiencia + 1.0);
            if (risco > aleat(0, 30)) { //Caso o risco seja maior que o numero sorteado, o herói morre
                cria_evento(tempo, E_MORRE, h->id, missao_id, lef); //Insere o evento de morte na lef
                return E_MORRE;
            } else {
                h->experiencia++; //Incrementa a experiencia do heroi
            }
        }
      }
        return E_MISSAO;
    }
    printf("%6d: MISSAO %d IMPOSSIVEL\n", tempo, missao_id);
    // Caso não haja uma base apta, adia a missão para o próximo dia
    cria_evento(tempo + (24 * 60), E_MISSAO, missao_id, (-1), lef);
    return E_MISSAO;
}
//Itera sobre as missões e acha a com menor numero de tentativas
int procura_min_tentativas(struct s_mundo *mundo){
  int min = mundo->missoes[0].tentativas;
  for (int i = 1; i < N_MISSOES; i++){ 
    if(mundo->missoes[i].tentativas < min){
      min = mundo->missoes[i].tentativas;
    }
  }
  return min;
}
//Itera sobre as missões e acha a com maior numero de tentativas
int procura_max_tentativas(struct s_mundo *mundo){
  int max = mundo->missoes[0].tentativas;
  for (int i = 1; i < N_MISSOES; i++){ 
    if(mundo->missoes[i].tentativas > max){
      max = mundo->missoes[i].tentativas;
    }
  }
  return max;
}
/*Calcula a média de tentativas*/
float media_tentativas(struct  s_mundo *mundo){
  int soma = 0;
  for (int i = 0; i < N_MISSOES; i++){
    soma = soma + mundo->missoes[i].tentativas;
  }
  return ((float)soma/N_MISSOES);
};
/*Evento de fim do mundo*/
int fim(struct s_mundo *mundo, int tempo) {

    printf("%6d: FIM\n\n", tempo);
    //Exibe as informações dos heróis
    printf("HEROIS:\n");
    for (int i = 0; i < N_HEROIS; i++) {
        struct s_heroi *heroi = &mundo->herois[i];
        char status[6];

      if (heroi->status == MORTO) {
        strcpy(status, "MORTO"); // Usa strcpy para copiar o texto para o array
      } else {
        strcpy(status, "VIVO");
      }

        printf(
            "HEROI %2d %s PAC %3d VEL %4d EXP %4d HABS [ ",
            heroi->id,
            status,
            heroi->paciencia,
            heroi->velocidade,
            heroi->experiencia
        );
        // Imprime as habilidades do herói
        cjto_imprime(mundo->herois[i].habilidades);
        printf(" ]\n");
    }

    //Exibe as informações das bases
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
        printf("Presentes: [");
        cjto_imprime(base->presentes);
        printf("]\n");
    }

    //Calcula e exibe as estatísticas gerais
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

    //Libera os recursos alocados
    for (int i = 0; i < N_BASES; i++) {
        cjto_destroi(mundo->bases[i].presentes);
        lista_destroi(mundo->bases[i].espera);
        cjto_destroi(mundo->bases[i].habilidades);
    }
    for (int i = 0; i < N_HEROIS; i++) {
        cjto_destroi(mundo->herois[i].habilidades);
    }
    for (int i = 0; i < N_MISSOES; i++) {
        cjto_destroi(mundo->missoes[i].habilidades);
    }

    // 5. Encerra a simulação
    printf("\nSimulação encerrada.\n");
    return -1;  // Retorna o código de encerramento
}
/*Função para executar a simulação*/
void executar_simulacao(struct s_mundo *mundo, struct fprio_t *lef) {
    int tempo, tipo; //Variaveis para armazenar o tempo e o tipo do evento atual
    while (1){
        struct s_evento * evento_atual;
        //Conversão de um ponteiro genérico void* para um ponteiro do tipo s_evento
        evento_atual = (struct s_evento *) lef->prim->item;
        fprio_retira(lef, &tipo, &tempo); //Retira o próximo evento da lef
        mundo->eventos_tratados += 1; //Incrementa o numero de eventos tratados no mundo
        // Processa o evento com base no tipo
        switch (tipo) {
            case E_CHEGA:
                int retorno = chega(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef); //Recebe se o herói espera ou desiste de entrar na base
                if(retorno == E_ESPERA){
                  printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ESPERA\n",tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                }
                if(retorno == E_DESISTE){
                  printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) DESISTE\n",tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                }
                break;
            case E_ESPERA:
                espera(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", tempo, evento_atual->heroi_id, evento_atual->base_id, lista_tamanho(mundo->bases[evento_atual->base_id].espera));
                break;
            case E_ENTRA:
                int t_saida = entra(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao, t_saida);
                break;
            case E_DESISTE:
                desiste(tempo, evento_atual->heroi_id, lef);
                printf("%6d: DESIST HEROI %2d BASE %d\n",tempo, evento_atual->heroi_id, evento_atual->base_id);
                break;
            case E_AVISA:
                printf("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA [ ", tempo, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                lista_imprime(mundo->bases[evento_atual->base_id].espera);
                printf(" ]\n");
                avisa(mundo, tempo, evento_atual->base_id, lef);
                break;
            case E_MISSAO:
                missao(mundo, tempo, evento_atual->heroi_id, lef); 
                break;
            case E_SAI:
                printf("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", tempo, evento_atual->heroi_id, evento_atual->base_id, cjto_card(mundo->bases[evento_atual->base_id].presentes), mundo->bases[evento_atual->base_id].lotacao);
                sai(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                break;
            case E_MORRE:
                morre(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                break;
            case E_VIAJA:
                viaja(mundo, tempo, evento_atual->heroi_id, evento_atual->base_id, lef);
                break;
            case E_FIM:
                fim(mundo, tempo);
                free(evento_atual);
                return;
            default:
                printf("Evento desconhecido: %d\n", evento_atual->tipo_evento);
        }
        free(evento_atual);
    }
}
// programa principal
int main ()
{
int ultimo_id_herois = 0; //Armazena o ultimo ID de herói criado (Permite criar novos heróis durante a execução)
int ultimo_id_bases = 0;//Armazena o ultimo ID de base criado (Permite criar novas bases durante a execução)
int ultimo_id_missoes = 0;//Armazena o ultimo ID de missão criado (Permite criar novas missões durante a execução)

struct s_mundo *mundo = inicializa_mundo(); //Inicializa o mundo
struct fprio_t *lef = fprio_cria(); //Cria a lef

srand(time(NULL));

inicializar_bases(mundo, &ultimo_id_bases); //Inicializa as bases existentes no começo do mundo
inicializar_herois(mundo, &ultimo_id_herois, lef); //Inicializa os heróis existentes no começo do mundo
inicializar_missoes(mundo, &ultimo_id_missoes, lef); //Inicializa as missões existentes no começo do mundo


cria_evento((T_FIM_DO_MUNDO), (E_FIM), 0, 0, lef); //Insere o evento de fim do mundo na lef

executar_simulacao(mundo, lef); //Chama a função de execução do mundo
fprio_destroi(lef); //Destroi e libera a memoria alocada para a lef
free(mundo); //Libera a memória alocada para o mundo

  return (0) ;
}