# TP3- Uso de FreeRTOS

## 2- Ejemplos 1 al 9

### Ejemplo 1

A continuacion se observa el diagrama temporal de la distribución del tiempo del CPU:

![](imagenes/ej1.PNG)  

En este ejemplo se crean dos tareas continuas de igual prioridad. Como ambas tienen la misma priorioridad, se ejecutan alternadas indefinidamente, teniendo ambas la misma cantidad de tiempo el CPU. En estas tareas se apaga y enciende un LED y cada una se imprime un mensaje por serial diciendo que tarea se esta ejecutando.
En caso del time slice de 1ms a la salida de la consola se observa el siguiente mensaje:
![](imagenes/ej1a.PNG)  
Esto se debe a que durante el envio de un string, debido a la velocidad del puerto serial, no se llega a enviar todo el mensaje en 1mS. Por eso, los mensajes de las tareas de ven cortados y pisado por la otra tarea. Para corregir esto se extiende el time_slice a 100mS y se observa la siguiente salida:
![](imagenes/ej1b.PNG)  

### Ejemplo 2

Este ejemplo es similar al anterior:

![](imagenes/ej1.PNG)

Sin embargo las funcion que corre en las tareas es la misma para ambas tareas, con la diferencia de que se pasa un parametro distinto a la hora de crearlas. Ambas tareas se crean con la misma priodidad, como en el ejemplo 1. Ese parametro en este caso es el string que identifica el nombre de la tarea. La tareas cuentas con un delay implementado de manera ineficiente con un ciclo for. Se tiene el mismo problema que en el ejercicio anterior con la salida por consola.


### Ejemplo 3
A continuacion se observa el diagrama temporal de la distribución del tiempo del CPU:

![](imagenes/ej3.PNG) 

Este ejercicio es totalmente igual al ejericicio anterior, con la diferencia del valor de prioridades: la prioridad de la Task2 es 1 unidad mayor q la Task1. Es por esto, que al ser tareas de procesamiento continuo y nunca delegar el uso del CPU, el kernel le otorga 100% del tiempo del CPU a la segunda tarea, sin importar el time_slice. Por eso a la salida de la consola se obtiene el siguiente resultado:

![](imagenes/ej3a.PNG) 

### Ejemplo 4

En este ejercicio vTaskDelay(). 

![](imagenes/ej4.PNG) 

Se crean dos tareas que realizan lo mismo que en los ejercicios anteriores. La tarea 2 tiene prioridad superior que la 1. En la funcion se conmuta un LED, se imprime el mensaje por consola y se ejecuta vTaskDelay(). Esta función coloca en estado BLOCKED a las tareas hasta que transcurre el tiempo pasado a la misma. La tarea corre hasta llegar a esta función y delega el uso del CPU. Cuando pasa el tiempo, la tarea pasa a READY y el kernel llama a la tarea 2, que tiene más prioridad. Cuando esta termina, se ejecuta la tarea 1. Esto se observa en la terminal:

![](imagenes/ej4a.PNG) 

### Ejemplo 5

En este ejercicio vTaskDelayUntil().

![](imagenes/ej4.PNG) 

Este ejercicio es similar al anterior, con la diferencia de utilizar vTaskDelayUntil() en vez de vTaskDelay(). A continución se explica la diferencia:
En vTaskDelay se le indica cuanto tiempo despues de llamar a vTaskDelay se quiere que la tarea se depierte (pase a READY). El parametro en vTaskDelay es el periodo de delay en numeros de ticks desde que se llama a la función.
En vTaskDelayUntil  se le indica en que tiempo se quiere despertar la tarea. El parametro en vTaskDelayUntil es el tiempo absoluto en ticks en el que se quiere despertar la tarea, calculado como un incremento del tiempo cuando fue despertado por última vez.

La salida por consola es la siguiente:

![](imagenes/ej5a.PNG) 

### Ejemplo 6

En este ejemplo se combina el uso de dos tareas continuas (Tarea 1 y 2) y una periodica (Tarea 3):

![](imagenes/ej6.PNG) 

La tarea periodica tiene más prioridad que las continuas, las cuales tienen misma prioridad entre ellas. La tarea periodica se ejecuta cada 10mS e imprime por consola un mensaje diciendo que esta corriendo. Las continuas imprimen cada un determinado tiempo (delay implementado con for) por consola un string diciendo el nombre de donde viene el mensaje. Lo que sucede es que cada vez que se llama a la tarea 3 se imprime un mensaje, cada 10 mS. En el caso de las continuas, si bien se le asigna tiempo del CPU, teiene una demora cada cuanto se imprime el mensaje. La salida de la consola se ve en la siguiente imagen:

![](imagenes/ej6a.PNG) 

Se observa que la impresion del mensaje de la tarea 3 es mucho más frecuente, debido a que el delay implemetado por un ciclo for en las tareas continuas es mayor a 10mS. Tambien se observa que la función.periodica se ejecuta en el medio de una continua, haciendo que el mensaje de la continua se vea interrumpido.

### Ejemplo 7

En este ejemplo se utiliza vApplicationIdleHook(), que permite utilizar funciones en la tarea Idle

![](imagenes/ej7.PNG) 

Se crean dos tareas peridicas (Tarea 1 y Tarea 2). Se ejecutan normalmente segun su prioridad y se bloquean. El tiempo restante hasta cumplir el time_slice, se ejecuta la tara Idle. Para utilizar esta tarea se utliza vApplicationIdleHook. En este ejemplo, en vApplicationIdleHook se incrementa un variable global y se manda a dormir al CPU hasta la siguiente interrupcion (__WFI()). En las tareas periodicas se imprime el valor de esta variable.

El resultado se ve en la consola:

![](imagenes/ej7a.PNG) 

### Ejemplo 8

En este ejemplo se hace uso del cambio dinamico de priodidades.

![](imagenes/ej8.PNG) 

En este ejemplo de se crean dos tareas. La Tarea 1 tiene más prioridad e incrementa la prioridad de la Tarea 2 cada vez que se ejecuta. La Tarea 2 baja su misma prioridad cada vez que se le asigna el CPU. Cuando arranca el programa, el kernel asigna el CPU a la Tarea 1, la cual lee su propia prioridad con uxTaskPriorityGet() y asigna a la Tarea 2 esa priorida más uno con vTaskPrioritySet(). En ese momento se genera un cambio de contexto y la tarea de mayor prioridad es ejecutada. Entonces se ejecuta Tarea 2, ya que ahora tiene mayor prioridad que la Tarea 1. Al igual que la Tarea 1, la Tarea 2 lee su prioridad y se baja en dos unidades. De esta manera pasa a ser de menos prioridad que la Tarea 1. En este momento el kernel cambia de contexto y el ciclo comienza de nuevo. Se remarca que un cambio de time_slice no generaria un cambio en el funcionamiento del programa ya que se tiene un cambio continuo de contexto entre dos tareas.

Por cada llamado de las tareas se imprime un string por la consola. El resultado se ve en la siguiente imagen. 

![](imagenes/ej8a.PNG) 

### Ejemplo 9

En esta tarea se implenta la creación y destrucción de tareas de manera dinámica con xTaskCreate() y vTaskDelete(). El siguiente diagrama de tiempo no está en escala, en el sentido que la demora de 100mS de la Tarea 1 está representada como 2mS por simplificación.

![](imagenes/ej9.PNG) 

En este ejemplo se crea solo la Tarea 1, la cual cada 100mS crea a la Tarea 2, con más prioridad que la propia e imprime un mensaje por consola. Cuando se ejecuta la Tarea 2, esta no hace más que imprimir un mensaje y destruirse. El resultado por consola es el siguiente:

![](imagenes/ej9a.PNG) 

## 3- Ejemplos 10 al 16

### Ejemplo 10

En este ejemplo de se sincronizan dos tareas creadoras con una consumidora a traves de colas:

![](imagenes/ej10.PNG) 

La Tarea Receiver tiene más prioridad que las dos Tareas Sender. Cuando arranca el programa, el CPU es asignado a la Tarea Receiver, la cual queda en estado BLOCKED en la espera de que haya un dato en la cola (xQueueReceive()). Luego corre la Tarea Sender2 y carga en la cola un valor con xQueueSendToBack() y llama a taskYIELD() para deelgar el uso del CPU. En este momento el kernel sabes que deja de estar vacia la cola y asigna el CPU a la Tarea Receiver, la cual imprime el valor por consola y vuelve a esperar un valor, el cual se llena por Tarea Sender 1. 

El resultado por consola es el siguiente:

![](imagenes/ej10a.PNG) 

### Ejemplo 11

En este ejemplo se pasan estructuras por las colas para sincronizar las tareas. 

![](imagenes/ej11.PNG) 

Este ejercicio es similar al anterior, sin embargo ahora las Tareas Sender tienen mayor prioridad que la Tares Receiver. Es por eso que se ejecutan alternadamente hasta llenar la cola. En este momento entra en escena el ultimo parametro de xQueueSendToBack(), el cual indica cuanto tiempo tiene que permanecer en BLOCKED la tareas hasta q haya espacio. Cuando ambas Sender quedan en BLOCKED, se ejecuta Receiver, la cual imprime un mensaje y libera en lugar en la cola. En este momento una de las Sender llena este espacio y se repite el ciclo. Ademas, ahora por la cola no se pasa solo el valor, sino una estructura que contiene el valor y un numero indicando el origen (cual de las Tareas Sender agregó el dato).

El resultado por pantalla es el siguiente:

![](imagenes/ej11a.PNG) 
