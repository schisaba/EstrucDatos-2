/**
 * @file servidor.cpp
 * @authors Sebastian Chisaba Cruz
 *          Ariadna Valentina Ramirez Santeliz
 *          Jeronimo Pascuas Barroso
 * @brief Servidor HTTP multiplataforma (Linux y Windows) en C++ que conecta
 * las estructuras de datos (Cola con Prioridad, Pila, Cola Circular, Arbol
 * y Monticulo Binario) con la interfaz grafica interactiva Noche / Dia.
 */

#include "Estructuras.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>

// Compatibilidad multiplataforma para Sockets (Windows Winsock2 y Linux POSIX)
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET SocketType;
#define CLOSESOCKET closesocket
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int SocketType;
#define CLOSESOCKET close
#define INVALID_SOCKET -1
#endif

// Plantilla fija de 4 detectives en arreglo estatico de Registros
Detective detectives[4] = {{"Sebastian", true, 3},
                           {"Jeronimo", true, 5},
                           {"Valentina", true, 4},
                           {"Daniel", true, 2}};

// Cola con Prioridad global de expedientes activos
ColaPrioridadCasos colaCasos;
int contadorCasos = 101;
string ultimoMensaje =
    "Bienvenido. Selecciona un detective en la central para iniciar la Noche de Investigacion.";
string estadoAlerta = "NORMAL"; // "NORMAL", "FALLO_NOCHE", "VEREDICTO_OK", "VEREDICTO_FAIL"

/**
 * @brief Escapa comillas y saltos de linea para garantizar un JSON valido.
 */
string escaparJSON(const string &texto) {
  string limpio = "";
  for (size_t i = 0; i < texto.length(); i++) {
    char c = texto[i];
    if (c == '\"')
      limpio += "\\\"";
    else if (c == '\\')
      limpio += "\\\\";
    else if (c == '\n' || c == '\r')
      limpio += " ";
    else
      limpio += c;
  }
  return limpio;
}

/**
 * @brief Decodifica parametros enviados en la URL (como %20, acentos, etc.).
 */
string decodificarURL(const string &str) {
  string ret;
  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '+') {
      ret += ' ';
    } else if (str[i] == '%' && i + 2 < str.length()) {
      int valor = 0;
      istringstream is(str.substr(i + 1, 2));
      if (is >> hex >> valor) {
        ret += static_cast<char>(valor);
        i += 2;
      } else {
        ret += str[i];
      }
    } else {
      ret += str[i];
    }
  }
  return ret;
}

/**
 * @brief Extrae el valor de un parametro de consulta (?clave=valor&...).
 */
string obtenerParametro(const string &query, const string &clave) {
  string patron = clave + "=";
  size_t pos = query.find(patron);
  if (pos == string::npos)
    return "";
  pos += patron.length();
  size_t fin = query.find("&", pos);
  if (fin == string::npos) {
    size_t espacio = query.find(" ", pos);
    fin = espacio;
  }
  string crudo = (fin == string::npos) ? query.substr(pos)
                                       : query.substr(pos, fin - pos);
  return decodificarURL(crudo);
}

/**
 * @brief Serializa el estado completo de todas las estructuras en formato JSON.
 */
string obtenerEstadoJSON() {
  stringstream ss;
  ss << "{";
  ss << "\"mensaje\":\"" << escaparJSON(ultimoMensaje) << "\",";
  ss << "\"estadoAlerta\":\"" << escaparJSON(estadoAlerta) << "\",";

  // 1. Arreglo fijo de Detectives
  ss << "\"detectives\":[";
  for (int i = 0; i < 4; i++) {
    ss << "{\"id\":" << i << ",\"nombre\":\""
       << escaparJSON(detectives[i].nombre) << "\","
       << "\"disponible\":" << (detectives[i].disponible ? "true" : "false")
       << ",\"resueltos\":" << detectives[i].casosResueltos << "}"
       << (i < 3 ? "," : "");
  }
  ss << "],";

  // 2. Cola con Prioridad de Casos
  ss << "\"casos\":[";
  NodoCaso *act = colaCasos.cabeza;
  while (act != NULL) {
    string detNom = (act->idDetectiveAsignado == -1)
                        ? "Sin asignar"
                        : detectives[act->idDetectiveAsignado].nombre;
    ss << "{\"id\":" << act->idCaso << ",\"descripcion\":\""
       << escaparJSON(act->descripcion) << "\","
       << "\"prioridad\":" << act->prioridad
       << ",\"evidencias\":" << act->evidenciasMostradas << ","
       << "\"fase\":\"" << act->faseActual << "\","
       << "\"detective\":\"" << escaparJSON(detNom) << "\"}";
    if (act->sig != NULL)
      ss << ",";
    act = act->sig;
  }
  ss << "],";

  // 3. Caso Activo (Cabeza de la Cola con Prioridad)
  if (colaCasos.cabeza != NULL) {
    NodoCaso *c = colaCasos.cabeza;
    ss << "\"casoActivo\":{";
    ss << "\"id\":" << c->idCaso << ",";
    ss << "\"descripcion\":\"" << escaparJSON(c->descripcion) << "\",";
    ss << "\"prioridad\":" << c->prioridad << ",";
    ss << "\"idDetective\":" << c->idDetectiveAsignado << ",";
    ss << "\"evidenciasMostradas\":" << c->evidenciasMostradas << ",";
    ss << "\"individuoInspeccionado\":" << c->individuoInspeccionado << ",";
    ss << "\"faseActual\":\"" << c->faseActual << "\",";
    ss << "\"idxSospechosoA\":" << c->idxSospechosoA << ",";
    ss << "\"idxSospechosoB\":" << c->idxSospechosoB << ",";
    ss << "\"actorAnimado\":\"" << escaparJSON(c->actorAnimado) << "\",";
    ss << "\"tituloModalPrueba\":\"" << escaparJSON(c->tituloModalPrueba)
       << "\",";
    ss << "\"textoModalPrueba\":\"" << escaparJSON(c->textoModalPrueba)
       << "\",";
    ss << "\"etapaTurno\":" << c->turnosJuzgado.etapaActual() << ",";
    ss << "\"descripcionTurno\":\""
       << escaparJSON(c->turnosJuzgado.descripcionActual()) << "\",";

    // Pila de Evidencias (del tope al fondo)
    ss << "\"pilaEvidencias\":[";
    NodoPila *p = c->historialEvidencias.cabeza;
    while (p != NULL) {
      ss << "{\"forma\":\"" << escaparJSON(p->tipoForma) << "\","
         << "\"titulo\":\"" << escaparJSON(p->tituloPista) << "\","
         << "\"descripcion\":\"" << escaparJSON(p->descripcionEvidencia)
         << "\"}";
      if (p->sig != NULL)
        ss << ",";
      p = p->sig;
    }
    ss << "],";

    // Los 5 individuos de la escena nocturna
    ss << "\"individuos\":[";
    for (int i = 0; i < 5; i++) {
      ss << "{\"indice\":" << i << ","
         << "\"nombre\":\"" << escaparJSON(c->individuos[i].nombre) << "\","
         << "\"contexto\":\"" << escaparJSON(c->individuos[i].contextoEscena)
         << "\","
         << "\"coartada\":\"" << escaparJSON(c->individuos[i].coartada) << "\","
         << "\"rolNoche\":" << c->individuos[i].rolNoche << ","
         << "\"spriteId\":" << c->individuos[i].spriteId << ","
         << "\"rasgos\":[\"" << escaparJSON(c->individuos[i].rasgos[0])
         << "\",\"" << escaparJSON(c->individuos[i].rasgos[1]) << "\",\""
         << escaparJSON(c->individuos[i].rasgos[2]) << "\",\""
         << escaparJSON(c->individuos[i].rasgos[3]) << "\"]}"
         << (i < 4 ? "," : "");
    }
    ss << "],";

    // Cartas del Arbol del Juzgado (Cartas 0 a 7 en la mesa durante el Dia)
    ss << "\"cartasMesa\":[";
    if (c->faseActual == "DIA" && c->arbolJuzgado.raiz != NULL) {
      for (int idC = 0; idC < 8; idC++) {
        NodoArbolJuicio *carta =
            c->arbolJuzgado.buscarCarta(c->arbolJuzgado.raiz, idC);
        if (carta != NULL) {
          ss << "{\"idCarta\":" << carta->idCarta << ","
             << "\"numeroEtapa\":" << carta->numeroEtapa << ","
             << "\"grupo\":\"" << escaparJSON(carta->grupo) << "\","
             << "\"autor\":\"" << escaparJSON(carta->autor) << "\","
             << "\"titulo\":\"" << escaparJSON(carta->tituloCarta) << "\","
             << "\"detalle\":\"" << escaparJSON(carta->detalle) << "\","
             << "\"peso\":" << carta->pesoIncriminatorio << ","
             << "\"objetivo\":\"" << escaparJSON(carta->acusadoObjetivo) << "\","
             << "\"revelada\":" << (carta->revelada ? "true" : "false") << "}"
             << (idC < 7 ? "," : "");
        }
      }
    }
    ss << "],";

    // Ranking en tiempo real del Monticulo Binario (Opcion B)
    ElementoMonticulo ranking[16];
    int cantRanking = 0;
    c->monticuloPruebas.obtenerRankingOrdenado(ranking, cantRanking);
    ss << "\"monticuloRanking\":[";
    for (int r = 0; r < cantRanking; r++) {
      ss << "{\"posicion\":" << (r + 1) << ","
         << "\"idCarta\":" << ranking[r].idCarta << ","
         << "\"numeroEtapa\":" << ranking[r].numeroEtapa << ","
         << "\"autor\":\"" << escaparJSON(ranking[r].autor) << "\","
         << "\"titulo\":\"" << escaparJSON(ranking[r].titulo) << "\","
         << "\"detalle\":\"" << escaparJSON(ranking[r].detalle) << "\","
         << "\"objetivo\":\"" << escaparJSON(ranking[r].acusadoObjetivo) << "\","
         << "\"peso\":" << ranking[r].peso << "}"
         << (r < cantRanking - 1 ? "," : "");
    }
    ss << "]";

    ss << "}";
  } else {
    ss << "\"casoActivo\":null";
  }

  ss << "}";
  return ss.str();
}

/**
 * @brief Procesa la creacion de un caso personalizado desde el menu del juego.
 */
void procesarCreacionCasoPersonalizado(const string &req) {
  string titulo = obtenerParametro(req, "titulo");
  if (titulo.empty())
    titulo = "Expediente Personalizado #" + to_string(contadorCasos);

  string prioStr = obtenerParametro(req, "prioridad");
  int prio = 1;
  if (prioStr == "2")
    prio = 2;
  else if (prioStr == "3")
    prio = 3;

  string culpStr = obtenerParametro(req, "culpable");
  int idxCulp = 0;
  if (!culpStr.empty()) {
    int val = atoi(culpStr.c_str());
    if (val >= 0 && val < 5)
      idxCulp = val;
  }

  NodoCaso *nuevo = new NodoCaso(contadorCasos++, titulo, prio);
  nuevo->indiceCulpable = idxCulp;

  // Configurar los 5 individuos ingresados por el usuario
  for (int i = 0; i < 5; i++) {
    string idxS = to_string(i);
    string nom = obtenerParametro(req, "nom" + idxS);
    string ctx = obtenerParametro(req, "ctx" + idxS);
    string coa = obtenerParametro(req, "coa" + idxS);

    if (!nom.empty())
      nuevo->individuos[i].nombre = nom;
    if (!ctx.empty())
      nuevo->individuos[i].contextoEscena = ctx;
    if (!coa.empty())
      nuevo->individuos[i].coartada = coa;

    nuevo->individuos[i].esCulpable = (i == idxCulp);
    nuevo->individuos[i].rolNoche = 0;
    nuevo->individuos[i].spriteId = i + 1;
  }

  // Configurar las 4 pistas de la Pila ingresadas por el usuario
  for (int k = 0; k < 4; k++) {
    string pStr = obtenerParametro(req, "pista" + to_string(k));
    if (!pStr.empty()) {
      nuevo->pistasPersonalizadas[k] = pStr;
    } else {
      nuevo->pistasPersonalizadas[k] =
          "Pista forense #" + to_string(k + 1) + ": incrimina a " +
          nuevo->individuos[idxCulp].nombre + " (" +
          nuevo->individuos[idxCulp].contextoEscena + ").";
    }
  }

  // Configurar argumentos por defecto para el Dia y sobreescribir si el usuario
  // ingreso textos personalizados para los sospechosos
  nuevo->configurarArgumentosJuzgadoPorDefecto();
  for (int i = 0; i < 5; i++) {
    string idxS = to_string(i);
    string tTest = obtenerParametro(req, "test" + idxS);
    string eAb1 = obtenerParametro(req, "ab1_" + idxS);
    string eAb2 = obtenerParametro(req, "ab2_" + idxS);
    string acus = obtenerParametro(req, "acu" + idxS);

    if (!tTest.empty())
      nuevo->individuos[i].testimonioTestigos = tTest;
    if (!eAb1.empty())
      nuevo->individuos[i].evidenciaAbogado1 = eAb1;
    if (!eAb2.empty())
      nuevo->individuos[i].evidenciaAbogado2 = eAb2;
    if (!acus.empty())
      nuevo->individuos[i].acusacionCruzada = acus;
  }

  colaCasos.insertarNodoConPrioridad(nuevo);
  estadoAlerta = "NORMAL";
  ultimoMensaje = "[Cola con prioridad] Caso '" + titulo +
                  "' insertado exitosamente con prioridad " + to_string(prio) +
                  ".";
}

int main() {
  srand(static_cast<unsigned int>(time(0)));

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    cerr << "Error al inicializar Winsock en Windows." << endl;
    return 1;
  }
#endif

  // Casos iniciales en la Cola con Prioridad
  colaCasos.insertarConPrioridad(contadorCasos++,
                                 "Homicidio en el Club Nocturno", 1);
  colaCasos.insertarConPrioridad(contadorCasos++,
                                 "Robo de Evidencia en el Archivo Central", 2);
  colaCasos.insertarConPrioridad(contadorCasos++,
                                 "Envenenamiento en la Cafeteria", 3);

  SocketType server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == INVALID_SOCKET) {
    cerr << "No se pudo crear el socket del servidor." << endl;
    return 1;
  }

  int opt = 1;
#ifdef _WIN32
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt,
             sizeof(opt));
#else
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    cerr << "Error en bind: El puerto 8080 ya esta en uso." << endl;
    CLOSESOCKET(server_fd);
    return 1;
  }

  listen(server_fd, 10);

  cout << "=========================================================" << endl;
  cout << " MOTOR C++ MULTIPLATAFORMA (WINDOWS / LINUX) ACTIVO" << endl;
  cout << " URL del Servidor: http://localhost:8080" << endl;
  cout << " Abre index.html en tu navegador para iniciar el juego." << endl;
  cout << "=========================================================" << endl;

  while (true) {
    SocketType client = accept(server_fd, NULL, NULL);
    if (client == INVALID_SOCKET)
      continue;

    char buffer[8192] = {0};
#ifdef _WIN32
    recv(client, buffer, sizeof(buffer) - 1, 0);
#else
    read(client, buffer, sizeof(buffer) - 1);
#endif
    string req(buffer);
    estadoAlerta = "NORMAL";

    // 1. Asignar detective al caso en la cabeza de la cola con prioridad
    if (req.find("GET /asignar?") != string::npos && colaCasos.cabeza != NULL) {
      int idDet = atoi(obtenerParametro(req, "id").c_str());
      if (colaCasos.cabeza->idDetectiveAsignado != -1) {
        ultimoMensaje =
            "El expediente actual ya cuenta con un detective asignado.";
      } else if (idDet >= 0 && idDet < 4 && detectives[idDet].disponible) {
        colaCasos.cabeza->idDetectiveAsignado = idDet;
        detectives[idDet].disponible = false;
        ultimoMensaje = "Detective " + detectives[idDet].nombre +
                        " asignado al Caso #" +
                        to_string(colaCasos.cabeza->idCaso) +
                        ". Recolecta pistas en la Pila y elige 2 sospechosos.";
      } else {
        ultimoMensaje = "El detective seleccionado no esta disponible.";
      }
    }
    // 2. Apilar nueva pista forense en la Pila del caso activo (Noche)
    else if (req.find("GET /evidencia") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      if (c->solicitarNuevaEvidencia()) {
        ultimoMensaje =
            "[Pila] Nueva pista forense apilada en el tope del expediente.";
      } else {
        ultimoMensaje =
            "Ya se han apilado las 4 pistas disponibles para este caso.";
      }
    }
    // 3. Seleccionar un individuo en la Noche para ver su foto y contexto
    else if (req.find("GET /inspeccionar?") != string::npos &&
             colaCasos.cabeza != NULL) {
      int idx = atoi(obtenerParametro(req, "idx").c_str());
      if (idx >= 0 && idx < 5) {
        colaCasos.cabeza->individuoInspeccionado = idx;
        ultimoMensaje = "Inspeccionando ficha y foto de " +
                        colaCasos.cabeza->individuos[idx].nombre + ".";
      }
    }
    // 4. Marcar o desmarcar a un individuo como Sospechoso (maximo 2 de los 5)
    else if (req.find("GET /alternar_sospechoso?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idx = atoi(obtenerParametro(req, "idx").c_str());
      if (idx >= 0 && idx < 5 && c->faseActual == "NOCHE") {
        c->individuoInspeccionado = idx;
        if (c->individuos[idx].rolNoche == 1) {
          c->individuos[idx].rolNoche = 0;
          ultimoMensaje = c->individuos[idx].nombre +
                          " vuelve a quedar clasificado como Testigo.";
        } else {
          int conteoSosp = 0;
          for (int i = 0; i < 5; i++) {
            if (c->individuos[i].rolNoche == 1)
              conteoSosp++;
          }
          if (conteoSosp >= 2) {
            ultimoMensaje = "Ya seleccionaste 2 sospechosos. Desmarca uno si "
                            "deseas cambiar tu eleccion.";
          } else {
            c->individuos[idx].rolNoche = 1;
            ultimoMensaje = c->individuos[idx].nombre +
                            " marcado como Sospechoso para el juicio.";
          }
        }
      }
    }
    // 5. Transicion de la Noche (Investigador) al Dia (Juez)
    else if (req.find("GET /pasar_dia") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      if (c->idDetectiveAsignado == -1) {
        ultimoMensaje =
            "Asigna primero un detective de la plantilla antes de pasar al Dia.";
      } else if (c->evidenciasMostradas == 0) {
        ultimoMensaje = "Recolecta al menos una pista en la Pila antes de "
                        "enviar el caso al juzgado.";
      } else {
        int sA = -1, sB = -1;
        string nombresTestigos = "";
        for (int i = 0; i < 5; i++) {
          if (c->individuos[i].rolNoche == 1) {
            if (sA == -1)
              sA = i;
            else
              sB = i;
          } else {
            if (!nombresTestigos.empty())
              nombresTestigos += ", ";
            nombresTestigos += c->individuos[i].nombre;
          }
        }

        if (sA == -1 || sB == -1) {
          ultimoMensaje = "Debes escoger exactamente 2 sospechosos (los otros "
                          "3 pasaran a ser testigos).";
        } else {
          // Verificar si el verdadero culpable esta entre los 2 sospechosos
          bool culpableCapturado = (c->individuos[sA].esCulpable ||
                                    c->individuos[sB].esCulpable);
          if (!culpableCapturado) {
            estadoAlerta = "FALLO_NOCHE";
            ultimoMensaje =
                "No lograste hallar al culpable entre tus 2 sospechosos (" +
                c->individuos[sA].nombre + " y " + c->individuos[sB].nombre +
                "). El verdadero culpable quedo libre entre los testigos. "
                "Repite la investigacion nocturna.";
            c->reiniciarInvestigacionNocturna();
          } else {
            c->faseActual = "DIA";
            c->idxSospechosoA = sA;
            c->idxSospechosoB = sB;
            c->actorAnimado = "TESTIGOS";
            c->turnosJuzgado.reiniciar();
            c->monticuloPruebas.vaciar();
            c->arbolJuzgado.construirArbol(c->descripcion, c->individuos[sA],
                                           c->individuos[sB], nombresTestigos);
            c->tituloModalPrueba = "Audiencia Iniciada - Etapa 1";
            c->textoModalPrueba =
                "Haz clic en las cartas amarillas (1) para escuchar el "
                "contexto de los 3 testigos (" +
                nombresTestigos + ") hacia cada acusado.";
            ultimoMensaje =
                "Instancia de Dia iniciada. El Arbol del juzgado ha sido "
                "construido con los 3 grupos.";
          }
        }
      }
    }
    // 6. Hacer clic en una de las 8 cartas probatorias de la mesa en el Dia
    else if (req.find("GET /clic_carta?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idCarta = atoi(obtenerParametro(req, "id").c_str());
      if (c->faseActual == "DIA" && c->arbolJuzgado.raiz != NULL) {
        NodoArbolJuicio *carta =
            c->arbolJuzgado.buscarCarta(c->arbolJuzgado.raiz, idCarta);
        if (carta != NULL) {
          int etapaActual = c->turnosJuzgado.etapaActual();
          if (carta->numeroEtapa > etapaActual && !carta->revelada) {
            ultimoMensaje = "Respeta el orden de turnos de la Cola Circular: "
                            "actualmente estas en la Etapa " +
                            to_string(etapaActual) +
                            ". Revisa primero las cartas de esta etapa.";
          } else {
            carta->revelada = true;
            c->actorAnimado = carta->grupo;
            c->tituloModalPrueba =
                "[" + to_string(carta->numeroEtapa) + "] " + carta->tituloCarta;
            c->textoModalPrueba =
                carta->detalle + " (Peso registrado en Monticulo Binario: " +
                to_string(carta->pesoIncriminatorio) + " pts)";

            // Insertar en el Monticulo Binario (Opcion B)
            c->monticuloPruebas.insertar(carta);

            // Verificar si todas las cartas de la etapa actual ya fueron
            // reveladas para rotar automaticamente la Cola Circular de turnos
            bool etapaCompleta = true;
            for (int k = 0; k < 8; k++) {
              NodoArbolJuicio *aux =
                  c->arbolJuzgado.buscarCarta(c->arbolJuzgado.raiz, k);
              if (aux != NULL && aux->numeroEtapa == etapaActual &&
                  !aux->revelada) {
                etapaCompleta = false;
                break;
              }
            }
            if (etapaCompleta && etapaActual < 3) {
              c->turnosJuzgado.avanzarTurnoCircular();
              ultimoMensaje =
                  "[Arbol + Monticulo Binario] Prueba inspeccionada y "
                  "jerarquizada. La Cola Circular avanza a la Etapa " +
                  to_string(c->turnosJuzgado.etapaActual()) + ".";
            } else {
              ultimoMensaje =
                  "[Arbol + Monticulo Binario] Prueba de " + carta->autor +
                  " inspeccionada y ordenada en el ranking del Monticulo.";
            }
          }
        }
      }
    }
    // 7. Emitir veredicto final como Juez en el Dia
    else if (req.find("GET /veredicto?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idxElegido = atoi(obtenerParametro(req, "idx").c_str());
      if (c->faseActual == "DIA" &&
          (idxElegido == c->idxSospechosoA || idxElegido == c->idxSospechosoB)) {
        int detIdx = c->idDetectiveAsignado;
        string nombreElegido = c->individuos[idxElegido].nombre;
        string nombreCulpable = c->individuos[c->indiceCulpable].nombre;

        if (c->individuos[idxElegido].esCulpable) {
          if (detIdx >= 0 && detIdx < 4)
            detectives[detIdx].casosResueltos++;
          estadoAlerta = "VEREDICTO_OK";
          ultimoMensaje = "VEREDICTO CORRECTO: " + nombreElegido +
                          " era el verdadero culpable. El caso se cierra y se "
                          "libera la memoria en cascada en el Heap.";
        } else {
          estadoAlerta = "VEREDICTO_FAIL";
          ultimoMensaje = "VEREDICTO ERRONEO: Condenaste a " + nombreElegido +
                          ", pero el verdadero culpable era " + nombreCulpable +
                          ". El expediente se cierra y pasamos al siguiente caso.";
        }
        if (detIdx >= 0 && detIdx < 4)
          detectives[detIdx].disponible = true;
        colaCasos.eliminarPrimero();
      }
    }
    // 8. Crear e insertar un nuevo caso desde el menu del juego
    else if (req.find("GET /crear_caso?") != string::npos) {
      procesarCreacionCasoPersonalizado(req);
    }

    string json = obtenerEstadoJSON();
    string resp =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n\r\n" +
        json;

    send(client, resp.c_str(), static_cast<int>(resp.length()), 0);
    CLOSESOCKET(client);
  }

#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
