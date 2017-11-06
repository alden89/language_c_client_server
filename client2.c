#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int check_date(char *date);

int convert_to_int(const char* str);

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int port, date;

    if(argc != 4)
    {
        printf("\n Uso: %s <ip del server> <puerto> <fecha:dd/mm/yyyy>\n",argv[0]);
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error al crear socket \n");
        return -1;
    }
    
    char *p;

    errno = 0;
    long conv = strtol(argv[2], &p, 10);

    if (errno != 0 || *p != '\0' || conv > 65535 || conv < 1) {
        printf("\n Error : Debe pasarse como parametro un número de puerto entre 1-65535 \n");
        return 1;
    } else {
        port = conv;    
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
    {
        printf("\nDireccion ip no valida \n");
        return -1;
    }
  
    date = check_date((char *)argv[3]);
    if (date == 1)
    {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nFallo la conexion \n");
            return -1;
        }
        send(sock , argv[3] , strlen(argv[3]) , 0 );
        //printf("Fecha enviada %s\n", argv[3]);
        valread = read( sock , buffer, 1024);
        printf("%s\n",buffer );
    }else{
        printf("\n La fecha no es valida! \n");
        return -1;
    }
    
    return 0;
}

int convert_to_int (const char* str)
{
    char *p;
    int num;
    
    long conv = strtol(str, &p, 10);    
    
    num = conv;
    return num;
}

int check_date(char *date)
{
    char day[2], month[2], year[4], aux[80];
    int dd, mm, yy;
    int valid=0;

    strcpy(aux,strtok(date , "/"));
    if (strlen(aux) != 2)
    {
        printf("\n Error : El formato de la fecha debe ser dd/mm/yyyy\n");
        return -1; 
    }else{        
        strcpy(day, aux);
        strcpy(aux,strtok(NULL, "/"));
        if (strlen(aux) != 2)
        {
            printf("\n Error : El formato de la fecha debe ser dd/mm/yyyy\n");
            return -1;
        }else{

            strcpy(month, aux);
            strcpy(aux,strtok(NULL, "/"));
            if (strlen(aux)!=4)
            {
                printf("\n Error : El formato de la fecha debe ser dd/mm/yyyy\n");
                return -1;
            }else{
                strcpy(year , aux);
            }
        }
        
    }

    dd = convert_to_int(day);
    mm = convert_to_int(month);
    yy = convert_to_int(year);

    //check year
    if(yy>=1900 && yy<=9999)
    {
        //check month
        if(mm>=1 && mm<=12)
        {
            //check days
            if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12)){
                valid = 1;
            }
            else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11)){
                valid = 1;
            }
            else if((dd>=1 && dd<=28) && (mm==2)){
                valid = 1;
            }
            else if(dd==29 && mm==2 && (yy%400==0 ||(yy%4==0 && yy%100!=0))){
                valid = 1;
            }
            else{
                valid = 0;
                printf("El dia no es valido.\n");
            }
        }
        else
        {
            valid = 0;
            printf("El mes no es valido.\n");
        }
    }
    else
    {
        valid = 0;
        printf("El año no es valido.\n");
    }
    strcat(date, "/");
    strcat(date, month);
    strcat(date, "/");
    strcat(date, year);
    return valid;
}
