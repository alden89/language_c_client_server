#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

int dayofweek(int d, int m, int y);
int convert_to_int(const char* str);
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1, d, m, y;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char dow[9] = {0};
    char message[100];
    char day[2], month[2], year[4];    
    char *p;
    int port;

    if(argc != 2)
    {
        printf("\n Uso: %s <puerto>\n",argv[0]);
        return 1;
    }

    errno = 0;
    long conv = strtol(argv[1], &p, 10);

    if (errno != 0 || *p != '\0' || conv > 65535 || conv < 1) {
        printf("\n Error : Debe pasarse como parametro un número de puerto entre 1-65535 \n");
        return 1;
    } else 
    {
        port = conv;    
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
          
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Servidor listo...\n");

    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket , buffer, 1024);
        //printf("%s\n",buffer );

        strcpy(day,strtok(buffer , "/"));
        strcpy(month,strtok(NULL, "/"));
        strcpy(year,strtok(NULL, "/"));

        d = convert_to_int(day);
        m = convert_to_int(month);
        y = convert_to_int(year);

        switch(dayofweek(d,m,y)) {

            case 0:
                strcpy(dow, "Domingo");
                break;
            case 1:
                strcpy(dow, "Lunes");
                break;
            case 2:
                strcpy(dow, "Martes");
                break;
            case 3:
                strcpy(dow, "Miercoles");
                break;
            case 4:
                strcpy(dow, "Jueves");
                break;
            case 5:
                strcpy(dow, "Viernes");
                break;
            case 6:
                strcpy(dow, "Sabado");
                break;
        }
        strcat(buffer,"/");
        strcat(buffer,month);
        strcat(buffer,"/");
        strcat(buffer,year);
        strcpy(message,"La fecha ");
        strcat(message,buffer);
        strcat(message," correponde al dia ");
        strcat(message,dow);

        send(new_socket , message , strlen(message) , 0 );
        printf("Fecha procesada %s\n", buffer);
        memset(buffer,0,strlen(buffer));
        memset(message,0,strlen(message));
        sleep(1);
    }
    return 0;
}

int dayofweek(int d, int m, int y)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
int convert_to_int (const char* str)
{
    char *p;
    int num;

    long conv = strtol(str, &p, 10);    
    
    num = conv;
    return num;
}
