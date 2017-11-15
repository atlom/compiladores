#include "anlex.h"

entrada *tabla;				
int tamTabla=TAMHASH;
int elems=0;				

int h(const char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if ( (g=h&0xf0000000) ){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}

void iniciarTabla()
{
	int i=0;

	tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
	for(i=0;i<tamTabla;i++)
	{
		tabla[i].compLex=opa;
	}
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla se llene, duplicar el tamanho
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamTabla=siguiente_primo(2*tamTabla);
	iniciarTabla();
	for (i=0;i<tamTabla/2;i++)
	{
		if(vieja[i].compLex!=opa)
			insertar(vieja[i]);
	}
	free(vieja);
}

//insertar una entrada 
void insertar(entrada e)
{
	int pos;
	if (++elems>=tamTabla/2)
		rehash();
	pos=h(e.lexema,tamTabla);
	while (tabla[pos].compLex!=opa)
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave, si no existe devuelve NULL, en caso contrario devuelve la posicion
entrada* buscar(const char *clave)
{
	int pos;
	pos=h(clave,tamTabla);
	while(tabla[pos].compLex!=opa && strcmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	return &tabla[pos];
}

void insertarSimbolos(const char *s, int n)
{
	entrada e;
	strcpy(e.lexema,s);
	e.compLex=n;
	insertar(e);
}

void iniciarTablaSimbolos()
{
    // inicializa los elementos de la tabla 
    insertarSimbolos(",",coma);
	insertarSimbolos(":",dospuntos);
	insertarSimbolos("NULL",nulo);
	insertarSimbolos("null",nulo);
	insertarSimbolos("{",lllave);
	insertarSimbolos("}",rllave);
	insertarSimbolos("[",lcorchete);
	insertarSimbolos("]",rcorchete);
	insertarSimbolos("true",verdadero);
	insertarSimbolos("TRUE",verdadero);
	insertarSimbolos("false",falso);
	insertarSimbolos("FALSO",falso);

}

void iniciarComlex()
{
	comp_lex [0]= "L_CORCHETE";
    comp_lex [1]= "R_CORCHETE" ;
    comp_lex [2]= "L_LLAVE";
    comp_lex [3]= "R_LLAVE";
    comp_lex [4]= "COMA";
    comp_lex [5]= "DOS_PUNTOS";
    comp_lex [6]= "LITERAL_CADENA";
    comp_lex [7]= "LITERAL_NUM";
    comp_lex [8]= "PR_TRUE";
    comp_lex [9]= "PR_FALSE";
    comp_lex [10]= "PR_NULL";
    comp_lex [11]= "EOF";
}
