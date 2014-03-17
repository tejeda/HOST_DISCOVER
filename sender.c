/*  HOST DISCOVER - EMISOR 
 ./sender <broadcast_ipaddress>  5000
*/

#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     

#include <netdb.h>
#include <ifaddrs.h>

int main(int argc, char *argv[])
{
    int bcSock, udpSocket;                        
    struct sockaddr_in broadcastAddr; 
    char *broadcastIP, strbuffer[255], buffer[255];                
    unsigned short broadcastPort;     
    pid_t forkID; //es un int                 
    
    int broadcastPermission;          
    unsigned int sendStringLen; 

	int status;
	int i,element=0,string=0,x=0;
	int found=0,size = 0;
	struct sockaddr_in udpClient,udpServer;
	socklen_t addrlen = sizeof(udpClient);
	char ip[17], *knownHosts[20];

	u_short clientPort;


    if (argc < 3)                     
    {
        //fprintf(stderr,"Usage:  %s <IP Address> <Port> <Send String>\n", argv[0]);
	fprintf(stderr,"Usage:  %s <IP Address> <Port>\n", argv[0]);
        exit(1);
    }

    broadcastIP = argv[1];            /* First arg:  broadcast IP address */ 
    broadcastPort = atoi(argv[2]);    /* Second arg:  broadcast port */
    sprintf(strbuffer,"WHO IS THERE %i\r\n\r\n",(int)broadcastPort);	
    
	//Creamos bcSock, el socket para enviar	broadcast
	bcSock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(bcSock == -1) {
		fprintf(stderr,"Can't create Socket");
		return 1;
	}
  	
	//Creamos el udpSocket para recibir mensajes
	udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(udpSocket == -1) {
		fprintf(stderr,"Can't create UDP Socket");
		return 1;
	}

    broadcastPermission = 1;
    status = setsockopt(bcSock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission));
    if(status == -1) {
		fprintf(stderr,"Can't set Brodcast Option");
		return 1;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   
    broadcastAddr.sin_family = AF_INET;                 
	inet_pton(AF_INET,broadcastIP,&broadcastAddr.sin_addr.s_addr); 
    broadcastAddr.sin_port = htons(broadcastPort);         
	 sendStringLen = strlen(strbuffer);  

    udpServer.sin_family = AF_INET;
    inet_pton(AF_INET,"0.0.0.0",&udpServer.sin_addr.s_addr);
    udpServer.sin_port = htons(5000);
	//con bind asociamos el socket para recibir (udpSocket) a la dirección del udpServer)
    status = bind(udpSocket, (struct sockaddr*)&udpServer,sizeof(udpServer));
	if(status != 0) { 
	  fprintf(stderr,"Can't bind");
    }	
	forkID = fork(); //////////////START FORK
	////Checar donde poner los exit() y donde cerrar los sockets UDP....
	if(forkID == -1) {
			fprintf(stderr,"Cant Fork\n");
		} else if(forkID == 0) {
			//Soy el HIJO
			printf("Recibimos: OK\n");
		while(1) {
		bzero(buffer,255);
		status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr*)&udpClient, &addrlen );

		////ver por donde se recibe los mensajes 
		inet_ntop(AF_INET,&(udpClient.sin_addr),ip,INET_ADDRSTRLEN);

		clientPort = ntohs(udpClient.sin_port);
	
		//if(strcmp(ip,host)!=0){ // no imprimir las respuestas de mi propia IP.
		fprintf(stderr,"Recibimos: %s desde: [%s:%i]\n",buffer,ip,clientPort);

		/// lista de hosts 
		for(i=0; i < size; i++) {
   			if(strcmp(knownHosts[i],ip)==0) {
      			  found = 1;
			break;
   			}
		} 

			if(!found) {
			   knownHosts[size] = (char *) malloc(16);
 			   strcpy(knownHosts[size],ip);
   			   size++;
			} 
		//f}	
		i=0;
		///En cada respuesta imprimimos la lista de hosts conocidos
		fprintf(stderr,"Hosts conocidos:");
		while(knownHosts[i]!=NULL){

		fprintf(stderr,"%s \n",knownHosts[i]);
		i++;	
		}
		udpClient.sin_port = 5000;
		bzero(buffer,255);
	   	    fflush(stdout);
	}
	close(udpSocket);
			return 0;
		} else {
	
	for(i=0; i<100; i++) {  //Este hilo (proceso) sólo manda WHO IS THERE cada 3 segundos
		status = sendto(bcSock,strbuffer,sendStringLen,0,(struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));    
      ///Respuestas cada 3 segundos
        sleep(3);   
    	}

		} /////////////////////FIN FORK
return 0;
} //fin main
