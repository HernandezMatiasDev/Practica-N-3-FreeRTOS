#include <Arduino_FreeRTOS.h>

void Tarea1(void*pvParameters)
{
  for(;;)
  {
    Serial.println("Tarea 1");
  }
}
void Tarea2(void*pvParameters)
{
  for(;;)
  {
    Serial.println("Tarea 2");
  }
}

void Tarea3(void*pvParameters)
{
  for(;;)
  {
    Serial.println("Tarea 3");
  }
}

void setup() {
  Serial.begin(9600); 
  
  //xTaskCreate(funcion, "Nombre", tamaño_pila, parametros, prioridad, handle)
  xTaskCreate(Tarea1, "T1", 128, NULL, 1, NULL);
  xTaskCreate(Tarea2, "T2", 128, NULL, 1, NULL);
  xTaskCreate(Tarea3, "T3", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {}





