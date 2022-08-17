# Protocolo para los laboratorios de "Sistemas de Control"
El procedimiento de comunicación es el siguiente.
Cada vez que se quiere modificar algun dato en el laboratorio. El Servidor debe enviar un
```Ruby 
POST /HTTP/1.1 + <JSON a enviar>
```

##### Json a enviar (ejemplo)
```Ruby 
{"Estado": [ 0, true, false], "Llaves": [ false, false ], "Analogico": [ 100, 200 ]}
```
Y para obtener avances de los datos o reporte de los datos debe enviar un:
```Ruby 
GET /HTTP/1.1
```
De esta forma el Arduino responde ante una petición.

##### Json a recibir (ejemplo)

```Ruby 
{"Estado": [ 0, true, false], "Llaves": [ false, false ], "Analogico": [ 100, 200], "Error": 0}
```

## **Interno Lab de control de posición**
Cada vez que se quiere modificar algun dato en el dispositivo de control de posición. El Servidor debe enviar un:
```Ruby 
WRITE + <JSON a enviar>
```

##### Json a enviar (ejemplo)
```Ruby 
{"PID": [0,0,0], "Planta": [0,0,0,0,0,0,0,0], "Exitacion": [0,0],"Errores":0}
```
>Donde: 
>
>Todas las variables son de 32 bits.
>- **PID:**
>   - P
>   - I
>   - D
>- **Planta=[polo0,polo1,polo2,polo3,cero0,cero1,cero2,k]:** Indica la cantidad de polos y ceros de la planta.
>   - Polos: Son las primeras 4 variables. Si se indica 1, el polo no se tiene en cuenta.
>   - Ceros: Son las 3 variables siguientes. Si se indica 1, el cero no se tiene en cuenta.
>   - K: Es la ultima variable. Corresponde a la ganancia. 
>   - Ej:[1,1,1,-2,1,1,-1,10] indica: $$F_s=\frac{10 \ (s+1)}{(s+2)} $$   
> - **Exitacion=[tipo , frecuencia]:** Indica el tipo de exitación de entrada.
>   - *Tipo:*
>     - 0: Escalón
>     - 1: Cuadrada
>     - 2: Senoidal
>     - 3: triangular
>   - *Frecuencia:* puede ser un numero entre 0 y 20000 expresado en Hz.
>- **Errores:**
>   - 0: Sin errores
>   - 1: Error 1
>   - 2: Error 2
>   - 3: Error 3

En cuanto a la obtención de los datos, el arduino está testeando constantemente el puerto serie y va almacenando los datos recibidos en una memoria para enviar por bloques al servidor.
Desde el dispositivo de control de posición se recibe.
```Ruby 
DATA <>
```
## Sintaxis

**Estado**
Es un array conformado por 3 elementos en el siguiente orden: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Inicio del experimento | Laboratorio Seleccionado | Estado del experimento|
|-|-----|-----|---------------------------------------------|--------|
|1|true |true |Submuestreo, aliasing y efecto estroboscópico|Inicia  |
|1|true |false|Submuestreo, aliasing y efecto estroboscópico|Finaliza|
|1|false|true |Control automatico de posición               |Inicia  |
|1|false|false|Control automatico de posición               |Finaliza|

**Error**
Es una variable numerica que representa un mensaje de error.

| Tipo de error                         |  Valor  |
| ----------------------------------    |---------|
| Sin errores                           |    0    |
| Error limites de angulo de azimut.    |    1    |
| Error limites de angulo de elevación  |    2    |
| Error de laboratorio incorrecto       |    3    |
**Elementos por Laboratorio**

***Submuestreo, aliasing y efecto estroboscópico***

- Llaves:  [habilitacion_agua, habilitacion_luz]
- Analogico: [frec_agua , frec_luz]
  - Donde:
    - frec_agua es la frecuencia de oscilación.
    - frec_luz es la frecuencia de parpadeo.
***Control automatico de posición***

- Llaves: [habilitacion_regulacion ]
- Analogico: [exitacion, frecuencia, regulacion]

## Diagramas

**Arduino Mega 2560**
Pin Out
<img alt = "Arduino Mega" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistemasDig/main/Imagenes/Arduino-Mega-Pinout.png" width=1920>

## Prototipo API

**LAB 1**
<img alt="Sub Laboratorio 1" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab1.png">

**LAB 2**
<img alt="Sub Laboratorio 2" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab2.png">
