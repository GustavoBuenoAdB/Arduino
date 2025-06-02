

// convencionado que P_LED_VERM é a menor porta
// convencionado que P_LED_CINZ é a maior porta
// os leds tem incremento unitario nos pinos adjacentes

#define P_LED_VERM 9
#define P_LED_VERD 10
#define P_LED_AZUL 11
#define P_LED_AMAR 12
#define P_LED_CINZ 13

#define P_BOTAO_CONFIRMA 2
#define P_BOTAO_TROCA 3

#define TAM_MAX 20

enum estados{
  desligado,
  iniciando,
  mostra_sequencia,
  lendo_sequencia,
  acerto,
  erro
};


long semente;
int estado;
int sequencia[TAM_MAX];
int atual_seq;


void setup()
{
    // inicializando leds
    pinMode(P_LED_VERM, OUTPUT);
    pinMode(P_LED_VERD, OUTPUT);
    pinMode(P_LED_AZUL, OUTPUT);
    pinMode(P_LED_AMAR, OUTPUT);
    pinMode(P_LED_CINZ, OUTPUT);
    
    // inicializando botoes
    pinMode(P_BOTAO_TROCA, INPUT);
    pinMode(P_BOTAO_CONFIRMA, INPUT);

    // inicializando maquina de estados
    estado = desligado;
}

void loop()
{
  switch (estado)
  {
    case desligado:
    {
        // espera ele apertar o botao
        if (digitalRead(P_BOTAO_CONFIRMA) || digitalRead(P_BOTAO_TROCA))
        {
            randomSeed(millis());
            estado = iniciando;
        }
        break;
    }
    case iniciando:
    {
        animacaozinha();
        pisca(1000);
        atual_seq = 0;
        estado = mostra_sequencia;
        break;
    }
    case mostra_sequencia:
    {
        sequencia[atual_seq] = led_aleatorio();
        atual_seq++;
        for(int i = 0 ; i < atual_seq ; i++)
        {
            digitalWrite(sequencia[i], HIGH);
            delay(1000);
            digitalWrite(sequencia[i], LOW);
            delay(300);
        }
        estado = lendo_sequencia;
        break;
    }
    case lendo_sequencia:
    {
        for(int i = 0 ; i < atual_seq ; i++)
        {
            int led_lido = le_led_usuario();
            if (led_lido != sequencia[i])
            {
                estado = erro;
                break;
            }
            for(int i = 0 ; i < 5 ; i++)
            {
                digitalWrite(led_lido, LOW);
                delay(100);
                digitalWrite(led_lido, HIGH);
                delay(100);
            }
            digitalWrite(led_lido, LOW);
        }
        if (estado != erro)
            estado = acerto;
        break;
    }
    case acerto:
    {
        for(int i = 0 ; i < 5 ; i++)
            pisca(400);
        estado = mostra_sequencia;
        break;
    }
    case erro:
    {
        animacaozinha();
        animacaozinha();
        estado = desligado;
        break;
    }
    default:
        estado = desligado;
        break;
  }
}

// retorna um numero pseudoaleatorio entre [9 - 13]
int led_aleatorio()
{
    return random(P_LED_VERM, P_LED_CINZ + 1);
}

int le_led_usuario()
{
    int atual = P_LED_VERM;
    while (!digitalRead(P_BOTAO_CONFIRMA))
    {
        if (digitalRead(P_BOTAO_TROCA))
        {
            digitalWrite(atual, LOW);
            atual++;
            if (atual > P_LED_CINZ)
                atual = P_LED_VERM;
            //delay(200);
        }
        while (!digitalRead(P_BOTAO_TROCA)); 
        digitalWrite(atual, HIGH);
    }

    while (digitalRead(P_BOTAO_CONFIRMA)); 
    return atual;
}

void animacaozinha()
{
    for (int i = 9 ; i < 14 ; i++)
    {
        digitalWrite(i, HIGH);
        delay(200);
        digitalWrite(i, LOW);
    }
}
void pisca(unsigned int tempo)
{
    for (int i = P_LED_VERM ; i < P_LED_CINZ + 1 ; i++)
        digitalWrite(i, HIGH);

    delay(tempo);

    for (int i = P_LED_VERM ; i < P_LED_CINZ + 1 ; i++)
        digitalWrite(i, LOW);
}