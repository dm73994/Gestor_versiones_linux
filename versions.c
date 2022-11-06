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

//DOCUMENTAR
int addVersion(file_version newVersion);




int copy(char * source, char * destination) {
	//REFERENCIA A ARCHIVOS 
    	FILE*sourceFile;
       	FILE*destFile;

	//VERIFICAR SI SE PUEDEN ABRIR LOS ARCHIVOS
	sourceFile = fopen(source, "r");
      	destFile= fopen(destination, "w");	
    
	//SI OCURRIO UN ERROR CON EL ARCHIVO SOURCE
	if(sourceFile == NULL){
		//perror(sourceFile);
		return 0;
	}	

	//LEER ARCHIVO SOURCE Y ESCRIBIR INFO EN EL ARCHIVO DESTINO
	char str[BUFSIZ];
	while( fgets(str, 255,sourceFile) ){
		fwrite(str, sizeof(char), BUFSIZ, destFile);		
	}

    	fclose(sourceFile);	
	fclose(destFile);

	return 1;
}



return_code add(char * filename, char * comment) {
	//verificar si el archivo existe
	struct stat s;	
	
	// si stat devuelve -1 hay un error
	if( stat(filename, &s) == -1){		
		perror("stat");
		return 0;
	}
	
	//SI NO ES UN ARCHIVO REGULAR
	if( !S_ISREG(s.st_mode) ){		
		return 0;
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
		
		if( addVersion(file) == 1){
			return VERSION_ADDED;
		}
	}
	
	return VERSION_ERROR;
}

//**
// RECIBE una estructura archivo y la guarda en la bd
//**
int addVersion(file_version newVersion){
	FILE*db = fopen(VERSIONS_DB_PATH, "ab");
	fwrite(&newVersion, sizeof(file_version), 1, db);

	if(fwrite != 0){
		fclose(db);
		return 1;  // INGRESAR NUEVO ARCHIVO EXITOSO
	}else{
		fclose(db);
		return 0; //ERROR
	}
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
