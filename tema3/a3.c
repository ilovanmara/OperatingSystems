#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define FIFO_NAME "RESP_PIPE_99320"
#define FIFO_NAME2 "REQ_PIPE_99320"

int main()
{
    int fd1 = -1, fd2 = -1;
    int validare = 0;
    if(mkfifo(FIFO_NAME, 0600) != 0) {
        printf("ERROR\n");
        printf("cannot create the response pipe");
        return 2;
    }

    fd2 = open(FIFO_NAME2, O_RDONLY);
    if(fd2 == -1) {
        validare++;
        printf("ERROR\n");
        printf("cannot open the request pipe");
        return 1;
    }   

    fd1 = open(FIFO_NAME, O_WRONLY);
    if(fd1 == -1) {
        validare++;
        printf("ERROR\n");
        printf("cannot open the response pipe");
        return 1;
    }
    char msg[] = "BEGIN$";
    write(fd1, msg, 6);
   
    //if(validare == 0)
        //printf("SUCCESS");
        
    char* variant = "VARIANT$";
    char* echo = "ECHO$";
    char* success = "SUCCESS$";
    char* error = "ERROR$";
    unsigned int offset;
    unsigned int value;
    char file_name[256];
    char* name;
    unsigned int offsetFile;
    unsigned int numBytes;
    
    unsigned int var = 99320;
    volatile unsigned int *shared;
    volatile char *sharedFile;
    int fileSize = 0;
    int shm;
     for(;;){
        char req_msg[265];
        char c = 'c';
        int i = 0;
        while(c != '$'){
            read(fd2, &c, 1);
            req_msg[i++] = c;
        }
        req_msg[i] = '\0';
        printf("%s\n", req_msg);
        
        if(strcmp(req_msg, "ECHO$") == 0){
            write(fd1, echo, 5);
            write(fd1, variant, 8);
            write(fd1, &var,sizeof(unsigned int));
            printf("%s %s %d", echo, variant, var);

        } else  if(strcmp(req_msg, "CREATE_SHM$") == 0){
            unsigned int numberOct;
            read(fd2, &numberOct, sizeof(unsigned int));
            shm = shm_open("/6M9tWh5Q", O_CREAT | O_RDWR, 0664);
            if(shm < 0) {
                write(fd1, &req_msg, 11);
                write(fd1, error, 6);
            } else
            ftruncate(shm, 3195537);
            shared = mmap(0, 3195537, PROT_WRITE, MAP_SHARED, shm, 0); 
             if(shared == (void*)-1) {
                write(fd1, &req_msg, 11);
                write(fd1, error, 6);
            }
            write(fd1, &req_msg, 11);
            write(fd1, success, 8);
            
        } else  if(strcmp(req_msg, "WRITE_TO_SHM$") == 0){
            read(fd2, &offset, sizeof(unsigned int));
            read(fd2, &value, sizeof(unsigned int));
            if(offset >= 0 && (offset + 4) <= 3195537){
                shared[offset/4] = value;
                printf("\n%d ", shared[offset]);
                write(fd1, req_msg, 13);
                write(fd1, success, 8);
            } else{
                write(fd1, req_msg, 13);
                write(fd1, error, 6);
            }
            
        } else if(strcmp(req_msg, "MAP_FILE$") == 0){
            //read(fd2, file_name, )
            char ch = 'a';
            int k = 0;
            //file_name[k++] = '/';
            while(ch != '$'){
                read(fd2, &ch, 1);
        
                    file_name[k++] = ch;
            }
            file_name[k-1] = '\0';
            name = file_name;
            printf("%s %s", file_name, name);
            int fd_map_file = -1;
            
            fd_map_file = open(name, O_RDONLY);
            printf("%d", fd_map_file);

            if(fd_map_file == -1){
                write(fd1, req_msg, 9);
                write(fd1, error, 6);
                printf("cannot open file");
            } else{

            fileSize = lseek(fd_map_file, 0, SEEK_END);
            lseek(fd_map_file, 0, SEEK_SET);
          
            sharedFile = (volatile char*)mmap(NULL, fileSize, PROT_READ, MAP_SHARED, fd_map_file, 0);
            if(sharedFile == (void*)-1){
                 write(fd1, req_msg, 9);
                 write(fd1, error, 6);
                 printf("cannot open map");
            } else{
                write(fd1, req_msg, 9);
                write(fd1, success, 8);
            }
            //munmap(sharedFile, fileSize);
            }
        } else if(strcmp(req_msg, "READ_FROM_FILE_OFFSET$")== 0){
            read(fd2, &offsetFile, sizeof(unsigned int));
            read(fd2, &numBytes, sizeof(unsigned int));
            
            if(offsetFile + numBytes > fileSize){
                write(fd1, req_msg, 22);
                write(fd1, error, 6);
            }
            else{
                write(fd1, req_msg, 22);
                write(fd1, success, 8);
            }
       }

        else if(strcmp(req_msg, "EXIT$") == 0){
            close(fd1);
            unlink(FIFO_NAME);
            break;
        } else break;
    
    }
    close(fd1);
    close(fd2);

    unlink(FIFO_NAME);
    unlink(FIFO_NAME2);

    return 0;
}