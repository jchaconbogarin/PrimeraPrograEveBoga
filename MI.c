/*
Instituto Tecnológico de Costa Rica
Primera Tarea Programada - Lenguajes de Programación
Jose Daniel Chacón Bogarín
Evelyn Madriz Mata
13 de setiembre de 2012
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define RECIBIDO 31
#define ENVIADO 32


int socketServ, socketCli, puerto, puertoCliente, datosRecibidos;
int tamanoStruct = sizeof(struct sockaddr);
char recibido[256], porEnviar[256];
char ipServidor[15];
int contadorEspecial = 0;

struct sockaddr_in datosServidor, datosCliente, datosServidorCliente;

int creaCliente()
{
	struct hostent *serv;
	serv = gethostbyname(&ipServidor);									//Obtenemos los datos del servidor por medio del IP ingresado por el usuario.

	//Asignación de valores de la estructura de datos del servidor.
	datosServidorCliente.sin_family = AF_INET;							//Protocolo TCP							
	datosServidorCliente.sin_port = htons(puertoCliente);				//Puerto. Se convierte para compatibilidad de bytes con la red.
	datosServidorCliente.sin_addr = *((struct in_addr *)serv->h_addr); 	//Se obtiene la estructura por medio de h_addr en serv
		

	socketCli = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socketCli == -1)
	{
		printf("Error creando el socket del cliente");
		exit(21);
	}
	
	struct sockaddr *ptrDatos = &datosServidorCliente;					//Puntero a la estructura de datos del servidor
		
	int conectar = connect(socketCli, ptrDatos, tamanoStruct);			//Intento de conexión con el servidor.

	if(conectar == -1)
	{
		printf("¡Intento de conexión fallido!");
		exit(22);
	}	
	else
	{
		system("clear");
		printf("¡Conexión exitosa! Comience a hablar.\n");
		while(1)
		{
			gets(porEnviar);
			if(strcmp(porEnviar, "Adios") == 0)
			{
				send(socketCli, porEnviar, sizeof(porEnviar), 0);
				close(socketCli);
				exit(0);
			}
			printf("\n%c[%dmMensaje enviado: %s", 0x1B, ENVIADO, porEnviar);
			send(socketCli, porEnviar, sizeof(porEnviar), 0);
		}
	}
}

void creaServidor()
{	
	socketServ = socket(AF_INET, SOCK_STREAM, 0);
	if(socketServ == -1)
	{
		printf("¡Error creando el socket del servidor!");
		exit(11);
	}
	
	//Asignación de valores de la estructura de datos del servidor.
	datosServidor.sin_family = AF_INET;									//Protocolo TCP							
	datosServidor.sin_port = htons(puerto);								//Puerto. Se convierte para compatibilidad de bytes con la red.
	datosServidor.sin_addr.s_addr = INADDR_ANY; 						//Obtiene el IP por sí mismo.
	
	struct sockaddr *ptrDatos = &datosServidor;
	
	int asociacion = bind(socketServ, ptrDatos, tamanoStruct);			//Asociación del socket.
	if(asociacion == -1)
	{
		printf("¡Error asociando el socket del servidor!");
		exit(12);														//Valor de retorno de fallo de la asociación de socket.
	}

	int cantidadClientes = listen(socketServ, 1);						//Apertura de servidor a clientes y cantidad de los mismos.
	if(cantidadClientes == -1)
	{
		printf("¡Error habilitando las peticiones de conexión!");
		exit(13);														//Fallo al abrir y asignar cantidad de clientes.
	}
	fflush(stdout);
	
	pid_t procesoHijo = fork();
	
	if (procesoHijo >= 0)
	{
		if(procesoHijo==0)
		{
			int confirmacion = 0;
			printf("Esperando confirmación para crear el cliente...digite 1.\n");
			scanf("%d", &confirmacion);
			if(confirmacion == 1)
			{
				creaCliente();
			}
			else
			{
				printf("Confirmación incorrecta. Se cerrará el programa.");
				exit(1);
			}
		}
		else
		{
			printf("Servidor creado correctamente.\n");
			printf("Servidor esperando conexión en el puerto: %d...\n", puerto);
			
			int tamanoStruct2 = sizeof(struct sockaddr_in);						//Definición del tamaño de la estructura. Necesario para la función accept()
			int conexion = accept(socketServ, (struct sockaddr *)&datosCliente	//Función accept que acepta una conexión.
						  , &tamanoStruct2);
			if (conexion == -1)
			{
				printf("Conexión fallida");
				exit(14);
			}				  
			system("clear");
			printf("¡Servidor recibió conexión!\n");
			
			int contadorEspecial = 0;
			while(1)
			{
				datosRecibidos = recv(conexion, recibido, 256, 0);
				recibido[datosRecibidos] = '\0';
				if(strcmp(recibido, "Adios") == 0)
				{
					exit(0);
				}
				else
				{
					printf("\n%c[%dmMensaje recibido: %s", 0x1B, RECIBIDO, recibido);
					fflush(stdout);
				}
			}
		}
	}
	else
	{
		printf("Error en el fork()");
		exit(15);
	}
}

int main()
{
	printf("Bienvenido al programa de Mensajería Instantánea.\nA continuación se le pedirán algunos datos necesarios para iniciar el programa.\n");
	
	printf("Datos para el servidor: \n");
	printf("Puerto que se abrirá: ");
	scanf("%d", &puerto);
	
	printf("Datos del cliente: \n");
	printf("IP del servidor al que se conectará: ");	
	scanf("%s", &ipServidor);

	printf("Puerto del servidor al que se conectará: ");	
	scanf("%d", &puertoCliente);
	
	creaServidor();
	
	return 0;
}
