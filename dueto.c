#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define NUM_PALAVRAS 101
#define TAM_PALAVRA 10
#define MAX_TENTATIVAS 6

char palavras[NUM_PALAVRAS][TAM_PALAVRA] = {
    "claro", "fases", "gesto", "jovem", "lugar", "mente", "noite", "papel", "reino", "senso",
    "texto", "unido", "vasto", "lenda", "doido", "carro", "sabor", "trato", "suave", "mover",
    "crepe", "risos", "gruta", "fusao", "salva", "feito", "gemer", "limpo", "macio", "curvo",
    "pomba", "quase", "tonal", "russo", "duras", "posse", "bravo", "esqui", "usina", "pilar",
    "baixo", "disco", "longo", "bocal", "moral", "altar", "breve", "troca", "sutil", "pobre",
    "densa", "costa", "pesca", "antes", "fruta", "livro", "bagre", "bruto", "ficha", "nuvem",
    "viver", "grama", "nevar", "floco", "forma", "gesso", "rezar", "jaula", "lotar", "nervo",
    "obeso", "patio", "quilo", "renda", "sorte", "terra", "ursos", "visto", "troco", "rapaz",
    "leite", "prova", "quota", "risco", "sinal", "temor", "trigo", "bolso", "cedro", "farto",
    "golpe", "haste", "imune", "janta", "luzir", "manha", "noiva", "haver", "pente", "dente",
    "sopro"
};

typedef struct Tentativa {
    char palavra[TAM_PALAVRA];
    struct Tentativa* prox;
} Tentativa;

typedef struct Jogador {
    char nome[50];
    int tentativas;
    int acertos;
    Tentativa* tentativasList;
    struct Jogador* prox;
} Jogador;

Jogador* ranking = NULL;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pausarParaContinuar() {
    printf("\nPressione Enter para continuar...");
    while (getchar() != '\n');
}

void jogar();
void adicionarAoRanking(char* nome, int tentativas, int acertos);
void exibirRanking();
void liberarRanking();
void menu();
void carregarRanking();
void salvarRanking();
void resetarRanking();
char* escolherPalavraSecreta(const char* exclude);
void verificarTentativas(Tentativa* head, char* secreta1, char* secreta2, bool* acertou1, bool* acertou2);
void verificarEImprimirPalavras(char* input, char* secreta1, char* secreta2, bool* acertou1, bool* acertou2);
void converterParaMinusculas(char* str);
void adicionarTentativa(Tentativa** head, char* palavra);
void liberarTentativas(Tentativa* head);

int main() {
    srand(time(NULL));
    carregarRanking();
    menu();
    salvarRanking();
    liberarRanking();
    return 0;
}

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void menu() {
    int opcao;
    do {
        limparTela();
        printf("\nMenu:\n");
        printf("1. Jogar\n");
        printf("2. Ver Ranking\n");
        printf("3. Resetar Ranking\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                jogar();
                break;
            case 2:
                exibirRanking();
                break;
            case 3:
                resetarRanking();
                break;
            case 4:
                printf("Obrigado por jogar!\n");
                break;
            default:
                printf("Opção inválida, tente novamente.\n");
        }
    } while (opcao != 4);
}

void jogar() {
    limparTela();

    char nomeJogador[50];
    printf("Digite seu nome: ");
    fgets(nomeJogador, sizeof(nomeJogador), stdin);
    nomeJogador[strcspn(nomeJogador, "\n")] = '\0';

    char palavraSecreta1[TAM_PALAVRA], palavraSecreta2[TAM_PALAVRA];
    strcpy(palavraSecreta1, escolherPalavraSecreta(NULL));
    strcpy(palavraSecreta2, escolherPalavraSecreta(palavraSecreta1));

    Jogador jogador;
    strcpy(jogador.nome, nomeJogador);
    jogador.tentativas = 0;
    jogador.acertos = 0;
    jogador.tentativasList = NULL;

    char tentativa[TAM_PALAVRA];
    bool acertou1 = false, acertou2 = false;

    while (jogador.tentativas < MAX_TENTATIVAS && (!acertou1 || !acertou2)) {
        printf("\nDigite uma palavra de 5 letras: ");
        fgets(tentativa, sizeof(tentativa), stdin);
        tentativa[strcspn(tentativa, "\n")] = '\0';

        if (strlen(tentativa) != 5) {
            printf("Erro: Você deve inserir exatamente 5 letras. Tente novamente.\n");
            continue;
        }

        converterParaMinusculas(tentativa);
        adicionarTentativa(&jogador.tentativasList, tentativa);

        limparTela();
        verificarTentativas(jogador.tentativasList, palavraSecreta1, palavraSecreta2, &acertou1, &acertou2);
        jogador.tentativas++;
    }

    printf("\nAs palavras eram \"%s\" e \"%s\".\n", palavraSecreta1, palavraSecreta2);
    jogador.acertos = (acertou1 ? 1 : 0) + (acertou2 ? 1 : 0);
    printf("\n%s: Você acertou %d palavras com %d tentativas.\n", jogador.nome, jogador.acertos, jogador.tentativas);
    adicionarAoRanking(jogador.nome, jogador.tentativas, jogador.acertos);

    liberarTentativas(jogador.tentativasList);

    pausarParaContinuar();
}

char* escolherPalavraSecreta(const char* exclude) {
    static char escolhida[TAM_PALAVRA];
    int idx;
    do {
        idx = rand() % NUM_PALAVRAS;
    } while (exclude && strcmp(palavras[idx], exclude) == 0);
    strcpy(escolhida, palavras[idx]);
    return escolhida;
}

void adicionarTentativa(Tentativa** head, char* palavra) {
    Tentativa* novaTentativa = (Tentativa*)malloc(sizeof(Tentativa));
    if (novaTentativa == NULL) {
        printf("Erro ao alocar memória para nova tentativa.\n");
        exit(1);
    }
    strcpy(novaTentativa->palavra, palavra);
    novaTentativa->prox = *head;
    *head = novaTentativa;
}

void verificarTentativas(Tentativa* head, char* secreta1, char* secreta2, bool* acertou1, bool* acertou2) {
    if (head == NULL) {
        return;
    }

    verificarTentativas(head->prox, secreta1, secreta2, acertou1, acertou2);
    verificarEImprimirPalavras(head->palavra, secreta1, secreta2, acertou1, acertou2);
}

void verificarEImprimirPalavras(char* input, char* secreta1, char* secreta2, bool* acertou1, bool* acertou2) {
    if (strcmp(input, secreta1) == 0) {
        *acertou1 = true;
    }
    if (strcmp(input, secreta2) == 0) {
        *acertou2 = true;
    }

    if (!*acertou1) {
        for (size_t i = 0; i < strlen(input); i++) {
            if (input[i] == secreta1[i]) {
                printf("\x1b[32m%c\x1b[0m", input[i]);
            } else if (strchr(secreta1, input[i])) {
                printf("\x1b[33m%c\x1b[0m", input[i]);
            } else {
                printf("\x1b[31m%c\x1b[0m", input[i]);
            }
        }
        printf(" ");
    } else {
        printf("\x1b[32m%s\x1b[0m ", secreta1);
    }

    if (!*acertou2) {
        for (size_t i = 0; i < strlen(input); i++) {
            if (input[i] == secreta2[i]) {
                printf("\x1b[32m%c\x1b[0m", input[i]);
            } else if (strchr(secreta2, input[i])) {
                printf("\x1b[33m%c\x1b[0m", input[i]);
            } else {
                printf("\x1b[31m%c\x1b[0m", input[i]);
            }
        }
    } else {
        printf("\x1b[32m%s\x1b[0m", secreta2);
    }
    printf("\n");
}

void adicionarAoRanking(char* nome, int tentativas, int acertos) {
    Jogador* novo = (Jogador*)malloc(sizeof(Jogador));
    if (novo == NULL) {
        printf("Erro ao alocar memória para novo jogador.\n");
        exit(1);
    }
    strcpy(novo->nome, nome);
    novo->tentativas = tentativas;
    novo->acertos = acertos;
    novo->tentativasList = NULL;
    novo->prox = NULL;

    if (ranking == NULL || (ranking->acertos < acertos) || 
        (ranking->acertos == acertos && ranking->tentativas > tentativas)) {
        novo->prox = ranking;
        ranking = novo;
    } else {
        Jogador* atual = ranking;
        while (atual->prox != NULL && 
              ((atual->prox->acertos > acertos) || 
              (atual->prox->acertos == acertos && atual->prox->tentativas <= tentativas))) {
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
    }
}

void carregarRanking() {
    FILE *file = fopen("ranking.txt", "r");
    if (!file) return;

    char nome[50];
    int tentativas, acertos;
    while (fscanf(file, "%s %d %d", nome, &tentativas, &acertos) == 3) {
        adicionarAoRanking(nome, tentativas, acertos);
    }
    fclose(file);
}

void salvarRanking() {
    FILE *file = fopen("ranking.txt", "w");
    if (!file) {
        printf("Erro ao salvar o ranking.\n");
        return;
    }

    Jogador *atual = ranking;
    while (atual) {
        fprintf(file, "%s %d %d\n", atual->nome, atual->tentativas, atual->acertos);
        atual = atual->prox;
    }
    fclose(file);
}

void exibirRanking() {
    limparTela();

    printf("Ranking:\n");
    Jogador *atual = ranking;
    while (atual) {
        printf("%s - Tentativas: %d, Acertos: %d\n", atual->nome, atual->tentativas, atual->acertos);
        atual = atual->prox;
    }

    pausarParaContinuar();
}

void liberarRanking() {
    Jogador *atual = ranking;
    while (atual != NULL) {
        Jogador *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    ranking = NULL;
}

void resetarRanking() {
    liberarRanking();
    FILE *file = fopen("ranking.txt", "w");
    if (file) {
        fclose(file);
        printf("Ranking limpo com sucesso.\n");
        pausarParaContinuar(); 
    } else {
        printf("Erro ao limpar o ranking.\n");
        pausarParaContinuar(); 
    }
}

void converterParaMinusculas(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void liberarTentativas(Tentativa* head) {
    Tentativa* atual = head;
    while (atual != NULL) {
        Tentativa* temp = atual;
        atual = atual->prox;
        free(temp);
    }
}
