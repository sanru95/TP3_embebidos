# TP3- Uso de FreeRTOS

## 2- Ejemplos 1 al 9

## Ejemplo 1

A continuacion se observa el diagrama temporal de la distribución del tiempo del CPU:

![](imagenes/ej1.PNG)  

En este ejemplo se crean dos tareas continuas de igual prioridad. Como ambas tienen la misma priorioridad, se ejecutan alternadas indefinidamente, teniendo ambas la misma cantidad de tiempo el CPU. En estas tareas se apaga y enciende un LED y cada una se imprime un mensaje por serial diciendo que tarea se esta ejecutando.
En caso del time slice de 1ms a la salida de la consola se observa el siguiente mensaje:
![](imagenes/ej1a.PNG)  
Esto se debe a que durante el envio de un string, debido a la velocidad del puerto serial, no se llega a enviar todo el mensaje en 1mS. Por eso, los mensajes de las tareas de ven cortados y pisado por la otra tarea. Para corregir esto se extiende el time_slice a 100mS y se observa la siguiente salida:
![](imagenes/ej1b.PNG)  

## Ejemplo 2

Este ejemplo es similar al anterior:

![](imagenes/ej1.PNG)

Sin embargo las funcion que corre en las tareas es la misma para ambas tareas, con la diferencia de que se pasa un parametro distinto a la hora de crearlas. Ambas tareas se crean con la misma priodidad, como en el ejemplo 1. Ese parametro en este caso es el string que identifica el nombre de la tarea. La tareas cuentas con un delay implementado de manera ineficiente con un ciclo for. Se tiene el mismo problema que en el ejercicio anterior con la salida por consola.


## Ejemplo 3
A continuacion se observa el diagrama temporal de la distribución del tiempo del CPU:

![](imagenes/ej3.PNG) 

Este ejercicio es totalmente igual al ejericicio anterior, con la diferencia del valor de prioridades: la prioridad de la Task2 es 1 unidad mayor q la Task1. Es por esto, que al ser tareas de procesamiento continuo y nunca delegar el uso del CPU, el kernel le otorga 100% del tiempo del CPU a la segunda tarea, sin importar el time_slice. Por eso a la salida de la consola se obtiene el siguiente resultado:

![](imagenes/ej3a.PNG) 

## Ejemplo 4

En este ejercicio vTaskDelay().

![](imagenes/ej4.PNG) 

Se crean dos tareas que realizan lo mismo que en los ejercicios anteriores. La tarea 2 tiene prioridad superior que la 1. En la funcion se conmuta un LED, se imprime el mensaje por consola y se ejecuta vTaskDelay(). Esta función coloca en estado BLOCKED a las tareas hasta que transcurre el tiempo pasado a la misma. La tarea corre hasta llegar a esta función y delega el uso del CPU. Cuando pasa el tiempo, la tarea pasa a READY y el kernel llama a la tarea 2, que tiene mas prioridad. Cuando esta termina, se ejecuta la tarea 1. Esto se observa en la terminal:

![](imagenes/ej4a.PNG) 


