#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <locale.h>

#define PILHA_TAM 256 //Tamanho da pilha
#define REG_TAM 1024 //Tamanho m�ximo de quantidade de registros de transi��es
#define TRANS_TAM 3 //Tamanho m�ximo de nome das transi��es
#define PROD_NUM 10 //N�mero de transi��es E s�mbolos (vetores 'S' e 'B')

//Prot�tipos de fun��es
int carregaProd(); //Carrega arquivos de produ��es
char inicializaProd(); //Inicializa os vetores de produ��es 'S' e 'B' e adiciona no topo da pilha o primeiro caractere n�o terminal
void mostraS(); //Mostra a vetor de s�mbolos 'S'
void mostraB(); //Mostra a vetor de conectivos 'B'
int buscaTransicaoS(char c); //Retorna a posi��o de um "transi��o" no vetor 'S' que deve ser empilhada
int buscaTransicaoB(char c); //Retorna a posi��o de um "transi��o" no vetor 'B' que deve ser empilhada
int pegaEntrada(); //Pega a cadeia de entrada
void mostraPilha(); //Exibe a pilha
void mostraEntrada(); //Exibe a cadeia de entrada
void mostraMenu(); //Exibe o menu principal
void notificarErroSintaxe(); //Notifica erro de sintaxe. A formula n�o � uma WFF
void notificarErroGramatica(); //Notifica erro de gram�tica. Algum s�mbolo n�o faz parte do alfabeto
int pop(); //Remove o elemento do topo da pilha
int push(char e); //Insere um elemento no topo da pilha
int empty(); //Retorna 1 se a pilha estiver vazia e 0 caso contr�rio
char read(); //Consome o elemento atual da entrada e faz pop na pilha
void init(); //Inicializa vari�veis e vetores
void iniciaAutomato(); //Inicia a verifica��o de sintaxe

//Vari�veis globais
char S[PROD_NUM][PROD_NUM], B[PROD_NUM][PROD_NUM], naoTermB;
int flag_setado_NaoTermB = 0; //Flag que determina se o s�mbolo n�o terminal B j� foi setado
char pilha[PILHA_TAM]; //Pilha
char entrada[PILHA_TAM]; //Cadeia de entrada
int reg_carret = 0; //Guarda a posi��o do cursor que varrer� os registros de transi��o
int op = -1; //Recebe a op��o do usu�rio no menu principal
int topo = -1; //Guarda o topo da pilha
int carret = -1; //Guarda a posi��o do cursor que varrer� a antrada
FILE *amostra, *resultados; //Arquivos de amostra e do resultado da an�lise
FILE *prod; //Arquivo de produ��es
int flag_escrevendo = -1; //1 se o resultado estiver sendo escrito num arquivo externo
int carret_amostra = -1; //Carret que percorrer� o arquivo de amostras
int num_amostras = 1024; //Numero de amostra

int main()
{
    setlocale(LC_ALL, "Portuguese"); //Permite acentua��o
    
    carregaProd();
    while(pegaEntrada())
		iniciaAutomato();
    
    /*
    while(op == -1){
    	mostraMenu();
	    switch(op){
	    	case 1:
	    		flag_escrevendo = 1;
	    		carregaAmostras();
	    		break;
	    	case 2:
	    		flag_escrevendo = 0;
	    		while(pegaEntrada())
	    			iniciaAutomato();
		    	break;
	    	default:
	    		printf("Op��o inv�lida!\n");
		}
	}
	*/

    return 0;
}

int carregaProd(){
	if(!(prod = fopen("prod.txt","r"))){
		return 0;
	} else {return 1;}
}

char inicializaProd(){
	char naoTermS,ch,auxString[PILHA_TAM];
	int i,j=0;
	rewind(prod); //Volta o ponteiro para o in�cio do arquivo
	naoTermS = fgetc(prod); //Pega o primeiro caractere do arquivo, supostamente o n�o terminal inicial
	rewind(prod); //Volta o ponteiro para o in�cio do arquivo
	
	for(i=0;(ch=fgetc(prod)) != EOF;i++){
		if(ch == naoTermS){
			fscanf(prod,"%s ",auxString);
			strcpy(S[i],auxString);
		} else {
			printf("ch = %c\n",ch);
			if(!flag_setado_NaoTermB){
				printf("Salvando NTB como: %c\n",ch);
				naoTermB = ch;
				flag_setado_NaoTermB = 1;
			}
			
			fscanf(prod,"%s ",auxString);
			strcpy(B[j++],auxString);
		}
	}
	
	return naoTermS;
}

void mostraS(){
	int i;
	for(i=0;S[i][0]!='\0';i++){
		printf("P%d -> %s\n",i,S[i]);
	}
}

void mostraB(){
	int i;
	for(i=0;B[i][0]!='\0';i++){
		printf("B%d -> %s\n",i,B[i]);
	}
}

int buscaTransicaoS(char c){
	int i;
	for(i=0; i<PROD_NUM; i++){
		if(entrada[carret]==S[i][0]){
			return i;
		}
	}
	return -1;
}

int buscaTransicaoB(char c){
	int i;
	for(i=0; i<PROD_NUM; i++){
		if(entrada[carret]==B[i][0]){
			return i;
		}
	}
	return -1;
}

int pegaEntrada(){
    printf("Insira uma string: [CTRL + C para sair]\n");
    return scanf("%s",&entrada);
}

void mostraPilha(){
    int i;
    printf("Pilha -> ");
    for(i=0;i<=topo;i++){
        printf("|");
        printf("%c",pilha[i]);
    }
    printf(" <- topo\n\n");
}

void mostraEntrada(){
    printf("Entrada: %s",entrada);
}

void mostraMenu(){
	printf("O que deseja fazer?\n\n1 = Carregar arquivo de amostras de f�rmulas\n2 = Inserir f�rmulas\n");
	scanf("%d",&op);
}

void carregaAmostras(){
	
    resultados = fopen("resultados.txt","w");
    
    if(!(amostra = fopen("amostra.txt","r"))){
	    printf("\nO arquivo de amostras n�o p�de ser encontrado!\nCertifique-se de que o arquivo \"amostra.txt\" se encontra no mesmo diret�rio do analisador\n");
	    fprintf(resultados, "FALHA AO CARREGAR O ARQUIVO DE AMOSTRAS");
	} else {
	   for(carret_amostra=0;carret_amostra<num_amostras;carret_amostra++){
        fscanf(amostra,"%s ",entrada);
        iniciaAutomato();
    	}
	}
	fclose(amostra);
	fclose(resultados);
	printf("\nAn�lise realizada com sucesso!\n\n");
	system("pause");
	exit(1);
}

void notificarErroSintaxe(){
	printf("\n\nA F�RMULA DIGITADA N�O � UMA F�RMULA BEM FORMADA!\n\n");
}

void notificarErroGramatica(){
	printf("\n\nA F�RMULA DIGITADA N�O FAZ PARTE DA GRAM�TICA!\n\n");
}

int pop(){
    if(topo>=0){
        topo--;
        return 1;
    }
    return 0;
}

int push(char e){
    if(topo<PILHA_TAM+1){
        pilha[++topo]=e;
        return 1;
    }
    return 0;
}

int empty(){
	return (topo<0?1:0);
}

char read(){
    char c = pilha[topo];
    if(topo>=0){
        pop();
        carret++;
        return c;
    }
    return ;
}

int tamanhoPilha(){
    return strlen(pilha);
}

void init(){
	reg_carret = 0; //Ajusta o cursor do registro de transi��es
    pilha[0] = inicializaProd(); //Inicia a pilha com o primeiro caractere n�o terminal do arquivo
    topo = 0; // Ajusta o topo da pilha
    carret = 0; //Ajusta o cursor para o in�cio da entrada
}

void iniciaAutomato(){
    init();
    while(entrada[carret]!='\0'){
    	char trans[PROD_NUM]; //Guarda o que ser� empilhado
    	
    	mostraPilha();
    	mostraS();
    	mostraB();
    	getch();
    	
		printf("Verificando caractere: %c\n",entrada[carret]);
    	if(entrada[carret] == pilha[topo]){
    		read();
		} else{
			int transicao;
			printf("Buscando transi��o...\n"); //Deve-se verificar se a transi��o � pra ser buscada no vetor 'S' ou no vetor 'B'
			if(pilha[topo]==naoTermB){ //Verifica se o que est� no topo da pilha � o segundo n�o-terminal
				printf("N�O TERMINAL 2!\n");
				transicao = buscaTransicaoB(entrada[carret]); //Busca uma produ��o com o segundo n�o-terminal
				if(transicao == -1){
					notificarErroGramatica();
					break;
				} else {
					strcpy(trans,B[transicao]);
				}
				
			} else { //sen�o for o segundo n�o-terminal, assume-se que � o primeiro n�o-terminal
				printf("N�O TERMINAL 1!\n");
				transicao = buscaTransicaoS(entrada[carret]); //Busca uma produ��o com o primeiro n�o-terminal
				if(transicao == -1){
					notificarErroGramatica();
					break;
				} else {
					strcpy(trans,S[transicao]);
				}
			}
			
			printf("Transi��o N� %d\n",transicao);
			printf("Transi��o selecionada: %s\n",trans);

				int i;
				printf("Removendo %c do tipo e...\n",pilha[topo]);
				if(pop()){
					for(i=strlen(trans)-1;i>=0;i--){
					push(trans[i]);
					printf("Fazendo push do %c\n",trans[i]);
					}
				} else {
					break;
				}
				
		}

    }
    if(empty() && carret == strlen(entrada)){ //Se a pilha estiver vazia e toda a entrada j� tiver sido lida...
		printf("\n\nF�RMULA BEM FORMADA ACEITA!\n\n");
	} else {
		notificarErroSintaxe();
	}
}
