
#include "anlex.h"
#include <stdio.h>
#include <locale.h>


int cantidadError=0;	// Cantidad de errores
char conjuntosPrimero[11][7][15];//conjuntos primeros de la gramatica 
char conjuntosSiguiente[11][4][15];//Conjuntos siguientes de la gramatica 
typedef int bool;		
enum { false, true };	
char token[50];
FILE *tokens;
FILE *salida;
int vacio = 0;
int volver = 0;
int n = 0;

void descendenteRecursivo()
{
	
    element();
	match("EOF");
	
	if (cantidadError == 0){
		printf("\nEl codigo fuente es sintacticamente correcto.\n");
	}else{
		printf("\nEl codigo fuente no es sintacticamente correcto.\n");
	}
}


void attribute_value(){
	char lexema_tagName[TAMLEX];
	if(strcmp("LITERAL_CADENA",comp_lex[t.compLex])==0){
		match("LITERAL_CADENA");
		//printf("LITERAL_CADENA %s %s %s \n",t.pe->lexema, e.lexema, comp_lex[t.compLex]);
		fprintf(salida,"\"%s\"",e.lexema);
	}
	else if(strcmp("LITERAL_NUM",comp_lex[t.compLex])==0){
		match("LITERAL_NUM");
		//printf("%s", comp_lex[t.compLex]);
		fprintf(salida,"%s",e.lexema);
	}
	else if(strcmp("PR_TRUE",comp_lex[t.compLex])==0){
		match("PR_TRUE");
		fprintf(salida,"true");
	}
	else if(strcmp("PR_FALSE",comp_lex[t.compLex])==0){
		match("PR_FALSE");
		fprintf(salida,"false");
	}
	else if(strcmp("PR_NULL",comp_lex[t.compLex])==0){
		match("PR_NULL");
		fprintf(salida,"null");
	}
	else if (strcmp("L_CORCHETE",comp_lex[t.compLex])==0){
		element();
	}
	else if (strcmp("L_LLAVE",comp_lex[t.compLex])==0){
		element();
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver =panicModeConSincronizacion(8,comp_lex[t.compLex]);
		if (volver == 1){
			attribute_value();
		}
	}
		
}

void attribute_name(){
	if(strcmp("LITERAL_CADENA",comp_lex[t.compLex])==0){
		match("LITERAL_CADENA");
		//fprintf(salida,"false");
	}else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(7,comp_lex[t.compLex]);
		if (volver == 1){
			attribute_name();
		}
	}
}

void attribute(){
	
	char lexema_tagName[TAMLEX];
	char lexema_elementList[TAMLEX];
	
	if (strcmp("LITERAL_CADENA",comp_lex[t.compLex])==0){
		strcpy(lexema_tagName,t.pe->lexema);
		if (n != 0){
			fprintf(salida, "\n");
			int s;
			for( s=0; s<n; s++){
				fprintf(salida, "\t");
			}
		}
		fprintf(salida,"<");
		//sacarComillas(lexema_tagName);
		fprintf(salida,"%s",lexema_tagName);
		attribute_name();
		fprintf(salida,">");
		match("DOS_PUNTOS");
		attribute_value();
		if (n == 1){
			fprintf(salida, "\n");
		}
		fprintf(salida,"</");
		fprintf(salida,"%s",lexema_tagName);
		//sacarComillas(lexema_tagName);
		fprintf(salida,">");
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(6,comp_lex[t.compLex]);
		if (volver == 1){
			attribute();
		}
	}
}

void a(){
	if(strcmp("COMA",comp_lex[t.compLex])==0){
		match("COMA");
		attribute();
		a();
	}else{	
		bool existeSig = existeTokenSiguiente(5,comp_lex[t.compLex]);
		if(!existeSig){
			if (!strcmp(comp_lex[t.compLex],"EOF")==0){
				cantidadError = cantidadError + 1;
				printf("\nError Sintactico \n");
				scan();
				a();
			}
		}
	}
}

void array(){
	if(strcmp("L_CORCHETE",comp_lex[t.compLex])==0)
	{
		match("L_CORCHETE");
		if(!strcmp("R_CORCHETE",comp_lex[t.compLex])==0){
			n++;
			element_list();
			match("R_CORCHETE");
			fprintf(salida, "\n");
			int s;
			for(s=0; s<n-1; s++){
				fprintf(salida, "\t");
			}
			n--;
		}
		else
		{
			match("R_CORCHETE");
		}
	}
	else
	{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(3,comp_lex[t.compLex]);
		if (volver == 1){
			array();
		}
	}
}

void object(){
	if (strcmp("L_LLAVE",comp_lex[t.compLex])==0){
		match("L_LLAVE");
		if (!strcmp("R_LLAVE",comp_lex[t.compLex])==0){
			attributes_list();
			match("R_LLAVE");
		}
		else{
			match("R_LLAVE");
		}
	} else{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(2,comp_lex[t.compLex]);
		if (volver == 1){
			object();
		}
	}
}

void element()
{		
		char lexema_tagName[TAMLEX];
		char lexema_elementList[TAMLEX];
		if (strcmp("L_LLAVE",comp_lex[t.compLex])==0)
		{
			object();
		}
		else if(strcmp("L_CORCHETE",comp_lex[t.compLex])==0)
		{	
			array();
		}
		else
		{	
			cantidadError = cantidadError + 1;
			printf("\nError Sintactico \n");
			volver = panicModeConSincronizacion(1,comp_lex[t.compLex]);
			if (volver == 1){
				element();
			}
		}
	
}

void attributes_list(){
	if(strcmp("LITERAL_CADENA",comp_lex[t.compLex])==0){
		attribute();
		a();
	}
	else{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(4,comp_lex[t.compLex]);
		if (volver == 1){
			attributes_list();
		}
	}
}

void element_list(){
	
	char lexema_tagName[TAMLEX];
	char lexema_elementList[TAMLEX];
	
	if(strcmp("L_CORCHETE",comp_lex[t.compLex])==0 || strcmp("L_LLAVE",comp_lex[t.compLex])==0){
		fprintf(salida, "\n");
		int s;
		for(s=0; s<n; s++){
			fprintf(salida, "\t");
		}
	    n++;
		fprintf(salida,"<item>");
		element();
		fprintf(salida, "\n");
		//int s;
		for(s=0; s<n-1; s++){
			fprintf(salida, "\t");
		}
		n--;
		fprintf(salida,"</item>");
		e1();
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(9,comp_lex[t.compLex]);
		if (volver == 1){
			element_list();
		}
	}
}	

void e1(){
	if (strcmp("COMA",comp_lex[t.compLex])==0){
		int s;
		match("COMA");
		fprintf(salida, "\n");
		for(s=0; s<n; s++){
			fprintf(salida, "\t");
		}
		n++;
		fprintf(salida,"<item>");
		element();
		fprintf(salida, "\n");
		//int s;
		for(s=0; s<n-1; s++){
			fprintf(salida, "\t");
		}
		n--;
		fprintf(salida,"</item>");
		e1();
	}else{
		bool existeSig = existeTokenSiguiente(10,comp_lex[t.compLex]);
		if(!existeSig){
			if (!strcmp(comp_lex[t.compLex],"EOF")==0){
				cantidadError = cantidadError + 1;
				printf("\nError Sintactico \n");
				scan();
				e1();
			}
		}
	}
}

void match(char* proximoToken){
	if (strcmp(comp_lex[t.compLex],proximoToken) == 0){
		if (!strcmp(comp_lex[t.compLex],"EOF") == 0){
			//printf("%s %s %s \n", comp_lex[t.compLex], t.pe->lexema, e.lexema);
			anaLex();
		}
	}
	else{
		cantidadError = cantidadError + 1;
		printf("\nError en linea %d se esperaba: %s, en vez de %s \n",numLinea, proximoToken, comp_lex[t.compLex]);
	}
	
}

void initConjuntosPrimero(){
	int i=0;
	//carga los conjuntos primeros en la matriz
	/*	0:json
		1:element
		2:object
		3:array
		4:attributes_list
		5:a
		6:attribute
		7:attribute_name
		8:attribute_value
		9:element_list
		10:e
	*/
		strcpy(conjuntosPrimero[1][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[1][1],"L_LLAVE");
		strcpy(conjuntosPrimero[2][0],"L_LLAVE");
		strcpy(conjuntosPrimero[3][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[4][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[5][0],"COMA");
		strcpy(conjuntosPrimero[5][1],"EMPTY");
		strcpy(conjuntosPrimero[6][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[7][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[8][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[8][1],"LITERAL_NUM");
		strcpy(conjuntosPrimero[8][2],"PR_TRUE");
		strcpy(conjuntosPrimero[8][3],"PR_FALSE");
		strcpy(conjuntosPrimero[8][4],"PR_NULL");
		strcpy(conjuntosPrimero[8][5],"L_CORCHETE");
		strcpy(conjuntosPrimero[8][6],"L_LLAVE");
		strcpy(conjuntosPrimero[9][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[9][1],"L_LLAVE");
		strcpy(conjuntosPrimero[10][0],"COMA");
		strcpy(conjuntosPrimero[10][1],"EMPTY");
}

void initconjuntosSiguiente(){
	int i=0;
	//carga los conjuntos siguiente en la matriz
	/*	0:json
		1:element
		2:object
		3:array
		4:attributes_list
		5:a
		6:attribute
		7:attribute_name
		8:attribute_value
		9:element_list
		10:e
	*/
	
		
	strcpy(conjuntosSiguiente[1][0],"EOF");
	strcpy(conjuntosSiguiente[1][1],"COMA");
	strcpy(conjuntosSiguiente[1][2],"R_LLAVE");
	strcpy(conjuntosSiguiente[1][3],"R_CORCHETE");
	strcpy(conjuntosSiguiente[2][0],"COMA");
	strcpy(conjuntosSiguiente[2][1],"EOF");
	strcpy(conjuntosSiguiente[2][2],"R_LLAVE");
	strcpy(conjuntosSiguiente[2][3],"R_CORCHETE");
	strcpy(conjuntosSiguiente[3][0],"COMA");
	strcpy(conjuntosSiguiente[3][1],"EOF");
	strcpy(conjuntosSiguiente[3][2],"R_LLAVE");
	strcpy(conjuntosSiguiente[3][3],"R_CORCHETE");
	strcpy(conjuntosSiguiente[4][0],"R_LLAVE");
	strcpy(conjuntosSiguiente[5][0],"R_LLAVE");
	strcpy(conjuntosSiguiente[6][0],"COMA");
	strcpy(conjuntosSiguiente[6][1],"R_LLAVE");
	strcpy(conjuntosSiguiente[7][0],"DOS_PUNTOS");
	strcpy(conjuntosSiguiente[8][0],"COMA");
	strcpy(conjuntosSiguiente[8][1],"R_LLAVE");
	strcpy(conjuntosSiguiente[9][0],"R_CORCHETE");
	strcpy(conjuntosSiguiente[10][0],"R_CORCHETE");

}


bool existeTokenPrimero(int produccion, char* tokenActual){
	
	int i=0;
	for(i=0; i<7; i++){
		//busca el token actual en la matriz de conjunto primero
		if(strcmp(tokenActual,conjuntosPrimero[produccion][i])==0){
			return true;
		}
	}
	return false;
}

bool existeTokenSiguiente(int produccion, char* tokenActual){
	
	int i=0;
	for(i=0; i<4; i++){
		//busca el token actual en la matriz de conjunto siguiente
		if(strcmp(tokenActual,conjuntosSiguiente[produccion][i])==0){
			return true;
		}
	}
	return false;
}	

int panicModeConSincronizacion(int produccion, char *tokenActual){
	
	bool existeSiguiente = existeTokenSiguiente(produccion,tokenActual);
	if(!existeSiguiente){
		bool existePrimero = existeTokenPrimero(produccion,tokenActual);
		if (!existePrimero){
			scan();
			return 1;
		}
	}else if(existeSiguiente || strcmp(tokenActual,"EOF")==0){
		pop();
		return 0;
	}

}

void scan(){
	int k = 0;
	int ban = 0;
	for(k=0; k<12; k++){
		if(strcmp(comp_lex[t.compLex],comp_lex[k])==0){
			ban = 1;
		}
	}
	if (ban == 0){
		//printf("En linea %d, error_lexico\n", numLinea);
	}else{
		printf("SCAN. En linea %d, token %s \n", numLinea, comp_lex[t.compLex]);
	}
	anaLex();
}

void pop(){
	printf("POP. En linea %d, token %s \n", numLinea, comp_lex[t.compLex]);
	//genera una produccion con EMPTY
}

void sacarComillas(char *original){
	char find = '\"';
	char *to = (char*) malloc(TAMLEX);
	strncpy(to, original+1, TAMLEX);
	const char *ptr = strchr(to, find);
	char *nuevo = (char*) malloc(TAMLEX);
	strncpy(nuevo,to,(ptr-to));
	//printf("%s",nuevo);
	fprintf(salida,"%s",nuevo);
	//printf("%s",nuevo);
	//return nuevo;
}
