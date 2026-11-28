/**
 * @file Estructuras.h
 * @authors Sebastian Chisaba Cruz
 *          Ariadna Valentina Ramirez Santeliz
 *          Jeronimo Pascuas Barroso
 * @brief Cabecera con la implementacion de las estructuras de datos del
 * Sistema de Investigacion Forense y Juzgado Penal (Ciclo Noche / Dia).
 */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// ============================================================================
// 1. REGISTROS (STRUCT) PARA INDIVIDUOS Y DETECTIVES
// ============================================================================

struct Individuo {
  string nombre;
  string contextoEscena;
  string rasgos[4];
  string coartada;
  bool esCulpable;
  int rolNoche;  // 0: Testigo (amarillo), 1: Sospechoso (naranja)
  int spriteId;  // 1 a 5 (sprites/personaje_1.jpg ... personaje_5.jpg)
  string imagen; // Ruta opcional

  // Argumentos del juicio (Dia)
  string testimonioTestigos; // Carta 1 (Amarilla)
  int pesoTestigos;
  string evidenciaAbogado1; // Carta 2A (Azul/Rosada)
  int pesoAbogado1;
  string evidenciaAbogado2; // Carta 2B (Azul/Rosada)
  int pesoAbogado2;
  string acusacionCruzada; // Carta 3 (Naranja)
  int pesoAcusacion;
};

struct Detective {
  string nombre;
  bool disponible;
  int casosResueltos;
};

Individuo CatalogoIndividuos[15] = {
    {"Carlos Mendoza",
     "Visto cerca de la caja fuerte con las manos manchadas de tinta.",
     {"tiene cabello castano", "usa un llavero de Petro", "usa lentes",
      "no utiliza redes sociales"},
     "",
     false,
     0,
     1,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Elena Suarez",
     "Encontrada en el pasillo trasero guardando un frasco pequeno.",
     {"es otaku", "bebe alcohol", "usa un llavero de Petro",
      "no utiliza redes sociales"},
     "",
     false,
     0,
     2,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Roberto Silva",
     "Discutia acaloradamente con la victima diez minutos antes del suceso.",
     {"usa un llavero de Petro", "tiene ojos azules", "tiene un perro",
      "va al gimnasio"},
     "",
     false,
     0,
     3,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Sofia Gomez",
     "Salia por la puerta de emergencia con su bolso entreabierto.",
     {"usa bicicleta", "bebe alcohol", "tiene ojos azules",
      "tiene cabello castano"},
     "",
     false,
     0,
     4,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Diego Alvarez",
     "Estaba junto al interruptor electrico cuando se corto la luz.",
     {"bebe alcohol", "usa lentes", "usa un llavero de Petro",
      "es de baja estatura"},
     "",
     false,
     0,
     5,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Valeria Rios",
     "Revisaba documentos confidenciales en el escritorio principal.",
     {"usa un llavero de Petro", "tiene cabello castano", "tiene ojos azules",
      "frecuenta la biblioteca"},
     "",
     false,
     0,
     1,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Gabriel Torres",
     "Tenia barro fresco en los zapatos igual al de la ventana rota.",
     {"es de baja estatura", "bebe alcohol", "no utiliza redes sociales",
      "tiene ojos azules"},
     "",
     false,
     0,
     2,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Isabella Vega",
     "Intentaba borrar las grabaciones de la camara de seguridad.",
     {"no utiliza redes sociales", "va al gimnasio", "es de baja estatura",
      "es otaku"},
     "",
     false,
     0,
     3,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Lucas Morales",
     "Escondia un recibo bancario arrugado en el bolsillo del abrigo.",
     {"va al gimnasio", "bebe alcohol", "tiene un perro", "es otaku"},
     "",
     false,
     0,
     4,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Camila Ramirez",
     "Fue la primera persona en dar aviso a la patrulla nocturna.",
     {"tiene cabello castano", "tiene un perro", "tiene ojos azules",
      "es otaku"},
     "",
     false,
     0,
     5,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Mateo Benitez",
     "Permanecia nervioso junto al vehiculo estacionado en el callejon.",
     {"va al gimnasio", "usa lentes", "usa bicicleta", "tiene un perro"},
     "",
     false,
     0,
     1,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Maria Santeliz",
     "Sostenia una llave maestra que abre las oficinas del segundo piso.",
     {"es de baja estatura", "va al gimnasio", "frecuenta la biblioteca",
      "no utiliza redes sociales"},
     "",
     false,
     0,
     2,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Tomas Castro",
     "Presentaba un rasguno reciente en el antebrazo derecho.",
     {"tiene un perro", "es de baja estatura", "usa un llavero de Petro",
      "frecuenta la biblioteca"},
     "",
     false,
     0,
     3,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Mariana Paredes",
     "Aseguraba buscar sus lentes cerca del lugar donde hallaron el arma.",
     {"usa bicicleta", "usa lentes", "frecuenta la biblioteca",
      "va al gimnasio"},
     "",
     false,
     0,
     4,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0},
    {"Javier Blanco",
     "Tenia en su poder el telefono movil de uno de los asistentes.",
     {"frecuenta la biblioteca", "usa lentes", "es otaku",
      "tiene cabello castano"},
     "",
     false,
     0,
     5,
     "",
     "",
     0,
     "",
     0,
     "",
     0,
     "",
     0}};

// ============================================================================
// 2. COLA CIRCULAR (COARTADAS Y TURNOS DEL JUZGADO)
// ============================================================================

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

class ColaCircularTurnos {
public:
  int etapas[3];
  string nombresEtapa[3];
  int cabeza;

  ColaCircularTurnos() {
    etapas[0] = 1;
    nombresEtapa[0] = "Fase 1: Testimonios de los Testigos";
    etapas[1] = 2;
    nombresEtapa[1] = "Fase 2: Evidencias de los Abogados";
    etapas[2] = 3;
    nombresEtapa[2] = "Fase 3: Acusaciones entre Sospechosos";
    cabeza = 0;
  }

  void reiniciar() { cabeza = 0; }
  int etapaActual() { return etapas[cabeza]; }
  string descripcionActual() { return nombresEtapa[cabeza]; }
  void avanzarTurnoCircular() { cabeza = (cabeza + 1) % 3; }
};

// ============================================================================
// 3. PILA PARA RECOLECCION DE EVIDENCIAS FORENSES (NOCHE - MAXIMO 2 PISTAS)
// ============================================================================

struct NodoPila {
  string tipoForma;
  string tituloPista;
  string descripcionEvidencia;
  NodoPila *sig;

  NodoPila(string _forma, string _titulo, string _desc)
      : tipoForma(_forma), tituloPista(_titulo), descripcionEvidencia(_desc),
        sig(NULL) {}
};

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
// 4. ARBOL JERARQUICO DE BUSQUEDA POR RANGOS DEL JUZGADO (DIA)
//    Busqueda dirigida con busqueda binaria por nivel: O(log V) tiempo, O(1)
//    aux
// ============================================================================

struct NodoArbolJuicio {
  int idCarta;
  int idMin; // Limite inferior de IDs en este subarbol
  int idMax; // Limite superior de IDs en este subarbol
  int numeroEtapa;
  string grupo;
  string autor;
  string tituloCarta;
  string detalle;
  int pesoIncriminatorio;
  string acusadoObjetivo;
  bool revelada;

  NodoArbolJuicio *hijos[4];
  int numHijos;

  NodoArbolJuicio(int _id, int _etapa, string _grupo, string _autor,
                  string _titulo, string _detalle, int _peso,
                  string _objetivo) {
    idCarta = _id;
    idMin = _id;
    idMax = _id;
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

  // Inserta el hijo manteniendo orden creciente por intervalo [idMin, idMax]
  void agregarHijo(NodoArbolJuicio *hijo) {
    if (hijo == NULL || numHijos >= 4)
      return;

    int pos = numHijos;
    while (pos > 0 && hijos[pos - 1]->idMin > hijo->idMin) {
      hijos[pos] = hijos[pos - 1];
      pos--;
    }
    hijos[pos] = hijo;
    numHijos++;

    if (numHijos == 1) {
      idMin = hijo->idMin;
      idMax = hijo->idMax;
    } else {
      if (hijo->idMin < idMin)
        idMin = hijo->idMin;
      if (hijo->idMax > idMax)
        idMax = hijo->idMax;
    }
  }
};

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

  void construirArbol(string nombreCaso, const Individuo &sospA,
                      const Individuo &sospB, string nombresTestigos) {
    reiniciar();
    raiz = new NodoArbolJuicio(-1, 0, "RAIZ", "Juzgado Penal", nombreCaso,
                               "Expediente en audiencia oral", 0, "");

    NodoArbolJuicio *grupoA = new NodoArbolJuicio(
        -1, 0, "GRUPO_A", "Defensa A", "Sospechoso A y su Abogado",
        "Estrado izquierdo: " + sospA.nombre, 0, sospA.nombre);
    NodoArbolJuicio *grupoTestigos = new NodoArbolJuicio(
        -1, 0, "GRUPO_TESTIGOS", "Panel de 3 Testigos",
        "3 Testigos Presenciales", "Testigos: " + nombresTestigos, 0, "Ambos");
    NodoArbolJuicio *grupoB = new NodoArbolJuicio(
        -1, 0, "GRUPO_B", "Defensa B", "Sospechoso B y su Abogado",
        "Estrado derecho: " + sospB.nombre, 0, sospB.nombre);

    // 1. Adjuntar hojas a cada grupo (propaga automaticamente [idMin, idMax])
    // Grupo A: rango [0, 2]
    grupoA->agregarHijo(new NodoArbolJuicio(
        0, 3, "SOSPECHOSO_A", sospA.nombre, "Acusacion de " + sospA.nombre,
        sospA.acusacionCruzada, sospA.pesoAcusacion, sospB.nombre));

    grupoA->agregarHijo(
        new NodoArbolJuicio(1, 2, "ABOGADO_A", "Abogado de " + sospA.nombre,
                            "Evidencia #1 - Abogado A", sospA.evidenciaAbogado1,
                            sospA.pesoAbogado1, sospA.nombre));

    grupoA->agregarHijo(
        new NodoArbolJuicio(2, 2, "ABOGADO_A", "Abogado de " + sospA.nombre,
                            "Evidencia #2 - Abogado A", sospA.evidenciaAbogado2,
                            sospA.pesoAbogado2, sospA.nombre));

    // Grupo Testigos: rango [3, 4]
    grupoTestigos->agregarHijo(new NodoArbolJuicio(
        3, 1, "TESTIGOS_A", "Testigos (" + nombresTestigos + ")",
        "Testigos hacia " + sospA.nombre, sospA.testimonioTestigos,
        sospA.pesoTestigos, sospA.nombre));

    grupoTestigos->agregarHijo(new NodoArbolJuicio(
        4, 1, "TESTIGOS_B", "Testigos (" + nombresTestigos + ")",
        "Testigos hacia " + sospB.nombre, sospB.testimonioTestigos,
        sospB.pesoTestigos, sospB.nombre));

    // Grupo B: rango [5, 7]
    grupoB->agregarHijo(
        new NodoArbolJuicio(5, 2, "ABOGADO_B", "Abogado de " + sospB.nombre,
                            "Evidencia #1 - Abogado B", sospB.evidenciaAbogado1,
                            sospB.pesoAbogado1, sospB.nombre));

    grupoB->agregarHijo(
        new NodoArbolJuicio(6, 2, "ABOGADO_B", "Abogado de " + sospB.nombre,
                            "Evidencia #2 - Abogado B", sospB.evidenciaAbogado2,
                            sospB.pesoAbogado2, sospB.nombre));

    grupoB->agregarHijo(new NodoArbolJuicio(
        7, 3, "SOSPECHOSO_B", sospB.nombre, "Acusacion de " + sospB.nombre,
        sospB.acusacionCruzada, sospB.pesoAcusacion, sospA.nombre));

    // 2. Adjuntar los 3 grupos ordenados a la raiz (propaga rango total [0, 7])
    raiz->agregarHijo(grupoA);
    raiz->agregarHijo(grupoTestigos);
    raiz->agregarHijo(grupoB);
  }

  /**
   * @brief Busqueda dirigida por rangos con busqueda binaria en cada nivel.
   * Complejidad temporal: O(h * log b) = O(log V) sin explorar ramas hermanas.
   * Complejidad espacial auxiliar: O(1) (iterativa).
   */
  NodoArbolJuicio *buscarCarta(NodoArbolJuicio *nodo, int idBuscado) {
    NodoArbolJuicio *actual = nodo;
    while (actual != NULL) {
      if (idBuscado < actual->idMin || idBuscado > actual->idMax)
        return NULL;
      if (actual->idCarta == idBuscado)
        return actual;
      if (actual->numHijos == 0)
        return NULL;

      int izq = 0;
      int der = actual->numHijos - 1;
      NodoArbolJuicio *siguiente = NULL;
      while (izq <= der) {
        int mid = izq + (der - izq) / 2;
        NodoArbolJuicio *hijo = actual->hijos[mid];
        if (idBuscado < hijo->idMin) {
          der = mid - 1;
        } else if (idBuscado > hijo->idMax) {
          izq = mid + 1;
        } else {
          siguiente = hijo;
          break;
        }
      }
      actual = siguiente;
    }
    return NULL;
  }

  NodoArbolJuicio *buscarCarta(int idBuscado) {
    return buscarCarta(raiz, idBuscado);
  }
};

// ============================================================================
// 5. MONTICULO BINARIO DE MAXIMOS PARA RANKING DE PRUEBAS REVELADAS
// ============================================================================

struct ElementoMonticulo {
  int idCarta;
  int numeroEtapa;
  string autor;
  string titulo;
  string detalle;
  string acusadoObjetivo;
  int peso;
};

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
// 6. COLA CON PRIORIDAD BASADA EN MONTICULO BINARIO DE MINIMOS (O(log n))
//    Insercion: O(log n) | Extraccion: O(log n) | Consulta del frente: O(1)
// ============================================================================

class NodoCaso {
public:
  int idCaso;
  string descripcion;
  int prioridad;
  Individuo individuos[5];
  int indiceCulpable;
  int individuoInspeccionado;
  string pistasPersonalizadas[2];
  string titulosPistas[2];
  string formasPistas[2];
  int evidenciasMostradas;
  int idDetectiveAsignado;

  string faseActual; // "NOCHE" o "DIA"
  int idxSospechosoA;
  int idxSospechosoB;
  string actorAnimado;
  string textoModalPrueba;
  string tituloModalPrueba;

  PilaEvidencias historialEvidencias;
  ArbolJuicio arbolJuzgado;
  MonticuloBinario monticuloPruebas;
  ColaCircularTurnos turnosJuzgado;

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

    formasPistas[0] = "rectangulo";
    titulosPistas[0] = "Fotografia Forense";
    formasPistas[1] = "rombo";
    titulosPistas[1] = "Objeto en Escena";

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
      individuos[i].imagen = "";
    }

    for (int k = 0; k < 2; k++) {
      pistasPersonalizadas[k] =
          "El analisis forense confirma que el culpable " +
          individuos[indiceCulpable].rasgos[k] + ".";
    }

    configurarArgumentosJuzgadoPorDefecto();
  }

  void configurarArgumentosJuzgadoPorDefecto() {
    for (int i = 0; i < 5; i++) {
      if (individuos[i].esCulpable) {
        individuos[i].testimonioTestigos =
            "Los 3 testigos coinciden en que " + individuos[i].nombre +
            " merodeaba el punto exacto del crimen (" +
            individuos[i].contextoEscena + ") y contradice la hora de salida.";
        individuos[i].pesoTestigos = 92;

        individuos[i].evidenciaAbogado1 =
            "El abogado presenta un supuesto recibo para respaldar que '" +
            individuos[i].coartada +
            "', pero la tinta esta borrosa y carece de sello oficial (Coartada "
            "dudosa).";
        individuos[i].pesoAbogado1 = 35;

        individuos[i].evidenciaAbogado2 =
            "El abogado alega que su cliente solo pasaba por el lugar, pero el "
            "peritaje hallo que el autor " +
            individuos[i].rasgos[0] + " y " + individuos[i].rasgos[1] +
            " (Contradiccion grave en la defensa).";
        individuos[i].pesoAbogado2 = 88;

        individuos[i].acusacionCruzada =
            individuos[i].nombre +
            " levanta la voz nerviosamente y acusa al otro sospechoso sin "
            "aportar pruebas fisicas verificables.";
        individuos[i].pesoAcusacion = 42;
      } else {
        individuos[i].testimonioTestigos =
            "Los 3 testigos declaran que aunque " + individuos[i].nombre +
            " estaba en el recinto (" + individuos[i].contextoEscena +
            "), permanecio a la vista del grupo cuando ocurrio el hecho.";
        individuos[i].pesoTestigos = 50;

        individuos[i].evidenciaAbogado1 =
            "El abogado exhibe un registro verificado en camaras externas que "
            "respalda su declaracion: '" +
            individuos[i].coartada + "' (Prueba de descargo solida).";
        individuos[i].pesoAbogado1 = 25;

        individuos[i].evidenciaAbogado2 =
            "El abogado demuestra que las huellas de " + individuos[i].nombre +
            " no coinciden con el objeto principal registrado en las "
            "evidencias del caso.";
        individuos[i].pesoAbogado2 = 20;

        individuos[i].acusacionCruzada =
            individuos[i].nombre +
            " senala directamente al otro acusado indicando que lo vio "
            "ocultando evidencia clave segundos despues del incidente.";
        individuos[i].pesoAcusacion = 84;
      }
    }
  }

  bool solicitarNuevaEvidencia() {
    if (evidenciasMostradas >= 2)
      return false;
    int idx = evidenciasMostradas;
    historialEvidencias.apilar(formasPistas[idx], titulosPistas[idx],
                               pistasPersonalizadas[idx]);
    evidenciasMostradas++;
    return true;
  }

  void reiniciarInvestigacionNocturna() {
    faseActual = "NOCHE";
    idxSospechosoA = -1;
    idxSospechosoB = -1;
    for (int i = 0; i < 5; i++) {
      individuos[i].rolNoche = 0;
    }
  }
};

class ColaPrioridadCasos {
private:
  NodoCaso **heap;
  int tamano;
  int capacidad;

  bool esMasUrgente(NodoCaso *a, NodoCaso *b) {
    if (a->prioridad != b->prioridad)
      return a->prioridad < b->prioridad;
    return a->idCaso < b->idCaso; // Desempate FIFO por orden de llegada
  }

  void intercambiar(int i, int j) {
    NodoCaso *temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
  }

  void redimensionar() {
    int nuevaCap = capacidad * 2;
    NodoCaso **nuevoHeap = new NodoCaso *[nuevaCap];
    for (int i = 0; i < tamano; i++) {
      nuevoHeap[i] = heap[i];
    }
    delete[] heap;
    heap = nuevoHeap;
    capacidad = nuevaCap;
  }

  // Reajuste ascendente: O(log n)
  void subir(int idx) {
    while (idx > 0) {
      int padre = (idx - 1) / 2;
      if (esMasUrgente(heap[idx], heap[padre])) {
        intercambiar(idx, padre);
        idx = padre;
      } else {
        break;
      }
    }
  }

  // Reajuste descendente: O(log n)
  void bajar(int idx) {
    while (true) {
      int menor = idx;
      int izq = 2 * idx + 1;
      int der = 2 * idx + 2;

      if (izq < tamano && esMasUrgente(heap[izq], heap[menor]))
        menor = izq;
      if (der < tamano && esMasUrgente(heap[der], heap[menor]))
        menor = der;

      if (menor != idx) {
        intercambiar(idx, menor);
        idx = menor;
      } else {
        break;
      }
    }
  }

  void actualizarCabeza() { cabeza = (tamano > 0) ? heap[0] : NULL; }

public:
  NodoCaso
      *cabeza; // Apunta siempre al caso de mayor urgencia (heap[0]) en O(1)

  ColaPrioridadCasos() {
    capacidad = 8;
    tamano = 0;
    heap = new NodoCaso *[capacidad];
    cabeza = NULL;
  }

  ~ColaPrioridadCasos() {
    for (int i = 0; i < tamano; i++) {
      delete heap[i];
    }
    delete[] heap;
    cabeza = NULL;
  }

  // Insercion en O(log n) amortizado
  void insertarNodoConPrioridad(NodoCaso *nuevo) {
    if (nuevo == NULL)
      return;
    if (tamano >= capacidad)
      redimensionar();
    heap[tamano] = nuevo;
    subir(tamano);
    tamano++;
    actualizarCabeza();
  }

  void insertarConPrioridad(int id, string desc, int prio) {
    NodoCaso *nuevo = new NodoCaso(id, desc, prio);
    insertarNodoConPrioridad(nuevo);
  }

  // Extraccion del caso mas urgente en O(log n) con liberacion en cascada
  void eliminarPrimero() {
    if (tamano == 0)
      return;
    delete heap[0];
    tamano--;
    if (tamano > 0) {
      heap[0] = heap[tamano];
      bajar(0);
    }
    actualizarCabeza();
  }

  NodoCaso *obtenerFrente() { return cabeza; }
  int obtenerTamano() { return tamano; }
  bool estaVacia() { return tamano == 0; }
};

#endif