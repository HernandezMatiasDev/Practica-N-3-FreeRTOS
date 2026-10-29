#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t sem[3];
const byte numero_tarea[3] = {1,2,3};

const byte secuencia[] = {1,1,1,1,1,2,1,3};// 4-a {2,2,3,1}, 4-b {3,3,3,1,2}
const byte longitud = sizeof(secuencia);

byte tarea_actual = 0;

void Tarea(void*pvParameters)
{
  byte numero = *(byte *)pvParameters;

  for(;;)
  {
    if (xSemaphoreTake(sem[numero - 1], portMAX_DELAY) == pdTRUE)
    {
      Serial.print("Tarea ");  
      Serial.println(numero);
      tarea_actual = (tarea_actual + 1) % longitud;
      xSemaphoreGive(sem[secuencia[tarea_actual] - 1]);
    }
  }
}

void setup() {
  Serial.begin(9600); 

  for(byte i = 0; i < 3; i++)
  {
    sem[i] = xSemaphoreCreateBinary();
  }
  
  //xTaskCreate(funcion, "Nombre", tamaño_pila, parametros, prioridad, handle)
  xTaskCreate(Tarea, "T1", 128, (void *)&numero_tarea[0], 1, NULL);
  xTaskCreate(Tarea, "T2", 128, (void *)&numero_tarea[1], 1, NULL);
  xTaskCreate(Tarea, "T3", 128, (void *)&numero_tarea[2], 1, NULL);

  xSemaphoreGive(sem[secuencia[tarea_actual] - 1]);
  vTaskStartScheduler();
}

void loop() {}





