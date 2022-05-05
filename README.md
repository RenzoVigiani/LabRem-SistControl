## Protocolo para los laboratorios de Sistemas de Control


#### Json a enviar (Indented style)
	
	{
	"Estado": [ 0, true, false],
	"Llaves": [	false,	false ],
	"Analogico": [	100,	200	]
	}

**Sintaxis**
#### Estado: [Laboratorio, SubLab, Inicio del experimento]

|Laboratorio  | Sub Laboratorio  | Estado del Experimento |
| ------------ | ------------ | ------------ |
| 1  | true  |  Sistemas de control: Aliassing |
| 1  | false  | Sistemas de control: Control automatico de posición |

**Inicio del experimento:**

- true: Inicia el experimento
- false: Finaliza el experimento

#### Elementos por Laboratorio

- Aliassing
  - Llaves:  [hab_agua, hab_luz]
  - Analogico: [f_agua , f_luz]
    - Donde:
      - f_agua es la frecuencia de oscilación.
      - f_luz es la frecuencia de parpadeo.
- Control automatico de posición
  - Llaves: [hab_regulacion ] 
  - Analogico: [exitacion, frecuencia, regulacion]

## Prototipo API

LAB 1

<img alt="Sub Laboratorio 1" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab1.png">

LAB 2

<img alt="Sub Laboratorio 2" src="https://raw.githubusercontent.com/RenzoVigiani/LabRem-SistControl/main/Imagenes/Lab2.png">
