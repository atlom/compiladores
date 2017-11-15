
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/**---------- Definiciones ----------**/

//Codigos
#define lcorchete	0
#define rcorchete	1
#define lllave		2
#define rllave      3
#define coma		4
#define dospuntos   5
#define cadena      6
#define numero		7
#define verdadero	8
#define falso	    9
#define nulo		10
#define opa		    11

#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/**---------- Estrucuturas ----------**/

char *comp_lex [12];


typedef struct entrada{

	int compLex;
	char lexema[TAMLEX];
	struct entrada *tipoDato;

} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token1;

token1 t;
int numLinea;
entrada e;
void insertar(entrada e);
entrada* buscar(const char *clave);
void iniciarTabla();
void iniciarTablaSimbolos();
void initconjuntosSiguiente();
void initConjuntosPrimero();
void iniciarComlex();
void attribute_value();
void attribute_name();
void attribute();
void a();
void array();
void object();
void element();
void attributes_list();
void element_list();
void e1();
void match();
void scan();
void get_token();
void pop();
int  panicModeConSincronizacion();
void anaLex();
void descendenteRecursivo();
int existeTokenSiguiente();
int existeTokenPrimero();
//


