#include "anlex.h"

entrada *tabla;				
int tamanoTabla=TAMHASH;
int elementos=0;				

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

// se inicia la tabla
void iniciarTabla()
{
	int i=0;

	tabla=(entrada*)malloc(tamanoTabla*sizeof(entrada));
	for(i=0;i<tamanoTabla;i++)
	{
		tabla[i].compLex=-1;
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

//cuando la tabla llega al limite de elementos se duplica el tamano
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamanoTabla=siguiente_primo(2*tamanoTabla);
	iniciarTabla();
	for (i=0;i<tamanoTabla/2;i++)
	{
		if(vieja[i].compLex!=-1)
			insertar(vieja[i]);
	}
	free(vieja);
}

//se inserta una entrada en la tabla
void insertar(entrada e)
{
	int pos;
	if (++elementos>=tamanoTabla/2)
		rehash();
	pos=h(e.lexema,tamanoTabla);
	while (tabla[pos].compLex!=-1)
	{
		pos++;
		if (pos==tamanoTabla)
			pos=0;
	}
	tabla[pos]=e;

}

// se busca una clave en la tabla, si no existe devuelve NULL, si existe se devuelve la posicion
entrada* buscar(const char *clave)
{
	int pos;
	pos=h(clave,tamanoTabla);
	while(tabla[pos].compLex!=-1 && strcmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamanoTabla)
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
    // se inicializa los elementos de la tabla con sus respectivas definiciones
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

