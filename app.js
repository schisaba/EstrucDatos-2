const API = "http://localhost:8080";
let estadoGlobal = null;

// Inicializar los campos del formulario para crear casos personalizados
function inicializarFormularioCasos() {
    const contenedor = document.getElementById("campos-individuos-form");
    contenedor.innerHTML = "";
    for (let i = 0; i < 5; i++) {
        contenedor.innerHTML += `
        <div class="fila-form-individuo">
            <strong>Individuo #${i + 1} (Sprite Fijo #${i + 1})</strong>
            <div class="grid-form-2">
                <input type="text" id="nc-nom${i}" required placeholder="Nombre del Individuo #${i + 1}">
                <input type="text" id="nc-ctx${i}" required placeholder="Contexto en la escena nocturna">
            </div>
            <div class="grid-form-2">
                <input type="text" id="nc-test${i}" placeholder="(Opcional Día) Contexto de los testigos [Carta 1]">
                <input type="text" id="nc-acu${i}" placeholder="(Opcional Día) Acusación hacia el otro [Carta 3]">
            </div>
            <div class="grid-form-2">
                <input type="text" id="nc-ab1_${i}" placeholder="(Opcional Día) Evidencia #1 del Abogado [Carta 2]">
                <input type="text" id="nc-ab2_${i}" placeholder="(Opcional Día) Evidencia #2 del Abogado [Carta 2]">
            </div>
        </div>`;
    }
}

async function llamarCPlusPlus(ruta = "/") {
    try {
        const res = await fetch(API + ruta);
        const data = await res.json();
        estadoGlobal = data;
        renderizarInterfaz(data);
    } catch (e) {
        document.getElementById("banner-mensaje").innerText =
            "⚠️ Ejecuta primero './servidor' (Linux) o 'servidor.exe' (Windows) en tu terminal para conectar con C++.";
    }
}

function renderizarInterfaz(data) {
    document.getElementById("banner-mensaje").innerText = data.mensaje;

    // Mostrar alerta emergente si fallo la transicion Noche -> Dia o se emitio veredicto
    if (data.estadoAlerta === "FALLO_NOCHE") {
        mostrarModalInfo("❌ Fallo al Pasar al Día (Culpable no identificado)", data.mensaje);
    } else if (data.estadoAlerta === "VEREDICTO_OK") {
        mostrarModalInfo("⚖️ ¡Veredicto Judicial Correcto!", data.mensaje);
    } else if (data.estadoAlerta === "VEREDICTO_FAIL") {
        mostrarModalInfo("⚖️ Veredicto Judicial Erróneo", data.mensaje);
    }

    // 1. Renderizar Cola con Prioridad
    const divCasos = document.getElementById("lista-casos");
    divCasos.innerHTML = data.casos.length === 0
        ? "<p style='font-size:0.82rem; color:#94a3b8;'>No hay casos pendientes. Usa '➕ Ingresar Nuevo Caso' para agregar uno.</p>"
        : "";

    data.casos.forEach((c, idx) => {
        const textoPrio = c.prioridad === 1 ? "Alta (1)" : c.prioridad === 2 ? "Media (2)" : "Baja (3)";
        divCasos.innerHTML += `
        <div class="card-caso ${idx === 0 ? 'activo' : ''}">
            <div style="display:flex; justify-content:space-between; align-items:center;">
                <strong>#${c.id} - ${c.descripcion}</strong>
                <span class="badge prio-${c.prioridad}">${textoPrio}</span>
            </div>
            <div style="margin-top:5px; color:#cbd5e1;">
                Agente: <b>${c.detective}</b> | Fase: <b>${c.fase}</b> | Pistas: <b>${c.evidencias}/4</b>
            </div>
        </div>`;
    });

    // 2. Renderizar Plantilla de Detectives (Arreglo Fijo)
    const divDet = document.getElementById("lista-detectives");
    divDet.innerHTML = "";
    data.detectives.forEach((d) => {
        divDet.innerHTML += `
        <div class="card-det" onclick="llamarCPlusPlus('/asignar?id=${d.id}')">
            <strong>#${d.id + 1} ${d.nombre}</strong>
            <div style="color:${d.disponible ? '#4ade80' : '#f87171'}; font-weight:700; margin-top:2px;">
                ● ${d.disponible ? 'Disponible' : 'En Caso'}
            </div>
            <div style="color:#94a3b8; font-size:0.74rem; margin-top:2px;">Resueltos: ${d.resueltos}</div>
        </div>`;
    });

    const vistaNoche = document.getElementById("vista-noche");
    const vistaDia = document.getElementById("vista-dia");
    const badgeInstancia = document.getElementById("badge-instancia");

    if (!data.casoActivo) {
        vistaNoche.classList.remove("oculto");
        vistaDia.classList.add("oculto");
        document.getElementById("info-expediente-noche").innerHTML =
            "<h3>Sin casos activos en la Cola con Prioridad</h3><p>Haz clic en '➕ Ingresar Nuevo Caso' en la esquina superior derecha para crear un expediente.</p>";
        document.getElementById("lista-individuos-noche").innerHTML = "";
        document.getElementById("pila-evidencias").innerHTML = "";
        return;
    }

    const c = data.casoActivo;
    if (c.faseActual === "NOCHE") {
        vistaNoche.classList.remove("oculto");
        vistaDia.classList.add("oculto");
        badgeInstancia.className = "badge-instancia modo-noche";
        badgeInstancia.innerText = "🌙 INSTANCIA DE NOCHE: INVESTIGADOR";
        renderizarEscenarioNoche(c, data.detectives);
    } else {
        vistaNoche.classList.add("oculto");
        vistaDia.classList.remove("oculto");
        badgeInstancia.className = "badge-instancia modo-dia";
        badgeInstancia.innerText = "☀️ INSTANCIA DE DÍA: JUEZ PENAL";
        renderizarEscenarioDia(c);
    }
}

function renderizarEscenarioNoche(c, detectives) {
    const detNombre = c.idDetective === -1 ? "Sin asignar (Haz clic en un detective arriba)" : detectives[c.idDetective].nombre;
    document.getElementById("info-expediente-noche").innerHTML = `
        <h3>Expediente #${c.id}: ${c.descripcion}</h3>
        <p style="margin-top:4px; color:#334155;">
            <b>Detective a cargo:</b> ${detNombre} &nbsp;|&nbsp;
            <b>Evidencias en la Pila:</b> ${c.evidenciasMostradas}/4
        </p>
        <p style="margin-top:8px; font-size:0.84rem; color:#475569;">
            💡 <b>Instrucción:</b> Haz clic en cada individuo para ver su foto y rasgos, apila pistas en la sección derecha y marca a <b>2 como Sospechosos</b> antes de pasar al Día.
        </p>`;

    // Contar sospechosos y testigos
    let cantSosp = 0;
    c.individuos.forEach(ind => { if (ind.rolNoche === 1) cantSosp++; });
    document.getElementById("contador-seleccion-noche").innerText =
        `Sospechosos elegidos: ${cantSosp}/2 | Testigos restantes: ${5 - cantSosp}`;

    // Renderizar el retrato del individuo inspeccionado arriba a la derecha
    const indActivo = c.individuos[c.individuoInspeccionado] || c.individuos[0];
    const retratoDiv = document.getElementById("retrato-inspeccionado");
    retratoDiv.style.background = indActivo.rolNoche === 1 ? "#fdba74" : "#fef08a";
    retratoDiv.innerHTML = `
        <img src="sprites/personaje_${indActivo.spriteId}.png" alt=""
             style="max-width:100%; max-height:100%; object-fit:contain;"
             onerror="this.style.display='none'; this.nextElementSibling.style.display='flex';">
        <div class="figura-humana" style="display:flex;">
            <div class="cabeza-humana" style="width:36px; height:36px;"></div>
            <div class="cuerpo-humano" style="width:52px; height:44px;"></div>
        </div>`;

    document.getElementById("datos-inspeccionado").innerHTML = `
        <strong>${indActivo.nombre} (${indActivo.rolNoche === 1 ? 'Sospechoso' : 'Testigo'})</strong>
        <div style="font-size:0.75rem; color:#475569; margin-top:3px; font-style:italic;">"${indActivo.coartada}"</div>
        <div style="margin-top:5px; font-size:0.74rem; color:#0f172a;">
            <b>Rasgos:</b> ${indActivo.rasgos.join(" • ")}
        </div>`;

    // Renderizar los 5 individuos con su Contexto (Boceto 1)
    const listaInd = document.getElementById("lista-individuos-noche");
    listaInd.innerHTML = "";
    c.individuos.forEach((ind, i) => {
        const esInspeccionado = (i === c.individuoInspeccionado);
        const esSosp = (ind.rolNoche === 1);
        listaInd.innerHTML += `
        <div class="fila-individuo-boceto ${esInspeccionado ? 'inspeccionado' : ''}"
             onclick="llamarCPlusPlus('/inspeccionar?idx=${i}')">
            <div class="flecha-azul">${esInspeccionado ? '➔' : ''}</div>
            <div class="icono-peon ${esSosp ? 'peon-sospechoso' : 'peon-testigo'}">
                <div class="peon-cabeza"></div>
                <div class="peon-cuerpo"></div>
            </div>
            <div class="texto-contexto-ind">
                <strong>${ind.nombre}</strong>
                <p><b>Contexto:</b> ${ind.contexto}</p>
            </div>
            <button class="btn-rol-noche ${esSosp ? 'es-sospechoso' : 'es-testigo'}"
                    onclick="event.stopPropagation(); llamarCPlusPlus('/alternar_sospechoso?idx=${i}')">
                ${esSosp ? '🟧 Sospechoso' : '🟨 Testigo'}
            </button>
        </div>`;
    });

    // Renderizar las figuras geometricas de Pistas (Rectangulo, Rombo, Cuadrado, Reporte)
    // Nota: en la Pila el elemento mas reciente esta en el indice 0, por lo que invertimos para mapear 1..4
    const pistasOrdenCronologico = [...c.pilaEvidencias].reverse();
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
                 onclick="mostrarModalInfo('🔍 ${tituloEsc}', '${descEsc}')">
                <span>${pista.titulo}<br><small>(Ver)</small></span>
            </div>`;
        } else {
            divFiguras.innerHTML += `
            <div class="figura-pista ${f.clase}">
                <span>${f.etiqueta}<br><small>(Bloqueada)</small></span>
            </div>`;
        }
    });

    // Renderizar nodos de la Pila (desde el Tope)
    const divPila = document.getElementById("pila-evidencias");
    divPila.innerHTML = c.pilaEvidencias.length === 0
        ? "<p style='font-size:0.8rem; color:#64748b; text-align:center;'>Pila vacía. Recolecta al menos una pista para habilitar el paso al Juzgado.</p>"
        : "";

    c.pilaEvidencias.forEach((p, idx) => {
        const tituloEsc = p.titulo.replace(/'/g, "\\'");
        const descEsc = p.descripcion.replace(/'/g, "\\'");
        divPila.innerHTML += `
        <div class="nodo-pila-noche" onclick="mostrarModalInfo('🔍 ${tituloEsc}', '${descEsc}')">
            <strong>${idx === 0 ? '[TOPE DE LA PILA]' : `Pista Apilada`} - ${p.titulo}:</strong> ${p.descripcion}
        </div>`;
    });
}

function renderizarEscenarioDia(c) {
    document.getElementById("indicador-turno-dia").innerText =
        `Cola Circular: ${c.descripcionTurno}`;

    const sospA = c.individuos[c.idxSospechosoA];
    const sospB = c.individuos[c.idxSospechosoB];
    const nombresTestigos = c.individuos
        .filter((_, idx) => idx !== c.idxSospechosoA && idx !== c.idxSospechosoB)
        .map(t => t.nombre)
        .join(", ");

    // Etiquetas de los personajes en el estrado
    document.getElementById("lbl-sosp-a").innerText = sospA ? sospA.nombre : "Sospechoso A";
    document.getElementById("lbl-abog-a").innerText = sospA ? `Abogado de ${sospA.nombre.split(" ")[0]}` : "Abogado A";
    document.getElementById("lbl-sosp-b").innerText = sospB ? sospB.nombre : "Sospechoso B";
    document.getElementById("lbl-abog-b").innerText = sospB ? `Abogado de ${sospB.nombre.split(" ")[0]}` : "Abogado B";
    document.getElementById("lbl-nombres-testigos").innerText = nombresTestigos;

    // Botones de veredicto
    document.getElementById("btn-condenar-a").innerText = `⚖️ Declarar Culpable a ${sospA ? sospA.nombre : 'Sospechoso A'}`;
    document.getElementById("btn-condenar-b").innerText = `⚖️ Declarar Culpable a ${sospB ? sospB.nombre : 'Sospechoso B'}`;

    // Animacion de movimiento al frente para el personaje activo
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

    // Actualizar el visor de la prueba activa en la mesa
    if (c.tituloModalPrueba) {
        document.getElementById("titulo-prueba-activa").innerText = c.tituloModalPrueba;
        document.getElementById("texto-prueba-activa").innerText = c.textoModalPrueba;
    }

    // Renderizar las 8 cartas en la mesa en el orden del Boceto 2: [3, 2, 2, 1, 1, 2, 2, 3]
    const divCartas = document.getElementById("contenedor-cartas-mesa");
    divCartas.innerHTML = "";

    const coloresPorCarta = [
        "carta-color-naranja",  // 0: Acusacion Sosp A (3)
        "carta-color-azul",     // 1: Evidencia 1 Abogado A (2)
        "carta-color-azul",     // 2: Evidencia 2 Abogado A (2)
        "carta-color-amarillo", // 3: Testigos hacia A (1)
        "carta-color-amarillo", // 4: Testigos hacia B (1)
        "carta-color-rosado",   // 5: Evidencia 1 Abogado B (2)
        "carta-color-rosado",   // 6: Evidencia 2 Abogado B (2)
        "carta-color-naranja"   // 7: Acusacion Sosp B (3)
    ];

    c.cartasMesa.forEach((carta, idx) => {
        const disponible = (carta.numeroEtapa <= c.etapaTurno) || carta.revelada;
        const claseEstado = carta.revelada ? "revelada" : (!disponible ? "bloqueada" : "");
        const textoEstado = carta.revelada
            ? "✔ Inspeccionada"
            : (disponible ? "🔓 Clic para abrir" : `🔒 Espera Etapa ${carta.numeroEtapa}`);

        divCartas.innerHTML += `
        <div class="carta-juicio ${coloresPorCarta[idx]} ${claseEstado}"
             onclick="clicEnCartaJuicio(${carta.idCarta})">
            <div class="texto-superior-carta">${carta.titulo}</div>
            <div class="numero-grande-carta">${carta.numeroEtapa}</div>
            <div class="estado-carta-tag">${textoEstado}</div>
        </div>`;
    });

    // Renderizar el Ranking del Montículo Binario (Opción B)
    const divMonticulo = document.getElementById("lista-monticulo");
    if (!c.monticuloRanking || c.monticuloRanking.length === 0) {
        divMonticulo.innerHTML =
            "<p style='font-size:0.82rem; color:#94a3b8;'>Aún no has inspeccionado cartas en la mesa. Haz clic en las cartas amarillas (1) para insertarlas en el Montículo Binario.</p>";
    } else {
        divMonticulo.innerHTML = "";
        c.monticuloRanking.forEach((item, idx) => {
            divMonticulo.innerHTML += `
            <div class="nodo-monticulo ${idx === 0 ? 'raiz-heap' : ''}">
                <div style="display:flex; justify-content:space-between; align-items:center;">
                    <strong>${idx === 0 ? '👑 [RAÍZ DEL MONTÍCULO]' : `#${item.posicion}`} - Etapa ${item.numeroEtapa}</strong>
                    <span class="badge prio-1">Peso: ${item.peso} pts</span>
                </div>
                <div style="color:#93c5fd; font-weight:700; margin-top:4px;">${item.titulo}</div>
                <p style="color:#cbd5e1; margin-top:4px;">${item.detalle}</p>
            </div>`;
        });
    }
}

async function clicEnCartaJuicio(idCarta) {
    await llamarCPlusPlus(`/clic_carta?id=${idCarta}`);
    if (estadoGlobal && estadoGlobal.casoActivo && estadoGlobal.casoActivo.tituloModalPrueba) {
        const c = estadoGlobal.casoActivo;
        // Mostrar ventana emergente con la informacion de la prueba al darle clic
        const cartaSeleccionada = c.cartasMesa.find(x => x.idCarta === idCarta);
        if (cartaSeleccionada && cartaSeleccionada.revelada) {
            mostrarModalInfo(c.tituloModalPrueba, c.textoModalPrueba);
        }
    }
}

function solicitarEvidencia() {
    llamarCPlusPlus("/evidencia");
}

function pasarAlDia() {
    llamarCPlusPlus("/pasar_dia");
}

function emitirVeredicto(lado) {
    if (!estadoGlobal || !estadoGlobal.casoActivo) return;
    const c = estadoGlobal.casoActivo;
    const idxElegido = (lado === 'A') ? c.idxSospechosoA : c.idxSospechosoB;
    llamarCPlusPlus(`/veredicto?idx=${idxElegido}`);
}

// Funciones de Modales
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
    document.getElementById("nc-titulo").value = "Robo del Códice en el Museo Universitario";
    document.getElementById("nc-prioridad").value = "1";
    document.getElementById("nc-culpable").value = "2"; // Individuo #3 culpable

    const ejemplos = [
        { nom: "Andrés Villamizar", ctx: "Revisaba las vitrinas del ala norte con guantes blancos." },
        { nom: "Laura Peñuela", ctx: "Apagó la alarma secundaria argumentando un falso cortocircuito." },
        { nom: "Camilo Cifuentes", ctx: "Salía del depósito con un tubo portaplanos oculto bajo el abrigo." },
        { nom: "Diana Monroy", ctx: "Tomaba fotografías del pasillo restringido sin autorización." },
        { nom: "Felipe Duarte", ctx: "Discutía con el guardia de turno en la puerta principal." }
    ];

    ejemplos.forEach((ej, i) => {
        document.getElementById(`nc-nom${i}`).value = ej.nom;
        document.getElementById(`nc-ctx${i}`).value = ej.ctx;
    });

    document.getElementById("nc-pista0").value = "Cámara térmica muestra un objeto cilíndrico saliendo del depósito.";
    document.getElementById("nc-pista1").value = "Se hallaron fibras de abrigo oscuro en la cerradura del códice.";
    document.getElementById("nc-pista2").value = "El registro biométrico incrimina a quien portaba un tubo portaplanos.";
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
        params.append(`test${i}`, document.getElementById(`nc-test${i}`).value);
        params.append(`acu${i}`, document.getElementById(`nc-acu${i}`).value);
        params.append(`ab1_${i}`, document.getElementById(`nc-ab1_${i}`).value);
        params.append(`ab2_${i}`, document.getElementById(`nc-ab2_${i}`).value);
    }

    for (let k = 0; k < 4; k++) {
        params.append(`pista${k}`, document.getElementById(`nc-pista${k}`).value);
    }

    cerrarModalCrearCaso();
    await llamarCPlusPlus(`/crear_caso?${params.toString()}`);
}

inicializarFormularioCasos();
llamarCPlusPlus();
