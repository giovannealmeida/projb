#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <locale.h>

#define PILHA_TAM 256 //Tamanho da pilha
#define REG_TAM 1024 //Tamanho máximo de quantidade de registros de transições
#define TRANS_TAM 3 //Tamanho máximo de nome das transições
#define PROD_NUM 10 //Número de transições E símbolos (vetores 'S' e 'B')

//Protótipos de funções
int carregaProd(); //Carrega arquivos de produções
char inicializaProd(); //Inicializa os vetores de produções 'S' e 'B' e adiciona no topo da pilha o primeiro caractere não terminal
void mostraS(); //Mostra a vetor de símbolos 'S'
void mostraB(); //Mostra a vetor de conectivos 'B'
int buscaTransicaoS(char c); //Retorna a posição de um "transição" no vetor 'S' que deve ser empilhada
int buscaTransicaoB(char c); //Retorna a posição de um "transição" no vetor 'B' que deve ser empilhada
int pegaEntrada(); //Pega a cadeia de entrada
void mostraPilha(); //Exibe a pilha
void mostraEntrada(); //Exibe a cadeia de entrada
void mostraMenu(); //Exibe o menu principal
void notificarErroSintaxe(); //Notifica erro de sintaxe. A formula não é uma WFF
void notificarErroGramatica(); //Notifica erro de gramática. Algum símbolo não faz parte do alfabeto
int pop(); //Remove o elemento do topo da pilha
int push(char e); //Insere um elemento no topo da pilha
int empty(); //Retorna 1 se a pilha estiver vazia e 0 caso contrário
char read(); //Consome o elemento atual da entrada e faz pop na pilha
void init(); //Inicializa variáveis e vetores
void iniciaAutomato(); //Inicia a verificação de sintaxe

//Variáveis globais
char S[PROD_NUM][PROD_NUM], B[PROD_NUM][PROD_NUM], naoTermB;
int flag_setado_NaoTermB = 0; //Flag que determina se o símbolo não terminal B já foi setado
char pilha[PILHA_TAM]; //Pilha
char entrada[PILHA_TAM]; //Cadeia de entrada
int reg_carret = 0; //Guarda a posição do cursor que varrerá os registros de transição
int op = -1; //Recebe a opção do usuário no menu principal
int topo = -1; //Guarda o topo da pilha
int carret = -1; //Guarda a posição do cursor que varrerá a antrada
FILE *amostra, *resultados; //Arquivos de amostra e do resultado da análise
FILE *prod; //Arquivo de produções
int flag_escrevendo = -1; //1 se o resultado estiver sendo escrito num arquivo externo
int carret_amostra = -1; //Carret que percorrerá o arquivo de amostras
int num_amostras = 1024; //Numero de amostra

int main()
{
    setlocale(LC_ALL, "Portuguese"); //Permite acentuação
    
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
	    		printf("Opção inválida!\n");
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
	rewind(prod); //Volta o ponteiro para o início do arquivo
	naoTermS = fgetc(prod); //Pega o primeiro caractere do arquivo, supostamente o não terminal inicial
	rewind(prod); //Volta o ponteiro para o início do arquivo
	
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
	printf("O que deseja fazer?\n\n1 = Carregar arquivo de amostras de fórmulas\n2 = Inserir fórmulas\n");
	scanf("%d",&op);
}

void carregaAmostras(){
	
    resultados = fopen("resultados.txt","w");
    
    if(!(amostra = fopen("amostra.txt","r"))){
	    printf("\nO arquivo de amostras não pôde ser encontrado!\nCertifique-se de que o arquivo \"amostra.txt\" se encontra no mesmo diretório do analisador\n");
	    fprintf(resultados, "FALHA AO CARREGAR O ARQUIVO DE AMOSTRAS");
	} else {
	   for(carret_amostra=0;carret_amostra<num_amostras;carret_amostra++){
        fscanf(amostra,"%s ",entrada);
        iniciaAutomato();
    	}
	}
	fclose(amostra);
	fclose(resultados);
	printf("\nAnálise realizada com sucesso!\n\n");
	system("pause");
	exit(1);
}

void notificarErroSintaxe(){
	printf("\n\nA FÓRMULA DIGITADA NÃO É UMA FÓRMULA BEM FORMADA!\n\n");
}

void notificarErroGramatica(){
	printf("\n\nA FÓRMULA DIGITADA NÃO FAZ PARTE DA GRAMÁTICA!\n\n");
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
	reg_carret = 0; //Ajusta o cursor do registro de transições
    pilha[0] = inicializaProd(); //Inicia a pilha com o primeiro caractere não terminal do arquivo
    topo = 0; // Ajusta o topo da pilha
    carret = 0; //Ajusta o cursor para o início da entrada
}

void iniciaAutomato(){
    init();
    while(entrada[carret]!='\0'){
    	char trans[PROD_NUM]; //Guarda o que será empilhado
    	
    	mostraPilha();
    	mostraS();
    	mostraB();
    	getch();
    	
		printf("Verificando caractere: %c\n",entrada[carret]);
    	if(entrada[carret] == pilha[topo]){
    		read();
		} else{
			int transicao;
			printf("Buscando transição...\n"); //Deve-se verificar se a transição é pra ser buscada no vetor 'S' ou no vetor 'B'
			if(pilha[topo]==naoTermB){ //Verifica se o que está no topo da pilha é o segundo não-terminal
				printf("NÃO TERMINAL 2!\n");
				transicao = buscaTransicaoB(entrada[carret]); //Busca uma produção com o segundo não-terminal
				if(transicao == -1){
					notificarErroGramatica();
					break;
				} else {
					strcpy(trans,B[transicao]);
				}
				
			} else { //senão for o segundo não-terminal, assume-se que é o primeiro não-terminal
				printf("NÃO TERMINAL 1!\n");
				transicao = buscaTransicaoS(entrada[carret]); //Busca uma produção com o primeiro não-terminal
				if(transicao == -1){
					notificarErroGramatica();
					break;
				} else {
					strcpy(trans,S[transicao]);
				}
			}
			
			printf("Transição Nº %d\n",transicao);
			printf("Transição selecionada: %s\n",trans);

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
    if(empty() && carret == strlen(entrada)){ //Se a pilha estiver vazia e toda a entrada já tiver sido lida...
		printf("\n\nFÓRMULA BEM FORMADA ACEITA!\n\n");
	} else {
		notificarErroSintaxe();
	}
}
