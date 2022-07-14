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
