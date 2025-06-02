
#define N_PAL_5_LET 5481
#define N_TENTATIVAS 7

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char palavras5l[N_PAL_5_LET][6];
char palavra[6];

void inicializa_palavras();
void escolhe_palavra();
void jogar();
int busca_binaria(char palavra[]);

int main()
{
    while (1)
    {
        inicializa_palavras();

        escolhe_palavra();
    
        jogar();
    }
}

void inicializa_palavras()
{
    FILE *arquivo_entrada, *arquivo_saida;
    arquivo_entrada = fopen("palavras_sa_5letras.txt", "r");

    int num;

    for (int i = 0 ; i < N_PAL_5_LET ; i++)
        fscanf(arquivo_entrada, "%d %s", &num, palavras5l[i]);
    
    fclose(arquivo_entrada); 
}

void escolhe_palavra()
{
    srand(time(NULL));
    int escolhida = rand() % N_PAL_5_LET;
    for (int i = 0 ; i < 6 ; i++)
    {
        palavra[i] = palavras5l[escolhida][i];
    }
}

void jogar()
{
    char jogo[N_TENTATIVAS*2 + 4][9];
    int linha_at = 0;
    char chute[40];
    int tentativas = N_TENTATIVAS;

    for (int i = 0 ; i < N_TENTATIVAS * 2 ; i++)
        jogo[i][6] = '\0';

    strcpy(jogo[linha_at++], "TERMO");
    strcpy(jogo[linha_at++], "ALICA");
    strcpy(jogo[linha_at++], "S3000");
    strcpy(jogo[linha_at++], "     ");

    while (tentativas > 0)
    {

        system("clear");
        for (int i = 0 ; i < linha_at ; i++)
        {
            printf("%s\n", jogo[i]);
        }

        scanf("%s", chute);

        if (chute[5] == '\0')
        {
            if (busca_binaria(chute))
            {

                for(int i = 0 ; i < 5 ; i++)
                    jogo[linha_at][i] = chute[i];
                linha_at++;
                for(int i = 0 ; i < 5 ; i++)
                {
                    if (palavra[i] == chute[i])
                        jogo[linha_at][i] = palavra[i];
                    else
                    {
                        for (int j = 0 ; j < 5 ; j++)
                        {
                            if (palavra[j] == chute[i] && palavra[j] != chute[j])
                                jogo[linha_at][i] = '-';
                        }
                        if (jogo[linha_at][i] != '-')
                            jogo[linha_at][i] = ' ';
                    }
                }
                linha_at++;
                tentativas--;
            }
        }
    }
}

int busca_binaria(char procura[])
{   
    int min = 0;
    int max = N_PAL_5_LET - 1;
    int meio;
    while (min <= max)
    {
        meio = (max + min) / 2;

        for (int i = 0 ; i < 6 ; i++)
        {
            if (palavras5l[meio][i] < procura[i])
            {
                min = meio + 1;
                break;
            }
            else if (palavras5l[meio][i] > procura[i])
            {
                max = meio - 1;
                break;
            }
            else 
                if (i == 5)
                    return 1;
        }
    }
    return 0;
}
    