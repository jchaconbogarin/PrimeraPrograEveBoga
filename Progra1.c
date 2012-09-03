//Servidor

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int socketProceso, datosRecibidos, puerto;
char recibido[256], porEnviar[256];
	
struct sockaddr_in datosServidor, datosCliente;
int tamanoSin;



void servidor()
{
	//int clienteConectado;
	printf("Iniciado proceso para crear servidor...\n");
	printf("Digite el puerto que desea abrir para el servidor: ");
	scanf("%d", &puerto);														//Obtención del puerto.
	
	
	//Asignación de valores de la estructura de datos del servidor.
	datosServidor.sin_family = AF_INET;					//Protocolo TCP							
	datosServidor.sin_port = htons(puerto);				//Puerto. Se convierte para compatibilidad de bytes con la red.
	datosServidor.sin_addr.s_addr = INADDR_ANY; 		//Obtiene el IP por sí mismo.
	
	struct sockaddr *ptrDatos = &datosServidor;									//Puntero a la estructura (necesario para bind())
	
	int asociacion = bind(socketProceso, ptrDatos, sizeof(struct sockaddr));	//Asociación del socket.
	if(asociacion == -1)
	{
		printf("Error asociando el socket");
		exit(12);																//Valor de retorno de fallo de la asociación de socket.
	}
	
	int cantidadClientes = listen(socketProceso, 2);							//Apertura de servidor a clientes y cantidad de los mismos.
	if(cantidadClientes == -1)
	{
		printf("Error asignando en la función listen()");
		exit(13);																//Fallo al abrir y asignar cantidad de clientes.
	}
	
	printf("Servidor creado correctamente.\n");
	printf("Esperando conexión en el puerto: %d...\n", puerto);
}



void cliente()
{
	char ipServidor[15];
	printf("Iniciado el proceso de creación del cliente...");
	printf("Digite el IP del servidor al que se desea conectar: ");
	scanf("%s", &ipServidor);
	printf("Digite el puerto del servidor al que se desea conectar: ");
	scanf("%d", &puerto);
}


//Definición de la función main. Recibe 0 argumentos. Funciona para la creación de servidor y cliente.
int main()
{
	int respuesta;												//Variable de servidor o Cliente.
	printf("1 para servidor o 2 para cliente");
	scanf("%d", &respuesta);									//Lectura del teclado.
	
	socketProceso = socket(AF_INET, SOCK_STREAM, 0);
	if(socketProceso == -1)
	{
		printf("Error creando el socket del cliente");
		exit(21);
	}
	
	if(respuesta == 1)											//Comparaciones
		servidor();
	else if(respuesta == 2)
		cliente();
	else
	{
		printf("¡Input incorrecto; programa terminado!");		//Input incorrecto.
		exit(1);												//Valor retorno 1 de input incorrecto.
	}
	
	return 0;
}
