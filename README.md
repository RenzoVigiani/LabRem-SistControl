# Protocolo para los laboratorios de "Sistemas de Control"

## Json a enviar (Indented style)

    {"Estado": [ 0, true, false], "Llaves": [ false, false ], "Analogico": [ 100, 200 ]}

## Sintaxis

**Estado**
Es un array conformado por 3 elementos en el siguiente orden: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Inicio del experimento | Laboratorio Seleccionado | Estado del experimento|
|-|-----|-----|---------------------------------------------|--------|
|1|true |true |Submuestreo, aliasing y efecto estroboscópico|Inicia  |
|1|true |false|Submuestreo, aliasing y efecto estroboscópico|Finaliza|
|1|false|true |Control automatico de posición               |Inicia  |
|1|false|false|Control automatico de posición               |Finaliza|

**Elementos por Laboratorio**

***Submuestreo, aliasing y efecto estroboscópico***

- Llaves:  [hab_agua, hab_luz]
- Analogico: [f_agua , f_luz]
  - Donde:
    - f_agua es la frecuencia de oscilación.
    - f_luz es la frecuencia de parpadeo.
***Control automatico de posición***

- Llaves: [hab_regulacion ]
- Analogico: [exitacion, frecuencia, regulacion]

## Prototipo API

**LAB 1**
<img alt="Sub Laboratorio 1" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab1.png">

**LAB 2**
<img alt="Sub Laboratorio 2" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab2.png">
