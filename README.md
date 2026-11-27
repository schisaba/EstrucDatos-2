# Investigación Forense y Juzgado Penal — Entrega 2

![C++](https://img.shields.io/badge/C%2B%2B-14-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-Vanilla-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
![Platform](https://img.shields.io/badge/SO-Linux%20%7C%20Windows-2ea44f?style=for-the-badge)

Juego interactivo de deducción forense y simulación judicial desarrollado en **C++** (motor lógico y servidor HTTP por sockets) con interfaz web ligera, creado para la asignatura **Estructuras de Datos** de la Universidad Nacional de Colombia.

> [!NOTE]
> Toda la lógica algorítmica y la gestión manual de memoria se ejecutan en el servidor de C++ (`Estructuras.h` y `servidor.cpp`). La interfaz web (`index.html`, `styles.css`, `app.js`) actúa únicamente como capa visual compacta (`100vh` sin desplazamiento vertical), manteniendo las estructuras de datos invisibles para el jugador.

---

## Tabla de Contenidos

1. [Descripción General](#descripción-general)
2. [Estructuras de Datos y Complejidad](#estructuras-de-datos-y-complejidad)
3. [Estructura del Proyecto](#estructura-del-proyecto)
4. [Compilación y Ejecución](#compilación-y-ejecución)
5. [Flujo de Juego (Noche y Día)](#flujo-de-juego-noche-y-día)
6. [Endpoints del Servidor Local](#endpoints-del-servidor-local)
7. [Documentación Adicional](#documentación-adicional)
8. [Autores](#autores)

---

## Descripción General

Cada partida se desarrolla en un ciclo continuo de dos actos:

* **Fase Nocturna (Rol de Investigador):** El jugador asigna un detective, inspecciona retratos, contextos y coartadas de cinco implicados, recolecta hasta **2 evidencias forenses** y selecciona exactamente **2 sospechosos** para llevar a juicio (los 3 restantes pasan a ser testigos). Si el culpable real queda libre entre los testigos, la investigación nocturna se reinicia.
* **Fase Diurna (Rol de Juez):** En el estrado del tribunal se enfrentan los dos acusados con sus abogados defensores y el panel de tres testigos. El juez abre las **8 cartas probatorias** respetando tres fases procesales obligatorias, compara su peso incriminatorio en el panel lateral de relevancia y dicta sentencia. Al cerrar el último expediente, el servidor genera nuevos casos automáticamente para mantener la partida activa.

---

## Estructuras de Datos y Complejidad

Todas las estructuras fueron programadas desde cero en `Estructuras.h` con gestión manual de memoria dinámica y liberación en cascada mediante destructores.

| Estructura | Clase / Registro | Entrega | Rol en el sistema | Complejidad Temporal | Complejidad Espacial |
| :--- | :--- | :---: | :--- | :--- | :--- |
| **Registros y arreglos fijos** | `Detective[4]`, `Individuo[15]` | 1 | Plantilla de detectives y catálogo de implicados | Acceso `O(1)` | `O(1)` |
| **Cola con Prioridad (Min-Heap dinámico)** | `ColaPrioridadCasos` | 1 *(opt. 2)* | Ordena expedientes por urgencia (`1` a `3`) con desempate FIFO por `idCaso` | Consulta `O(1)` / Inserción `O(log n)`* / Extracción `O(log n)` | `O(n)` |
| **Pila dinámica (LIFO)** | `PilaEvidencias` | 1 | Almacena las pistas forenses recolectadas durante la noche | Apilar / Desapilar `O(1)` | `O(e)`, con `e <= 2` |
| **Cola Circular de Coartadas** | `ColaCircularCoartadas` | 1 | Rota y reutiliza las coartadas entre los implicados de cada caso | Desencolar y rotar `O(1)` | `O(1)` (cap. 10) |
| **Árbol Jerárquico de Búsqueda por Rangos** | `ArbolJuicio` | **2** | Modela el tribunal (`raíz -> 3 grupos -> 8 cartas`) manteniendo intervalos ordenados `[idMin, idMax]` | Búsqueda `O(log V)` / Construcción y liberación `O(V)` | `O(V)`, con `V = 12` (`O(1)` aux. en búsqueda) |
| **Cola Circular de Turnos** | `ColaCircularTurnos` | **2** | Controla el orden procesal en 3 fases: Testigos -> Abogados -> Acusaciones | Consulta / Avance `O(1)` | `O(1)` (cap. 3) |
| **Montículo Binario de Máximos** | `MonticuloBinario` | **2** | Ordena en vivo las pruebas abiertas según su puntaje incriminatorio | Inserción `O(log k)` / Ranking `O(k log k)` | `O(k)`, con `k <= 8` |

> [!TIP]
> *Variables de análisis:* `n` = casos en cola, `e` = evidencias apiladas en la noche (máx. 2), `V` = nodos del árbol del tribunal (12 fijos) y `k` = pruebas reveladas en el juzgado (máx. 8).
> *La inserción en `ColaPrioridadCasos` es `O(log n)` amortizado al duplicar la capacidad del arreglo dinámico cuando se llena. En `ArbolJuicio`, la búsqueda dirigida aplica búsqueda binaria en cada nivel sin explorar ramas hermanas (`O(h * log b) = O(log V)`).

---

## Estructura del Proyecto

```text
.
├── Estructuras.h                     # Implementación de las estructuras de datos y lógica de casos
├── servidor.cpp                      # Servidor HTTP multiplataforma (Winsock2 / POSIX) en el puerto 8081
├── index.html                        # Interfaz gráfica compacta (Noche y Día en pantalla completa)
├── styles.css                        # Diseño responsivo 100vh, animaciones del estrado y modales
├── app.js                            # Cliente Fetch API, renderizado dinámico y validaciones visuales
├── sprites/                          # Retratos ilustrados de personajes y abogados (.jpg / .png)
│   ├── personaje_1.jpg
│   ├── personaje_2.jpg
│   ├── personaje_3.jpg
│   ├── personaje_4.jpg
│   ├── personaje_5.jpg
│   ├── abogado_a.jpg
│   └── abogado_b.jpg
├── manual_de_usuario.pdf             # Guía paso a paso para instalar y jugar
├── explicacion_estructuras_datos.pdf # Informe técnico: por qué y cómo se usa cada estructura
└── README.md                         # Documentación principal del repositorio
```

---

## Compilación y Ejecución

### Requisitos
* Compilador **g++** con soporte para **C++14** (GCC en Linux o MinGW/MSYS2 en Windows).
* Navegador web moderno (Chrome, Firefox, Edge o Brave).
* Puerto local **`8081`** disponible.

### En Linux

```bash
g++ -std=c++14 servidor.cpp -o servidor
./servidor
```

### En Windows (PowerShell)

```powershell
g++ -std=c++14 servidor.cpp -o servidor.exe -lws2_32
.\servidor.exe
```

> [!IMPORTANT]
> **Nota para Windows 11:** Si PowerShell muestra el error *"Una directiva de Control de aplicaciones bloqueó este archivo"* al ejecutar `.\servidor.exe`, desactiva **Control inteligente de aplicaciones (Smart App Control)** en **Seguridad de Windows -> Control de aplicaciones y navegador -> Configuración de Control inteligente de aplicaciones**.

### Abrir el juego
1. Mantén abierta la terminal donde aparece `SERVIDOR DEL JUEGO ACTIVO EN: http://localhost:8081`.
2. Abre `index.html` directamente en tu navegador (se recomienda pantalla completa con `F11`).

---

## Flujo de Juego (Noche y Día)

1. **Asignar Detective:** Selecciona un detective disponible en la barra superior.
2. **Investigar la Escena (Noche):** Haz clic en cada uno de los 5 implicados para revisar su retrato, coartada y rasgos; recolecta hasta **2 pistas forenses** con el botón **Recolectar Evidencia**.
3. **Seleccionar Sospechosos:** Marca exactamente **2 sospechosos** y pulsa **Pasar al Día (Juzgado)**.
4. **Dirigir la Audiencia (Día):** Abre las cartas en orden estricto:
   * **Fase 1 (Amarillas):** Testimonios de los 3 testigos.
   * **Fase 2 (Azules y Rosadas):** Evidencias de los abogados defensores.
   * **Fase 3 (Naranjas):** Acusaciones cruzadas entre ambos sospechosos.
5. **Dictar Sentencia:** Analiza el panel derecho **Pruebas Revisadas por Relevancia** y condena al culpable (es obligatorio abrir al menos una carta antes de emitir veredicto).

---

## Endpoints del Servidor Local

El frontend se comunica con `servidor.cpp` en `http://localhost:8081` mediante respuestas JSON:

| Método y Ruta | Acción en el Motor C++ |
| :--- | :--- |
| `GET /` | Retorna el estado completo actual en formato JSON |
| `GET /asignar?id={0..3}` | Asigna un detective disponible al expediente activo |
| `GET /evidencia` | Apila una nueva pista forense en `PilaEvidencias` (máx. 2) |
| `GET /inspeccionar?idx={0..4}` | Muestra retrato, coartada y rasgos del individuo seleccionado |
| `GET /alternar_sospechoso?idx={0..4}` | Alterna el rol entre Testigo y Sospechoso (máx. 2 sospechosos) |
| `GET /pasar_dia` | Valida requisitos, construye `ArbolJuicio` e inicia la audiencia |
| `GET /clic_carta?id={0..7}` | Valida fase en `ColaCircularTurnos`, consulta carta en `O(log V)` e inserta en `MonticuloBinario` |
| `GET /veredicto?idx={0..4}` | Evalúa sentencia, extrae el caso en `O(log n)` liberando memoria en cascada y carga el siguiente |

---

## Documentación Adicional

* **Manual de Usuario:** Consulta `manual_de_usuario.pdf` para la guía visual de instalación, juego y solución de problemas.
* **Informe Técnico de Estructuras:** Consulta `explicacion_estructuras_datos.pdf` para el análisis detallado de diseño, complejidades y gestión de memoria dinámica en el Heap.

---

## Autores

* **Sebastian Chisaba Cruz**
* **Ariadna Valentina Ramirez Santeliz**
* **Jeronimo Pascuas Barroso**

*Universidad Nacional de Colombia — Estructuras de Datos (2026)*
