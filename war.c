/*
 * WAR - O jogo em C
 * Sistema de jogo War com cadastro dinâmico de territórios e jogadores,
 * mecânica de combate baseada em dados e sistema de missões para vitória.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura que representa cada território do mapa
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Estrutura que armazena os dados de cada jogador
typedef struct {
    char nome[30];
    char cor[10];
    char* missao;  // alocada dinamicamente para cada jogador
} Jogador;

// Protótipos das funções
void cadastrarTerritorios(Territorio* mapa, int quantidade);
void cadastrarJogadores(Jogador* jogadores, int quantidade, char* missoes[], int totalMissoes);
void exibirTerritorios(Territorio* mapa, int quantidade);
void exibirJogadores(Jogador* jogadores, int quantidade);
void executarAtaque(Territorio* atacante, Territorio* defensor);
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, char* corJogador);
void exibirMissao(char* missao, char* nomeJogador);
int verificarFimDeJogo(Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int numJogadores);

int main() {
    int numTerritorios, numJogadores;
    Territorio* mapa = NULL;
    Jogador* jogadores = NULL;
    int opcao;
    int jogoAtivo = 1;
    
    // Vetor com as missões disponíveis no jogo
    char* missoes[] = {
        "Conquistar 3 territorios consecutivos",
        "Eliminar todas as tropas de uma cor especifica",
        "Controlar 5 territorios simultaneamente",
        "Acumular 50 tropas no total em todos os seus territorios",
        "Conquistar todos os territorios de uma cor inimiga"
    };
    int totalMissoes = 5;
    
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));
    
    printf("=== WAR: O JOGO ===\n\n");
    
    // Solicita quantidade de territórios
    printf("Quantos territorios deseja cadastrar? ");
    scanf("%d", &numTerritorios);
    
    // Solicita a quantidade de jogadores que irá participar
    printf("Quantos jogadores vao participar? ");
    scanf("%d", &numJogadores);
    
    // Aloca memória para os territórios
    mapa = (Territorio*) calloc(numTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria para territorios!\n");
        return 1;
    }
    
    // Aloca memória para os jogadores
    jogadores = (Jogador*) calloc(numJogadores, sizeof(Jogador));
    if (jogadores == NULL) {
        printf("Erro ao alocar memoria para jogadores!\n");
        free(mapa);
        return 1;
    }
    
    // Cadastro dos territórios
    cadastrarTerritorios(mapa, numTerritorios);
    
    // Cadastro dos jogadores e atribui missões
    cadastrarJogadores(jogadores, numJogadores, missoes, totalMissoes);
    
    // Exibe as missões atribuídas a cada jogador
    printf("\n=== MISSOES DOS JOGADORES ===\n\n");
    for (int i = 0; i < numJogadores; i++) {
        exibirMissao(jogadores[i].missao, jogadores[i].nome);
    }
    
    // Loop principal do jogo
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Exibir territorios\n");
        printf("2. Exibir jogadores e missoes\n");
        printf("3. Realizar ataque\n");
        printf("4. Verificar missoes\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        printf("\n");
        
        switch(opcao) {
            case 1:
                exibirTerritorios(mapa, numTerritorios);
                break;
                
            case 2:
                exibirJogadores(jogadores, numJogadores);
                break;
                
            case 3: {
                int indiceAtacante, indiceDefensor;
                char corJogador[10];
                
                exibirTerritorios(mapa, numTerritorios);
                
                // Identifica qual jogador está realizando o ataque
                printf("\nQual a sua cor? ");
                scanf("%s", corJogador);
                
                // Seleciona o território atacante
                printf("Escolha o territorio atacante (0 a %d): ", numTerritorios - 1);
                scanf("%d", &indiceAtacante);
                
                // Seleciona o território defensor
                printf("Escolha o territorio defensor (0 a %d): ", numTerritorios - 1);
                scanf("%d", &indiceDefensor);
                
                // Valida os índices fornecidos
                if (indiceAtacante < 0 || indiceAtacante >= numTerritorios ||
                    indiceDefensor < 0 || indiceDefensor >= numTerritorios) {
                    printf("\nIndices invalidos!\n");
                    break;
                }
                
                // Verifica se o território atacante pertence ao jogador
                if (strcmp(mapa[indiceAtacante].cor, corJogador) != 0) {
                    printf("\nVoce nao controla esse territorio!\n");
                    break;
                }
                
                // Impede ataque a territórios da mesma cor (inclui atacar a si mesmo)
                if (strcmp(mapa[indiceAtacante].cor, mapa[indiceDefensor].cor) == 0) {
                    printf("\nNao e possivel atacar territorios da mesma cor!\n");
                    break;
                }
                
                // Verifica se há tropas suficientes para atacar
                if (mapa[indiceAtacante].tropas < 2) {
                    printf("\nO atacante precisa ter pelo menos 2 tropas!\n");
                    break;
                }
                
                // Executa o ataque
                executarAtaque(&mapa[indiceAtacante], &mapa[indiceDefensor]);
                
                // Verifica se o jogo acabou (apenas uma cor restante)
                if (verificarFimDeJogo(mapa, numTerritorios)) {
                    printf("\n");
                    printf("===========================================\n");
                    printf("  FIM DE JOGO! Apenas uma cor restante!\n");
                    printf("===========================================\n");
                    jogoAtivo = 0;
                    opcao = 5;
                    break;
                }
                
                // Verifica automaticamente se algum jogador completou sua missão
                printf("\n--- Verificando missoes ---\n");
                for (int i = 0; i < numJogadores; i++) {
                    if (verificarMissao(jogadores[i].missao, mapa, numTerritorios, jogadores[i].cor)) {
                        printf("\n");
                        printf("*******************************************\n");
                        printf("*  VITORIA! %s completou a missao!  *\n", jogadores[i].nome);
                        printf("*  Missao: %s\n", jogadores[i].missao);
                        printf("*******************************************\n");
                        jogoAtivo = 0;
                        opcao = 5;
                        break;
                    }
                }
                break;
            }
                
            case 4: {
                // Verificação manual do status de todas as missões
                printf("=== STATUS DAS MISSOES ===\n\n");
                int alguemVenceu = 0;
                
                for (int i = 0; i < numJogadores; i++) {
                    printf("Jogador: %s (%s)\n", jogadores[i].nome, jogadores[i].cor);
                    printf("Missao: %s\n", jogadores[i].missao);
                    
                    if (verificarMissao(jogadores[i].missao, mapa, numTerritorios, jogadores[i].cor)) {
                        printf("Status: *** MISSAO CUMPRIDA! ***\n\n");
                        alguemVenceu = 1;
                    } else {
                        printf("Status: Em andamento\n\n");
                    }
                }
                
                if (alguemVenceu) {
                    printf("Fim de jogo! Um ou mais jogadores completaram suas missoes!\n");
                    jogoAtivo = 0;
                    opcao = 5;
                }
                break;
            }
                
            case 5:
                printf("Encerrando o jogo...\n");
                break;
                
            default:
                printf("Opcao invalida!\n");
        }
        
    } while(opcao != 5);
    
    // Libera toda a memória alocada
    liberarMemoria(mapa, jogadores, numJogadores);
    
    return 0;
}

// Função responsável pelo cadastro dos territórios
void cadastrarTerritorios(Territorio* mapa, int quantidade) {
    printf("\n=== CADASTRO DE TERRITORIOS ===\n\n");
    
    for (int i = 0; i < quantidade; i++) {
        printf("--- Territorio %d ---\n", i);
        
        // Limpa buffer do stdin
        if (i == 0) {
            while (getchar() != '\n');
        }
        
        // Lê o nome do território
        printf("Nome do territorio: ");
        fgets(mapa[i].nome, 30, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';
        
        // Lê a cor do exército
        printf("Cor do exercito: ");
        scanf("%s", mapa[i].cor);
        
        // Lê o número de tropas
        printf("Quantidade de tropas: ");
        scanf("%d", &mapa[i].tropas);
        
        // Limpa buffer após scanf
        while (getchar() != '\n');
        
        printf("\n");
    }
}

// Função responsável pelo cadastro dos jogadores
void cadastrarJogadores(Jogador* jogadores, int quantidade, char* missoes[], int totalMissoes) {
    printf("\n=== CADASTRO DE JOGADORES ===\n\n");
    
    for (int i = 0; i < quantidade; i++) {
        printf("--- Jogador %d ---\n", i + 1);
        
        // Lê o nome do jogador
        printf("Nome do jogador: ");
        fgets(jogadores[i].nome, 30, stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = '\0';
        
        // Lê a cor escolhida pelo jogador
        printf("Cor do exercito: ");
        scanf("%s", jogadores[i].cor);
        
        // Limpa buffer
        while (getchar() != '\n');
        
        // Aloca memória para a missão do jogador
        jogadores[i].missao = (char*) malloc(100 * sizeof(char));
        if (jogadores[i].missao == NULL) {
            printf("Erro ao alocar memoria para missao!\n");
            exit(1);
        }
        
        // Atribui uma missão aleatória
        atribuirMissao(jogadores[i].missao, missoes, totalMissoes);
        
        printf("\n");
    }
}

// Exibe todos os territórios cadastrados
void exibirTerritorios(Territorio* mapa, int quantidade) {
    printf("\n=== TERRITORIOS CADASTRADOS ===\n\n");
    
    for (int i = 0; i < quantidade; i++) {
        printf("Territorio %d:\n", i);
        printf("  Nome: %s\n", mapa[i].nome);
        printf("  Cor do Exercito: %s\n", mapa[i].cor);
        printf("  Tropas: %d\n", mapa[i].tropas);
        printf("--------------------------------\n");
    }
}

// Exibe todos os jogadores e suas respectivas missões
void exibirJogadores(Jogador* jogadores, int quantidade) {
    printf("\n=== JOGADORES E MISSOES ===\n\n");
    
    for (int i = 0; i < quantidade; i++) {
        printf("Jogador %d:\n", i + 1);
        printf("  Nome: %s\n", jogadores[i].nome);
        printf("  Cor: %s\n", jogadores[i].cor);
        printf("  Missao: %s\n", jogadores[i].missao);
        printf("--------------------------------\n");
    }
}

// Simula um combate entre dois territórios usando dados
void executarAtaque(Territorio* atacante, Territorio* defensor) {
    printf("\n=== BATALHA ===\n");
    printf("%s (%s) ataca %s (%s)\n", 
           atacante->nome, atacante->cor,
           defensor->nome, defensor->cor);
    
    // Simula rolagem de dados (1 a 6)
    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;
    
    printf("\nResultado dos dados:\n");
    printf("Atacante: %d\n", dadoAtacante);
    printf("Defensor: %d\n", dadoDefensor);
    
    // Determina vencedor do combate
    if (dadoAtacante > dadoDefensor) {
        printf("\n*** VITORIA DO ATACANTE! ***\n");
        printf("%s conquistou %s!\n", atacante->nome, defensor->nome);
        
        // Calcula tropas a serem transferidas (metade)
        int tropasTransferidas = atacante->tropas / 2;
        
        // Atualiza o território conquistado
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = tropasTransferidas;
        
        // Atualiza o território atacante
        atacante->tropas = atacante->tropas - tropasTransferidas;
        
        printf("Tropas transferidas: %d\n", tropasTransferidas);
        
    } else {
        printf("\n*** DEFESA BEM-SUCEDIDA! ***\n");
        printf("%s defendeu o territorio!\n", defensor->nome);
        
        // Atacante perde uma tropa
        atacante->tropas--;
        printf("%s perdeu 1 tropa.\n", atacante->nome);
    }
    
    // Exibe estado após o combate
    printf("\n--- Situacao apos o combate ---\n");
    printf("%s: %d tropas (%s)\n", atacante->nome, atacante->tropas, atacante->cor);
    printf("%s: %d tropas (%s)\n", defensor->nome, defensor->tropas, defensor->cor);
}

// Sorteia e atribui uma missão aleatória ao jogador
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int indiceAleatorio = rand() % totalMissoes;
    strcpy(destino, missoes[indiceAleatorio]);
}

// Verifica se o jogador completou sua missão
int verificarMissao(char* missao, Territorio* mapa, int tamanho, char* corJogador) {
    
    // Missão: conquistar 3 territórios consecutivos
    if (strstr(missao, "3 territorios consecutivos") != NULL) {
        int sequencia = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                sequencia++;
                if (sequencia >= 3) {
                    return 1;
                }
            } else {
                sequencia = 0;
            }
        }
        return 0;
    }
    
    // Missão: controlar 5 territórios simultaneamente
    if (strstr(missao, "5 territorios simultaneamente") != NULL) {
        int territoriosControlados = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                territoriosControlados++;
            }
        }
        return (territoriosControlados >= 5);
    }
    
    // Missão: acumular 50 tropas no total
    if (strstr(missao, "50 tropas") != NULL) {
        int totalTropas = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                totalTropas += mapa[i].tropas;
            }
        }
        return (totalTropas >= 50);
    }
    
    // Missão: eliminar todas as tropas de uma cor específica
    if (strstr(missao, "Eliminar todas as tropas") != NULL) {
        // Identifica cores inimigas ainda presentes no mapa
        char coresPresentes[10][10];
        int numCores = 0;
        
        for (int i = 0; i < tamanho; i++) {
            int corJaRegistrada = 0;
            for (int j = 0; j < numCores; j++) {
                if (strcmp(coresPresentes[j], mapa[i].cor) == 0) {
                    corJaRegistrada = 1;
                    break;
                }
            }
            // Adiciona apenas cores inimigas
            if (!corJaRegistrada && strcmp(mapa[i].cor, corJogador) != 0) {
                strcpy(coresPresentes[numCores], mapa[i].cor);
                numCores++;
            }
        }
        
        // Se não há cores inimigas, todas foram eliminadas
        return (numCores == 0);
    }
    
    // Missão: conquistar todos os territórios de uma cor inimiga
    if (strstr(missao, "todos os territorios de uma cor inimiga") != NULL) {
        char coresPresentes[10][10];
        int numCores = 0;
        
        for (int i = 0; i < tamanho; i++) {
            int corJaRegistrada = 0;
            for (int j = 0; j < numCores; j++) {
                if (strcmp(coresPresentes[j], mapa[i].cor) == 0) {
                    corJaRegistrada = 1;
                    break;
                }
            }
            if (!corJaRegistrada && strcmp(mapa[i].cor, corJogador) != 0) {
                strcpy(coresPresentes[numCores], mapa[i].cor);
                numCores++;
            }
        }
        
        return (numCores == 0);
    }
    
    return 0;
}

// Exibe a missão atribuída a um jogador
void exibirMissao(char* missao, char* nomeJogador) {
    printf("Jogador: %s\n", nomeJogador);
    printf("Missao: %s\n\n", missao);
}

// Verifica se o jogo acabou (apenas uma cor no mapa)
int verificarFimDeJogo(Territorio* mapa, int tamanho) {
    if (tamanho == 0) return 0;
    
    char primeiraCor[10];
    strcpy(primeiraCor, mapa[0].cor);
    
    // Verifica se todos os territórios têm a mesma cor
    for (int i = 1; i < tamanho; i++) {
        if (strcmp(mapa[i].cor, primeiraCor) != 0) {
            return 0;  // ainda há cores diferentes
        }
    }
    
    return 1;  // todos os territórios são da mesma cor
}

// Libera toda a memória alocada dinamicamente
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int numJogadores) {
    // Libera as missões de cada jogador
    for (int i = 0; i < numJogadores; i++) {
        if (jogadores[i].missao != NULL) {
            free(jogadores[i].missao);
        }
    }
    
    // Libera o vetor de jogadores
    if (jogadores != NULL) {
        free(jogadores);
    }
    
    // Libera o vetor de territórios
    if (mapa != NULL) {
        free(mapa);
    }
    
    printf("\nMemoria liberada com sucesso!\n");
}