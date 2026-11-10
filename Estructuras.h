/**
 * @file Estructuras.h
 * @authors Sebastian Chisaba Cruz
 *          Ariadna Valentina Ramirez Santeliz
 *          Jeronimo Pascuas Barroso
 * @brief Cabecera con la implementacion de las estructuras de datos del
 * Sistema de Investigacion Forense y Juzgado Penal (Ciclo Noche / Dia).
 * @details Incluye Registros (struct), Cola con Prioridad, Pila, Cola Circular,
 * Arbol Jerarquico para el Juzgado y Monticulo Binario para el ranking de pruebas.
 */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// ============================================================================
// 1. REGISTROS (STRUCT) PARA INDIVIDUOS, DETECTIVES Y ELEMENTOS PROBATORIOS
// ============================================================================

/**
 * @struct Individuo
 * @brief Modela a cada una de las 5 personas presentes en la escena del crimen.
 */
struct Individuo {
  string nombre;
  string contextoEscena; // Contexto del individuo en la noche
  string rasgos[4];      // 4 caracteristicas del perfil
  string coartada;       // Coartada asignada por la cola circular
  bool esCulpable;       // Indica si es el verdadero autor del crimen
  int rolNoche;          // 0: Testigo (amarillo), 1: Sospechoso elegido (naranja)
  int spriteId;          // Identificador fijo de sprite (1 a 5)

  // Textos especificos para la fase de Dia (Juzgado) si pasa como sospechoso
  string testimonioTestigos; // Carta 1 (Amarilla)
  int pesoTestigos;
  string evidenciaAbogado1;  // Carta 2A (Azul/Rosada)
  int pesoAbogado1;
  string evidenciaAbogado2;  // Carta 2B (Azul/Rosada)
  int pesoAbogado2;
  string acusacionCruzada;   // Carta 3 (Naranja)
  int pesoAcusacion;
};

/**
 * @struct Detective
 * @brief Estado operativo de cada detective de la central.
 */
struct Detective {
  string nombre;
  bool disponible;
  int casosResueltos;
};

/**
 * @brief Catalogo base de 15 individuos para casos predefinidos.
 */
Individuo CatalogoIndividuos[15] = {
    {"Carlos Mendoza",
     "Visto cerca de la caja fuerte con las manos manchadas de tinta.",
     {"tiene cabello castano", "usa un llavero de Petro", "usa lentes",
      "no utiliza redes sociales"},
     "", false, 0, 1, "", 0, "", 0, "", 0, "", 0},
    {"Elena Suarez",
     "Encontrada en el pasillo trasero guardando un frasco pequeno.",
     {"es otaku", "bebe alcohol", "usa un llavero de Petro",
      "no utiliza redes sociales"},
     "", false, 0, 2, "", 0, "", 0, "", 0, "", 0},
    {"Roberto Silva",
     "Discutia acaloradamente con la victima diez minutos antes del suceso.",
     {"usa un llavero de Petro", "tiene ojos azules", "tiene un perro",
      "va al gimnasio"},
     "", false, 0, 3, "", 0, "", 0, "", 0, "", 0},
    {"Sofia Gomez",
     "Salia por la puerta de emergencia con su bolso entreabierto.",
     {"usa bicicleta", "bebe alcohol", "tiene ojos azules",
      "tiene cabello castano"},
     "", false, 0, 4, "", 0, "", 0, "", 0, "", 0},
    {"Diego Alvarez",
     "Estaba junto al interruptor electrico cuando se corto la luz.",
     {"bebe alcohol", "usa lentes", "usa un llavero de Petro",
      "es de baja estatura"},
     "", false, 0, 5, "", 0, "", 0, "", 0, "", 0},
    {"Valeria Rios",
     "Revisaba documentos confidenciales en el escritorio principal.",
     {"usa un llavero de Petro", "tiene cabello castano", "tiene ojos azules",
      "frecuenta la biblioteca"},
     "", false, 0, 1, "", 0, "", 0, "", 0, "", 0},
    {"Gabriel Torres",
     "Tenia barro fresco en los zapatos igual al de la ventana rota.",
     {"es de baja estatura", "bebe alcohol", "no utiliza redes sociales",
      "tiene ojos azules"},
     "", false, 0, 2, "", 0, "", 0, "", 0, "", 0},
    {"Isabella Vega",
     "Intentaba borrar las grabaciones de la camara de seguridad.",
     {"no utiliza redes sociales", "va al gimnasio", "es de baja estatura",
      "es otaku"},
     "", false, 0, 3, "", 0, "", 0, "", 0, "", 0},
    {"Lucas Morales",
     "Escondia un recibo bancario arrugado en el bolsillo del abrigo.",
     {"va al gimnasio", "bebe alcohol", "tiene un perro", "es otaku"},
     "", false, 0, 4, "", 0, "", 0, "", 0, "", 0},
    {"Camila Ramirez",
     "Fue la primera persona en dar aviso a la patrulla nocturna.",
     {"tiene cabello castano", "tiene un perro", "tiene ojos azules",
      "es otaku"},
     "", false, 0, 5, "", 0, "", 0, "", 0, "", 0},
    {"Mateo Benitez",
     "Permanecia nervioso junto al vehiculo estacionado en el callejon.",
     {"va al gimnasio", "usa lentes", "usa bicicleta", "tiene un perro"},
     "", false, 0, 1, "", 0, "", 0, "", 0, "", 0},
    {"Maria Santeliz",
     "Sostenia una llave maestra que abre las oficinas del segundo piso.",
     {"es de baja estatura", "va al gimnasio", "frecuenta la biblioteca",
      "no utiliza redes sociales"},
     "", false, 0, 2, "", 0, "", 0, "", 0, "", 0},
    {"Tomas Castro",
     "Presentaba un rasguno reciente en el antebrazo derecho.",
     {"tiene un perro", "es de baja estatura", "usa un llavero de Petro",
      "frecuenta la biblioteca"},
     "", false, 0, 3, "", 0, "", 0, "", 0, "", 0},
    {"Mariana Paredes",
     "Aseguraba buscar sus lentes cerca del lugar donde hallaron el arma.",
     {"usa bicicleta", "usa lentes", "frecuenta la biblioteca",
      "va al gimnasio"},
     "", false, 0, 4, "", 0, "", 0, "", 0, "", 0},
    {"Javier Blanco",
     "Tenia en su poder el telefono movil de uno de los asistentes.",
     {"frecuenta la biblioteca", "usa lentes", "es otaku",
      "tiene cabello castano"},
     "", false, 0, 5, "", 0, "", 0, "", 0, "", 0}
};

// ============================================================================
// 2. COLA CIRCULAR (COARTADAS Y TURNOS DEL JUZGADO)
// ============================================================================

/**
 * @class ColaCircularCoartadas
 * @brief Cola circular de 10 posiciones para rotar y reciclar coartadas.
 */
class ColaCircularCoartadas {
public:
  string elementos[10];
  int cabeza;
  int ultimo;
  int tamano;

  ColaCircularCoartadas() {
    cabeza = 0;
    ultimo = -1;
    tamano = 0;

    encolar("Asegura que estaba cenando solo en Burger King.");
    encolar("Afirma que se encontraba viendo una batalla de aura.");
    encolar("Declara que estuvo paseando a su pez.");
    encolar("Sostiene que estaba en su casa durmiendo.");
    encolar("Manifiesta que asistia a una conferencia de negocios.");
    encolar("Afirma que estaba en casa de su madre.");
    encolar("Asegura que se encontraba entrenando en el gimnasio.");
    encolar("Sostiene que no se encontraba en la ciudad.");
    encolar("Se encontraba en su casa preparando la cena.");
    encolar("Estaba en la biblioteca estudiando.");
  }

  void encolar(string coartada) {
    if (tamano == 10)
      return;
    ultimo = (ultimo + 1) % 10;
    elementos[ultimo] = coartada;
    tamano++;
  }

  string desencolarYRotar() {
    if (tamano == 0)
      return "Sin coartada.";
    string dato = elementos[cabeza];
    cabeza = (cabeza + 1) % 10;
    ultimo = (ultimo + 1) % 10;
    elementos[ultimo] = dato;
    return dato;
  }
};

ColaCircularCoartadas CoartadasCirculares;

/**
 * @class ColaCircularTurnos
 * @brief Cola circular de 3 posiciones que administra el orden de revelacion
 * en el juzgado: 1 (Testigos) -> 2 (Evidencias de Abogados) -> 3 (Acusaciones).
 */
class ColaCircularTurnos {
public:
  int etapas[3];
  string nombresEtapa[3];
  int cabeza;

  ColaCircularTurnos() {
    etapas[0] = 1;
    nombresEtapa[0] = "Etapa 1: Contexto de los Testigos hacia cada Acusado";
    etapas[1] = 2;
    nombresEtapa[1] = "Etapa 2: Evidencias Contundentes por cada Abogado";
    etapas[2] = 3;
    nombresEtapa[2] = "Etapa 3: Acusacion Cruzada entre Sospechosos";
    cabeza = 0;
  }

  void reiniciar() { cabeza = 0; }

  int etapaActual() { return etapas[cabeza]; }

  string descripcionActual() { return nombresEtapa[cabeza]; }

  void avanzarTurnoCircular() { cabeza = (cabeza + 1) % 3; }
};

// ============================================================================
// 3. PILA PARA RECOLECCION DE EVIDENCIAS FORENSES (NOCHE)
// ============================================================================

struct NodoPila {
  string tipoForma; // "rectangulo", "rombo", "cuadrado", "reporte"
  string tituloPista;
  string descripcionEvidencia;
  NodoPila *sig;

  NodoPila(string _forma, string _titulo, string _desc)
      : tipoForma(_forma), tituloPista(_titulo), descripcionEvidencia(_desc),
        sig(NULL) {}
};

/**
 * @class PilaEvidencias
 * @brief Pila dinamica encapsulada en cada caso donde la ultima evidencia
 * recolectada permanece en el tope.
 */
class PilaEvidencias {
public:
  NodoPila *cabeza;

  PilaEvidencias() { cabeza = NULL; }

  ~PilaEvidencias() {
    while (cabeza != NULL) {
      desapilar();
    }
  }

  void apilar(string forma, string titulo, string evidencia) {
    NodoPila *nuevo = new NodoPila(forma, titulo, evidencia);
    nuevo->sig = cabeza;
    cabeza = nuevo;
  }

  void desapilar() {
    if (cabeza != NULL) {
      NodoPila *temp = cabeza;
      cabeza = cabeza->sig;
      delete temp;
    }
  }
};

// ============================================================================
// 4. ARBOL JERARQUICO DEL JUZGADO (DIA)
// ============================================================================

/**
 * @struct NodoArbolJuicio
 * @brief Nodo del arbol que organiza el Caso en Juicio (Raiz), los 3 Grupos
 * del Juzgado (Nivel 1) y las 8 Cartas Probatorias en la mesa (Nivel 2).
 */
struct NodoArbolJuicio {
  int idCarta;        // -1 para raiz/grupos, 0..7 para las 8 cartas de la mesa
  int numeroEtapa;    // 1: Testigos, 2: Abogado, 3: Acusacion sospechoso
  string grupo;       // "SOSPECHOSO_A", "TESTIGOS", "SOSPECHOSO_B"
  string autor;       // Nombre de quien presenta el elemento
  string tituloCarta; // Etiqueta visible en la carta
  string detalle;     // Texto completo que aparece al hacer clic en la prueba
  int pesoIncriminatorio; // Valor de peso para el Monticulo Binario (1..100)
  string acusadoObjetivo; // A cual de los 2 sospechosos apunta o pertenece
  bool revelada;          // Indica si el Juez ya le dio clic en la mesa

  // Punteros jerarquicos del Arbol (hasta 4 hijos por nodo para las 4 pruebas)
  NodoArbolJuicio *hijos[4];
  int numHijos;

  NodoArbolJuicio(int _id, int _etapa, string _grupo, string _autor,
                  string _titulo, string _detalle, int _peso, string _objetivo) {
    idCarta = _id;
    numeroEtapa = _etapa;
    grupo = _grupo;
    autor = _autor;
    tituloCarta = _titulo;
    detalle = _detalle;
    pesoIncriminatorio = _peso;
    acusadoObjetivo = _objetivo;
    revelada = false;
    numHijos = 0;
    for (int i = 0; i < 4; i++) {
      hijos[i] = NULL;
    }
  }

  void agregarHijo(NodoArbolJuicio *hijo) {
    if (numHijos < 4) {
      hijos[numHijos++] = hijo;
    }
  }
};

/**
 * @class ArbolJuicio
 * @brief Arbol jerarquico que modela el expediente de audiencia durante el Dia.
 */
class ArbolJuicio {
public:
  NodoArbolJuicio *raiz;

  ArbolJuicio() { raiz = NULL; }

  ~ArbolJuicio() { limpiarSubarbol(raiz); }

  void limpiarSubarbol(NodoArbolJuicio *nodo) {
    if (nodo == NULL)
      return;
    for (int i = 0; i < nodo->numHijos; i++) {
      limpiarSubarbol(nodo->hijos[i]);
    }
    delete nodo;
  }

  void reiniciar() {
    limpiarSubarbol(raiz);
    raiz = NULL;
  }

  /**
   * @brief Construye el arbol del juicio con los 2 sospechosos y 3 testigos.
   */
  void construirArbol(string nombreCaso, const Individuo &sospA,
                      const Individuo &sospB, string nombresTestigos) {
    reiniciar();
    raiz = new NodoArbolJuicio(-1, 0, "RAIZ", "Juzgado Penal", nombreCaso,
                               "Expediente en audiencia oral", 0, "");

    // Nivel 1: Los 3 grupos principales del juzgado
    NodoArbolJuicio *grupoA = new NodoArbolJuicio(
        -1, 0, "GRUPO_A", "Defensa A", "Sospechoso A y su Abogado",
        "Estrado izquierdo: " + sospA.nombre, 0, sospA.nombre);
    NodoArbolJuicio *grupoTestigos = new NodoArbolJuicio(
        -1, 0, "GRUPO_TESTIGOS", "Panel de 3 Testigos", "3 Testigos Presenciales",
        "Testigos: " + nombresTestigos, 0, "Ambos");
    NodoArbolJuicio *grupoB = new NodoArbolJuicio(
        -1, 0, "GRUPO_B", "Defensa B", "Sospechoso B y su Abogado",
        "Estrado derecho: " + sospB.nombre, 0, sospB.nombre);

    raiz->agregarHijo(grupoA);
    raiz->agregarHijo(grupoTestigos);
    raiz->agregarHijo(grupoB);

    // Nivel 2: Las 8 cartas en el orden visual de la mesa (boceto 2)
    // --- LADO IZQUIERDO: SOSPECHOSO A (Cartas 0, 1, 2, 3) ---
    // Carta 0 (Tipo 3 - Naranja): Acusacion de Sospechoso A al Sospechoso B
    grupoA->agregarHijo(new NodoArbolJuicio(
        0, 3, "SOSPECHOSO_A", sospA.nombre,
        "Acusacion de " + sospA.nombre + " al otro sospechoso",
        sospA.acusacionCruzada, sospA.pesoAcusacion, sospB.nombre));

    // Carta 1 (Tipo 2 - Azul): Evidencia contundente 1 del Abogado A
    grupoA->agregarHijo(new NodoArbolJuicio(
        1, 2, "ABOGADO_A", "Abogado de " + sospA.nombre,
        "Evidencia contundente #1 por Abogado de " + sospA.nombre,
        sospA.evidenciaAbogado1, sospA.pesoAbogado1, sospA.nombre));

    // Carta 2 (Tipo 2 - Azul): Evidencia contundente 2 del Abogado A
    grupoA->agregarHijo(new NodoArbolJuicio(
        2, 2, "ABOGADO_A", "Abogado de " + sospA.nombre,
        "Evidencia contundente #2 por Abogado de " + sospA.nombre,
        sospA.evidenciaAbogado2, sospA.pesoAbogado2, sospA.nombre));

    // Carta 3 (Tipo 1 - Amarilla): Contexto de los testigos hacia Sospechoso A
    grupoTestigos->agregarHijo(new NodoArbolJuicio(
        3, 1, "TESTIGOS_A", "Testigos (" + nombresTestigos + ")",
        "Contexto de los testigos hacia " + sospA.nombre,
        sospA.testimonioTestigos, sospA.pesoTestigos, sospA.nombre));

    // --- LADO DERECHO: SOSPECHOSO B (Cartas 4, 5, 6, 7) ---
    // Carta 4 (Tipo 1 - Amarilla): Contexto de los testigos hacia Sospechoso B
    grupoTestigos->agregarHijo(new NodoArbolJuicio(
        4, 1, "TESTIGOS_B", "Testigos (" + nombresTestigos + ")",
        "Contexto de los testigos hacia " + sospB.nombre,
        sospB.testimonioTestigos, sospB.pesoTestigos, sospB.nombre));

    // Carta 5 (Tipo 2 - Rosada): Evidencia contundente 1 del Abogado B
    grupoB->agregarHijo(new NodoArbolJuicio(
        5, 2, "ABOGADO_B", "Abogado de " + sospB.nombre,
        "Evidencia contundente #1 por Abogado de " + sospB.nombre,
        sospB.evidenciaAbogado1, sospB.pesoAbogado1, sospB.nombre));

    // Carta 6 (Tipo 2 - Rosada): Evidencia contundente 2 del Abogado B
    grupoB->agregarHijo(new NodoArbolJuicio(
        6, 2, "ABOGADO_B", "Abogado de " + sospB.nombre,
        "Evidencia contundente #2 por Abogado de " + sospB.nombre,
        sospB.evidenciaAbogado2, sospB.pesoAbogado2, sospB.nombre));

    // Carta 7 (Tipo 3 - Naranja): Acusacion de Sospechoso B al Sospechoso A
    grupoB->agregarHijo(new NodoArbolJuicio(
        7, 3, "SOSPECHOSO_B", sospB.nombre,
        "Acusacion de " + sospB.nombre + " al otro sospechoso",
        sospB.acusacionCruzada, sospB.pesoAcusacion, sospA.nombre));
  }

  /**
   * @brief Recorre el arbol buscando una carta probatoria por su identificador.
   */
  NodoArbolJuicio *buscarCarta(NodoArbolJuicio *nodo, int idBuscado) {
    if (nodo == NULL)
      return NULL;
    if (nodo->idCarta == idBuscado)
      return nodo;
    for (int i = 0; i < nodo->numHijos; i++) {
      NodoArbolJuicio *encontrado = buscarCarta(nodo->hijos[i], idBuscado);
      if (encontrado != NULL)
        return encontrado;
    }
    return NULL;
  }
};

// ============================================================================
// 5. MONTICULO BINARIO PARA RANKING DE PRUEBAS REVELADAS (OPCION B)
// ============================================================================

struct ElementoMonticulo {
  int idCarta;
  int numeroEtapa;
  string autor;
  string titulo;
  string detalle;
  string acusadoObjetivo;
  int peso; // Clave de ordenamiento en el Monticulo Binario
};

/**
 * @class MonticuloBinario
 * @brief Monticulo Binario de Maximos que recibe cada prueba inspeccionada por
 * el Juez y las mantiene jerarquizadas desde la mas incriminatoria/grave hasta
 * la mas debil.
 */
class MonticuloBinario {
public:
  ElementoMonticulo arreglo[16];
  int tamano;

  MonticuloBinario() { tamano = 0; }

  void vaciar() { tamano = 0; }

  bool yaExiste(int idCarta) {
    for (int i = 0; i < tamano; i++) {
      if (arreglo[i].idCarta == idCarta)
        return true;
    }
    return false;
  }

  void intercambiar(int i, int j) {
    ElementoMonticulo temp = arreglo[i];
    arreglo[i] = arreglo[j];
    arreglo[j] = temp;
  }

  void subir(int idx) {
    while (idx > 0) {
      int padre = (idx - 1) / 2;
      if (arreglo[idx].peso > arreglo[padre].peso) {
        intercambiar(idx, padre);
        idx = padre;
      } else {
        break;
      }
    }
  }

  void bajar(int idx, int n) {
    while (true) {
      int mayor = idx;
      int izq = 2 * idx + 1;
      int der = 2 * idx + 2;

      if (izq < n && arreglo[izq].peso > arreglo[mayor].peso)
        mayor = izq;
      if (der < n && arreglo[der].peso > arreglo[mayor].peso)
        mayor = der;

      if (mayor != idx) {
        intercambiar(idx, mayor);
        idx = mayor;
      } else {
        break;
      }
    }
  }

  void insertar(NodoArbolJuicio *carta) {
    if (carta == NULL || tamano >= 16 || yaExiste(carta->idCarta))
      return;
    ElementoMonticulo nuevo;
    nuevo.idCarta = carta->idCarta;
    nuevo.numeroEtapa = carta->numeroEtapa;
    nuevo.autor = carta->autor;
    nuevo.titulo = carta->tituloCarta;
    nuevo.detalle = carta->detalle;
    nuevo.acusadoObjetivo = carta->acusadoObjetivo;
    nuevo.peso = carta->pesoIncriminatorio;

    arreglo[tamano] = nuevo;
    subir(tamano);
    tamano++;
  }

  /**
   * @brief Devuelve en un arreglo auxiliar los elementos extraidos del
   * Monticulo Binario de mayor a menor peso sin perder el estado del monticulo.
   */
  void obtenerRankingOrdenado(ElementoMonticulo salida[], int &cantidad) {
    cantidad = tamano;
    MonticuloBinario copia = *this;
    for (int i = 0; i < cantidad; i++) {
      salida[i] = copia.arreglo[0];
      copia.arreglo[0] = copia.arreglo[copia.tamano - 1];
      copia.tamano--;
      copia.bajar(0, copia.tamano);
    }
  }
};

// ============================================================================
// 6. COLA CON PRIORIDAD PARA GESTION DE CASOS (NOCHE -> DIA)
// ============================================================================

class NodoCaso {
public:
  int idCaso;
  string descripcion;
  int prioridad; // 1: Alta, 2: Media, 3: Baja
  Individuo individuos[5];
  int indiceCulpable;
  int individuoInspeccionado; // Indice (0..4) cuya foto se ve arriba a la derecha
  string pistasPersonalizadas[4];
  string titulosPistas[4];
  string formasPistas[4];
  int evidenciasMostradas;
  int idDetectiveAsignado;

  // Estado del ciclo Noche / Dia
  string faseActual; // "NOCHE" o "DIA"
  int idxSospechosoA;
  int idxSospechosoB;
  string actorAnimado; // Controla que sprite da un paso al frente en la interfaz
  string textoModalPrueba;
  string tituloModalPrueba;

  // Estructuras propias encapsuladas en el caso
  PilaEvidencias historialEvidencias;
  ArbolJuicio arbolJuzgado;
  MonticuloBinario monticuloPruebas;
  ColaCircularTurnos turnosJuzgado;

  NodoCaso *sig;

  /**
   * @brief Constructor para casos generados a partir del catalogo base.
   */
  NodoCaso(int _id, string _desc, int _prio) {
    idCaso = _id;
    descripcion = _desc;
    prioridad = _prio;
    evidenciasMostradas = 0;
    idDetectiveAsignado = -1;
    individuoInspeccionado = 0;
    faseActual = "NOCHE";
    idxSospechosoA = -1;
    idxSospechosoB = -1;
    actorAnimado = "NINGUNO";
    tituloModalPrueba = "";
    textoModalPrueba = "";
    sig = NULL;

    formasPistas[0] = "rectangulo";
    titulosPistas[0] = "Fotografia Forense";
    formasPistas[1] = "rombo";
    titulosPistas[1] = "Objeto en la Escena";
    formasPistas[2] = "cuadrado";
    titulosPistas[2] = "Reporte de Laboratorio";
    formasPistas[3] = "reporte";
    titulosPistas[3] = "Registro de Testigos";

    indiceCulpable = rand() % 5;
    bool yaElegido[15] = {false};

    for (int i = 0; i < 5; i++) {
      int r;
      do {
        r = rand() % 15;
      } while (yaElegido[r]);
      yaElegido[r] = true;

      individuos[i] = CatalogoIndividuos[r];
      individuos[i].coartada = CoartadasCirculares.desencolarYRotar();
      individuos[i].esCulpable = (i == indiceCulpable);
      individuos[i].rolNoche = 0;
      individuos[i].spriteId = i + 1;
    }

    for (int k = 0; k < 4; k++) {
      pistasPersonalizadas[k] =
          "El analisis forense confirma que el culpable " +
          individuos[indiceCulpable].rasgos[k] + ".";
    }

    configurarArgumentosJuzgadoPorDefecto();
  }

  /**
   * @brief Genera automaticamente los textos de las cartas 1, 2 y 3 del juzgado
   * diferenciando al verdadero culpable del sospechoso inocente.
   */
  void configurarArgumentosJuzgadoPorDefecto() {
    for (int i = 0; i < 5; i++) {
      if (individuos[i].esCulpable) {
        individuos[i].testimonioTestigos =
            "Los 3 testigos coinciden en que " + individuos[i].nombre +
            " merodeaba el punto exacto del crimen (" +
            individuos[i].contextoEscena +
            ") y contradice la hora de salida.";
        individuos[i].pesoTestigos = 92;

        individuos[i].evidenciaAbogado1 =
            "El abogado presenta un supuesto ticket de compra para respaldar que '" +
            individuos[i].coartada +
            "', pero la tinta esta borrosa y carece de sello oficial (Coartada dudosa).";
        individuos[i].pesoAbogado1 = 35;

        individuos[i].evidenciaAbogado2 =
            "El abogado alega que su cliente solo pasaba por el lugar, pero el peritaje hallo que el autor " +
            individuos[i].rasgos[0] + " y " + individuos[i].rasgos[1] +
            " (Contradiccion grave en la defensa).";
        individuos[i].pesoAbogado2 = 88;

        individuos[i].acusacionCruzada =
            individuos[i].nombre +
            " levanta la voz nerviosamente y acusa al otro sospechoso sin aportar pruebas fisicas verificables.";
        individuos[i].pesoAcusacion = 42;
      } else {
        individuos[i].testimonioTestigos =
            "Los 3 testigos declaran que aunque " + individuos[i].nombre +
            " estaba en el recinto (" + individuos[i].contextoEscena +
            "), permanecio a la vista del grupo cuando ocurrio el hecho.";
        individuos[i].pesoTestigos = 50;

        individuos[i].evidenciaAbogado1 =
            "El abogado exhibe un registro verificado en camaras externas que respalda su declaracion: '" +
            individuos[i].coartada + "' (Prueba de descargo solida).";
        individuos[i].pesoAbogado1 = 25;

        individuos[i].evidenciaAbogado2 =
            "El abogado demuestra que las huellas de " + individuos[i].nombre +
            " no coinciden con el objeto principal recolectado en la pila de evidencias.";
        individuos[i].pesoAbogado2 = 20;

        individuos[i].acusacionCruzada =
            individuos[i].nombre +
            " senala directamente al otro acusado indicando que lo vio ocultando evidencia clave segundos despues del incidente.";
        individuos[i].pesoAcusacion = 84;
      }
    }
  }

  /**
   * @brief Apila una nueva pista forense en la pila del caso.
   */
  bool solicitarNuevaEvidencia() {
    if (evidenciasMostradas >= 4)
      return false;
    int idx = evidenciasMostradas;
    historialEvidencias.apilar(formasPistas[idx], titulosPistas[idx],
                               pistasPersonalizadas[idx]);
    evidenciasMostradas++;
    return true;
  }

  /**
   * @brief Reinicia la seleccion nocturna cuando el jugador se equivoca al
   * pasar al Dia sin incluir al verdadero culpable entre los 2 sospechosos.
   */
  void reiniciarInvestigacionNocturna() {
    faseActual = "NOCHE";
    idxSospechosoA = -1;
    idxSospechosoB = -1;
    for (int i = 0; i < 5; i++) {
      individuos[i].rolNoche = 0;
    }
  }
};

/**
 * @class ColaPrioridadCasos
 * @brief Cola con prioridad implementada sobre lista enlazada ordenada.
 */
class ColaPrioridadCasos {
public:
  NodoCaso *cabeza;

  ColaPrioridadCasos() { cabeza = NULL; }

  ~ColaPrioridadCasos() {
    while (cabeza != NULL) {
      eliminarPrimero();
    }
  }

  void insertarNodoConPrioridad(NodoCaso *nuevo) {
    if (cabeza == NULL || nuevo->prioridad < cabeza->prioridad) {
      nuevo->sig = cabeza;
      cabeza = nuevo;
    } else {
      NodoCaso *actual = cabeza;
      while (actual->sig != NULL && actual->sig->prioridad <= nuevo->prioridad) {
        actual = actual->sig;
      }
      nuevo->sig = actual->sig;
      actual->sig = nuevo;
    }
  }

  void insertarConPrioridad(int id, string desc, int prio) {
    NodoCaso *nuevo = new NodoCaso(id, desc, prio);
    insertarNodoConPrioridad(nuevo);
  }

  void eliminarPrimero() {
    if (cabeza != NULL) {
      NodoCaso *aux = cabeza;
      cabeza = cabeza->sig;
      delete aux; // Destruye en cascada PilaEvidencias y ArbolJuicio en el Heap
    }
  }
};

#endif
