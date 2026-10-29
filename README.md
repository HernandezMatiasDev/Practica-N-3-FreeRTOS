# Práctica Nº 3 – FreeRTOS

**Materia:** Programación en Tiempo Real

## Introducción

Este repositorio es la resolución de la **Práctica Nº 3 – FreeRTOS**. El enunciado completo está en este mismo repo: [Práctica Nº 3 (PDF)](Práctica Nro 3.pdf). Lo voy a ir referenciando a medida que resuelvo cada ejercicio.

Dentro del repositorio van a encontrar:

- Este README, que funciona como informe de la práctica.
- La carpeta [`Ejercicio_2`](Ejercicio_2) con los códigos de las 6 pruebas del ejercicio 2.
- La carpeta [`Ejercicio_4`](Ejercicio_4) con los códigos de las 3 secuencias del ejercicio 4.

## Antes de empezar: el entorno que usé

La práctica se podía hacer con una placa Arduino UNO física o simulando una en Proteus 8. Yo intente usar Proteus: me descargué el IDE de Arduino, armé en Proteus un Arduino UNO con una terminal virtual y seguí la guía de instalación.

<img width="1217" height="688" alt="proteus" src="https://github.com/user-attachments/assets/c830941e-4a6b-48ed-818c-bd1ddb84cbc5" />


El problema es que no sé si fue porque Proteus iba muy lento o por otra razón, pero las salidas de la consola no se comportaban como deberían: o nunca se interrumpían, o la consola se tildaba y directamente no imprimía nada. Así no había forma de sacar conclusiones de las pruebas.

Por eso terminé usando **[Wokwi](https://wokwi.com)** para escribir el código y simularlo directamente ahí. Además de que anduvo bien, no tuve que descargar nada y fue súper cómodo y práctico, porque tampoco hace falta generar el archivo `.hex` ni cargarlo a mano en el simulador. Escribís, le das play y ya ves la salida en la terminal serie.

En todos los códigos se usa la librería `Arduino_FreeRTOS`.

---

## Ejercicio 1

> *Investigar y analizar las posibilidades que brinda FreeRTOS para la creación y el manejo de tareas. Identificar cada una de las características que deben ser consideradas al momento de definir una tarea.*

### ¿Qué es FreeRTOS?

FreeRTOS es un sistema operativo de tiempo real embebido, de código abierto, escrito casi todo en C, portado a más de 35 arquitecturas y con mucha documentación y comunidad. Está pensado para ser mínimo (un binario compilado ocupa entre 4 KB y 9 KB) y para hardware orientado a entrada/salida más que a procesamiento pesado, con cambios de contexto rápidos.

Dentro de la clasificación de los SO embebidos, FreeRTOS es **dinámico**: la creación de tareas, la asignación de memoria y las estructuras de control se resuelven en tiempo de ejecución (en los sistemas estáticos todo eso se define en tiempo de compilación).

### Modos de funcionamiento

| Modo | Cómo funciona |
|---|---|
| **Cooperativo** | Una tarea sigue ejecutando hasta que ella misma cede el procesador (por ejemplo, bloqueándose con un delay). Se usa con `configUSE_PREEMPTION = 0`, y puede usar corrutinas (`configUSE_CO_ROUTINES`). |
| **Preemptive (preventivo)** | El planificador puede sacarle el procesador a una tarea en cualquier momento (en cada tick, o cuando una tarea de mayor prioridad queda lista). Se usa con `configUSE_PREEMPTION = 1`. |
| **Mixto** | Combina los dos esquemas. |

### Configuración (`FreeRTOSConfig.h`)

Parte de lo que se puede configurar del sistema:

| Parámetro | Para qué sirve |
|---|---|
| `configUSE_PREEMPTION` | 1 = RTOS preventivo, 0 = cooperativo |
| `configCPU_CLOCK_HZ` | Velocidad del reloj del sistema |
| `configTICK_RATE_HZ` | Cada cuánto se genera un tick del sistema |
| `configMAX_PRIORITIES` | Número máximo de prioridades a utilizar |
| `configMINIMAL_STACK_SIZE` | Tamaño mínimo de la pila |
| `configTOTAL_HEAP_SIZE` | Memoria disponible para el RTOS |
| `configUSE_MUTEXES` | Si vale 1, se habilita el uso de mutex |
| `configUSE_CO_ROUTINES` | Si vale 1, se habilitan las corrutinas (manejo cooperativo) |
| `INCLUDE_vTaskDelete` | Incluye en el build la función para borrar tareas |

### API para el manejo de tareas

| Función | Qué hace |
|---|---|
| `xTaskCreate` | Inicia (crea) una tarea |
| `vTaskDelete` | Finaliza una tarea |
| `vTaskDelay` | Detiene una tarea un tiempo determinado (medido en ticks) |
| `vTaskPrioritySet` | Asigna la prioridad de una tarea |
| `uxTaskPriorityGet` | Obtiene la prioridad de una tarea |
| `vTaskSuspend` | Suspende la ejecución de una tarea |
| `vTaskResume` | Reanuda la ejecución de una tarea |
| `vTaskStartScheduler` | Arranca el planificador (a partir de acá mandan las prioridades) |

### Estados de una tarea

Una tarea puede estar en alguno de estos estados:

- **Running**: es la que está usando el procesador en este momento.
- **Ready**: está lista para ejecutar, pero está esperando que le toque el procesador.
- **Blocked**: está esperando un evento (por ejemplo, que se cumpla un `vTaskDelay` o que llegue un semáforo). Se llega acá al llamar a una función bloqueante de la API.
- **Suspended**: fue suspendida con `vTaskSuspend()` y no vuelve a ser planificada hasta que alguien llame a `vTaskResume()`.

Ready, Blocked y Suspended se agrupan dentro del "super estado" **Not Running**. Una tarea recién creada arranca en Ready.

### Características a considerar al definir una tarea

Todo esto se ve en los parámetros de `xTaskCreate`:

```c
xTaskCreate(Tarea1, "T1", 128, NULL, 1, NULL);
```

| Parámetro | En el ejemplo | Qué es |
|---|---|---|
| Función | `Tarea1` | Puntero a la función que implementa la tarea (normalmente tiene un `for(;;)` infinito adentro). |
| Nombre | `"T1"` | Nombre de la tarea (identificador, sirve más que nada para debug). |
| Tamaño de pila | `128` | Memoria de pila que se le reserva a la tarea. Hay que dimensionarla bien porque la RAM es muy poca. |
| Parámetros | `NULL` | Puntero a los parámetros que se le pasan a la tarea. |
| Prioridad | `1` | Prioridad de la tarea (0 es la mínima). Es lo que usa el planificador para decidir quién ejecuta. |
| Handle | `NULL` | Manejador de la tarea, para poder referenciarla después (borrarla, suspenderla, cambiarle la prioridad, etc.). |

Además de esos parámetros, también hay que pensar en qué modo de planificación se va a trabajar (cooperativo o preventivo), cada cuánto es el tick del sistema y cómo se va a comportar la tarea respecto de los estados (si se bloquea o no, y por qué evento).

---

## Ejercicio 2

> *Desarrollar una aplicación de planificación que contenga 3 tareas. Las mismas deben competir por imprimir en la terminal un mensaje con el nombre de cada tarea. Realizar pruebas variando las prioridades de cada una de las tareas, generando así diferentes escenarios en el mensaje de salida de la terminal.*

La aplicación base tiene tres tareas (`Tarea1`, `Tarea2` y `Tarea3`). Cada una, dentro de un `for(;;)`, imprime por el puerto serie "Tarea 1", "Tarea 2" o "Tarea 3" según corresponda. Después fui jugando con dos cosas: las **prioridades** de cada tarea y si tienen o no un **`vTaskDelay`** al final del ciclo.

Resumen de las 6 pruebas (en todas el tamaño de pila es 128):

| Prueba | Prioridad T1 / T2 / T3 | `vTaskDelay` | Código |
|---|---|---|---|
| Test 1 | 1 / 1 / 1 | 200 / 200 / 200 | [Test_1.ino](Ejercicio_2/Test_1.ino) |
| Test 2 | 1 / 2 / 3 | 200 / 200 / 200 | [Test_2.ino](Ejercicio_2/Test_2.ino) |
| Test 3 | 1 / 2 / 1 | 200 / 200 / 100 | [Test_3.ino](Ejercicio_2/Test_3.ino) |
| Test 4 | 1 / 1 / 1 | sin delay | [Test_4.ino](Ejercicio_2/Test_4.ino) |
| Test 5 | 1 / 2 / 3 | sin delay | [Test_5.ino](Ejercicio_2/Test_5.ino) |
| Test 6 | 1 / 3 / 3 | sin delay | [Test_6.ino](Ejercicio_2/Test_6.ino) |

### Test 1: misma prioridad y solo con delay

<img width="1865" height="829" alt="test_1" src="https://github.com/user-attachments/assets/b52acd1b-4929-4b84-966a-7a7ee652e5b8" />

Las tres tareas tienen la misma prioridad (1) y un `vTaskDelay(200)` al final de cada ciclo. El resultado es la secuencia `Tarea 1 – Tarea 2 – Tarea 3` repetida una y otra vez, **en orden y sin que se pisen los mensajes**.

Esto me llamó la atención: esperaba que, al tener la misma prioridad y competir por la terminal, se mezclaran los mensajes. Así que me puse a investigar:

- Al arrancar el planificador, las tres tareas están en estado *Ready* con la misma prioridad. Se ejecutan en el orden en que se crearon (T1, T2, T3).
- Cada tarea imprime su mensaje y enseguida llama a `vTaskDelay`, que la pasa a estado *Blocked*. Imprimir una línea es muy rápido comparado con la duración de un tick: `Serial.println` solo copia los caracteres al buffer de transmisión de la UART y retorna, y la transmisión real se hace por interrupciones. O sea que cada tarea termina de imprimir antes de que el planificador tenga ocasión de interrumpirla.
- Mientras las tres están bloqueadas, el procesador queda con la tarea *idle* (la tarea de inactividad de FreeRTOS).
- Cuando se cumplen los 200 ticks, las tres vuelven a *Ready* en el mismo orden en que se habían bloqueado, y todo se repite.

Las tareas pasan casi todo el tiempo bloqueadas y casi nunca compiten realmente por el procesador. Como detalle, en la librería para Arduino UNO el tick es de aproximadamente 15 ms, así que 200 ticks son unos 3 segundos. Coincide con lo que se ve en Wokwi.

### Test 2: distintas prioridades con delay

<img width="1854" height="844" alt="test_2" src="https://github.com/user-attachments/assets/3b5c185a-f00d-4d33-a0a7-b72e52c72d56" />


Ahora T1 tiene prioridad 1, T2 prioridad 2 y T3 prioridad 3, con el mismo `vTaskDelay(200)`. Cambia el orden de impresión: ahora sale `Tarea 3 – Tarea 2 – Tarea 1`. Siguen sin pisarse.

La razón es que cuando las tres se despiertan juntas, el planificador elige siempre la tarea *Ready* de **mayor prioridad**. Corre T3, imprime y se bloquea; entonces corre T2, imprime y se bloquea; y por último T1. Como las tres se bloquean enseguida, ninguna llega a interrumpir a otra a mitad de línea.

### Test 3: prioridades mixtas y delays distintos

<img width="1881" height="820" alt="test_3" src="https://github.com/user-attachments/assets/0817d215-450f-47cd-94c1-2dcdd3f4d6bc" />


Acá dejé T1 y T3 con la misma prioridad (1) y a T2 con una prioridad mayor (2). Además, a T3 le puse un delay de la mitad, `vTaskDelay(100)`, mientras que T1 y T2 siguen con 200.

La salida que se repite es `Tarea 2 – Tarea 1 – Tarea 3 – Tarea 3`. Me resultó curioso porque el patrón es súper estable:

- Siempre va primero la **2**, lo cual tiene sentido porque tiene mayor prioridad.
- Después siempre viene la **1**, y a continuación la **3**. T1 y T3 tienen la misma prioridad, así que se atienden en el orden en que quedaron listas (T1 se creó antes que T3).
- Y luego siempre aparece la **3 una segunda vez**: como su delay es de 100 ticks (la mitad), se despierta a mitad del ciclo de las otras dos, cuando T1 y T2 todavía están bloqueadas, e imprime sola.

Entonces cada 200 ticks las tres coinciden y se imprime `2 – 1 – 3`, y 100 ticks después se despierta solo la tarea 3 y agrega el segundo `3`. Tampoco se mezclan los mensajes, por la misma razón de antes.

### Test 4: misma prioridad y sin delays

<img width="1843" height="879" alt="test_4" src="https://github.com/user-attachments/assets/acaadb8b-b1fc-470e-8e92-8f44fca4824f" />


Acá se pone divertido. Saqué los `vTaskDelay` y dejé las tres tareas con la misma prioridad. Ahora ninguna se bloquea nunca, así que las tres están siempre listas para ejecutar. La salida queda **toda mezclada**: aparecen "Tarea" cortados, números pegados a la línea anterior, `Tarea 1Tarea 2`, etc.

Explicación técnica:

- Con tareas de igual prioridad, el planificador preventivo hace *time slicing*: en cada tick le saca el procesador a la tarea que está corriendo y se lo da a la siguiente.
- Como ninguna cede el procesador por su cuenta, el cambio de contexto llega **en cualquier punto**, incluso en medio de un `Serial.println`.
- Para peor, al no haber delays el buffer de transmisión se llena rápido y el `println` se queda esperando lugar. En ese rato es muy probable que llegue un tick y se cambie de tarea con la línea a medio escribir.
- La terminal serie es un recurso compartido y no hay ningún mecanismo que garantice que una tarea termine de escribir antes de que otra empiece. Se interrumpen mutuamente y los caracteres se intercalan.

Esto es el problema que se resuelve con las herramientas de sincronización de los ejercicios 3 y 4.

### Test 5: distintas prioridades y sin delays

<img width="1872" height="750" alt="test_5" src="https://github.com/user-attachments/assets/1ff5ea0d-5262-4728-a8ec-b3fa496eda98" />


Con T1 = 1, T2 = 2 y T3 = 3 y sin delays, **solo se imprime `Tarea 3`**, una y otra vez. Nunca aparecen la 1 ni la 2.

La razón es cómo funciona un planificador preventivo con prioridades fijas: siempre ejecuta la tarea *Ready* de mayor prioridad. T3 es la de mayor prioridad y, al no tener ningún `vTaskDelay` ni esperar ningún semáforo, **nunca pasa a *Blocked***: está siempre lista. Cada tick el planificador se fija si hay alguna tarea más prioritaria que T3, no encuentra ninguna y le devuelve el procesador a ella misma. T1 y T2 se quedan en *Ready* para siempre sin que les toque el procesador (esto se conoce como *starvation* o inanición). Ni siquiera llega a ejecutar la tarea *idle*.


### Test 6: dos tareas con la misma prioridad alta

<img width="1857" height="777" alt="test_6" src="https://github.com/user-attachments/assets/c1ed6462-f443-4053-94f7-b50f8e4692a5" />


Acá T2 y T3 tienen prioridad 3 y T1 tiene prioridad 1, sin delays. Pasan dos cosas a la vez:

- **La Tarea 1 nunca sale**, por el mismo motivo del test 5: hay tareas de mayor prioridad que nunca se bloquean, entonces T1 jamás recibe el procesador.
- **T2 y T3 compiten entre sí** por el procesador. Como tienen la misma prioridad, el planificador las alterna en cada tick, y por lo tanto se pisan mutuamente como en el test 4, con mensajes cortados y mezclados.

---

## Ejercicio 3

> *Investigar y analizar las herramientas que provee FreeRTOS para la sincronización de tareas.*

FreeRTOS ofrece varias herramientas para que las tareas se coordinen, se comuniquen y no se pisen al usar recursos compartidos. Las principales son los **semáforos** y las **colas**. Para usar los semáforos en Arduino hay que incluir `#include <semphr.h>`.

### Semáforos binarios

Tienen solo dos valores (disponible o no disponible). Se crean con `xSemaphoreCreateBinary()` y, al crearse, arrancan **sin estar disponibles**: hace falta un `give` para que alguna tarea pueda hacer `take`.

- `xSemaphoreTake(sem, timeout)`: toma el semáforo. Si no está disponible, la tarea pasa a *Blocked* hasta que lo esté o se cumpla el timeout (con `portMAX_DELAY` espera indefinidamente).
- `xSemaphoreGive(sem)`: libera el semáforo y deja el acceso disponible para las demás tareas.

Una tarea puede hacer el `take` y otra tarea distinta puede ser la que hace el `give`. Eso los hace ideales para control por eventos y para sincronizar tareas entre sí, que es justo lo que use en el ejercicio 4.

### Semáforos contadores

Son como los binarios, pero con un contador que puede valer más que 1. Cada `give` suma uno al contador y cada `take` resta uno (si está en cero, la tarea se bloquea). Sirven para contar eventos o para administrar un conjunto de N recursos idénticos. Se crean con `xSemaphoreCreateCounting(max, inicial)`.

**Aplicación al Ejercicio 4.** Estos semáforos también habrían servido para resolver los patrones con tareas repetidas. Por ejemplo, para la secuencia B (Tarea 2 - Tarea 2 - Tarea 3 - Tarea 1), se podría crear el semáforo de la Tarea 2 con capacidad para dos permisos:

```cpp
semT2 = xSemaphoreCreateCounting(2, 0);
```

La tarea anterior (Tarea 1) le entregaría dos permisos antes de pasarle el turno:

```cpp
xSemaphoreGive(semT2);
xSemaphoreGive(semT2);
```

Del mismo modo, en `setup()` se daría el permiso inicial dos veces para que la secuencia arranque en la Tarea 2. La Tarea 2, por su parte, consumiría ambos permisos (un `take` por cada impresión) antes de darle el turno a la Tarea 3.

Un semáforo binario no sirve para esto, porque no acumula permisos: el segundo `give` se pierde si el semáforo ya vale 1.

Por qué no se eligió esta solución. Con este enfoque, el código de cada tarea queda atado a un patrón específico: cantidad de `give` y de `take`, capacidad del semáforo y permisos iniciales. Para pasar de la secuencia A a la B o a la C habría que modificar el cuerpo de las tareas cada vez. Y queria hacer algo mas general, por eso se optó por semáforos binarios (uno por tarea) junto con una tabla que define la secuencia y un índice que indica el paso actual. Así, cambiar de patrón solo requiere editar el arreglo `secuencia`, sin tocar la lógica de las tareas.

### Mutex (exclusión mutua)

Sirven para proteger un recurso compartido (por ejemplo la terminal). Se habilitan con `configUSE_MUTEXES = 1` y se crean con `xSemaphoreCreateMutex()`; también existen los **recursivos**, que la misma tarea puede tomar varias veces seguidas (`xSemaphoreCreateRecursiveMutex()`). La idea es "el que toma el mutex es el que lo devuelve". A diferencia de un semáforo binario, el mutex incluye **herencia de prioridad**, que ayuda a evitar la inversión de prioridades.

### Colas (queues)

Sirven para el pasaje de mensajes entre tareas: una tarea pone datos y otra los saca, y el RTOS se encarga de bloquear y desbloquear según haya datos o lugar.

| Función | Qué hace |
|---|---|
| `xQueueCreate` | Crea una cola, especificando cantidad y tamaño de ítems |
| `xQueueSend` | Pone un nuevo dato en la cola |
| `xQueueReceive` | Saca un dato de la cola |
| `xQueuePeek` | Lee un dato de la cola, pero sin sacarlo |
| `uxQueueSpacesAvailable` | Devuelve la cantidad de lugares libres en la cola |

### Resumen

| Herramienta | Para qué se usa |
|---|---|
| Semáforo binario | Señalizar eventos / sincronizar una tarea con otra |
| Semáforo contador | Contar eventos o administrar varios recursos iguales |
| Mutex | Exclusión mutua sobre un recurso compartido |
| Cola | Pasar datos entre tareas |

Además de estas, FreeRTOS tiene otras herramientas, como los *event groups* y las notificaciones directas a tareas (*task notifications*). No las usé en esta práctica.

---

## Ejercicio 4

> *Utilizando las herramientas de sincronización, implementar una aplicación multitarea que muestre constantemente en la terminal las siguientes secuencias:*
> - *A. Tarea 1 - Tarea 3 - Tarea 2*
> - *B. Tarea 2 - Tarea 2 - Tarea 3 - Tarea 1*
> - *C. Tarea 3 - Tarea 3 - Tarea 3 - Tarea 1 - Tarea 2*
>
> *Todas las tareas deben tener la misma prioridad.*

### Ejercicio 4-A

<img width="438" height="758" alt="4-A" src="https://github.com/user-attachments/assets/338b7d60-a921-4e2a-a489-676d94cb212d" />

En esta imagen no pongo el código porque no entra en una captura. Lo pueden ver completo acá: [Ejercicio_4_A.ino](Ejercicio_4/Ejercicio_4_A.ino).

La idea es usar un semáforo binario por tarea (`semT1`, `semT2` y `semT3`), y que cada tarea se quede bloqueada esperando el suyo:

1. Cada tarea hace `xSemaphoreTake` de su propio semáforo con `portMAX_DELAY`, o sea que se bloquea hasta que alguien se lo dé.
2. Cuando lo consigue, imprime su mensaje y le hace `xSemaphoreGive` al semáforo de la tarea que tiene que ir después.
3. Para lograr la secuencia `1 – 3 – 2`, los semáforos se encadenan así: la Tarea 1 le da el turno a la Tarea 3, la Tarea 3 se lo da a la Tarea 2, y la Tarea 2 se lo devuelve a la Tarea 1.
4. En el `setup` se crean los tres semáforos (arrancan vacíos, así que ninguna tarea puede avanzar) y se hace un `xSemaphoreGive(semT1)` antes de arrancar el planificador, para darle la largada a la Tarea 1.

Como en todo momento hay un único semáforo disponible, solo una tarea puede estar imprimiendo, y las tres pueden tener la misma prioridad sin que se pisen, que es lo que pide el enunciado.

La verdad es que fue divertido armar los semáforos, y muy interesante hacerlo directamente en código,

### Ejercicio 4-B y 4-C

Este fue el ejercicio más difícil. Para las secuencias B y C hay tareas que se repiten (la 2 dos veces seguidas, la 3 tres veces), y no queria hacer una cadena fija como en el 4-A, queria hacer una solución general.

A diferencia del 4-A, acá no dejo solo el enlace al código: lo explico con imágenes. El código completo está en [Ejercicio_4_B-C.ino](Ejercicio_4/Ejercicio_4_B-C.ino).

#### Cómo funciona

<img width="1216" height="800" alt="configuracion" src="https://github.com/user-attachments/assets/1bfcad50-26b4-48cb-b81f-451d0917321e" />

La idea es tener **una sola función `Tarea`** que sirve para las tres tareas y una **tabla con la secuencia** que se quiere imprimir:

- `sem[3]`: un semáforo binario por tarea.
- `numero_tarea[3] = {1, 2, 3}`: el número de cada tarea. Se le pasa a cada una por el parámetro `pvParameters` de `xTaskCreate`, así la misma función sabe quién es (`numero = *(byte *)pvParameters`).
- `secuencia[]`: el orden en el que quiero que se impriman las tareas (por ejemplo `{2,2,3,1}`).
- `longitud = sizeof(secuencia)`: la cantidad de pasos de la secuencia (sirve porque los elementos son de 1 byte). Se calcula al principio como constante para ahorrar procesamiento al no tener que calcularla varias veces.
- `tarea_actual`: el índice del paso de la secuencia en el que estamos.

Cada tarea hace lo siguiente en su ciclo:

1. Se bloquea con `xSemaphoreTake(sem[numero - 1], portMAX_DELAY)` esperando **su** semáforo.
2. Cuando lo consigue, imprime `Tarea N`.
3. Avanza el índice: `tarea_actual = (tarea_actual + 1) % longitud`. El `%` hace que, al llegar al final de la secuencia, vuelva a empezar de cero y la secuencia se repita para siempre.
4. Le da el semáforo a la tarea que corresponde al nuevo paso: `xSemaphoreGive(sem[secuencia[tarea_actual] - 1])`.

Dos cosas que hacen que esto ande bien:

- Hay un solo semáforo disponible a la vez (el del turno actual), así que nunca imprimen dos tareas al mismo tiempo. Por eso `tarea_actual` tampoco necesita protección extra: solo la modifica la tarea que tiene el turno.
- Si la secuencia pide que la misma tarea vaya dos veces seguidas (como el `2, 2`), la tarea se da el semáforo a sí misma. Al volver al inicio de su ciclo, el `take` lo consigue enseguida y vuelve a imprimir.

#### Cómo se inicializa

<img width="1137" height="663" alt="setup" src="https://github.com/user-attachments/assets/0f141d40-89dc-46e0-b266-71a16e5c1798" />


En el `setup`:

1. Se inicia el puerto serie con `Serial.begin(9600)`.
2. Con un `for` se crean los tres semáforos binarios (quedan vacíos, así que todas las tareas arrancan bloqueadas).
3. Se crean las tres tareas con `xTaskCreate`, todas con **la misma función `Tarea`**, la **misma prioridad (1)** y un parámetro distinto (`&numero_tarea[0]`, `&numero_tarea[1]` y `&numero_tarea[2]`).
4. Se le da el primer turno a la tarea que corresponde al primer paso de la secuencia: `xSemaphoreGive(sem[secuencia[tarea_actual] - 1])`.
5. Se arranca el planificador con `vTaskStartScheduler()`.

#### Resultado 4-B: `Tarea 2 – Tarea 2 – Tarea 3 – Tarea 1`

<img width="530" height="762" alt="4-B" src="https://github.com/user-attachments/assets/ff773904-bd53-4980-9872-27de20530bfa" />


Con `secuencia = {2, 2, 3, 1}` se imprime la secuencia B, repetida de forma constante y sin mezclarse.

#### Resultado 4-C: `Tarea 3 – Tarea 3 – Tarea 3 – Tarea 1 – Tarea 2`

<img width="346" height="693" alt="4-C" src="https://github.com/user-attachments/assets/257a29d9-abb9-40cd-9d09-c8262a9451c7" />


Con `secuencia = {3, 3, 3, 1, 2}` se imprime la secuencia C. Lo único que cambié respecto de la prueba anterior fue el contenido del arreglo `secuencia`.

#### Cualquier secuencia que queramos

<img width="270" height="304" alt="1,1,1,1,1,2,1,3" src="https://github.com/user-attachments/assets/8ae529d1-23cb-4140-b269-6e5c7e9577fe" />

Para probar que la solución es realmente general, armé una secuencia rara: `{1,1,1,1,1,2,1,3}`. En la imagen se ve cómo sale cinco veces la Tarea 1, después la 2, otra vez la 1 y por último la 3, y vuelve a empezar.

Con este código podemos hacer cualquier secuencia que queramos, de la longitud que queramos y con las tareas repetidas las veces que haga falta: alcanza con cambiar el arreglo `secuencia`. No hay que tocar ni las tareas ni los semáforos ni el `setup`.

---

## Estructura del repositorio

```
.
├── README.md
├── Práctica_Nro_3.pdf
├── Ejercicio_2/
│   ├── Test_1.ino
│   ├── Test_2.ino
│   ├── Test_3.ino
│   ├── Test_4.ino
│   ├── Test_5.ino
│   └── Test_6.ino
└── Ejercicio_4/
    ├── Ejercicio_4_A.ino
    └── Ejercicio_4_B-C.ino
```
