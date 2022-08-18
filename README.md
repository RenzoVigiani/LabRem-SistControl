# Protocolo para los laboratorios de "Sistemas de Control"
El procedimiento de comunicación es el siguiente.
Cada vez que se quiere modificar algun dato en el laboratorio. El Servidor debe enviar un
```Ruby 
POST /HTTP/1.1 + <JSON a enviar>
```

#### <ins>**Json a enviar (ejemplo)**</ins>
_Sub-Lab: Submuestreo, aliasing y efecto estroboscópico_
```Ruby 
{"Estado": [ 1, false, false], "Habilitadores": [ false, false ], "Frecuencias": [ 0, 0 ]}
```
>Donde:
> - **Habilitadores** es un array de dos variables booleanas.
>``` Habilitadores:[Habilitacion de agua, Habilitacion de luz]```
> - **Frecuencia** es un array de dos variables enteras de 32bit que indican la frecuencias de agua y luz. 
> ```Frecuencias:[Frecuencia de agua , Frecuencia de luz]```

_SubLab: Control automatico de posición_
```Ruby 
{"Estado": [ 1, true, false], "Exitacion": [ 0, 0 ], "Regulacion": [ 0, 0, 0 ], "Planta":[0,0,0,0,1,1,1,2]}
```
>Donde:
> - **Exitacion** es un array de dos variables enteras de 32 bits que indican el tipo de exitación y su frecuencia.
>    ```"Exitacion":[Tipo, frecuencia] ```
> - **Regulacion** es un array de tres variables enteras de 32 bits que indican el tipo de regulación. ```"Regulacion":[P,I,D] ```
> - **Planta** es un array de ocho variables de 32 bits que indican los polos, ceros y la constante k de la planta a utilizar.
> ```"Planta":[Polo_0, Polo_1, Polo_2, Polo_3, Cero_0, Cero_1, Cero_2, k] ```

Y para obtener avances de los datos o reporte de los datos debe enviar un:
```Ruby 
GET /HTTP/1.1
```
De esta forma el Arduino responde ante una petición.

#### <ins>**Json a recibir (ejemplo)**</ins>
_Sub-Lab: Submuestreo, aliasing y efecto estroboscópico_
```Ruby 
{"Estado": [ 0, false, true], "Habilitadores": [ false, false ], "Frecuencias": [ 0, 0 ], "Errores":0 }
```
> Cada no de las variables indica el estado actual.

_SubLab: Control automatico de posición_
```Ruby 
{"Estado":[2,true,true],"Posicion":[0], "Velocidad":[0], "Tiempo":[0],"Exitacion":[ 0 , 10 ]}
```
>Donde: 
> - **Exitación:** Señal y frecuencia de exitación utilizada. Es la misma para un experimento en concreto.
>   - *Tipo:*
>     - 0: Escalón
>     - 1: Cuadrada
>     - 2: Senoidal
>     - 3: triangular
>   - *Frecuencia:* puede ser un numero entre 1 y 1000 expresado en Hz.
> - Cada una de las siguientes variables son arrays de hasta 200 valores cada uno.
>   - **Tiempo:** Se refiere al tiempo que dura el experimento. Se establece un tiempo de default (60 s o 120 s). se expresa en segundos (s).
>   - **Velocidad:** Velocidad angular obtenida. Se mide en: [rad/s]
>   - **Posición:** Posición obtenida. Se mide en [m]
> 
> **Estos datos deben ser guardados en una tabla para luego ser graficados.**   

## Sintaxis

**Estado:**
Es un array conformado por 3 elementos en el siguiente orden: ``[Laboratorio, SubLab, Inicio del experimento]``

>Donde:
> - **Laboratorio:** es el numero de laboratorio. En este caso ``1``
> - **SubLab:** es el sub laboratorio seleccionado. En este caso
  ``false = SubLab1 y true = SubLab2``
>   - SubLab1= _``Sub-Lab: Submuestreo, aliasing y efecto estroboscópico``_
>   - SubLab2= _``SubLab: Control automatico de posición``_
> - **Inicio del experimento** es una variable booleana que indica el inicio del experimento. ``false = parado y true = iniciado``

**Error:**
Es una variable numerica que representa un mensaje de error.

| Tipo de error |  Valor  |
| ------------- | ------- |
| Sin errores   |    0    |
| Error 1       |    1    |
| Error 2       |    2    |
| Error 3       |    3    |


## **Protocolo Interno Lab de control de posición**
Cada vez que se quiere modificar algun dato en el dispositivo de control de posición. El Servidor debe enviar un:

##### Json a enviar (ejemplo)
```Ruby 
{"Regulacion": [0,0,0], "Planta": [0,0,0,0,0,0,0,0], "Exitacion": [0,0],"Errores":0}
```
>Donde: 
>
>Todas las variables son de 32 bits.
>- **Regulación:**
>   - P - Constante a modificar
>   - I - Constante a modificar
>   - D - Constante a modificar
>- **Planta=[polo0,polo1,polo2,polo3,cero0,cero1,cero2,k]:** 
>
>   Indica la cantidad de polos y ceros de la planta.
>   - Polos: Son las primeras 4 variables. Si se indica 1, el polo no se tiene en cuenta.
>   - Ceros: Son las 3 variables siguientes. Si se indica 1, el cero no se tiene en cuenta.
>   - K: Es la ultima variable. Corresponde a la ganancia. 
>    
>   **Ej:[1,1,1,-2,1,1,-1,10] indica:** $$F_s=\frac{10 \ (s+1)}{(s+2)} $$   
> - **Exitacion=[tipo , frecuencia]:** Indica el tipo de exitación de entrada.
>   - *Tipo:*
>     - 0: Escalón
>     - 1: Cuadrada
>     - 2: Senoidal
>     - 3: triangular
>   - *Frecuencia:* puede ser un numero entre 0 y 1000 expresado en Hz.
> - **Errores:**
> Es una variable numerica que representa un mensaje de error.
> 
> | Tipo de error |  Valor  |
> | ------------- | ------- |
> | Sin errores   |    0    |
> | Error 1       |    1    |
> | Error 2       |    2    |
> | Error 3       |    3    |


En cuanto a la obtención de los datos, el arduino está testeando constantemente el puerto serie y va almacenando los datos recibidos en una memoria para enviar por bloques al servidor.
Desde el dispositivo de control de posición se recibe.

##### Json a recibir (ejemplo)
```Ruby 
{"Posicion":0, "Velocidad":0, "Tiempo":10,"Exitacion":[0,10]}
```
>Donde:
> - **Tiempo:** Se refiere al tiempo que dura el experimento. Se establece un tiempo de default (60 s o 120 s). se expresa en segundos (s).
> - **Velocidad:** Velocidad angular obtenida. Se mide en: [rad/s]
> - **Posición:** Posición obtenida. Se mide en [m]
> - **Exitación:** Señal y frecuencia de exitación utilizada.
   
## Diagramas

**Arduino Mega 2560**
Pin Out
<img alt = "Arduino Mega" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pinout.png" width=1920>

## Prototipo API

**LAB 1**
<img alt="Sub Laboratorio 1" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab1.png">

**LAB 2**
<img alt="Sub Laboratorio 2" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab2.png">
