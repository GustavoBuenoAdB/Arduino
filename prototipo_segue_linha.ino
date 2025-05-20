
/// Porta Sensor de Linha Preta Esquerdo mais distante do centro
#define PSLP_ESQ_LADO 7
// Porta Sensor de Linha Preta Direito mais distante do centro 
#define PSLP_DIR_LADO 8
// Porta Sensor de Linha Preta Esquerdo no centro
#define PSLP_ESQ_MEIO 9
// Porta Sensor de Linha Preta Direito no centro
#define PSLP_DIR_MEIO 10

// globais para armazenar a leitura dos sensores de linha
int sen_Esq_Lado = 0;
int sen_Dir_Lado = 0;
int sen_Esq_Meio = 0;
int sen_Dir_Meio = 0;

typedef struct Vetor2i{
    int x;
    int y;
} Vetor2i;

Vetor2i rotacao;

#define MOV_LEVE 1
#define MOV_BRUCO 2
#define MOV_DELAY 3.0

AF_DCMotor Motor_esq(PORTA_MOT_E);
AF_DCMotor Motor_dir(PORTA_MOT_D);

uint_8 velocidade = 200;

#define INC_MOTOR_REAL 0.02 // 2%
double porcentagem_motor_Esq_real = 0.9;
double porcentagem_motor_Dir_real = 0.9;

void setup()
{
    // inicializando os sensores de linha preta
    pinMode(PSLP_ESQ_LADO, INPUT);
    pinMode(PSLP_DIR_LADO, INPUT);
    pinMode(PSLP_ESQ_MEIO, INPUT);
    pinMode(PSLP_DIR_MEIO, INPUT);
}

// loop de estados simples para gerar rotina
void loop()
{
    le_linhas();
    define_curva();
    anda();
    delay();
}

// funcao que inicializa e atualiza todas as informacoes nescessarias para fazer a escolha de rotacao
void le_linhas()
{
    sen_Esq_Lado = digitalRead(PSLP_ESQ_LADO);
    sen_Dir_Lado = digitalRead(PSLP_DIR_LADO);
    sen_Esq_Meio = digitalRead(PSLP_ESQ_MEIO);
    sen_Dir_Meio = digitalRead(PSLP_DIR_MEIO);
    delay(1); // delay pra ler os sensores
}

// função que avalia as informações lidas e escolhe que caminho seguir
void define_curva()
{
    // possibilidades de leitura
    // definir o comportamento pra mais de uma leitura igual e nao só na primeira

    //arvore de escolhas
    if (sen_Esq_Meio)
    {
        if (sen_Dir_Meio)
        {
            if (sen_Esq_Lado)
            {
                if (sen_Dir_Lado)
                    para(); // P P P P
                else // !sen_Dir_Lado
                    curva_quadrada_esq(); // P P P B
            }
            else // !sen_Esq_Lado
            {
                if (sen_Dir_Lado)
                    curva_quadrada_dir(); // B P P P
                else // !sen_Dir_Lado
                    reto(); // B P P B
            }
        }
        else // !sen_Dir_Meio
        {
            if ( !(sen_Esq_Lado || sen_Dir_Lado) )
                curva_pouquinho_dir(); // B P B B
        }
    }
    else // !sen_Esq_Meio
    {
        if(sen_Dir_Meio)
            if(!(sen_Esq_Lado || sen_Dir_Lado))
                curva_pouquinho_esq(); // B B P B
        else // !sen_Dir_Meio
        {
            if( !(sen_Esq_Lado || sen_Dir_Lado) )
                anda_sem_rumo(); // B B B B
            else if (sen_Esq_Lado)
                curva_quadrada_esq(); // P B B B
            else if (sen_Dir_Lado)
                curva_quadrada_dir(); // B B B P
        }
    }
}

// funcoes de definicao de rotacao // TEM MUITO O QUE MELHORAR, È SÒ UMA BASE

void para()
{
    rotacao.x = 0;
    rotacao.y = 0;
    set_motores(0, RELEASE, RELEASE);
}
void reto()
{
    rotacao.x = 0;
    rotacao.y = 0;
}
void curva_quadrada_esq()
{
    rotacao.x = 0;
    rotacao.y = 90;
}
void curva_quadrada_dir()
{
    rotacao.x = 0;
    rotacao.y = -90;
}
void curva_pouquinho_esq()
{
    rotacao.x = 0;
    rotacao.y = 30;
}
void curva_pouquinho_dir()
{
    rotacao.x = 0;
    rotacao.y = -30;
}
void anda_sem_rumo()
{
    reto();
}

// dada a escolha de caminho essa função segue o caminho
void anda()
{
    if (rotacao.y > 0)
        curva(ESQUERDA, rotacao.y);
    else if (rotacao.y < 0)
        curva(DIREITA, abs(rotacao.y));
}


void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir)
{
  Motor_esq.setSpeed((uint8_t) (velocidade * porcentagem_motor_Esq_real));
  Motor_dir.setSpeed((uint8_t) (velocidade * porcentagem_motor_Dir_real));
  Motor_esq.run(direcao_esq);
  Motor_dir.run(direcao_dir);
}

void curva(int direcao, double angulo)
{  
  if (direcao == ESQUERDA)
    set_motores(velocidade, FORWARD, BACKWARD);
  else if (direcao == DIREITA)
    set_motores(velocidade, BACKWARD, FORWARD);

  delay(MOV_DELAY * angulo);
}