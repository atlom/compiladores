/*---------- 	Lexer     ----------*/

/**---------- Cabecera ----------**/

#include "anlex.h"

#ifdef __linux__
#define AUX "/dev/tty"
#else 
#define AUX "CON"
#endif 

/**---------- Variables globales que se utilizaran ----------**/

int consumir;

char cad[5*TAMLEX];
FILE *archivo; // Fuente en JSON
FILE *salida; 
char buff[2*TAMBUFF];	// Buffer para lectura 
char id[TAMLEX];		// Utilizado por el lexer
int delantero=-1;		// Utilizado por el lexer
int fin=0;				// Utilizado por el lexer


		// Numero de Linea

/**---------- Funciones ----------**/

void error(const char* mensaje)
{
	printf("\nError_Lexico en linea %d, lexer %s \n ",numLinea, id);
	
}

void anaLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];

	while((c=fgetc(archivo))!=EOF)
	{

		if (c==' '){
			//fprintf(salida, " ");	//imprimir espacios en blanco
		}
        else if (c=='\t'){

			//fprintf(salida, "\t"); 
              // imprimir tabulaciones
		}
		else if(c=='\n')
		{
			//incrementar el numero de linea
			//fprintf(salida, "\n"); 
			//printf("%d", numLinea);
			numLinea++;
			continue;
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
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
                anaLex();
            }
			if (t.pe->compLex==opa)
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
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia de digitos, puede venir . o e
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

					case 1://un punto, debe seguir un digito
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
							estado=11;
						}
						break;
					case 2://una fraccion decimal, pueden seguir los digitos o e
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
					case 3://una e, puede seguir +, - o una secuencia de digitos
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
							estado=11;
						}
						break;
					case 4://debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=11;
						}
						break;
					case 5://una secuencia de digitos que corresponde al exponente
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
						if (t.pe->compLex==opa)
						{
							strcpy(e.lexema,id);
							e.compLex=numero;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=numero;
						break;
					case 11:
						if (c==EOF)
							error(msg);
						else
                            ungetc(c,archivo);
                            error(msg);
                            anaLex();
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
		{//un caracter o una cadena de caracteres
			i=0;
			
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
                anaLex();
			}
			if (c=='\"'){
                t.pe=buscar(id);
                t.compLex=t.pe->compLex;
			}
			if (t.pe->compLex==opa)
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
		t.compLex=opa;
	
	}
}

int main(int argc,char* args[])
{
	// inicializar lexer
	iniciarTabla();
	iniciarTablaSimbolos();
	iniciarComlex();
	initconjuntosSiguiente();
	initConjuntosPrimero();
    int g = 0;
	char d = 0;
	

	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
			g = 1;
		}
		
		salida = fopen("output.xml", "w"); // crea el archivo de salida output.txt
		numLinea = 1;
		anaLex();
		descendenteRecursivo();
			
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
		g = 1;
		
	}
	fclose(archivo);
	fclose(salida);
	return 0;
	
}

