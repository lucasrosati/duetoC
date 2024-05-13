#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

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

typedef struct Jogador {
    char nome[50];
    int tentativas;
    int acertos;
    struct Jogador* prox;
} Jogador;

Jogador* ranking = NULL;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
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
void verificarEImprimirPalavras(char* input, char* secreta1, char* secreta2, bool* acertou1, bool* acertou2);

int main() {
    srand(time(NULL));
    carregarRanking();
    menu();
    salvarRanking();
    liberarRanking();
    return 0;
}

void menu() {
    int opcao;
    do {
        printf("\nMenu:\n");
        printf("1. Jogar\n");
        printf("2. Ver Ranking\n");
        printf("3. Resetar Ranking\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

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
    char nomeJogador[50];
    printf("Digite seu nome: ");
    fgets(nomeJogador, sizeof(nomeJogador), stdin);
    nomeJogador[strcspn(nomeJogador, "\n")] = '\0';

    char palavraSecreta1[TAM_PALAVRA], palavraSecreta2[TAM_PALAVRA];
    strcpy(palavraSecreta1, escolherPalavraSecreta(NULL));
    strcpy(palavraSecreta2, escolherPalavraSecreta(palavraSecreta1));

    char tentativa[TAM_PALAVRA];
    int numTentativas = 0;
    bool acertou1 = false, acertou2 = false;

    while (numTentativas < MAX_TENTATIVAS && (!acertou1 || !acertou2)) {
        printf("\nDigite uma palavra de 5 letras: ");
        fgets(tentativa, sizeof(tentativa), stdin);
        tentativa[strcspn(tentativa, "\n")] = '\0';

        if (strlen(tentativa) != 5) {
            printf("Erro: Você deve inserir exatamente 5 letras. Tente novamente.\n");
            continue;
        }

        verificarEImprimirPalavras(tentativa, palavraSecreta1, palavraSecreta2, &acertou1, &acertou2);
        numTentativas++;
    }

    printf("\nAs palavras eram \"%s\" e \"%s\".\n", palavraSecreta1, palavraSecreta2);
    int acertos = (acertou1 ? 1 : 0) + (acertou2 ? 1 : 0);
    printf("\n%s: Você acertou %d palavras com %d tentativas.\n", nomeJogador, acertos, numTentativas);
    adicionarAoRanking(nomeJogador, numTentativas, acertos);
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
    strcpy(novo->nome, nome);
    novo->tentativas = tentativas;
    novo->acertos = acertos;
    novo->prox = NULL;

    Jogador **ptr = &ranking, *anterior = NULL;
    while (*ptr && ((*ptr)->acertos > acertos || ((*ptr)->acertos == acertos && (*ptr)->tentativas <= tentativas))) {
        anterior = *ptr;
        ptr = &(*ptr)->prox;
    }
    if (anterior) {
        novo->prox = anterior->prox;
        anterior->prox = novo;
    } else {
        novo->prox = ranking;
        ranking = novo;
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
    printf("Ranking:\n");
    Jogador *atual = ranking;
    while (atual) {
        printf("%s - Tentativas: %d, Acertos: %d\n", atual->nome, atual->tentativas, atual->acertos);
        atual = atual->prox;
    }
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
    } else {
        printf("Erro ao limpar o ranking.\n");
    }
}