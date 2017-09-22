
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/**---------- Definiciones de los Tokens Válidos ----------**/

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
#define opa		    -1

#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/**---------- Estructuras ----------**/

typedef struct entrada{

	int compLex;
	char lexema[TAMLEX];
	struct entrada *tipoDato;

} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;

void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
