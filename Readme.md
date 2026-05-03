# PriorityTest - Comando para probar el planificador de MINIX

## 📋 Descripción

`PriorityTest` es una herramienta de prueba diseñada para evaluar el comportamiento del planificador de procesos (scheduler) del sistema operativo MINIX. Esta utilidad crea dos procesos con diferentes patrones de ejecución:

- **Proceso intensivo en CPU** (`cpuIntensiveFunc`): Ejecuta un bucle aritmético continuo que consume grandes cantidades de tiempo de procesador.
- **Proceso interactivo de E/S** (`IOFunc`): Genera salida por pantalla periódicamente, simulando un proceso con operaciones de entrada/salida.

El objetivo es observar cómo el planificador asigna prioridades dinámicamente, dando mayor prioridad al proceso interactivo (E/S) sobre el proceso intensivo en CPU.

## 📁 Ubicación del código fuente
/usr/src/minix/usr.bin/PlanificadorTest/

