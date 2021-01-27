# Repositorio para omnetpp de RRT

## Resumen

En este repositorio se encuentran todas las fuentes de codugo usabas para la realizacion de silulaciones de protocolos ARQ usando Omnetpp.

### Enunciado

El objetivo es realizar la simulacion de una red tipo Jackson para estudiar el rendimiento de diferentes protocolos a nivel ARQ, lo protocolos que se estudiaran seran:

- Sin protocolo (FIFO)
- Stop & Wait
- Go-Back-N

![enunciado](files/Enunciado.png)

#### Stop & Wait

Condiciones de stop and wait

1. Se transmiten Packetes normales, Ack y Nack.
2. Se puede recuperar por ACK y NACK y timmer.
3. Los paquetes de la cola tienen prioridad sobre los que llegan.

##### Funcionamiento

Cuando el nodo A envia un paquete al nodo B pueden ocurrir 3 cosas, que se envie correctamente, que el paquete llegue con error al nodo B o que el paquete se pierda y no llegue nunca a l nodo B.

###### Diagrama de estados básico

<p align="center">
  <img src="files/sw_states.png" width = "60%"/> 
<p/>

###### Gestion de eventos

<p align="center">
  <img src="files/sw_eventos.png" width = "60%"/> 
<p/>

###### Ejemplos

- Envio sin error

<p align="center">
  <img src="files/sw_envio_sin_error.gif" width = "80%"/> 
<p/>

- Recuperacion por NACK

<p align="center">
  <img src="files/sw_recuperacion_nack.gif" width = "80%"/> 
<p/>

- Recuperacion por Timer

<p align="center">
  <img src="files/sw_recuperacion_timer.gif" width = "80%"/> 
<p/>
