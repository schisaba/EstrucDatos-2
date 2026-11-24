const API = "http://localhost:8081";
let estadoGlobal = null;
let imagenesSubidas = ["", "", "", "", ""];

function obtenerRutaImagenIndividuo(ind) {
    if (ind && ind.imagen && ind.imagen.trim() !== "") {
        return ind.imagen;
    }
    const idSprite = (ind && ind.spriteId) ? ind.spriteId : 1;
    return `sprites/personaje_${idSprite}.png`;
}

function manejarErrorImagen(imgEl) {
    const intento = imgEl.getAttribute("data-intento") || "0";
    const actual = imgEl.getAttribute("src") || "";
    if (intento === "0" && actual.endsWith(".png")) {
        imgEl.setAttribute("data-intento", "1");
        imgEl.src = actual.replace(/\.png$/, ".jpg");
        return;
    }
    imgEl.style.display = "none";
    if (imgEl.nextElementSibling) {
        imgEl.nextElementSibling.style.display = "flex";
    }
}

function crearHtmlAvatar(src, claseFallback = "figura-humana") {
    return `
        <img src="${src}" alt="" data-intento="0" onerror="manejarErrorImagen(this)">
        <div class="${claseFallback}" style="display:none;">
            <div class="cabeza-humana"></div>
            <div class="cuerpo-humano"></div>
        </div>`;
}

function inicializarFormularioCasos() {
    const contenedor = document.getElementById("campos-individuos-form");
    contenedor.innerHTML = "";
    imagenesSubidas = ["", "", "", "", ""];

    for (let i = 0; i < 5; i++) {
        contenedor.innerHTML += `
        <div class="fila-form-individuo">
            <div class="cabecera-ind-form">
                <strong>Individuo #${i + 1}</strong>
                <div class="subida-imagen-box">
                    <img id="nc-preview${i}" class="preview-img-form" src="sprites/personaje_${i + 1}.png" alt=""
                         data-intento="0"
                         onerror="if(this.getAttribute('data-intento')==='0'){this.setAttribute('data-intento','1');this.src='sprites/personaje_${i + 1}.jpg';}else{this.style.visibility='hidden';}">
                    <label for="nc-file${i}" style="font-size:0.72rem; color:#93c5fd;">Imagen:</label>
                    <input type="file" id="nc-file${i}" class="input-archivo-img" accept="image/*"
                           onchange="procesarImagenSubida(${i}, this)">
                </div>
            </div>
            <div class="grid-form-2">
                <input type="text" id="nc-nom${i}" required placeholder="Nombre del Individuo #${i + 1}">
                <input type="text" id="nc-ctx${i}" required placeholder="Contexto en la escena nocturna">
            </div>
            <div class="grid-form-2">
                <input type="text" id="nc-coa${i}" placeholder="(Opcional) Coartada del individuo">
                <input type="text" id="nc-test${i}" placeholder="(Opcional Dia) Testimonio de testigos [Carta 1]">
            </div>
            <div class="grid-form-2">
                <input type="text" id="nc-ab1_${i}" placeholder="(Opcional Dia) Evidencia #1 del Abogado [Carta 2]">
                <input type="text" id="nc-ab2_${i}" placeholder="(Opcional Dia) Evidencia #2 del Abogado [Carta 2]">
            </div>
            <div style="margin-top:6px;">
                <input type="text" id="nc-acu${i}" placeholder="(Opcional Dia) Acusacion hacia el otro sospechoso [Carta 3]">
            </div>
        </div>`;
    }
}

function procesarImagenSubida(indice, input) {
    const archivo = input.files && input.files[0];
    if (!archivo) {
        imagenesSubidas[indice] = "";
        return;
    }

    const lector = new FileReader();
    lector.onload = function (ev) {
        const img = new Image();
        img.onload = function () {
            // Redimensionar a 160x160 centrado para guardar ligero y rapido en el servidor
            const tam = 160;
            const canvas = document.createElement("canvas");
            canvas.width = tam;
            canvas.height = tam;
            const ctx = canvas.getContext("2d");

            const lado = Math.min(img.width, img.height);
            const sx = (img.width - lado) / 2;
            const sy = (img.height - lado) / 2;
            ctx.drawImage(img, sx, sy, lado, lado, 0, 0, tam, tam);

            const dataUrl = canvas.toDataURL("image/jpeg", 0.72);
            imagenesSubidas[indice] = dataUrl;

            const preview = document.getElementById(`nc-preview${indice}`);
            if (preview) {
                preview.style.visibility = "visible";
                preview.src = dataUrl;
            }
        };
        img.src = ev.target.result;
    };
    lector.readAsDataURL(archivo);
}

async function llamarServidor(ruta = "/", opciones = {}) {
    try {
        const res = await fetch(API + ruta, opciones);
        const data = await res.json();
        estadoGlobal = data;
        renderizarInterfaz(data);
    } catch (e) {
        document.getElementById("banner-mensaje").innerText =
            "Ejecuta primero el servidor (./servidor en Linux o .\\servidor.exe en Windows) para conectar la interfaz.";
    }
}

function renderizarInterfaz(data) {
    document.getElementById("banner-mensaje").innerText = data.mensaje;

    if (data.estadoAlerta === "FALLO_NOCHE") {
        mostrarModalInfo("Fallo al Pasar al Dia", data.mensaje);
    } else if (data.estadoAlerta === "VEREDICTO_OK") {
        mostrarModalInfo("Veredicto Judicial Correcto", data.mensaje);
    } else if (data.estadoAlerta === "VEREDICTO_FAIL") {
        mostrarModalInfo("Veredicto Judicial Erroneo", data.mensaje);
    } else if (data.estadoAlerta === "ERROR_VALIDACION") {
        mostrarModalInfo("Requisito Faltante", data.mensaje);
    }

    const divDet = document.getElementById("lista-detectives");
    divDet.innerHTML = "";
    data.detectives.forEach((d) => {
        divDet.innerHTML += `
        <div class="card-det" onclick="llamarServidor('/asignar?id=${d.id}')" title="Casos resueltos: ${d.resueltos}">
            <span class="punto-estado ${d.disponible ? 'punto-libre' : 'punto-ocupado'}"></span>
            <strong>${d.nombre}</strong>
            <span style="color:#94a3b8;">(${d.resueltos})</span>
        </div>`;
    });

    const vistaNoche = document.getElementById("vista-noche");
    const vistaDia = document.getElementById("vista-dia");
    const badgeInstancia = document.getElementById("badge-instancia");

    if (!data.casoActivo) {
        vistaNoche.classList.remove("oculto");
        vistaDia.classList.add("oculto");
        document.getElementById("info-expediente-noche").innerHTML =
            "<strong>Sin casos activos.</strong> Usa el boton 'Nuevo Caso' arriba a la derecha para registrar un expediente.";
        document.getElementById("lista-individuos-noche").innerHTML = "";
        document.getElementById("lista-evidencias").innerHTML = "";
        return;
    }

    const c = data.casoActivo;
    if (c.faseActual === "NOCHE") {
        vistaNoche.classList.remove("oculto");
        vistaDia.classList.add("oculto");
        badgeInstancia.className = "badge-instancia modo-noche";
        badgeInstancia.innerText = "NOCHE: INVESTIGADOR";
        renderizarEscenarioNoche(c, data.detectives);
    } else {
        vistaNoche.classList.add("oculto");
        vistaDia.classList.remove("oculto");
        badgeInstancia.className = "badge-instancia modo-dia";
        badgeInstancia.innerText = "DIA: JUEZ PENAL";
        renderizarEscenarioDia(c);
    }
}

function renderizarEscenarioNoche(c, detectives) {
    const detNombre = c.idDetective === -1 ? "Sin asignar (elige uno arriba)" : detectives[c.idDetective].nombre;
    document.getElementById("info-expediente-noche").innerHTML = `
        <strong>Caso #${c.id}: ${c.descripcion}</strong>
        <span class="sub-inline">Detective: ${detNombre} | Pistas: ${c.evidenciasMostradas}/4</span>`;

    let cantSosp = 0;
    c.individuos.forEach(ind => { if (ind.rolNoche === 1) cantSosp++; });
    document.getElementById("contador-seleccion-noche").innerText =
        `Sospechosos: ${cantSosp}/2 | Testigos: ${5 - cantSosp}`;

    // Retrato del individuo inspeccionado arriba a la derecha
    const indActivo = c.individuos[c.individuoInspeccionado] || c.individuos[0];
    const retratoDiv = document.getElementById("retrato-inspeccionado");
    retratoDiv.style.background = indActivo.rolNoche === 1 ? "#fdba74" : "#fef08a";
    retratoDiv.innerHTML = crearHtmlAvatar(obtenerRutaImagenIndividuo(indActivo));

    document.getElementById("datos-inspeccionado").innerHTML = `
        <strong>${indActivo.nombre} (${indActivo.rolNoche === 1 ? 'Sospechoso' : 'Testigo'})</strong>
        <div style="color:#475569; font-style:italic; margin:2px 0;">"${indActivo.coartada}"</div>
        <div><b>Rasgos:</b> ${indActivo.rasgos.join(", ")}</div>`;

    // Los 5 individuos con su avatar e informacion
    const listaInd = document.getElementById("lista-individuos-noche");
    listaInd.innerHTML = "";
    c.individuos.forEach((ind, i) => {
        const esInspeccionado = (i === c.individuoInspeccionado);
        const esSosp = (ind.rolNoche === 1);
        const imgSrc = obtenerRutaImagenIndividuo(ind);
        listaInd.innerHTML += `
        <div class="fila-individuo-boceto ${esInspeccionado ? 'inspeccionado' : ''}"
             onclick="llamarServidor('/inspeccionar?idx=${i}')">
            <div class="flecha-azul">${esInspeccionado ? '&gt;' : ''}</div>
            <div class="icono-peon ${esSosp ? 'peon-sospechoso' : 'peon-testigo'}">
                <img src="${imgSrc}" alt="" data-intento="0" onerror="manejarErrorImagen(this)">
                <div style="display:none; flex-direction:column; align-items:center;">
                    <div class="peon-cabeza"></div>
                    <div class="peon-cuerpo"></div>
                </div>
            </div>
            <div class="texto-contexto-ind">
                <strong>${ind.nombre}</strong>
                <p>Contexto: ${ind.contexto}</p>
            </div>
            <button class="btn-rol-noche ${esSosp ? 'es-sospechoso' : 'es-testigo'}"
                    onclick="event.stopPropagation(); llamarServidor('/alternar_sospechoso?idx=${i}')">
                ${esSosp ? 'Sospechoso' : 'Testigo'}
            </button>
        </div>`;
    });

    // Figuras geometricas de Pistas
    const pistasOrdenCronologico = [...c.evidencias].reverse();
    const formasConfig = [
        { clase: "forma-rectangulo", etiqueta: "Foto #1" },
        { clase: "forma-rombo", etiqueta: "Objeto #2" },
        { clase: "forma-cuadrado", etiqueta: "Reporte #3" },
        { clase: "forma-cuadrado", etiqueta: "Peritaje #4" }
    ];

    const divFiguras = document.getElementById("figuras-pistas");
    divFiguras.innerHTML = "";
    formasConfig.forEach((f, idx) => {
        const pista = pistasOrdenCronologico[idx];
        if (pista) {
            const tituloEsc = pista.titulo.replace(/'/g, "\\'");
            const descEsc = pista.descripcion.replace(/'/g, "\\'");
            divFiguras.innerHTML += `
            <div class="figura-pista ${f.clase} revelada"
                 onclick="mostrarModalInfo('${tituloEsc}', '${descEsc}')">
                <span>${pista.titulo}<br><small>Ver</small></span>
            </div>`;
        } else {
            divFiguras.innerHTML += `
            <div class="figura-pista ${f.clase}">
                <span>${f.etiqueta}<br><small>Pendiente</small></span>
            </div>`;
        }
    });

    const divEvid = document.getElementById("lista-evidencias");
    divEvid.innerHTML = c.evidencias.length === 0
        ? "<p style='font-size:0.74rem; color:#64748b; text-align:center; margin-top:6px;'>Aun no has recolectado pistas. Presiona 'Recolectar Evidencia'.</p>"
        : "";

    c.evidencias.forEach((p, idx) => {
        const tituloEsc = p.titulo.replace(/'/g, "\\'");
        const descEsc = p.descripcion.replace(/'/g, "\\'");
        divEvid.innerHTML += `
        <div class="item-evidencia-noche" onclick="mostrarModalInfo('${tituloEsc}', '${descEsc}')">
            <strong>${idx === 0 ? '[Reciente]' : '[Evidencia]'} ${p.titulo}:</strong> ${p.descripcion}
        </div>`;
    });
}

function renderizarEscenarioDia(c) {
    document.getElementById("titulo-caso-dia").innerText = `Audiencia - Caso #${c.id}: ${c.descripcion}`;
    document.getElementById("indicador-turno-dia").innerText = c.descripcionTurno;

    const sospA = c.individuos[c.idxSospechosoA];
    const sospB = c.individuos[c.idxSospechosoB];
    const testigos = c.individuos.filter((_, idx) => idx !== c.idxSospechosoA && idx !== c.idxSospechosoB);
    const nombresTestigos = testigos.map(t => t.nombre).join(", ");

    // Cargar retratos reales en el estrado (con fallback a silueta si falta el archivo)
    document.getElementById("img-sosp-a").innerHTML = crearHtmlAvatar(obtenerRutaImagenIndividuo(sospA));
    document.getElementById("img-abog-a").innerHTML = crearHtmlAvatar("sprites/abogado_a.png");
    document.getElementById("img-abog-b").innerHTML = crearHtmlAvatar("sprites/abogado_b.png");
    document.getElementById("img-sosp-b").innerHTML = crearHtmlAvatar(obtenerRutaImagenIndividuo(sospB));

    const divTrio = document.getElementById("img-trio-testigos");
    divTrio.innerHTML = "";
    testigos.forEach(t => {
        divTrio.innerHTML += `
        <div class="avatar-testigo-mini" title="${t.nombre}">
            ${crearHtmlAvatar(obtenerRutaImagenIndividuo(t), "figura-humana mini-testigo")}
        </div>`;
    });

    document.getElementById("lbl-sosp-a").innerText = sospA ? sospA.nombre : "Sospechoso A";
    document.getElementById("lbl-abog-a").innerText = sospA ? `Abogado de ${sospA.nombre.split(" ")[0]}` : "Abogado A";
    document.getElementById("lbl-sosp-b").innerText = sospB ? sospB.nombre : "Sospechoso B";
    document.getElementById("lbl-abog-b").innerText = sospB ? `Abogado de ${sospB.nombre.split(" ")[0]}` : "Abogado B";
    document.getElementById("lbl-nombres-testigos").innerText = nombresTestigos;

    document.getElementById("btn-condenar-a").innerText = `Condenar a ${sospA ? sospA.nombre : 'Sospechoso A'}`;
    document.getElementById("btn-condenar-b").innerText = `Condenar a ${sospB ? sospB.nombre : 'Sospechoso B'}`;

    const spSospA = document.getElementById("sprite-sospechoso-a");
    const spAbogA = document.getElementById("sprite-abogado-a");
    const spTest = document.getElementById("sprite-testigos");
    const spAbogB = document.getElementById("sprite-abogado-b");
    const spSospB = document.getElementById("sprite-sospechoso-b");

    [spSospA, spAbogA, spTest, spAbogB, spSospB].forEach(el => el.classList.remove("personaje-al-frente"));

    if (c.actorAnimado === "SOSPECHOSO_A") spSospA.classList.add("personaje-al-frente");
    else if (c.actorAnimado === "ABOGADO_A") spAbogA.classList.add("personaje-al-frente");
    else if (c.actorAnimado === "TESTIGOS" || c.actorAnimado === "TESTIGOS_A" || c.actorAnimado === "TESTIGOS_B") spTest.classList.add("personaje-al-frente");
    else if (c.actorAnimado === "ABOGADO_B") spAbogB.classList.add("personaje-al-frente");
    else if (c.actorAnimado === "SOSPECHOSO_B") spSospB.classList.add("personaje-al-frente");

    if (c.tituloModalPrueba) {
        document.getElementById("titulo-prueba-activa").innerText = c.tituloModalPrueba;
        document.getElementById("texto-prueba-activa").innerText = c.textoModalPrueba;
    }

    const divCartas = document.getElementById("contenedor-cartas-mesa");
    divCartas.innerHTML = "";

    const coloresPorCarta = [
        "carta-color-naranja",
        "carta-color-azul",
        "carta-color-azul",
        "carta-color-amarillo",
        "carta-color-amarillo",
        "carta-color-rosado",
        "carta-color-rosado",
        "carta-color-naranja"
    ];

    c.cartasMesa.forEach((carta, idx) => {
        const disponible = (carta.numeroEtapa <= c.etapaTurno) || carta.revelada;
        const claseEstado = carta.revelada ? "revelada" : (!disponible ? "bloqueada" : "");
        const textoEstado = carta.revelada
            ? "Revisada"
            : (disponible ? "Abrir" : `Fase ${carta.numeroEtapa}`);

        divCartas.innerHTML += `
        <div class="carta-juicio ${coloresPorCarta[idx]} ${claseEstado}"
             onclick="clicEnCartaJuicio(${carta.idCarta})">
            <div class="texto-superior-carta">${carta.titulo}</div>
            <div class="numero-grande-carta">${carta.numeroEtapa}</div>
            <div class="estado-carta-tag">${textoEstado}</div>
        </div>`;
    });

    const divRanking = document.getElementById("lista-ranking-pruebas");
    if (!c.rankingPruebas || c.rankingPruebas.length === 0) {
        divRanking.innerHTML =
            "<p style='font-size:0.74rem; color:#94a3b8;'>Aun no has revisado cartas. Haz clic en las cartas amarillas (1) para comenzar.</p>";
    } else {
        divRanking.innerHTML = "";
        c.rankingPruebas.forEach((item, idx) => {
            divRanking.innerHTML += `
            <div class="item-ranking ${idx === 0 ? 'prueba-principal' : ''}">
                <div style="display:flex; justify-content:space-between; align-items:center;">
                    <strong>${idx === 0 ? '[Prueba Principal]' : `#${item.posicion}`} - Fase ${item.numeroEtapa}</strong>
                    <span class="badge-peso">${item.peso} pts</span>
                </div>
                <div style="color:#93c5fd; font-weight:700; margin-top:2px;">${item.titulo}</div>
                <div style="color:#cbd5e1; margin-top:2px;">${item.detalle}</div>
            </div>`;
        });
    }
}

async function clicEnCartaJuicio(idCarta) {
    await llamarServidor(`/clic_carta?id=${idCarta}`);
    if (estadoGlobal && estadoGlobal.estadoAlerta !== "ERROR_VALIDACION" &&
        estadoGlobal.casoActivo && estadoGlobal.casoActivo.tituloModalPrueba) {
        const c = estadoGlobal.casoActivo;
        const cartaSeleccionada = c.cartasMesa.find(x => x.idCarta === idCarta);
        if (cartaSeleccionada && cartaSeleccionada.revelada) {
            mostrarModalInfo(c.tituloModalPrueba, c.textoModalPrueba);
        }
    }
}

function solicitarEvidencia() {
    llamarServidor("/evidencia");
}

function pasarAlDia() {
    llamarServidor("/pasar_dia");
}

function emitirVeredicto(lado) {
    if (!estadoGlobal || !estadoGlobal.casoActivo) return;
    const c = estadoGlobal.casoActivo;
    const idxElegido = (lado === 'A') ? c.idxSospechosoA : c.idxSospechosoB;
    llamarServidor(`/veredicto?idx=${idxElegido}`);
}

function mostrarModalInfo(titulo, texto) {
    document.getElementById("modal-info-titulo").innerText = titulo;
    document.getElementById("modal-info-texto").innerText = texto;
    document.getElementById("modal-info").classList.remove("oculto");
}

function cerrarModalInfo() {
    document.getElementById("modal-info").classList.add("oculto");
}

function abrirModalCrearCaso() {
    document.getElementById("modal-crear-caso").classList.remove("oculto");
}

function cerrarModalCrearCaso() {
    document.getElementById("modal-crear-caso").classList.add("oculto");
}

function cargarPlantillaEjemplo() {
    document.getElementById("nc-titulo").value = "Robo del Codice en el Museo Universitario";
    document.getElementById("nc-prioridad").value = "1";
    document.getElementById("nc-culpable").value = "2";

    const ejemplos = [
        { nom: "Andres Villamizar", ctx: "Revisaba las vitrinas del ala norte con guantes blancos." },
        { nom: "Laura Penuela", ctx: "Apago la alarma secundaria argumentando un falso cortocircuito." },
        { nom: "Camilo Cifuentes", ctx: "Salia del deposito con un tubo portaplanos oculto bajo el abrigo." },
        { nom: "Diana Monroy", ctx: "Tomaba fotografias del pasillo restringido sin autorizacion." },
        { nom: "Felipe Duarte", ctx: "Discutia con el guardia de turno en la puerta principal." }
    ];

    ejemplos.forEach((ej, i) => {
        document.getElementById(`nc-nom${i}`).value = ej.nom;
        document.getElementById(`nc-ctx${i}`).value = ej.ctx;
    });

    document.getElementById("nc-pista0").value = "Camara termica muestra un objeto cilindrico saliendo del deposito.";
    document.getElementById("nc-pista1").value = "Se hallaron fibras de abrigo oscuro en la cerradura del codice.";
    document.getElementById("nc-pista2").value = "El registro biometrico incrimina a quien portaba un tubo portaplanos.";
    document.getElementById("nc-pista3").value = "Las huellas en la vitrina coinciden con Camilo Cifuentes.";
}

async function enviarNuevoCaso(e) {
    e.preventDefault();
    const params = new URLSearchParams();
    params.append("titulo", document.getElementById("nc-titulo").value);
    params.append("prioridad", document.getElementById("nc-prioridad").value);
    params.append("culpable", document.getElementById("nc-culpable").value);

    for (let i = 0; i < 5; i++) {
        params.append(`nom${i}`, document.getElementById(`nc-nom${i}`).value);
        params.append(`ctx${i}`, document.getElementById(`nc-ctx${i}`).value);
        params.append(`coa${i}`, document.getElementById(`nc-coa${i}`).value);
        params.append(`test${i}`, document.getElementById(`nc-test${i}`).value);
        params.append(`acu${i}`, document.getElementById(`nc-acu${i}`).value);
        params.append(`ab1_${i}`, document.getElementById(`nc-ab1_${i}`).value);
        params.append(`ab2_${i}`, document.getElementById(`nc-ab2_${i}`).value);
        params.append(`img${i}`, imagenesSubidas[i] || "");
    }

    for (let k = 0; k < 4; k++) {
        params.append(`pista${k}`, document.getElementById(`nc-pista${k}`).value);
    }

    cerrarModalCrearCaso();
    await llamarServidor("/crear_caso", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded;charset=UTF-8" },
        body: params.toString()
    });
    document.getElementById("form-nuevo-caso").reset();
    inicializarFormularioCasos();
}

inicializarFormularioCasos();
llamarServidor();
