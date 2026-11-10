# Sistema de Investigación Forense y Juzgado Penal (Entrega 2)

**Integrantes:**
- Sebastian Chisaba Cruz
- Ariadna Valentina Ramirez Santeliz
- Jeronimo Pascuas Barroso

## Novedades de la Entrega 2
1. **Soporte Multiplataforma (Linux y Windows):** El servidor HTTP en C++ (`servidor.cpp`) detecta automáticamente el sistema operativo mediante directivas `#ifdef _WIN32` para usar `Winsock2` en Windows o sockets POSIX en Linux.
2. **Eliminación del Temporizador de 60s y Nuevo Creador de Casos:** Se eliminó la generación automática de casos cada 60 segundos y se incorporó el menú **"➕ Ingresar Nuevo Caso"** para introducir expedientes personalizados, contextos de los 5 individuos, pistas para la **pila** y argumentos del juicio.
3. **Ciclo Jugable Noche (Investigador) y Día (Juez):**
   - **Noche (Boceto 1):** Evaluación de los 5 individuos con su contexto en la escena, marco de foto superior derecho, recolección de pistas en la **pila** y selección de **2 sospechosos** y **3 testigos**. Si el jugador pasa al Día sin haber incluido al verdadero culpable entre los 2 sospechosos, el sistema le notifica el fallo en la transición y reinicia la investigación nocturna.
   - **Día (Boceto 2):** Estrado con el Sospechoso A + Abogado A, los 3 Testigos, y el Sospechoso B + Abogado B (con animación de dar un paso al frente al presentar sus pruebas). Mesa con las 8 cartas probatorias (`3, 2, 2, 1 | 1, 2, 2, 3`) organizadas en un **Árbol**, turnos controlados por una **Cola Circular** (`1` Testigos -> `2` Abogados -> `3` Acusaciones) y ranking automático en tiempo real mediante un **Montículo Binario** (Opción B).

---

## Cómo Compilar y Ejecutar

### En Linux:
```bash
g++ servidor.cpp -o servidor
./servidor
```

### En Windows (MinGW / g++):
```cmd
g++ servidor.cpp -o servidor.exe -lws2_32
servidor.exe
```

Una vez iniciado el servidor en la terminal, abre el archivo `index.html` en tu navegador web.
