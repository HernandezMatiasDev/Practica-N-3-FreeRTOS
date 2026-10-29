#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t semT1;
SemaphoreHandle_t semT2;
SemaphoreHandle_t semT3;

void Tarea1(void*pvParameters)
{
  
  for(;;)
  {
    if (xSemaphoreTake(semT1, portMAX_DELAY) == pdTRUE)
    {
      Serial.println("Tarea 1");  
      xSemaphoreGive(semT3);
    }
  }
}
void Tarea2(void*pvParameters)
{
  for(;;)
  {
    if (xSemaphoreTake(semT2, portMAX_DELAY) == pdTRUE)
    {
      Serial.println("Tarea 2");  
      xSemaphoreGive(semT1);
    }
  }
}

void Tarea3(void*pvParameters)
{
  for(;;)
  {
    if (xSemaphoreTake(semT3, portMAX_DELAY) == pdTRUE)
    {
      Serial.println("Tarea 3");  
      xSemaphoreGive(semT2);
    }
  }
}

void setup() {
  Serial.begin(9600); 

  semT1 = xSemaphoreCreateBinary();
  semT2 = xSemaphoreCreateBinary();
  semT3 = xSemaphoreCreateBinary();

  //xTaskCreate(funcion, "Nombre", tamaño_pila, parametros, prioridad, handle)
  xTaskCreate(Tarea1, "T1", 128, NULL, 1, NULL);
  xTaskCreate(Tarea2, "T2", 128, NULL, 1, NULL);
  xTaskCreate(Tarea3, "T3", 128, NULL, 1, NULL);

  xSemaphoreGive(semT1);
  vTaskStartScheduler();
}

void loop() {}





