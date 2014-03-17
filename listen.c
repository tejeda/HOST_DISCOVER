#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>
#include <errno.h>

/7 escribir en la CLI  ./listen 



int main(int argc, char* argv[]) {

	int udpSocket,udpSocket2;
	struct sockaddr_in udpServer, udpClient;
	int localerror;
	socklen_t addrlen = sizeof(udpClient);
	char buffer[255];
	char ip[17];
	u_short clientPort;


	int status;

	//Creamos el Socket para recibir mensajes
	udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(udpSocket == -1) {
		fprintf(stderr,"Can't create UDP Socket");
		return 1;
	}

	//Creamos el Socket para enviar mensajes
	udpSocket2 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(udpSocket == -1) {
		fprintf(stderr,"Can't create UDP Socket");
		return 1;
	}	

    udpServer.sin_family = AF_INET;
    inet_pton(AF_INET,"0.0.0.0",&udpServer.sin_addr.s_addr);
    udpServer.sin_port = htons(5000);
	//con bind asociamos el socket para recibir (udpSocket) a la dirección del udpServer)
    status = bind(udpSocket, (struct sockaddr*)&udpServer,sizeof(udpServer));
	//Validamos cualquier error al hacer el bind
    if(status != 0) { 
	  fprintf(stderr,"Can't bind");
    }

	/////escuchar las respuestas de los hosts (listeners)
	while(1) {
		bzero(buffer,255);
		status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr*)&udpClient, &addrlen );
		////ver por donde se recibe los mensajes del sender para en el sender recibir el ME PC2
		inet_ntop(AF_INET,&(udpClient.sin_addr),ip,INET_ADDRSTRLEN);
		clientPort = ntohs(udpClient.sin_port);

		printf("Recibimos: [%s:%i] %s\n",ip,clientPort,buffer);

		udpClient.sin_port =  htons(5000);

		///Mandamos como respuesta un ME PC2" al sender.
		status = sendto(udpSocket2,"ME PC2\r\n\r\n",strlen("ME PC2\r\n\r\n"),0,(struct sockaddr*)&udpClient, addrlen);
		if(status==-1){
                localerror = errno;
                printf("%s",strerror(localerror));
		}
   	    fflush(stdout);
	}

	close(udpSocket);

	return 0;

}
