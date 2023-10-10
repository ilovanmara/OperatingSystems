#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


int list(char *path, int size_smaller, int havePermission, char *permission)
{   
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statBuf;
    char perm[10];
    
    //in caz ca am deja / pus la final
   if (path[strlen(path) - 1] != '/'){
	int len = strlen(path);
        path[len] = '/';
        path[len+1] = '\0';
}		
    dir = opendir(path);
    if(dir == NULL) {
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    printf("SUCCESS\n");

         while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                
                snprintf(filePath, 512, "%s%s", path, entry->d_name);
            
                if (lstat(filePath, &statBuf) == 0){
                      if(S_ISREG(statBuf.st_mode) || S_ISDIR(statBuf.st_mode)|| S_ISLNK(statBuf.st_mode)){
                         if(size_smaller == -1 && havePermission == -1){
                            printf("%s\n", filePath);
                        }
                        if(size_smaller != -1 && statBuf.st_size < size_smaller && S_ISREG(statBuf.st_mode) )
                            printf("%s\n", filePath);

                         if(havePermission != -1){
                            /*la transformarea in string de permissions m-am inspirat de pe internet*/
                            if (statBuf.st_mode & S_IRUSR ) 
                                perm[0] = 'r'; 
                            else perm[0] = '-';
                            if (statBuf.st_mode & S_IWUSR)  
                                perm[1] = 'w';
                            else perm[1] = '-';
                            if(statBuf.st_mode & S_IXUSR)  
                                perm[2] = 'x'; 
                            else  perm[2] = '-';
                            if(statBuf.st_mode & S_IRGRP)  
                                perm[3] = 'r'; 
                            else perm[3] = '-';
                            if(statBuf.st_mode & S_IWGRP)  
                                perm[4] = 'w'; 
                            else perm[4] = '-';
                            if(statBuf.st_mode & S_IXGRP)  
                                perm[5] = 'x' ; 
                            else perm[5] = '-';
                            if(statBuf.st_mode & S_IROTH)  
                                perm[6] = 'r'; 
                            else perm[6] = '-';
                            if(statBuf.st_mode & S_IWOTH)  
                                perm[7] = 'w'; 
                            else perm[7] = '-';
                            if(statBuf.st_mode & S_IXOTH)  
                                perm[8] = 'x'; 
                            else perm[8] = '-';
                            perm[9] = '\0';
                          
                            if(strcmp(perm, permission) == 0)
                                printf("%s\n", filePath);

                        } 
                   } 
                }
            }
        }
    closedir(dir);
    return 0;
}

void list_rec(char *path, int size_smaller, char* permission, int havePermission)
{   
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statBuf;

   if (path[strlen(path) - 1] != '/'){
        int len = strlen(path);
        path[len] = '/';
        path[len+1] = '\0';
   }

   char perm[10];

    dir = opendir(path);
    if(dir == NULL) {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

         while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                
                snprintf(fullPath, 512, "%s%s", path, entry->d_name);
            
                if (lstat(fullPath, &statBuf) == 0){
                    if(S_ISREG(statBuf.st_mode) || S_ISDIR(statBuf.st_mode) || S_ISLNK(statBuf.st_mode)){
                         if(size_smaller == -1 && havePermission == -1 ){
                            printf("%s\n", fullPath);
                        }
                        if(size_smaller != -1 && statBuf.st_size  < size_smaller && S_ISREG(statBuf.st_mode))
                            printf("%s\n", fullPath);
                        //rintf("%d\n", havePermission)
                        if(havePermission != -1){
                            //*la transformarea in string de permissions m-am inspirat de pe internet*/
                            if (statBuf.st_mode & S_IRUSR ) 
                                perm[0] = 'r'; 
                            else perm[0] = '-';
                            if (statBuf.st_mode & S_IWUSR)  
                                perm[1] = 'w';
                            else perm[1] = '-';
                            if(statBuf.st_mode & S_IXUSR)  
                                perm[2] = 'x'; 
                            else  perm[2] = '-';
                            if(statBuf.st_mode & S_IRGRP)  
                                perm[3] = 'r'; 
                            else perm[3] = '-';
                            if(statBuf.st_mode & S_IWGRP)  
                                perm[4] = 'w'; 
                            else perm[4] = '-';
                            if(statBuf.st_mode & S_IXGRP)  
                                perm[5] = 'x' ; 
                            else perm[5] = '-';
                            if(statBuf.st_mode & S_IROTH)  
                                perm[6] = 'r'; 
                            else perm[6] = '-';
                            if(statBuf.st_mode & S_IWOTH)  
                                perm[7] = 'w'; 
                            else perm[7] = '-';
                            if(statBuf.st_mode & S_IXOTH)  
                                perm[8] = 'x'; 
                            else perm[8] = '-';
                             perm[9] = '\0';
                            if(strcmp(perm, permission) == 0)
                                printf("%s\n", fullPath);

                        } 
                   } 
                    if (S_ISDIR(statBuf.st_mode)){ 
                        list_rec(fullPath , size_smaller, permission, havePermission);
                    }
                }
            }
        }
    closedir(dir);
}

int parse(char* path, int *findall_types, int afis){
    int fd;
    char magic[3];
    int version = 0;
    int headerSize = 0;
    int noOfSections = 0;
    int verifyClose = 0;

    //char c;
    //magic = b5 -validat
    //version apartine 101 , 141
    // nr de sectiuni intre 2 si 19
    //tipurile sect: 41 75 94 90 32
    *findall_types = 0;
	
    fd = open(path, O_RDONLY);
    if(fd == -1) {
        printf("ERROR\ninvalid path\n");
        return 1;
    }
   
    lseek(fd, 0, SEEK_SET);

    read(fd, magic, 2);
    magic[2] = '\0';
    if(strcmp(magic, "b5") != 0){
        if(afis == 0)
       	    printf("ERROR\nwrong magic\n");
       	//return 1;
        verifyClose = 1;
     }

    lseek(fd, 2, SEEK_SET);
    read(fd, &headerSize,2);
      
    lseek(fd, 4, SEEK_SET);
    read(fd, &version, 4);
      
    if((version > 141 || version < 101) ){
        if(verifyClose == 0){
        if(afis == 0)
       	    printf("ERROR\nwrong version\n");
        //return 1;
        }
        verifyClose = 1;
    }

    lseek(fd, 8, SEEK_SET);
    read(fd, &noOfSections, 1);
    
    if ((noOfSections < 2 || noOfSections > 19)){
        if(afis == 0 && verifyClose == 0)
            printf("ERROR\nwrong sect_nr\n");
        //return 1;
        verifyClose = 1;
    }
    char sectName[noOfSections][64];
    int sectType[noOfSections];
    int sectOffset[noOfSections];
    int sectSize[noOfSections];

    for(int i = 0; i < noOfSections; i++){

        lseek(fd, 9 + i * 18, SEEK_SET);
        char aux[64];
        read(fd, aux, 6);
        sprintf(sectName[i], "%s", aux);

        lseek(fd, 15 + i * 18, SEEK_SET);
        read(fd, &sectType[i], 4);
        if(sectType[i] == 75){
            (*findall_types)++;
        }

        lseek(fd, 19 + i * 18, SEEK_SET);
        read(fd, &sectOffset[i], 4);

        lseek(fd, 23 + i * 18, SEEK_SET);
        read(fd, &sectSize[i], 4);

        if(sectType[i] != 41 && sectType[i] != 75 && sectType[i] != 94 && sectType[i] != 90 && sectType[i] != 32 ){
            if(afis == 0 && verifyClose == 0) 
                printf("ERROR\nwrong sect_types\n");
             //return 1;
            verifyClose = 1;
        }
    }
    if(afis == 0 && verifyClose == 0){
        printf("SUCCESS\n");
        printf("version=%d\n", version);
        printf("nr_sections=%d\n", noOfSections);
        for(int i = 0; i < noOfSections; i++){
            printf("section%d: %s %d %d\n", i+1, sectName[i], sectType[i], sectSize[i]);
        }
    } 

    close(fd);
    if (verifyClose == 1)
        return 1;
    return 0;
}

void findall(char *path){

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statBuf;

   if (path[strlen(path) - 1] != '/'){
        int len = strlen(path);
        path[len] = '/';
        path[len+1] = '\0';
   }
   
    dir = opendir(path);
     if(dir == NULL) {
        printf("ERROR\ninvalid directory path\n");
        return;
    }

         while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                
                snprintf(fullPath, 512, "%s%s", path, entry->d_name);
            
                if (lstat(fullPath, &statBuf) == 0){
                    if(S_ISREG(statBuf.st_mode) || S_ISLNK(statBuf.st_mode)){
                        int types = 0;
                        if(parse(fullPath, &types, 1) == 0){
                            if(types >= 2){
                                printf("%s\n", fullPath);
                            }
                        }
                   } 
                    if (S_ISDIR(statBuf.st_mode)){ 
                        findall(fullPath);
                    }
                }
            }
        }
    closedir(dir);
}

int extract(char *path, int line, int section){
    int type;
    if(parse(path, &type, 1) != 0){
        printf("ERROR\ninvalid file\n");
        return 1;
    }
       int fd;
    char magic[3];
    int version = 0;
    int headerSize = 0;
    int noOfSections = 0;

    fd = open(path, O_RDONLY);
    if(fd == -1) {
        perror("ERROR\ninvalid path\n");
        return 1;
    }
   
    lseek(fd, 0, SEEK_SET);

    read(fd, magic, 2);
    magic[2] = '\0';

    lseek(fd, 2, SEEK_SET);
    read(fd, &headerSize,2);
      
    lseek(fd, 4, SEEK_SET);
    read(fd, &version, 4);

    lseek(fd, 8, SEEK_SET);
    read(fd, &noOfSections, 1);

    char sectName[noOfSections][64];
    int sectType[noOfSections];
    int sectOffset[noOfSections];
    int sectSize[noOfSections];

    int sectionOf = 0;
    int sectionSize = 0;
    char lines[64][101000];

    for(int i = 0; i < noOfSections; i++){

        lseek(fd, 9 + i * 18, SEEK_SET);
        char aux[64];
        read(fd, aux, 6);
        sprintf(sectName[i], "%s", aux);

        lseek(fd, 15 + i * 18, SEEK_SET);
        read(fd, &sectType[i], 4);
       
        lseek(fd, 19 + i * 18, SEEK_SET);
        read(fd, &sectOffset[i], 4);

        lseek(fd, 23 + i * 18, SEEK_SET);
        read(fd, &sectSize[i], 4);

        if((i+1) == section){
            sectionOf = sectOffset[i];
            sectionSize = sectSize[i];
        }
    }
    if(sectionOf == 0){
        printf("ERROR\ninvalid section\n");
    }
    
    int finalPos = lseek(fd, sectionOf + sectionSize, SEEK_SET);
    lseek(fd, sectionOf, SEEK_SET);
    int k = 0;
    int i = 0;
 
    while(finalPos != lseek(fd, 0, SEEK_CUR)){
        if(read(fd, &lines[k][i], 1) > 0){
            if(lines[k][i] != '\n'){
                i++;
            }else{
                lines[k][i] = '\0';
                k++;
                //lines[k][i] = '\0';
                i = 0;
            }
        }
    }
    if(lines[k][i] != '\0'){
        lines[k][i]=0;
    }

    int j = 1;
    int len = k;
    int printOk = 0;

    while(j <= line && j <= len){
        if(j == line){
            printf("SUCCESS\n");
            printf("%s", lines[k]);
            printOk = 1;
        }
        k--;
        j++; 
    }
    if(printOk == 0){
        printf("ERROR\ninvalid line\n");
    }
    close(fd);
    return 0;

}

int main (int argc, char **argv){
    int lis = 0;
    int pars = 0;
    int extr = 0;
    int find = 0;
    if(argc < 2){
        return 1;
    } else if(strcmp(argv[1], "variant") == 0){
            printf("99320");
    } //else if(strcmp(argv[1], "list") == 0){
    	else if (argc >= 3){
    
    	for (int i = 1; i < argc; i++){
    	    if(strcmp(argv[i], "list") == 0)
    	     	lis = 1;
    	    if(strcmp(argv[i], "parse") == 0)
    	     	pars = 1;
            if(strcmp(argv[i], "extract") == 0)
    	     	extr = 1;
            if(strcmp(argv[i], "findall") == 0)
    	     	find = 1;
	    }
	    if (lis == 1){
            int i = 2;
            //char* rec = "nerec";
        	int rec = 0;
        	int size_smaller = -1;
        	char permission[11];
        	int havePermission =-1;
       
        	char* path;
        	path = NULL;
        
        	for(i = 1; i < argc; i++){
            	if((strcmp(argv[i], "recursive")) == 0){
                	//strcpy(rec, "rec");
                	rec = 1;
            	}
            	if(strncmp(argv[i], "size_smaller=", 13) == 0){
                	size_smaller = 0;
                	int len = strlen(argv[i]);
                	for(int k = 13; k < len; k++){
                    		size_smaller = size_smaller*10 +  (argv[i][k] - '0');
                	}
            	}	 
            	if (strncmp (argv[i], "permissions=", 12) == 0){
               		strcpy(permission , argv[i] + 12);
               		havePermission = 1;
            	}
            	else if (strncmp (argv[i], "path=", 5) == 0){
               		path = argv[i];
               		path = path + 5;
            	}
        	}
        	struct stat statbuf;
      
        	if (rec == 0){ 
			    list(path, size_smaller, havePermission, permission);
		    }
            
        	if (rec == 1){ 
            	if(lstat(path, &statbuf)== 0 && S_ISDIR(statbuf.st_mode)){
				    printf("SUCCESS\n");
				    //list_rec(path, size_smaller, permission, havePermission);
		        }
                list_rec(path, size_smaller, permission, havePermission);
        	}
    	}else if (pars == 1){
    	    char* path;
            path = NULL;
    	    if (strncmp (argv[2], "path=", 5) == 0){
                path = argv[2];
                path = path + 5;
            } else if (strncmp (argv[1], "path=", 5) == 0){
                path = argv[1];
                path = path + 5;
            }
            int types;
            parse(path, &types, 0);  
    	
        } else if (extr == 1){
            int line = 0;
            int section = 0;
            char* path;
            path = NULL;
            for(int i = 1; i < argc; i++){
                 if (strncmp (argv[i], "path=", 5) == 0){
               		path = argv[i];
               		path = path + 5;
            	}
                if (strncmp (argv[i], "section=", 8) == 0){
               		int len = strlen(argv[i]);
                	for(int k = 8; k < len; k++){
                    		line = line*10 +  (argv[i][k] - '0');
                	}
            	}
                if (strncmp (argv[i], "line=", 5) == 0){
               		int len = strlen(argv[i]);
                	for(int k = 5; k < len; k++){
                    		section = section*10 +  (argv[i][k] - '0');
                	}
            	}
        	}
            extract(path, section, line);

        }else if (find == 1){
    	    char* path;
            path = NULL;
    	    if (strncmp (argv[2], "path=", 5) == 0 ){
                path = argv[2];
                path = path + 5;
            } else  if (strncmp (argv[1], "path=", 5) == 0 ){
                path = argv[1];
                path = path + 5;
            }
            struct stat statbuf;
            if(lstat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)){
				    printf("SUCCESS\n");
				    //findall(path);
		    }
            findall(path);
        } 
 
    }
        
    return 0;
}
  
