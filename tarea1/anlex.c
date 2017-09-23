/*********** Inclusión de cabecera **************/
#include "anlex.h"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente JSON
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea



/**************** Funciones **********************/


// Rutinas del analizador lexico


/*Esta Runtina es para comprobar si existe o no el "Output.txt"*/
int comprobarOutput(){
	FILE *output;
	output = fopen("output.txt","r");
	if(output != NULL){
		return 0;
	}else{
		return 1;
	}
}

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}


void lexer(){
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	static int rep = 0; //Controla si se esta usando el Output actual
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
		if (comprobarOutput()==0 && rep==0){ //Si existe el archivo y no se a usado todavia el Output
			remove("output.txt");
			freopen("output.txt", "a",stdout);
			rep=1;

		}else if(comprobarOutput()==0 && rep==1){//Si existe el archivo y se esta usando el Output
			freopen("output.txt", "a",stdout);

		}else if(comprobarOutput()==1){//Si no existe
			freopen("output.txt", "a",stdout);
			rep=1;
		}

		if (c==' ' || c=='\t')
			continue;

		else if(c=='\n')
		{
			//se incrementa el numero de linea
			numLinea++;
			continue;
		}
		else if (isalpha(c))
		{
			//corresponde a un identificador
			i=0;
			do{
				id[i]= tolower(c);
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error(msg);
			}while(isalpha(c));
			id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            else
                c=0;
            if (strcmp(id,"true")==0 || strcmp(id,"false")==0 || strcmp(id,"null")==0){
                t.pe=buscar(id);
                t.compLex=t.pe->compLex;
            }else{
                error(msg);
                lexer();
            }
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
				e.compLex=cadena;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=cadena;
			}
			break;
		}
		else if (isdigit(c))
		{
				//corresponde a un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;

					case 1://llega un punto, debe seguir un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							estado=-1;
						}
						break;
					case 2://llega una fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://llega una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=-1;
						}
						break;
					case 4://debe venir necesariamente por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=-1;
						}
						break;
					case 5://llega una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,id);
							e.compLex=numero;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=numero;
						break;
					case -1:
						if (c==EOF)
							error(msg);
						else
                            ungetc(c,archivo);
                            error(msg);
                            lexer();
                        acepto=1;
						break;
					}
				}
			break;
		}
		else if (c==':')
		{
			t.compLex=dospuntos;
            t.pe=buscar(":");
            break;
		}
		else if (c==',')
		{
			t.compLex=coma;
			t.pe=buscar(",");
			break;
		}
		else if (c=='{')
		{
			if ((c=fgetc(archivo))=='*')
			{
				while(c!=EOF)
				{
					c=fgetc(archivo);
					if (c=='*')
					{
						if ((c=fgetc(archivo))=='}')
						{
							break;
						}
					}
					else if(c=='\n')
					{
						numLinea++;
					}
				}
				if (c==EOF)
					error(msg);
				continue;
			}
			else
			{
				ungetc(c,archivo);
				t.compLex=lllave;
				t.pe=buscar("{");
			}
			break;
		}
		else if (c=='}')
		{
			t.compLex=rllave;
			t.pe=buscar("}");
			break;
		}
		else if (c=='[')
		{
			t.compLex=lcorchete;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=rcorchete;
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"')
		{//puede ser un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"')
				{
                    break;
				}
				else if((c==EOF) || (c=='\n'))
				{
					error(msg);
                    break;
				}else{
					id[i]=c;
					i++;
				}
			}while(!(c=='\"') || (c!='\n'));
			id[i]='\0';
			if ((c!=EOF) && (c!='\"')){
				ungetc(c,archivo);
                lexer();
			}
			if (c=='\"'){
                t.pe=buscar(id);
                t.compLex=t.pe->compLex;
			}
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
				if (strlen(id)==3 || strcmp(id,"''''")==0)
					e.compLex=cadena;
				else
					e.compLex=cadena;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=e.compLex;
			}
			break;
		}
		else if (c=='{')
		{
			while(c!=EOF)
			{
				c=fgetc(archivo);
				if (c=='}')
					break;
				else if(c=='\n')
				{
					numLinea++;
				}
			}
			if (c==EOF)
				error(msg);
		}
		else if (c!=EOF)
		{
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
}


int main(int argc,char* args[])
{
	// inicializar analizador lexico

	iniciarTabla();
	iniciarTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			lexer();
			printf("Lin %d: %s -> %d \n",numLinea,t.pe->lexema,t.compLex);
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}



