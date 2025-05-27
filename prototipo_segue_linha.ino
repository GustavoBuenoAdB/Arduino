
/// Porta Sensor de Linha Preta Esquerdo mais distante do centro
#define PSLP_ESQ_LADO 7
// Porta Sensor de Linha Preta Direito mais distante do centro 
#define PSLP_DIR_LADO 8
// Porta Sensor de Linha Preta Esquerdo no centro
#define PSLP_ESQ_MEIO 9
// Porta Sensor de Linha Preta Direito no centro
#define PSLP_DIR_MEIO 10

// Porta Motor Esquerdo
#define PB1A_MOTOR_ESQ
#define PB1B_MOTOR_ESQ
#define PA1A_MOTOR_DIR
#define PA1B_MOTOR_DIR

#define AVANCAR 1
#define RETORNAR -1
#define PARAR 0

// Definicoes de Valores para traducao real ===

// velocidades
#define VEL_LENTA 180
#define VEL_PADRAO 220
#define VEL_RAPIDA 255

// duracoes
#define DURACAO_CURTA 2
#define DURACAO_PADRAO 5
#define DURACAO_LONGA 10

// duracoes para cada angulo de curva em cada velocidade
#define DURACAO_ANG_CURVA_P_VEL_LENTA 3
#define DURACAO_ANG_CURVA_P_VEL_PADRAO 2
#define DURACAO_ANG_CURVA_P_VEL_RAPIDA 1


// globais para armazenar a leitura dos sensores de linha
int sen_Esq_Lado = 0;
int sen_Dir_Lado = 0;
int sen_Esq_Meio = 0;
int sen_Dir_Meio = 0;

typedef struct Comando
{
    int sent_MotEsq; // define se o motor avança (1) retrocede (-1) ou fica parado (0).
    int sent_MotDir; // define se o motor avança (1) retrocede (-1) ou fica parado (0).
    int mov_duracao; // duracao em ms do movimento dos motores.
    uint8_t mov_velocidade; // velocidade pra aplicar idealmente em ambos os motores.
    int leitura_anterior[4]; // leitura anterior dos 4 sensores de linha.

} Comando;

Comando comando;

#define INC_MOTOR_REAL 0.02 // 2%
double porcentagem_motor_Esq_real = 1;
double porcentagem_motor_Dir_real = 1;

void setup()
{
    // inicializando os sensores de linha preta
    pinMode(PSLP_ESQ_LADO, INPUT);
    pinMode(PSLP_DIR_LADO, INPUT);
    pinMode(PSLP_ESQ_MEIO, INPUT);
    pinMode(PSLP_DIR_MEIO, INPUT);

    // inicializando portas dos Motores
    pinMode(PA1A_MOTOR_DIR, OUTPUT);
    pinMode(PA1B_MOTOR_DIR, OUTPUT);
    pinMode(PB1A_MOTOR_ESQ, OUTPUT);
    pinMode(PB1B_MOTOR_ESQ, OUTPUT);
}

// loop de estados simples para gerar rotina
void loop()
{
    atualiza_Sensores(); // atualiza os sensores 
    define_Comando(); // gera um Comando baseado na Interpretacao dos sensores
    executa_Comando(); // executa o comando gerado anteriormente
}

void atualiza_Sensores()
{
    // Atualizando sensores de Linha Preta
    sen_Esq_Lado = digitalRead(PSLP_ESQ_LADO);
    sen_Dir_Lado = digitalRead(PSLP_DIR_LADO);
    sen_Esq_Meio = digitalRead(PSLP_ESQ_MEIO);
    sen_Dir_Meio = digitalRead(PSLP_DIR_MEIO);
    delay(1); // delay pra ler os sensores
}

void define_Comando()
{

    // Situacao Ideal.
    // if (B - P - P - B) {...}
    if ( !(sen_Esq_Lado) && (sen_Esq_Meio) && (sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.sent_MotEsq = AVANCAR;
        comando.sent_MotDir = AVANCAR;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_PADRAO;
    }

    // if (B - B - P - B) {...}
    else if ( !(sen_Esq_Lado) && !(sen_Esq_Meio) && (sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.sent_MotEsq = PARAR;
        comando.sent_MotDir = RETORNAR;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (B - P - B - B) {...}
    else if ( !(sen_Esq_Lado) && (sen_Esq_Meio) && !(sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.sent_MotEsq = RETORNAR;
        comando.sent_MotDir = PARAR;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (P - P - P - P) {...}
    else if ( (sen_Esq_Lado) && (sen_Esq_Meio) && (sen_Dir_Meio) && (sen_Dir_Lado) )
    {
        comando.sent_MotEsq = PARAR;
        comando.sent_MotDir = PARAR;
    }

    // Situacao Problema.
    // if (B - B - B - B) {...}
    else if ( !(sen_Esq_Lado) && !(sen_Esq_Meio) && !(sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        // supondo um gap
        comando.sent_MotEsq = AVANCAR;
        comando.sent_MotDir = AVANCAR;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (B - P - P - P) ou (B - B - B - P) {...}
    else if ( !(sen_Esq_Lado) && ( ( (sen_Esq_Meio) && (sen_Dir_Meio) ) || ( !(sen_Esq_Meio) && !(sen_Dir_Meio) ) ) && (sen_Dir_Lado) )
    {
        comando.sent_MotEsq = AVANCAR;
        comando.sent_MotDir = PARAR;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_ANG_CURVA_P_VEL_LENTA * 90;
    }

    // if (P - P - P - B) ou (P - B - B - B) {...}
    else if ( (sen_Esq_Lado) && ( ( (sen_Esq_Meio) && (sen_Dir_Meio) ) || ( !(sen_Esq_Meio) && !(sen_Dir_Meio) ) ) && !(sen_Dir_Lado) )
    {
        comando.sent_MotEsq = RETORNAR;
        comando.sent_MotDir = AVANCAR;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_ANG_CURVA_P_VEL_LENTA * 90;
    }
}


void executa_Comando()
{
    set_motores(comando.mov_velocidade, comando.sent_MotEsq, comando.sent_MotDir);
    delay(comando.mov_duracao);
}



void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir)
{
    if (direcao_esq == AVANCAR)
    {
        // 1 - % pq nesse sentido o calculo é invertido
        int pwm = max(0, min(255, int(velocidade * (1.0 - porcentagem_motor_Esq_real))));
        analogWrite(PB1A_MOTOR_ESQ, pwm); 
        digitalWrite(PB1B_MOTOR_ESQ, LOW);
    }
    else if (direcao_esq == RETORNAR)
    {
        digitalWrite(PB1A_MOTOR_ESQ, LOW);
        analogWrite(PB1B_MOTOR_ESQ, (int)(velocidade * ( porcentagem_motor_Esq_real )) ); 
    }
    else
    {
        digitalWrite(PB1A_MOTOR_ESQ, HIGH);
        digitalWrite(PB1B_MOTOR_ESQ, HIGH);
    }

    if (direcao_dir == AVANCAR)
    {
        // 1 - % pq nesse sentido o calculo é invertido
        int pwm = max(0, min(255, int(velocidade * (1.0 - porcentagem_motor_Esq_real))));
        analogWrite(PA1A_MOTOR_DIR, pwm); 
        digitalWrite(PA1B_MOTOR_DIR, LOW);
    }
    else if (direcao_dir == RETORNAR)
    {
        digitalWrite(PA1A_MOTOR_DIR, LOW);
        analogWrite(PA1B_MOTOR_DIR, (int)(velocidade * ( porcentagem_motor_Dir_real ))); 
    }
    else
    {
        digitalWrite(PA1A_MOTOR_DIR, HIGH);
        digitalWrite(PA1B_MOTOR_DIR, HIGH);
    }
}