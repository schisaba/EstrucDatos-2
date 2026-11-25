/**
 * @file servidor.cpp
 * @authors Sebastian Chisaba Cruz
 *          Ariadna Valentina Ramirez Santeliz
 *          Jeronimo Pascuas Barroso
 * @brief Servidor HTTP multiplataforma (Linux y Windows) en C++ con
 * persistencia permanente de casos personalizados en archivo local.
 */

// 1. Cargar primero los sockets del sistema operativo para evitar conflicto con
// std::byte en Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
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

// 2. Cargar estructuras de datos y librerias estandar
#include "Estructuras.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>

const string ARCHIVO_CASOS = "casos_guardados.txt";

Detective detectives[4] = {{"Sebastian", true, 3},
                           {"Jeronimo", true, 5},
                           {"Valentina", true, 4},
                           {"Daniel", true, 2}};

ColaPrioridadCasos colaCasos;
int contadorCasos = 101;
string ultimoMensaje =
    "Selecciona un detective para iniciar la investigacion nocturna.";
string estadoAlerta = "NORMAL";

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

string limpiarLineaArchivo(const string &texto) {
  string limpio = "";
  for (size_t i = 0; i < texto.length(); i++) {
    char c = texto[i];
    if (c == '\n' || c == '\r')
      limpio += ' ';
    else
      limpio += c;
  }
  return limpio;
}

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
 * @brief Extrae un parametro de una cadena URL-encoded (query GET o cuerpo
 * POST).
 */
string obtenerParametro(const string &datos, const string &clave) {
  string patron = clave + "=";
  size_t pos = 0;
  while (pos < datos.length()) {
    size_t encontrado = datos.find(patron, pos);
    if (encontrado == string::npos)
      return "";
    // Verificar delimitador previo para evitar coincidencias parciales
    if (encontrado == 0 || datos[encontrado - 1] == '&' ||
        datos[encontrado - 1] == '?') {
      size_t inicio = encontrado + patron.length();
      size_t fin = datos.find('&', inicio);
      if (fin == string::npos) {
        size_t espacio = datos.find(' ', inicio);
        fin = espacio;
      }
      string crudo = (fin == string::npos) ? datos.substr(inicio)
                                           : datos.substr(inicio, fin - inicio);
      return decodificarURL(crudo);
    }
    pos = encontrado + 1;
  }
  return "";
}

/**
 * @brief Lee la peticion HTTP completa (incluyendo el cuerpo POST de imagenes).
 */
string leerPeticionHTTP(SocketType client) {
  string req = "";
  char chunk[4096];
  size_t headerEnd = string::npos;
  size_t contentLength = 0;

  while (true) {
#ifdef _WIN32
    int bytes = recv(client, chunk, sizeof(chunk), 0);
#else
    int bytes = read(client, chunk, sizeof(chunk));
#endif
    if (bytes <= 0)
      break;
    req.append(chunk, bytes);

    if (headerEnd == string::npos) {
      headerEnd = req.find("\r\n\r\n");
      if (headerEnd != string::npos) {
        // Buscar Content-Length de forma insensible a mayusculas
        string headersLower = req.substr(0, headerEnd);
        for (size_t i = 0; i < headersLower.length(); i++) {
          headersLower[i] = static_cast<char>(
              tolower(static_cast<unsigned char>(headersLower[i])));
        }
        size_t clPos = headersLower.find("content-length:");
        if (clPos != string::npos) {
          size_t valStart = clPos + 15;
          size_t valEnd = headersLower.find("\r\n", valStart);
          string valStr =
              (valEnd == string::npos)
                  ? headersLower.substr(valStart)
                  : headersLower.substr(valStart, valEnd - valStart);
          contentLength = static_cast<size_t>(atoi(valStr.c_str()));
        }
      }
    }

    if (headerEnd != string::npos) {
      size_t totalEsperado = headerEnd + 4 + contentLength;
      if (req.length() >= totalEsperado)
        break;
    }

    // Limite de seguridad (2 MB)
    if (req.length() > 2 * 1024 * 1024)
      break;
  }
  return req;
}

/**
 * @brief Guarda permanentemente un caso personalizado en casos_guardados.txt.
 */
void guardarCasoEnArchivo(NodoCaso *c) {
  if (c == NULL)
    return;
  ofstream out(ARCHIVO_CASOS.c_str(), ios::app);
  if (!out.is_open())
    return;

  out << "===CASO===" << "\n";
  out << limpiarLineaArchivo(c->descripcion) << "\n";
  out << c->prioridad << "\n";
  out << c->indiceCulpable << "\n";
  for (int k = 0; k < 2; k++) {
    out << limpiarLineaArchivo(c->pistasPersonalizadas[k]) << "\n";
  }
  for (int i = 0; i < 5; i++) {
    out << limpiarLineaArchivo(c->individuos[i].nombre) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].contextoEscena) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].coartada) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].testimonioTestigos) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].evidenciaAbogado1) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].evidenciaAbogado2) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].acusacionCruzada) << "\n";
    out << limpiarLineaArchivo(c->individuos[i].imagen) << "\n";
  }
  out << "===FIN===" << "\n";
  out.close();
}

/**
 * @brief Carga al iniciar el servidor todos los casos guardados en disco.
 */
int cargarCasosGuardados() {
  ifstream in(ARCHIVO_CASOS.c_str());
  if (!in.is_open())
    return 0;

  int cargados = 0;
  string linea;
  while (getline(in, linea)) {
    if (!linea.empty() && linea[linea.length() - 1] == '\r')
      linea.erase(linea.length() - 1);
    if (linea != "===CASO===")
      continue;

    string titulo, prioStr, culpStr;
    if (!getline(in, titulo))
      break;
    if (!getline(in, prioStr))
      break;
    if (!getline(in, culpStr))
      break;

    int prio = atoi(prioStr.c_str());
    if (prio < 1 || prio > 3)
      prio = 1;
    int idxCulp = atoi(culpStr.c_str());
    if (idxCulp < 0 || idxCulp > 4)
      idxCulp = 0;

    NodoCaso *nuevo = new NodoCaso(contadorCasos++, titulo, prio);
    nuevo->indiceCulpable = idxCulp;

    for (int k = 0; k < 2; k++) {
      string p;
      if (getline(in, p) && !p.empty()) {
        nuevo->pistasPersonalizadas[k] = p;
      }
    }

    // Valores temporales para restaurar textos del juicio
    string tTest[5], eAb1[5], eAb2[5], acus[5];
    for (int i = 0; i < 5; i++) {
      string nom, ctx, coa, img;
      getline(in, nom);
      getline(in, ctx);
      getline(in, coa);
      getline(in, tTest[i]);
      getline(in, eAb1[i]);
      getline(in, eAb2[i]);
      getline(in, acus[i]);
      getline(in, img);

      if (!nom.empty())
        nuevo->individuos[i].nombre = nom;
      if (!ctx.empty())
        nuevo->individuos[i].contextoEscena = ctx;
      if (!coa.empty())
        nuevo->individuos[i].coartada = coa;
      nuevo->individuos[i].esCulpable = (i == idxCulp);
      nuevo->individuos[i].rolNoche = 0;
      nuevo->individuos[i].spriteId = i + 1;
      nuevo->individuos[i].imagen = img;
    }

    nuevo->configurarArgumentosJuzgadoPorDefecto();
    for (int i = 0; i < 5; i++) {
      if (!tTest[i].empty())
        nuevo->individuos[i].testimonioTestigos = tTest[i];
      if (!eAb1[i].empty())
        nuevo->individuos[i].evidenciaAbogado1 = eAb1[i];
      if (!eAb2[i].empty())
        nuevo->individuos[i].evidenciaAbogado2 = eAb2[i];
      if (!acus[i].empty())
        nuevo->individuos[i].acusacionCruzada = acus[i];
    }

    // Consumir marca de cierre
    getline(in, linea);

    colaCasos.insertarNodoConPrioridad(nuevo);
    cargados++;
  }

  in.close();
  return cargados;
}

string obtenerEstadoJSON() {
  stringstream ss;
  ss << "{";
  ss << "\"mensaje\":\"" << escaparJSON(ultimoMensaje) << "\",";
  ss << "\"estadoAlerta\":\"" << escaparJSON(estadoAlerta) << "\",";

  // Detectives
  ss << "\"detectives\":[";
  for (int i = 0; i < 4; i++) {
    ss << "{\"id\":" << i << ",\"nombre\":\""
       << escaparJSON(detectives[i].nombre) << "\","
       << "\"disponible\":" << (detectives[i].disponible ? "true" : "false")
       << ",\"resueltos\":" << detectives[i].casosResueltos << "}"
       << (i < 3 ? "," : "");
  }
  ss << "],";

  // Caso Activo
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

    // Evidencias recolectadas
    ss << "\"evidencias\":[";
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
         << "\"imagen\":\"" << escaparJSON(c->individuos[i].imagen) << "\","
         << "\"rasgos\":[\"" << escaparJSON(c->individuos[i].rasgos[0])
         << "\",\"" << escaparJSON(c->individuos[i].rasgos[1]) << "\",\""
         << escaparJSON(c->individuos[i].rasgos[2]) << "\",\""
         << escaparJSON(c->individuos[i].rasgos[3]) << "\"]}"
         << (i < 4 ? "," : "");
    }
    ss << "],";

    // Cartas del juicio (Dia)
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
             << "\"objetivo\":\"" << escaparJSON(carta->acusadoObjetivo)
             << "\","
             << "\"revelada\":" << (carta->revelada ? "true" : "false") << "}"
             << (idC < 7 ? "," : "");
        }
      }
    }
    ss << "],";

    // Ranking de pruebas por relevancia
    ElementoMonticulo ranking[16];
    int cantRanking = 0;
    c->monticuloPruebas.obtenerRankingOrdenado(ranking, cantRanking);
    ss << "\"rankingPruebas\":[";
    for (int r = 0; r < cantRanking; r++) {
      ss << "{\"posicion\":" << (r + 1) << ","
         << "\"idCarta\":" << ranking[r].idCarta << ","
         << "\"numeroEtapa\":" << ranking[r].numeroEtapa << ","
         << "\"autor\":\"" << escaparJSON(ranking[r].autor) << "\","
         << "\"titulo\":\"" << escaparJSON(ranking[r].titulo) << "\","
         << "\"detalle\":\"" << escaparJSON(ranking[r].detalle) << "\","
         << "\"objetivo\":\"" << escaparJSON(ranking[r].acusadoObjetivo)
         << "\","
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

void procesarCreacionCasoPersonalizado(const string &payload) {
  string titulo = obtenerParametro(payload, "titulo");
  if (titulo.empty())
    titulo = "Expediente Personalizado #" + to_string(contadorCasos);

  string prioStr = obtenerParametro(payload, "prioridad");
  int prio = 1;
  if (prioStr == "2")
    prio = 2;
  else if (prioStr == "3")
    prio = 3;

  string culpStr = obtenerParametro(payload, "culpable");
  int idxCulp = 0;
  if (!culpStr.empty()) {
    int val = atoi(culpStr.c_str());
    if (val >= 0 && val < 5)
      idxCulp = val;
  }

  NodoCaso *nuevo = new NodoCaso(contadorCasos++, titulo, prio);
  nuevo->indiceCulpable = idxCulp;

  for (int i = 0; i < 5; i++) {
    string idxS = to_string(i);
    string nom = obtenerParametro(payload, "nom" + idxS);
    string ctx = obtenerParametro(payload, "ctx" + idxS);
    string coa = obtenerParametro(payload, "coa" + idxS);
    string img = obtenerParametro(payload, "img" + idxS);

    if (!nom.empty())
      nuevo->individuos[i].nombre = nom;
    if (!ctx.empty())
      nuevo->individuos[i].contextoEscena = ctx;
    if (!coa.empty())
      nuevo->individuos[i].coartada = coa;
    if (!img.empty())
      nuevo->individuos[i].imagen = img;

    nuevo->individuos[i].esCulpable = (i == idxCulp);
    nuevo->individuos[i].rolNoche = 0;
    nuevo->individuos[i].spriteId = i + 1;
  }

  for (int k = 0; k < 2; k++) {
    string pStr = obtenerParametro(payload, "pista" + to_string(k));
    if (!pStr.empty()) {
      nuevo->pistasPersonalizadas[k] = pStr;
    } else {
      nuevo->pistasPersonalizadas[k] =
          "Pista forense #" + to_string(k + 1) + ": incrimina a " +
          nuevo->individuos[idxCulp].nombre + " (" +
          nuevo->individuos[idxCulp].contextoEscena + ").";
    }
  }

  nuevo->configurarArgumentosJuzgadoPorDefecto();
  for (int i = 0; i < 5; i++) {
    string idxS = to_string(i);
    string tTest = obtenerParametro(payload, "test" + idxS);
    string eAb1 = obtenerParametro(payload, "ab1_" + idxS);
    string eAb2 = obtenerParametro(payload, "ab2_" + idxS);
    string acus = obtenerParametro(payload, "acu" + idxS);

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
  guardarCasoEnArchivo(nuevo);
  estadoAlerta = "NORMAL";
  ultimoMensaje = "Caso '" + titulo +
                  "' guardado permanentemente y agregado al juego (urgencia " +
                  to_string(prio) + ").";
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

  // 1. Cargar primero los casos personalizados guardados permanentemente
  int guardados = cargarCasosGuardados();

  // 2. Cargar los 3 casos base predefinidos
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
  address.sin_port = htons(8081);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    cerr << "Error en bind: El puerto 8081 ya esta en uso." << endl;
    CLOSESOCKET(server_fd);
    return 1;
  }

  listen(server_fd, 10);

  cout << "=========================================================" << endl;
  cout << " SERVIDOR DEL JUEGO ACTIVO EN: http://localhost:8081" << endl;
  cout << " Casos personalizados cargados de disco: " << guardados << endl;
  cout << " Abre index.html en tu navegador para iniciar la partida." << endl;
  cout << "=========================================================" << endl;

  while (true) {
    SocketType client = accept(server_fd, NULL, NULL);
    if (client == INVALID_SOCKET)
      continue;

    string req = leerPeticionHTTP(client);
    if (req.empty()) {
      CLOSESOCKET(client);
      continue;
    }

    // Responder preflight CORS si el navegador envia OPTIONS
    if (req.find("OPTIONS ") == 0) {
      string optResp = "HTTP/1.1 204 No Content\r\n"
                       "Access-Control-Allow-Origin: *\r\n"
                       "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                       "Access-Control-Allow-Headers: Content-Type\r\n"
                       "Connection: close\r\n\r\n";
      send(client, optResp.c_str(), static_cast<int>(optResp.length()), 0);
      CLOSESOCKET(client);
      continue;
    }

    estadoAlerta = "NORMAL";

    // Extraer primera linea y cuerpo POST (si existe)
    size_t finLinea = req.find("\r\n");
    string primeraLinea =
        (finLinea != string::npos) ? req.substr(0, finLinea) : req;
    size_t sepCuerpo = req.find("\r\n\r\n");
    string cuerpoPost =
        (sepCuerpo != string::npos) ? req.substr(sepCuerpo + 4) : "";

    // 1. Asignar detective al caso activo
    if (primeraLinea.find("GET /asignar?") != string::npos &&
        colaCasos.cabeza != NULL) {
      int idDet = atoi(obtenerParametro(primeraLinea, "id").c_str());
      if (colaCasos.cabeza->idDetectiveAsignado != -1) {
        ultimoMensaje = "El caso actual ya tiene un detective asignado.";
      } else if (idDet >= 0 && idDet < 4 && detectives[idDet].disponible) {
        colaCasos.cabeza->idDetectiveAsignado = idDet;
        detectives[idDet].disponible = false;
        ultimoMensaje = "Detective " + detectives[idDet].nombre +
                        " asignado al Caso #" +
                        to_string(colaCasos.cabeza->idCaso) +
                        ". Recolecta evidencias y elige 2 sospechosos.";
      } else {
        ultimoMensaje = "El detective seleccionado no esta disponible.";
      }
    }
    // 2. Recolectar nueva pista forense (Noche)
    else if (primeraLinea.find("GET /evidencia") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      if (c->solicitarNuevaEvidencia()) {
        ultimoMensaje = "Nueva evidencia recolectada en la escena del crimen.";
      } else {
        ultimoMensaje =
            "Ya se recolectaron las 2 evidencias disponibles para este caso.";
      }
    }
    // 3. Inspeccionar individuo en la Noche
    else if (primeraLinea.find("GET /inspeccionar?") != string::npos &&
             colaCasos.cabeza != NULL) {
      int idx = atoi(obtenerParametro(primeraLinea, "idx").c_str());
      if (idx >= 0 && idx < 5) {
        colaCasos.cabeza->individuoInspeccionado = idx;
        ultimoMensaje = "Inspeccionando ficha de " +
                        colaCasos.cabeza->individuos[idx].nombre + ".";
      }
    }
    // 4. Alternar individuo entre Testigo y Sospechoso (maximo 2)
    else if (primeraLinea.find("GET /alternar_sospechoso?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idx = atoi(obtenerParametro(primeraLinea, "idx").c_str());
      if (idx >= 0 && idx < 5 && c->faseActual == "NOCHE") {
        c->individuoInspeccionado = idx;
        if (c->individuos[idx].rolNoche == 1) {
          c->individuos[idx].rolNoche = 0;
          ultimoMensaje =
              c->individuos[idx].nombre + " ahora figura como Testigo.";
        } else {
          int conteoSosp = 0;
          for (int i = 0; i < 5; i++) {
            if (c->individuos[i].rolNoche == 1)
              conteoSosp++;
          }
          if (conteoSosp >= 2) {
            estadoAlerta = "ERROR_VALIDACION";
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
    // 5. Transicion de la Noche al Dia
    else if (primeraLinea.find("GET /pasar_dia") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      if (c->idDetectiveAsignado == -1) {
        estadoAlerta = "ERROR_VALIDACION";
        ultimoMensaje =
            "Asigna primero un detective antes de pasar a la audiencia.";
      } else if (c->evidenciasMostradas == 0) {
        estadoAlerta = "ERROR_VALIDACION";
        ultimoMensaje = "Recolecta al menos una pista antes de enviar el caso "
                        "al juzgado.";
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
          estadoAlerta = "ERROR_VALIDACION";
          ultimoMensaje = "Debes escoger exactamente 2 sospechosos (los otros "
                          "3 pasaran a ser testigos).";
        } else {
          bool culpableCapturado =
              (c->individuos[sA].esCulpable || c->individuos[sB].esCulpable);
          if (!culpableCapturado) {
            estadoAlerta = "FALLO_NOCHE";
            ultimoMensaje =
                "No lograste hallar al culpable entre tus 2 sospechosos (" +
                c->individuos[sA].nombre + " y " + c->individuos[sB].nombre +
                "). El verdadero culpable quedo entre los testigos. Repite la "
                "investigacion nocturna.";
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
            c->tituloModalPrueba = "Audiencia Iniciada - Fase 1";
            c->textoModalPrueba =
                "Haz clic en las cartas amarillas (1) para escuchar el "
                "testimonio de los 3 testigos (" +
                nombresTestigos + ") hacia cada acusado.";
            ultimoMensaje = "Audiencia en el juzgado iniciada. Revisa primero "
                            "las cartas de la Fase 1.";
          }
        }
      }
    }
    // 6. Hacer clic en una de las 8 cartas del juzgado (Dia)
    else if (primeraLinea.find("GET /clic_carta?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idCarta = atoi(obtenerParametro(primeraLinea, "id").c_str());
      if (c->faseActual == "DIA" && c->arbolJuzgado.raiz != NULL) {
        NodoArbolJuicio *carta =
            c->arbolJuzgado.buscarCarta(c->arbolJuzgado.raiz, idCarta);
        if (carta != NULL) {
          int etapaActual = c->turnosJuzgado.etapaActual();
          if (carta->numeroEtapa > etapaActual && !carta->revelada) {
            estadoAlerta = "ERROR_VALIDACION";
            ultimoMensaje = "Sigue el orden del juicio: actualmente estas en "
                            "la Fase " +
                            to_string(etapaActual) +
                            ". Revisa primero las cartas de esta fase.";
          } else {
            carta->revelada = true;
            c->actorAnimado = carta->grupo;
            c->tituloModalPrueba =
                "[" + to_string(carta->numeroEtapa) + "] " + carta->tituloCarta;
            c->textoModalPrueba = carta->detalle + " (Relevancia probatoria: " +
                                  to_string(carta->pesoIncriminatorio) +
                                  " pts)";

            c->monticuloPruebas.insertar(carta);

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
              ultimoMensaje = "Prueba registrada. La audiencia avanza a la "
                              "Fase " +
                              to_string(c->turnosJuzgado.etapaActual()) + ".";
            } else {
              ultimoMensaje = "Prueba de " + carta->autor +
                              " revisada y clasificada por relevancia.";
            }
          }
        }
      }
    }
    // 7. Emitir veredicto final como Juez en el Dia (con autoregeneracion si se
    // agotan casos)
    else if (primeraLinea.find("GET /veredicto?") != string::npos &&
             colaCasos.cabeza != NULL) {
      NodoCaso *c = colaCasos.cabeza;
      int idxElegido = atoi(obtenerParametro(primeraLinea, "idx").c_str());
      if (c->faseActual == "DIA" && (idxElegido == c->idxSospechosoA ||
                                     idxElegido == c->idxSospechosoB)) {
        if (c->monticuloPruebas.tamano == 0) {
          estadoAlerta = "ERROR_VALIDACION";
          ultimoMensaje = "Debes abrir al menos una prueba en la mesa del "
                          "juzgado antes de declarar un culpable.";
        } else {
          int detIdx = c->idDetectiveAsignado;
          string nombreElegido = c->individuos[idxElegido].nombre;
          string nombreCulpable = c->individuos[c->indiceCulpable].nombre;

          if (c->individuos[idxElegido].esCulpable) {
            if (detIdx >= 0 && detIdx < 4)
              detectives[detIdx].casosResueltos++;
            estadoAlerta = "VEREDICTO_OK";
            ultimoMensaje = "VEREDICTO CORRECTO: " + nombreElegido +
                            " era el verdadero culpable. El expediente ha sido "
                            "cerrado exitosamente.";
          } else {
            estadoAlerta = "VEREDICTO_FAIL";
            ultimoMensaje = "VEREDICTO ERRONEO: Condenaste a " + nombreElegido +
                            ", pero el verdadero culpable era " +
                            nombreCulpable +
                            ". El caso se cierra y pasamos al siguiente "
                            "expediente.";
          }
          if (detIdx >= 0 && detIdx < 4)
            detectives[detIdx].disponible = true;
          colaCasos.eliminarPrimero();

          // Correccion: generar automaticamente un nuevo caso si se terminaron
          // todos
          if (colaCasos.cabeza == NULL) {
            string delitos[] = {"Robo en la Galeria de Arte",
                                "Sabotaje en el Laboratorio Central",
                                "Desaparicion en el Hotel Mirador",
                                "Incidente en la Estacion Norte"};
            int prioAuto = (rand() % 3) + 1;
            colaCasos.insertarConPrioridad(contadorCasos++, delitos[rand() % 4],
                                           prioAuto);
          }
        }
      }
    }
    // 8. Crear y guardar permanentemente un nuevo caso (POST o GET)
    else if (primeraLinea.find("POST /crear_caso") != string::npos) {
      procesarCreacionCasoPersonalizado(cuerpoPost);
    } else if (primeraLinea.find("GET /crear_caso?") != string::npos) {
      procesarCreacionCasoPersonalizado(primeraLinea);
    }

    string json = obtenerEstadoJSON();
    string resp = "HTTP/1.1 200 OK\r\n"
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
