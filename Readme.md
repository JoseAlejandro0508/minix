# Nuevo Planificador de MINIX

## 📋 Descripción General

Se implementó una política de planificación basada en **ventanas temporales** para el 
scheduler de MINIX, reemplazando la política original que penalizaba los procesos de 
forma inmediata cada vez que agotaban su quantum. El nuevo enfoque evalúa el 
comportamiento de cada proceso a lo largo de una ventana de tiempo completa antes 
de tomar decisiones de prioridad, logrando una clasificación más justa entre procesos 
intensivos en CPU y procesos interactivos de E/S.

---

## 🗂️ Archivos Modificados

### 1. `minix/servers/sched/schedproc.h`
Se modificó la estructura `schedproc`, que representa la entrada de cada proceso en 
la tabla del scheduler. Se agregó el campo:

```c
unsigned quantumConsumedTimes;
```

Este campo actúa como **contador de ventana**: registra cuántas veces un proceso 
agotó completamente su quantum de CPU dentro del intervalo de balance actual. Se 
inicializa en `0` al crear el proceso y se reinicia al finalizar cada ventana.

---

### 2. `minix/servers/sched/schedule.c`
Este archivo concentra la mayor parte de los cambios. Las funciones modificadas e 
implementadas son:

#### `do_noquantum()` — Modificada
Función llamada por el kernel cada vez que un proceso agota su quantum. En la 
política original, esta función penalizaba la prioridad del proceso de forma inmediata. 
En la nueva implementación:
- Solo **incrementa el contador** `quantumConsumedTimes`.
- No modifica la prioridad directamente.
- Delega la decisión de penalización al balance periódico.

```c
rmp->quantumConsumedTimes++;  // registra el agotamiento, sin penalizar aún
```

#### `reset_quantumsConsumedTimes()` — Nueva
Función auxiliar que reinicia el contador de quantums consumidos de un proceso al 
finalizar cada ventana de balance:

```c
void reset_quantumsConsumedTimes(struct schedproc *rmp) {
    rmp->quantumConsumedTimes = 0;
}
```

#### `balance_queues()` — Modificada
Se le agregó la llamada a `reset_quantumsConsumedTimes()` para que al finalizar 
cada ventana se limpie el contador de todos los procesos activos, preparándolos para 
la siguiente ventana.

#### `new_balance_queues()` — Nueva
Es el núcleo de la nueva política. Se ejecuta periódicamente mediante el mecanismo 
de alarma del sistema (`sys_setalarm`). Por cada proceso activo evalúa su 
comportamiento en la ventana que acaba de terminar y toma una de tres decisiones:

| Comportamiento en la ventana | Acción |
|---|---|
| Consumió **≥ N quantums** (intensivo en CPU) | Baja prioridad un nivel |
| Consumió **0 quantums** (interactivo / E/S) | Sube prioridad un nivel |
| Consumió entre 1 y N-1 quantums | No cambia la prioridad |

Donde `N = MAX_QUANTUM_CONSUMED_TIMES = 3`.

Además respeta los límites del sistema en ambas direcciones:
- No baja de `MIN_USER_Q` (cola 15, la peor).
- No sube de `MAX_USER_Q` (cola 0, la mejor).

---

## 🧪 Herramienta de Prueba: `PlanificadorTest`

Ubicada en `/usr/src/minix/usr.bin/PlanificadorTest/`, esta utilidad fue desarrollada 
para observar empíricamente el comportamiento del nuevo scheduler. Utiliza `fork()` 
para crear dos procesos con patrones de ejecución opuestos:

- **Proceso intensivo en CPU** (`cpuIntensiveFunc`): ejecuta un bucle aritmético 
  hasta `100,000,000,000` iteraciones de forma continua, consumiendo su quantum 
  repetidamente. El scheduler lo detecta como intensivo y degrada su prioridad.

- **Proceso interactivo de E/S** (`IOFunc`): imprime por pantalla de forma continua 
  pero con llamadas al sistema (`printf`), cediendo CPU frecuentemente. El scheduler 
  lo detecta como interactivo y mantiene o mejora su prioridad.

El objetivo es verificar que el proceso de E/S recibe mayor tiempo de CPU relativo 
gracias a la política de ventanas, reflejando el comportamiento esperado de un 
scheduler que favorece la interactividad.