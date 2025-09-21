#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct do território
struct Territorio {
    char nome[30];
    char cor[10];
    int tropas;
};

// --- Protótipos das funções ---
void cadastrarTerritorios(struct Territorio* mapa, int quantidade);
void exibirTerritorios(struct Territorio* mapa, int quantidade);
void atacar(struct Territorio* atacante, struct Territorio* defensor);
void atribuirMissao(char** destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, struct Territorio* mapa, int tamanho, const char* cor_jogador);
void liberarMemoria(struct Territorio* mapa, char* missao_jogador);

int main() {
    int total_territorios;
    struct Territorio* mapa = NULL;
    char* missao_do_jogador = NULL; // Ponteiro para a string da missão
    char cor_jogador[] = "Azul"; // Cor do jogador principal

    // --- Vetor de missões ---
    char* missoes[] = {
        "Conquistar 3 territorios.",
        "Eliminar todas as tropas da cor Vermelha.",
        "Ter um total de 10 tropas somando todos os seus territorios."
    };
    int total_missoes = sizeof(missoes) / sizeof(missoes[0]);
    
    srand(time(NULL));

    printf("--- JOGO WAR ESTRUTURADO: NIVEL MESTRE ---\n");
    printf("Quantos territorios deseja criar no mapa? ");
    scanf("%d", &total_territorios);

    mapa = (struct Territorio*) malloc(total_territorios * sizeof(struct Territorio));
    if (mapa == NULL) {
        printf("Erro de alocacao de memoria para o mapa.\n");
        return 1;
    }

    cadastrarTerritorios(mapa, total_territorios);

    // --- Atribuição da missão ---
    atribuirMissao(&missao_do_jogador, missoes, total_missoes);
    printf("\n============================================\n");
    printf("SUA MISSAO: %s\n", missao_do_jogador);
    printf("============================================\n");
    
    exibirTerritorios(mapa, total_territorios);

    // --- Loop principal do jogo ---
    int jogo_ativo = 1;
    while (jogo_ativo) {
        int idx_atacante, idx_defensor;

        printf("\n--- FASE DE ATAQUE ---\n");
        printf("Digite o indice do territorio atacante (ou -1 para sair): ");
        scanf("%d", &idx_atacante);

        if (idx_atacante == -1) {
            jogo_ativo = 0;
            continue;
        }

        printf("Digite o indice do territorio defensor: ");
        scanf("%d", &idx_defensor);

        if (idx_atacante < 0 || idx_atacante >= total_territorios ||
            idx_defensor < 0 || idx_defensor >= total_territorios) {
            printf("Indices invalidos!\n");
            continue;
        }

        if (strcmp((mapa + idx_atacante)->cor, (mapa + idx_defensor)->cor) == 0) {
            printf("Ataque invalido! Voce nao pode atacar um territorio aliado.\n");
            continue;
        }

        if (strcmp((mapa + idx_atacante)->cor, cor_jogador) != 0) {
            printf("Ataque invalido! Voce so pode atacar com exercitos da sua cor (%s).\n", cor_jogador);
            continue;
        }

        printf("\nIniciando ataque de %s contra %s!\n", (mapa + idx_atacante)->nome, (mapa + idx_defensor)->nome);
        atacar(mapa + idx_atacante, mapa + idx_defensor);
        exibirTerritorios(mapa, total_territorios);

        // --- Verificação da missão ao final do turno ---
        if (verificarMissao(missao_do_jogador, mapa, total_territorios, cor_jogador)) {
            printf("\n**************************************\n");
            printf("VITORIA! Voce cumpriu sua missao!\n");
            printf("**************************************\n");
            jogo_ativo = 0; // Termina o jogo
        }
    }
    
    liberarMemoria(mapa, missao_do_jogador);
    printf("\nJogo finalizado e memoria liberada.\n");

    return 0;
}


void cadastrarTerritorios(struct Territorio* mapa, int quantidade) {
    printf("\n--- CADASTRO DE TERRITORIOS ---\n");
    for (int i = 0; i < quantidade; i++) {
        printf("--- Territorio %d ---\n", i);
        printf("Nome: ");
        scanf("%s", (mapa + i)->nome);
        printf("Cor do exercito (Ex: Azul, Vermelho, Verde): ");
        scanf("%s", (mapa + i)->cor);
        printf("Tropas: ");
        scanf("%d", &(mapa + i)->tropas);
    }
}

void exibirTerritorios(struct Territorio* mapa, int quantidade) {
    printf("\n--- ESTADO ATUAL DO MAPA ---\n");
    for (int i = 0; i < quantidade; i++) {
        printf("Indice: %d | Territorio: %s | Cor: %s | Tropas: %d\n",
               i, (mapa + i)->nome, (mapa + i)->cor, (mapa + i)->tropas);
    }
    printf("-----------------------------\n");
}


void atacar(struct Territorio* atacante, struct Territorio* defensor) {
    int dado_ataque = (rand() % 6) + 1;
    int dado_defesa = (rand() % 6) + 1;

    printf("Dado de Ataque: %d | Dado de Defesa: %d\n", dado_ataque, dado_defesa);

    if (dado_ataque > dado_defesa) {
        printf("VITORIA DO ATAQUE! O territorio %s foi conquistado.\n", defensor->nome);
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = atacante->tropas / 2;
        atacante->tropas = atacante->tropas - (atacante->tropas / 2);
    } else {
        printf("VITORIA DA DEFESA! O territorio %s resistiu ao ataque.\n", defensor->nome);
        if (atacante->tropas > 1) {
            atacante->tropas--;
        }
    }
}

/**
 * @brief Sorteia uma missão e aloca memória para armazená-la
 * @param destino Ponteiro para o ponteiro da string da missão do jogador
 * @param missoes Vetor de strings com todas as missões disponíveis
 * @param totalMissoes Número de missões no vetor
 */
void atribuirMissao(char** destino, char* missoes[], int totalMissoes) {
    int indice_sorteado = rand() % totalMissoes;
    
    *destino = (char*) malloc(strlen(missoes[indice_sorteado]) + 1);
    
    if (*destino == NULL) {
        printf("Erro ao alocar memoria para a missao!\n");
        exit(1);
    }
    
    strcpy(*destino, missoes[indice_sorteado]);
}

/**
 * @brief Verifica se a condição de vitória da missão foi atingida
 * @param missao A string da missão do jogador
 * @param mapa Ponteiro para o vetor de territórios
 * @param tamanho Número total de territórios
 * @param cor_jogador A cor do jogador para verificação
 * @return 1 se a missão foi cumprida, 0 caso contrário
 */
int verificarMissao(char* missao, struct Territorio* mapa, int tamanho, const char* cor_jogador) {
    if (strcmp(missao, "Conquistar 3 territorios.") == 0) {
        int contador = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp((mapa + i)->cor, cor_jogador) == 0) {
                contador++;
            }
        }
        return contador >= 3;
    }
    
    if (strcmp(missao, "Eliminar todas as tropas da cor Vermelha.") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp((mapa + i)->cor, "Vermelha") == 0) {
                return 0;
            }
        }
        return 1;
    }

    if (strcmp(missao, "Ter um total de 10 tropas somando todos os seus territorios.") == 0) {
        int soma_tropas = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp((mapa + i)->cor, cor_jogador) == 0) {
                soma_tropas += (mapa + i)->tropas;
            }
        }
        return soma_tropas >= 10;
    }
    
    return 0; // Missão desconhecida
}

/**
 * @brief Libera toda a memória alocada dinamicamente
 * @param mapa Ponteiro para o mapa
 * @param missao_jogador Ponteiro para a missão do jogador
 */
void liberarMemoria(struct Territorio* mapa, char* missao_jogador) {
    free(mapa);
    free(missao_jogador); // Libera também a memória da string da missão
}