#include <arpa/inet.h>
#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{

    /*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
    int sd;
    struct sockaddr_in sockname;
    char buffer[100];
    socklen_t len_sockname;
    fd_set fdset;
    /* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    /* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2050);
    sockname.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
    len_sockname = sizeof(sockname);

    if (connect(sd, (struct sockaddr*)&sockname, len_sockname) == -1) {
        perror("Error de conexión");
        exit(1);
    }

    /* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
    bool salir = false;
    do {
        FD_ZERO(&fdset);
        FD_SET(0, &fdset);
        FD_SET(sd, &fdset);
        select(sd + 1, &fdset, NULL, NULL, NULL);
        if (FD_ISSET(sd, &fdset)) {
            if ((recv(sd, buffer, sizeof(buffer), 0) <= 0) || (strcmp(buffer, "CONNECTION_NOT_ALLOWED") == 0))
                salir = true;
            else
                printf("\n%s\n", buffer);
        }
        if (FD_ISSET(0, &fdset)) {
            fgets(buffer, sizeof(buffer), stdin);
            if (send(sd, buffer, 100, 0) == -1)
                perror("Error enviando datos");
        }

    } while (!salir);

    close(sd);
    return 0;
}
