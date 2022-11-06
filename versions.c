#include <limits.h>
#include <sys/stat.h>
#include "versions.h"


/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);

/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return 1 en caso de exito, 0 si existe error.
 */
int copy(char * source, char * destination);


// MODIFICARLO 
int copy(char * source, char * destination) {
    //Declaración de la referencia del archivo source
    FILE*f;
    //Declaracion de la referencia del archivo destino
    FILE*df;
    //Declaración del buffer donde se va a leer
    char buffer[BUFSIZ];
    //Abrir source en modo lectura
    f = fopen(source,"r");
    //Comprobar si se abrió source
    if(!f)
    {
    	//no abrio el archivo
        return 1;
    }
    //Crear y abrir el archivo destino, en modo escritura
    df = fopen(destination,"w");
    //Comprobar si se abrió el archivo
    if(!df)
    {
        // Se cierra el archivo abierto
        fclose(f);
        //Retornar bandera error
        return 0;
    }

    //Declaracion de la bandera de lineas leidas
    size_t nreads;
    while(!feof(f))
    {
        nreads = fread(buffer,sizeof(char), BUFSIZ,f);
        if(nreads == 0)
        {
            break;
        }
        fwrite(buffer, sizeof(char), nreads, df);

    }
    fclose(f);
    fclose(df);
    return 1;

}



return_code add(char * filename, char * comment) {
	//verificar si el archivo existe
	struct stat s;	
	
	// si stat devuelve -1 hay un error
	if( stat(filename, &s) == -1){
		printf("ERROR");
		perror("stat");
		return 1;
	}
	
	//SI NO ES UN ARCHIVO REGULAR
	if( !S_ISREG(s.st_mode) ){
		printf("entro");
		return 1;
	}
	
	//hash del archivo
	char*hash = (char*) malloc(HASH_SIZE);
	get_file_hash(filename, hash);
	
	// TODO VERIFICAR HASH
	
	//generar nuevo nombre del archivo
	char*nuevoNombre = (char*) malloc(strlen( VERSIONS_DIR ) + HASH_SIZE + 1);
	strcpy(nuevoNombre, VERSIONS_DIR);
	strcat(nuevoNombre, "/");
	strcat(nuevoNombre, hash);
	
	if( copy(filename, nuevoNombre) == 1 ){		
		file_version file;
		strcpy(file.filename, filename);
		strcpy(file.hash, hash);
		strcpy(file.comment, comment);
		
			
		return VERSION_ADDED;
	}

	
	return VERSION_ERROR;
}


void list(char * filename) {
	//TODO implementar
}

char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;

}


return_code get(char * filename, int version) {

	//TODO implementar
	return VERSION_DOES_NOT_EXIST;
}
