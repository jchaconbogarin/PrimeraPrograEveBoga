//Servidor

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


int socketProceso, puerto, datosRecibidos;
int tamanoStruct = sizeof(struct sockaddr);
char recibido[256], porEnviar[256];
int *cliente1; 
int *cliente2;

struct sockaddr_in datosServidor, datosCliente;

/*  Definición de la función servidor. Recibe 0 argumentos. Es llamada por el main.
 *  cuando el usuario digita "1".
 */
void servidor()
{
	printf("Iniciado proceso para crear servidor...\n");
	printf("Digite el puerto que desea abrir para el servidor: ");
	scanf("%d", &puerto);														//Obtención del puerto.


	//Asignación de valores de la estructura de datos del servidor.
	datosServidor.sin_family = AF_INET;					//Protocolo TCP							
	datosServidor.sin_port = htons(puerto);				//Puerto. Se convierte para compatibilidad de bytes con la red.
	datosServidor.sin_addr.s_addr = INADDR_ANY; 		//Obtiene el IP por sí mismo.

	struct sockaddr *ptrDatos = &datosServidor;									//Puntero a la estructura (necesario para bind())

	int asociacion = bind(socketProceso, ptrDatos, tamanoStruct);				//Asociación del socket.
	if(asociacion == -1)
	{
		printf("Error asociando el socket");
		exit(11);																//Valor de retorno de fallo de la asociación de socket.
	}

	int cantidadClientes = listen(socketProceso, 2);							//Apertura de servidor a clientes y cantidad de los mismos.
	if(cantidadClientes == -1)
	{
		printf("Error asignando en la función listen()");
		exit(12);																//Fallo al abrir y asignar cantidad de clientes.
	}

	printf("Servidor creado correctamente.\n");
	printf("Esperando conexión en el puerto: %d...\n", puerto);

	int tamanoStruct2 = sizeof(struct sockaddr_in);								//Definición del tamaño de la estructura. Necesario para la función accept()
	//int contador = 0;															//Se define el contador de las conexiones.

	//int conexiones[2];															//Se define el arreglo que tiene los id de las conexiones.

	int conexion1 = accept(socketProceso, (struct sockaddr *)&datosCliente	//Función accept que acepta una conexión.
							  , &tamanoStruct2);		
	printf("Recibida la primera conexión con identificador %d y dirección de memoria %d\n", conexion1, &conexion1);
	
	int conexion2 = accept(socketProceso, (struct sockaddr *)&datosCliente	//Función accept que acepta una conexión.
							  , &tamanoStruct2);		
	printf("Recibida la segunda conexión con identificador %d y dirección de memoria %d\n", conexion2, &conexion2);
		
	cliente1 = &conexion1;
	cliente2 = &conexion2;
	
	printf("Asignados los valores\n");
	
	pid_t hijo = fork();
	
	printf("Iniciado fork");

	if(hijo>=0)
	{
		if(hijo==0)
		{
			printf("Proceso hijo");
			cicloServidor(cliente1, cliente2);
		}
		else
		{
			printf("Proceso padre");
			cicloServidor(cliente2, cliente1);
		}
	}
	else
	{
		printf("Fallo al crear hilo");
		exit(13);
	}	
}

void cicloServidor(int *recibe, int *envia)
{
	printf("Entra ciclo servidor");
	while(1)
	{
		datosRecibidos = recv(*recibe, recibido, 256, 0);
		printf("%d\n", datosRecibidos);
		recibido[datosRecibidos] = "\0";
		printf("%s", recibido);
		send(*recibe, recibido, sizeof(recibido), 0);
	}
}

/*  Definición de la función cliente. Recibe 0 argumentos. Es llamada por el main.
 *  cuando el usuario digita "2". 
 */
void cliente()
{
	char ipServidor[15];
	printf("Iniciado el proceso de creación del cliente...\n");

	printf("Digite el IP del servidor al que se desea conectar: ");	
	scanf("%s", &ipServidor);

	printf("Digite el puerto del servidor al que se desea conectar: ");	
	scanf("%d", &puerto);

	struct hostent *serv;
	serv = gethostbyname(&ipServidor);									//Obtenemos los datos del servidor por medio del IP ingresado por el usuario.

	//Asignación de valores de la estructura de datos del servidor.
	datosServidor.sin_family = AF_INET;									//Protocolo TCP							
	datosServidor.sin_port = htons(puerto);								//Puerto. Se convierte para compatibilidad de bytes con la red.
	datosServidor.sin_addr = *((struct in_addr *)serv->h_addr); 		//Se obtiene la estructura por medio de h_addr en serv

	struct sockaddr *ptrDatos = &datosServidor;							//Puntero a la estructura de datos del servidor

	int conectar = connect(socketProceso, ptrDatos, tamanoStruct);		//Intento de conexión con el servidor.

	if(conectar == -1)
	{
		printf("Error conectando con el servidor");
		exit(21);
	}

	pid_t hijo = fork();

	if(hijo>=0)
	{
		if(hijo==0)
		{
			while(1)
			{
				datosRecibidos = recv(socketProceso, recibido, 256, 0);		//Asginación a datosRecibidos por medio de la función recv().
				recibido[datosRecibidos] = '\0';							//Recibe como parámetros socket, un arreglo al cual enviar los
				printf("Mensaje recibido: %s", recibido);					//datos, el valor máximo de bytes a recibir y 0 por bandera.
			}																
		}
		else
		{
			while(1)
			{
				gets(porEnviar);
				printf("Mensaje enviado: %s", porEnviar);
				if( strcmp(porEnviar, "adios") == 0)
				{
					close(socketProceso);
					break;
				}
				send(socketProceso, porEnviar, sizeof(porEnviar), 0);
			}
		}
	}
	else
	{
		printf("Fallo al crear hilo");
		exit(22);
	}	
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
		exit(1);
	}

	if(respuesta == 1)											//Comparaciones
		servidor();
	else if(respuesta == 2)
		cliente();
	else
	{
		printf("¡Input incorrecto; programa terminado!");		//Input incorrecto.
		exit(2);												//Valor retorno 1 de input incorrecto.
	}

	return 0;
}

