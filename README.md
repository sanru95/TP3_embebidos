# TP3- Uso de FreeRTOS

## 2- Ejemplos 1 al 9

## Ejemplo1

A continuacion se observa el diagrama temporal de la distribución del tiempo del CPU
![](imagenes/ej1.PNG)  
En este ejemplo se crean dos tareas continuas de igual prioridad. Como ambas tienen la misma priorioridad, se ejecutan alternadamente indefinidamente, teniendo ambas la misma cantidad de tiempo el CPU. En estas tareas se apaga y enciende un LED y cada una se imprime un mensaje por serial diciendo que tarea se esta ejecutando.
En caso del time slice de 1ms a la salida de la consola se observa el siguiente mensaje:
![](imagenes/ej1a.PNG)  
Esto se debe a que durante el envio de un string, debido a la velocidad del puerto serial, no se llega a enviar todo el mensaje en 1mS. Por eso, los mensajes de las tareas de ven cortados y pisado por la otra tarea. Para corregir esto se extiende el time_slice a 100mS y se observa la siguiente salida:
![](imagenes/ej1b.PNG)  