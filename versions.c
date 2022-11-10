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

/**
 * @brief registra una estructura archivo en la base de datos
 * 
 * @param newVersion estructura que será almacenada
 * @return int 1 si se pudo sobreescribir, en cualquier otro caso será un error
 */
int addVersion(file_version newVersion);

/**
 * @brief imprime en consola la estructura de un archivo
 * 
 * @param version estructura del archivo que queremos mostrar en consola
 */
void printVersionStruct(file_version version);

/**
 * @brief Verifica si un hash y un nombre ya existen o no en la base de datos y en la carpeta de versiones
 * 
 * @param verifyHash hash a buscar
 * @param filename nombre de archivo a buscar
 * @return return_code  HASH_NAME_ALREADY_EXIST || HASH_EXIST || HASH_DOESNT_EXIST
 */
return_code checkVersion(char*verifyHash, char*filename);


int copy(char * source, char * destination) {
	//REFERENCIA A ARCHIVOS 
    FILE*sourceFile;
    FILE*destFile;

	//VERIFICAR SI SE PUEDEN ABRIR LOS ARCHIVOS
	sourceFile = fopen(source, "r");
    destFile = fopen(destination, "w");
    
	//SI OCURRIO UN ERROR CON EL ARCHIVO SOURCE
	if(sourceFile == NULL){
		return OPEN_FILE_ERROR;
	}	


    //Declaración del buffer donde se va a almacenar temporalmente una parte de lo leido
    char buffer[BUFSIZ];
    //Declaracion de la bandera de lineas leidas
    size_t nreads;
    while(!feof(sourceFile))
    {
        nreads = fread(buffer,sizeof(char), BUFSIZ, sourceFile);
        if(nreads == 0) break;
        
        fwrite(buffer, sizeof(char), nreads, destFile);
    }

	// cerrar los archivos
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
	
	
	//generar nuevo nombre del archivo
	char*nuevoNombre = (char*) malloc(strlen( VERSIONS_DIR ) +  + HASH_SIZE + 1);
	strcpy(nuevoNombre, VERSIONS_DIR);
	strcat(nuevoNombre, "/");
	strcat(nuevoNombre, hash);
	
	//SE CREA LA POSIBLE ESTRUCTURA HA ALMACENAR
	file_version file;
	strcpy(file.filename, filename);
	strcpy(file.hash, hash);
	strcpy(file.comment, comment);


	return_code verify = checkVersion(hash, filename);

	if( verify == HASH_DOESNT_EXIST ){
		if( copy(filename, nuevoNombre) == 1 ){		
			if( addVersion(file) == 1) return VERSION_ADDED;
		}
	}
	else if(verify == HASH_EXIST){
		if( addVersion(file) == 1) return VERSION_ADDED;
	}
	
	return HASH_NAME_ALREADY_EXIST;
}

//return 1 si la version ya exixst, 3 si ocurrio un error en la carpeta, 0 successfull
//
return_code checkVersion(char*hash, char*filename){
	FILE*db = fopen(VERSIONS_DB_PATH, "r");

	if(db == NULL) return OPEN_FILE_ERROR;
	
	file_version vStruct;
	int hashFlag = 0;
	while( fread(&vStruct, sizeof(file_version), 1, db) ){ // LEEMOS LA BASE DE DATOS
		if( EQUALS(vStruct.hash, hash) ){ // SE ENCUENTRA EL HASH
			if( EQUALS(vStruct.filename, filename) ){	// HASH Y NOMBRE EXISTENTES
				return HASH_NAME_ALREADY_EXIST;
			}
			hashFlag = 1; // HASH EXISTENTE PERO NOMBRE NO
		}
	}

	if(hashFlag == 1) return HASH_EXIST; //EL HASH EXISTE IGNORAMOS SI EL NOMBRE EXISTE, LA ESTRUCTURA DEBE SER ALMACENADA EN LA BS

	return HASH_DOESNT_EXIST; // LA ESTRUCTURA DEBE SE ALMACENADA EN DB Y GENERAR ARCHIVO EN /.VERSIONS
}

int addVersion(file_version newVersion){
	// abrir para excritura en formato append
	FILE*db = fopen(VERSIONS_DB_PATH, "ab");

	// sino se pudo abrir la bd
	if(db == NULL) return OPEN_FILE_ERROR;
	
	//escribir en la bd
	fwrite(&newVersion, sizeof(file_version), 1, db);

	return fwrite != 0 ? 1 : 0;
}


void list(char * filename) {
	//abrir la base de datos
	FILE*db = fopen(VERSIONS_DB_PATH, "r");

	//verificar si se abrio la base de datos
	if(db == NULL) 	return;

	// se almacena temporarmente la estructura leida en version
	file_version versionStruc;
	
	//SI EL PARAMETRO ARCHIVO ES NULL LISTAR TODOS LOS BACKUP
	if(filename == NULL){
		while( fread(&versionStruc, sizeof(file_version), 1, db) ) printVersionStruct(versionStruc);
		fclose(db);
		return;
	}

	//leer la bd
	int counter = 1;
	while( fread(&versionStruc, sizeof(file_version), 1, db) ){
		// SI EL PARAMETRO DE NOMBRE DE ARCHIVO COINCIDE EN UNA ESTRUCTURA DE LA BASE DE DATOS:
		if(EQUALS(versionStruc.filename, filename)) {
			printf("V.%d \t", counter);
			printVersionStruct(versionStruc);
			counter++;
		}
	}
	fclose(db);
}

void printVersionStruct(file_version version){	
	printf("[FILENAME]  %s\t", version.filename);	
	printf("[HASH] %.6s ... %s \t", version.hash, version.hash + strlen(version.hash)-4);
	printf("[COMMENTS]  %s ", version.comment);
	printf("\n");
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
	FILE*db = fopen(VERSIONS_DB_PATH, "r");
	
	//verificar archivo
	if(db == NULL)	return OPEN_FILE_ERROR;
	
	file_version versionStruct;
	//fread(&versionStruct, sizeof(file_version), 1, db);
	int count = 1;
	int nameFlag = 0;
	while( fread(&versionStruct, sizeof(file_version), 1, db) ){
		if( EQUALS(versionStruct.filename, filename)){			
			nameFlag = 1;	
			if(count == version){
				// ruta del archivo a recuperar
				char*path = (char*)malloc(strlen(VERSIONS_DIR) + HASH_SIZE + 2);
				strcpy(path, VERSIONS_DIR);
				strcat(path, "/");
				strcat(path, versionStruct.hash);
								
				//ruta de la carpeta raiz de destino
				char*destFile = (char*)malloc(strlen(versionStruct.filename) + 2);
				strcpy(destFile, "./");
				strcat(destFile, versionStruct.filename);

				copy(path, destFile);

				fclose(db);
				return VERSION_RECOVERY;
			}else count++;		
		}
	}
	
	fclose(db);

	if(nameFlag == 0) return FILENAME_DOESNT_EXIST;
	
	return FILE_VERSION_DOESNT_EXIST;
}

void version(char*filename){
	char*hash = (char*)malloc(HASH_SIZE);
	get_file_hash(filename, hash);

	FILE*db = fopen(VERSIONS_DB_PATH, "r");

	if(db == NULL){
		perror("Abrir base de datos");
		exit(1);
	}

//	leer la base de datos
	file_version version;
	int counter = 1;
	while(fread(&version, sizeof(file_version), 1, db)){
		if( EQUALS(version.hash,hash) ){
			printf("[FILENAME] %s\t[VERSION] %d\n", filename, counter);
			exit(0);
		}
		counter++;
	}	
	printf("Asegurese de agregar esta version del archivo %s a la base de datos..\n", filename);
	exit(0);
}
