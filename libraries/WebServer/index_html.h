const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Detector de Gases</title>
</head>

<body>
    <header class="site-header">
        <h1>Detector de Gases</h1>
    </header>

    <div class="contenedor">
        <div id="contenedor-tiempo-de-espera">
            <p id="texto-de-espera">Por favor espere a que el equipo este listo...</p>
        </div>

        <form action="" id="myForm" class="datos-usuario" onsubmit="return validateForm()">
            <fieldset class="contenedor-datos">
                <legend>Punto Inicial</legend>

                <label for="latitud_inicial">Latitud:</label>
                <div class="contenedor-inputs">
                    <input type="number" id="lat_inicial_grad" value="1"
                        onkeyup="latInicialMinMax(); calcularTiempoRecorrido();" onblur="latInicialMinMax();"
                        placeholder="Grados">
                    <input type="number" id="lat_inicial_min" value="1"
                        onkeyup="latInicialMinMax(); calcularTiempoRecorrido();" onblur="latInicialMinMax();"
                        placeholder="Minutos">
                    <input type="number" id="lat_inicial_seg" value="1"
                        onkeyup="latInicialMinMax(); calcularTiempoRecorrido();" onblur="latInicialMinMax()"
                        placeholder="Segundos">
                </div>

                <label for="longitud">Longitud:</label>
                <div class="contenedor-inputs">
                    <input type="number" id="long_inicial_grad" value="1"
                        onkeyup="longInicialMinMax(); calcularTiempoRecorrido();" onblur="longInicialMinMax();"
                        placeholder="Grados">
                    <input type="number" id="long_inicial_min" value="1"
                        onkeyup="longInicialMinMax(); calcularTiempoRecorrido();" onblur="longInicialMinMax();"
                        placeholder="Minutos">
                    <input type="number" id="long_inicial_seg" value="2"
                        onkeyup="longInicialMinMax(); calcularTiempoRecorrido();" onblur="longInicialMinMax();"
                        placeholder="Segundos">
                </div>
            </fieldset>

            <fieldset class="contenedor-datos">
                <legend>Punto Final</legend>

                <label for="latitud">Latitud:</label>
                <div class="contenedor-inputs">
                    <input type="number" id="lat_final_grad" value="1"
                        onkeyup="latFinalMinMax(); calcularTiempoRecorrido();" onblur="latFinalMinMax();"
                        placeholder="Grados">
                    <input type="number" id="lat_final_min" value="1"
                        onkeyup="latFinalMinMax(); calcularTiempoRecorrido();" onblur="latFinalMinMax();"
                        placeholder="Minutos">
                    <input type="number" id="lat_final_seg" value="3"
                        onkeyup="latFinalMinMax(); calcularTiempoRecorrido();" onblur="latFinalMinMax();"
                        placeholder="Segundos">
                </div>

                <label for="longitud">Longitud:</label>
                <div class="contenedor-inputs">
                    <input type="number" id="long_final_grad" value="1"
                        onkeyup="longFinalMinMax(); calcularTiempoRecorrido();" onblur="longFinalMinMax();"
                        placeholder="Grados">
                    <input type="number" id="long_final_min" value="1"
                        onkeyup="longFinalMinMax(); calcularTiempoRecorrido();" onblur="longFinalMinMax();"
                        placeholder="Minutos">
                    <input type="number" id="long_final_seg" value="4"
                        onkeyup="longFinalMinMax(); calcularTiempoRecorrido();" onblur="longFinalMinMax();"
                        placeholder="Segundos">
                </div>
            </fieldset>

            <fieldset class="contenedor-datos">
                <legend>Concentración de Gas</legend>
                <input type="number" id="concentracion" value="5" placeholder="Nivel de concentración">
            </fieldset>

            <fieldset class="contenedor-datos">
                <legend>Declinación Magnética (Opcional)</legend>
                <input type="number" id="declinacion" placeholder="Ángulo">
            </fieldset>

            <div class="form-footer">
                <div class="contenedor-tiempos">
                    <div class="contenedor-t_estimado">
                        <p>Tiempo estimado de recorrido:</p>
                        <p id="t_estimado">0 min</p>
                    </div>
                    <div class="contenedor-t_disponible">
                        <p>Tiempo estimado disponible:</p>
                        <p id="t_disponible">0 min</p>
                    </div>
                </div>

                <button type="submit" class="boton boton-verde" id="boton-iniciar">iniciar</button>
            </div>
        </form>

        <div id="contenedor-tabla-boton">
            <div class="botones-detener-cancelar">
                <button type="button" class="boton boton-amarillo" id="boton-detener"
                    onclick="detenerRecorrido()">detener</button>
                <button type="button" class="boton boton-verde" id="boton-continuar"
                    onclick="continuarRecorrido()">continuar</button>
                <button type="button" class="boton boton-rojo" id="boton-cancelar"
                    onclick="cancelarRecorrido()">cancelar</button>
                <button type="button" class="boton boton-verde" id="boton-volver-inicio"
                    onclick="cancelarRecorrido()">volver al inicio</button>
            </div>
            <div class="contenedor-tabla">
                <table id="dataTable">
                    <tr class="fila-titulo">
                        <th>N</th>
                        <th>CONCENTRACIÓN</th>
                        <th>LATITUD</th>
                        <th>LONGITUD</th>
                        <th>TIEMPO</th>
                        <th>INCLINACIÓN</th>
                    </tr>
                </table>
            </div>
        </div>
    </div>

    <!-- ********************************* SCRIPTS ********************************* -->

    <script type="text/javascript">
        //Variables para todo el código JavaScript
        var concentracion_ref;
        var concentracion;
        var latitud;
        var longitud;
        var t_disponible;
        var t_transcurrido;
        var inclinacion;
        var t_estimado = Number(0);
        var vect_datos = [];

        var intervalTiempoDeEspera;
        var intervalActualizarTiempoDisp;
        var intervalActualizarDatos;

        //Se ejecuta al cargar la página
        document.addEventListener("DOMContentLoaded", function () {
            formToggleDisplayed(); //Hasta que el equipo no este listo no se pueden ingresar datos
            intervalVerSiEstaListo = setInterval(verSiEstaListo, 1000);

            tableToggleDisplayed(); //Se esconde la tabla
        });

        // --------------- Comprobar si el euipo esta listo --------------

        //Si el equipo esta listo se habilida el formulario
        function verSiEstaListo() {
            fetch("/consultar", { method: 'GET' })
                .then(function (respuesta) {
                    if (respuesta.ok) {
                        respuesta.json().then(function (datos) {
                            if (datos.listo_para_explorar) { //Si el dispositivo esta preparado para recibir instrucciones
                                clearInterval(intervalVerSiEstaListo);
                                document.getElementById("contenedor-tiempo-de-espera").style.display = "none";
                                formToggleDisplayed();

                                actualizarTiempoDisp();
                                intervalActualizarTiempoDisp = setInterval(actualizarTiempoDisp, 100000); //Se llama a esta función cada un minuto;
                            }
                        })
                    }
                })
                .catch(function () {
                })
        }

        // --------------- Mostrar el tiempo disponible --------------

        //Muestra en la página el tiempo maximo de exploración
        function actualizarTiempoDisp() {
            fetch("/consultar", { method: 'GET' })
                .then(function (respuesta) {
                    if (respuesta.ok) {
                        respuesta.json().then(function (datos) {
                            t_disponible = datos.t_disponible;
                            document.getElementById("t_disponible").innerHTML = secondsToHm(t_disponible);
                        })
                    }
                })
                .catch(function () {
                })
        }

        // --------------- Chequeo de formulario y envio --------------

        //Se valida el formulario cuando se aprieta el boton de iniciar
        function validateForm() {
            actualizarVectorDatos();
            concentracion_ref = vect_datos[12];

            if (vect_datos[0] == "" || vect_datos[1] == "" || vect_datos[2] == "") {
                alert("Complete los campos con la latitud inicial");
                return false;
            }

            if (vect_datos[3] == "" || vect_datos[4] == "" || vect_datos[5] == "") {
                alert("Complete los campos con la longitud inicial");
                return false;
            }

            if (vect_datos[6] == "" || vect_datos[7] == "" || vect_datos[8] == "") {
                alert("Complete los campos con la latitud final");
                return false;
            }

            if (vect_datos[9] == "" || vect_datos[10] == "" || vect_datos[11] == "") {
                alert("Complete los campos con la longitud final");
                return false;
            }

            if ((vect_datos[0] == vect_datos[6] && vect_datos[1] == vect_datos[7] && vect_datos[2] == vect_datos[8]) ||
                (vect_datos[3] == vect_datos[9] && vect_datos[4] == vect_datos[10] && vect_datos[5] == vect_datos[11])) {
                alert("Las latitudes y/o lonigutdes iniciales y finales no pueden ser iguales");
                return false;
            }

            if (vect_datos[12] == "") {
                alert("Ingrese un valor de concentración");
                return false;
            }

            if (vect_datos[13] < 0 || vect_datos[13] > 360) {
                alert("Ingrese un ángulo de declinación entre 0 y 360º");
                return false;
            }

            if (t_disponible < t_estimado + 900 /* 15 min*/) {
                alert("El tiempo de batería disponible debe ser al menos 15 minutos mayor al tiempo estimado de recorrido");
                return false;
            }

            enviarDatosUsuario(vect_datos);

            return false;
        }

        //Se envian los datos del usuario
        function enviarDatosUsuario(vect_datos) {
            fetch("/datos_usuario",
                {
                    method: 'PUT',
                    body: JSON.stringify(
                        {
                            lat_inicial_grad: vect_datos[0],
                            lat_inicial_min: vect_datos[1],
                            lat_inicial_seg: vect_datos[2],
                            long_inicial_grad: vect_datos[3],
                            long_inicial_min: vect_datos[4],
                            long_inicial_seg: vect_datos[5],

                            lat_final_grad: vect_datos[6],
                            lat_final_min: vect_datos[7],
                            lat_final_seg: vect_datos[8],
                            long_final_grad: vect_datos[9],
                            long_final_min: vect_datos[10],
                            long_final_seg: vect_datos[11],

                            concentracion: vect_datos[12],
                            declinacion: vect_datos[13],
                        })
                })
                .then(function () {
                    enviarDatosMovimiento("iniciar"); //explorando=true, detenido=false
                    formToggleDisplayed();
                    clearInterval(intervalActualizarTiempoDisp);

                    intervalActualizarDatos = setInterval(actualizarDatos, 1000);
                    configBotonesDisplay(1, 0, 1, 0); // botones: detener(1), continuar(0), cancelar(1), volver-inicio(0)
                    tableToggleDisplayed();
                })
                .catch(function () {
                    console.log("No se pudieron cargar correctamente los datos, exploración abortada");
                })
        }

        // --------------- Pedido de datos y actualización de la información --------------

        // Se ejecuta cada 1 segundo para actualizar la página
        function actualizarDatos() {
            fetch("/consultar", { method: 'GET' })
                .then(function (respuesta) {
                    if (respuesta.ok) {
                        respuesta.json().then(function (datos) {
                            if (chequearErrores(datos.error))
                                return;
                            if (datos.concentracion == 65535) //Valor de error
                                concentracion = "Fuera de Escala";
                            else
                                concentracion = datos.concentracion;
                            latitud = decToGradMinSeg(datos.latitud);
                            longitud = decToGradMinSeg(datos.longitud);
                            t_transcurrido = secondsToHms(datos.t_transcurrido);
                            inclinacion = datos.inclinacion + "º";
                            agregarFila();
                            if (datos.explorando == false) //Terminó
                                exploracionFinalizada();
                        })
                    }
                })
                .catch(function () {
                })
        }

        //Chequea si el equipo se encuentra en estado de error y lo comunica al usuario
        function chequearErrores(e) { // true->hay errores, false-> sin errores
            if (e == 0)
                return false;
            cancelarRecorrido(); //Si hay algún error lo primero que se hace es cancelar el recorrido
            switch (e) {
                case 1: //Rueda trabada
                    alert("Rueda trabada, abortando exploración");
                    return true;
                    break;
                case 2: //Motor no responde
                    alert("Uno o más motores no responden, abortando exploración");
                    return true;
                    break;
                case 3: //Encoder no responde
                    alert("Uno o más encoders no responden, abortando exploración");
                    return true;
                    break;
                case 4: //GPS no responde
                    alert("Comunicación GPS perdida, abortando exploración");
                    return true;
                    break;
                case 5: //Brújula no responde
                    alert("La brújula no responde, abortando exploración");
                    return true;
                    break;
                case 6: //No se puede sensar el nivel de batería
                    alert("No es posible sensar el nivel de batería, abortando exploración");
                    return true;
                    break;
                default: //Error desconcido
                    alert("Error desconocido");
                    return true;
                    break;
            }
        }

        //Se ejecuta cuando la exploración llega a su fin
        function exploracionFinalizada() {
            var table = document.getElementById("dataTable");

            var colCount = table.rows[0].cells.length;
            var row = table.insertRow(1);

            var cell = row.insertCell(0);
            cell.innerHTML = "Exploración Finalizada!";

            row.style.color = "white";
            row.style.background = "#2b7d1f";
            cell.colSpan = String(colCount);
            cell.style.fontSize = "2rem";

            clearInterval(intervalActualizarDatos);

            configBotonesDisplay(0, 0, 0, 1); // botones: detener(0), continuar(0), cancelar(0), volver-inicio(1)
        }

        //Se agrega una nueva fila con datos
        function agregarFila() {
            var table = document.getElementById("dataTable");

            var rowCount = table.rows.length;
            var row = table.insertRow(1);

            var cell1 = row.insertCell(0);
            cell1.innerHTML = rowCount;

            var cell2 = row.insertCell(1);
            cell2.innerHTML = concentracion + " PPM";

            var cell3 = row.insertCell(2);
            cell3.innerHTML = latitud;

            var cell4 = row.insertCell(3);
            cell4.innerHTML = longitud;

            var cell5 = row.insertCell(4);
            cell5.innerHTML = t_transcurrido;

            var cell6 = row.insertCell(5);
            cell6.innerHTML = inclinacion;

            if (concentracion >= concentracion_ref) { // >= 100%
                row.style.color = "white";
                row.style.background = "#d23131";
            }
            else if (concentracion >= concentracion_ref * 0.85) { // >= 85%
                row.style.color = "white";
                row.style.background = "#e26f42";
            }
            else if (concentracion >= concentracion_ref * 0.7) { // >= 70%
                row.style.color = "white";
                row.style.background = "#e29b42";
            }
        }

        // --------------- Botones detener, continuar y cancelar --------------

        //Da la orden de detener el recorrido pero no lo cancela
        function detenerRecorrido() {
            clearInterval(intervalActualizarDatos);
            configBotonesDisplay(0, 1, 1, 0); // botones: detener(0), continuar(1), cancelar(1), volver-inicio(0)
            enviarDatosMovimiento("detener");
        }

        //Da la orden de continuar el recorrido
        function continuarRecorrido() {
            intervalActualizarDatos = setInterval(actualizarDatos, 1000);
            configBotonesDisplay(1, 0, 1, 0); // botones: detener(1), continuar(0), cancelar(1), volver-inicio(0)
            enviarDatosMovimiento("continuar");
        }

        //Da la orden de cancelar el recorrido, lo aborta
        function cancelarRecorrido() {
            clearInterval(intervalActualizarDatos);
            document.getElementById("contenedor-tabla-boton").style.display = "none";
            limpiarTabla();

            document.getElementById("myForm").reset();
            document.getElementById("myForm").style.display = "inline-block";

            actualizarTiempoDisp();
            intervalActualizarTiempoDisp = setInterval(actualizarTiempoDisp, 100000) //Se llama a esta función cada un minuto;
            enviarDatosMovimiento("abortar");

            calcularTiempoRecorrido();
        }

        // --------------- Funciones útiles --------------

        //Calcula el tiempo de recorrido estimado según los datos ingresados y lo muestra en la página
        function calcularTiempoRecorrido() {
            actualizarVectorDatos();

            for (var i = 0; i < 12; i++) {
                if (vect_datos[i] == "") { //Si hay algun campo vacio el tiepo estimado es 0
                    document.getElementById("t_estimado").innerHTML = secondsToHm("0 min");
                    document.getElementById("t_estimado").style.color = "black";
                    return
                }
            }

            lat_i = gradMinSecToSec(vect_datos[0], vect_datos[1], vect_datos[2]);
            long_i = gradMinSecToSec(vect_datos[3], vect_datos[4], vect_datos[5]);
            lat_f = gradMinSecToSec(vect_datos[6], vect_datos[7], vect_datos[8]);
            long_f = gradMinSecToSec(vect_datos[9], vect_datos[10], vect_datos[11]);

            lat_m = 0;
            long_m = 0;

            lat_i > lat_f ? lat_m = (lat_i - lat_f) * 30 + 3 : lat_m = (lat_f - lat_i) * 30 + 3;         //guarda en lat_m la diferencia de latitud en metros.
            long_i > long_f ? long_m = (long_i - long_f) * 30 + 3 : long_m = (long_f - long_i) * 30 + 3; //guarda en long_m la diferencia de latitud en metros.

            recorrido_m = Number(long_m * Math.round(lat_m / 0.2) + lat_m); //Recorrido en metros
            t_estimado = Number(recorrido_m / 0.6) //Si recorre 0.6[m/s]

            document.getElementById("t_estimado").innerHTML = secondsToHm(t_estimado);

            if (t_disponible < t_estimado + 900 /* 15 min*/)
                document.getElementById("t_estimado").style.color = "red";
            else
                document.getElementById("t_estimado").style.color = "black";
        }

        //Recibe grados minutos y segundo y devuelve la cantidad de segundos equivalentes
        function gradMinSecToSec(grad, min, sec) {
            return (grad * 3600) + (min * 60) + sec;
        }

        //Actualiza el vector que contiene los datos
        function actualizarVectorDatos() {
            while (vect_datos.length > 0) {
                vect_datos.pop();
            }

            vect_datos.push(document.getElementById("lat_inicial_grad").value);
            vect_datos.push(document.getElementById("lat_inicial_min").value);
            vect_datos.push(document.getElementById("lat_inicial_seg").value);
            vect_datos.push(document.getElementById("long_inicial_grad").value);
            vect_datos.push(document.getElementById("long_inicial_min").value);
            vect_datos.push(document.getElementById("long_inicial_seg").value);

            vect_datos.push(document.getElementById("lat_final_grad").value);
            vect_datos.push(document.getElementById("lat_final_min").value);
            vect_datos.push(document.getElementById("lat_final_seg").value);
            vect_datos.push(document.getElementById("long_final_grad").value);
            vect_datos.push(document.getElementById("long_final_min").value);
            vect_datos.push(document.getElementById("long_final_seg").value);

            vect_datos.push(document.getElementById("concentracion").value);
            vect_datos.push(document.getElementById("declinacion").value);
        }

        //Se envia al equipo ordenes para que inice, continue o detenga el movimiento
        function enviarDatosMovimiento(str) {
            fetch("/act_estado",
                {
                    method: 'POST',
                    body: JSON.stringify(
                        {
                            msg: str,
                        })
                })
                .then(function () {
                })
                .catch(function () {
                })
        }

        //Muestra o esconde el form
        function formToggleDisplayed() {
            var form = document.getElementById("myForm");

            if (form.style.display != "none")
                form.style.display = "none";
            else
                form.style.display = "inline-block";
        }

        //Muestra o se esconde la tabla
        function tableToggleDisplayed() {
            var tabla = document.getElementById("contenedor-tabla-boton");
            if (tabla.style.display != "none")
                tabla.style.display = "none";
            else
                tabla.style.display = "inline-block";
        }

        //Limpia la tabla
        function limpiarTabla() {
            var rows = document.getElementById("dataTable").rows;
            var i = rows.length;
            while (--i) {
                rows[i].parentNode.removeChild(rows[i]);
            }
        }

        //Se muestran o se esconden los botones de detner, continuar, cancelar y volver-inicio
        function configBotonesDisplay(detener, continuar, cancelar, volver_inicio) {
            opt_detener = detener ? "inline-block" : "none";
            opt_continuar = continuar ? "inline-block" : "none";
            opt_cancelar = cancelar ? "inline-block" : "none";
            opt_volver_inicio = volver_inicio ? "inline-block" : "none";

            document.getElementById("boton-detener").style.display = opt_detener;
            document.getElementById("boton-continuar").style.display = opt_continuar;
            document.getElementById("boton-cancelar").style.display = opt_cancelar;
            document.getElementById("boton-volver-inicio").style.display = opt_volver_inicio;
        }

        //Convierte segundos en horas y minutos
        function secondsToHm(d) {
            d = Number(d);
            var h = Math.floor(d / 3600);
            var m = Math.floor(d % 3600 / 60);

            var hDisplay = h > 0 ? h + " h " : "";
            var mDisplay = m > 0 ? m + " min" : "";

            if (hDisplay + mDisplay != "")
                return hDisplay + mDisplay;
            else
                return "0 min"
        }

        //Convierte segundos en horas, minutos y segundos
        function secondsToHms(d) {
            d = Number(d);
            var h = Math.floor(d / 3600);
            var m = Math.floor(d % 3600 / 60);
            var s = Math.floor(d % 3600 % 60);

            var hDisplay = h > 0 ? h + " h " : "";
            var mDisplay = m > 0 ? m + " min " : "";
            var sDisplay = s > 0 ? s + " s" : "";

            if (hDisplay + mDisplay + sDisplay != "")
                return hDisplay + mDisplay + sDisplay;
            else
                return "0 s"
        }

        //Convierte valores decimales a grados, minutos y segundos
        function decToGradMinSeg(dec) {
            grad = Number(dec);
            min = Number(((dec % 1).toFixed(4) * 60));
            seg = Number(((min % 1).toFixed(4) * 60));

            grad = truncateDecimals(grad);
            min = truncateDecimals(min);
            seg = truncateDecimals(seg);

            return String(grad + "º " + Math.abs(min) + "' " + Math.abs(seg) + "''");
        }

        //Trunca a los decimales
        function truncateDecimals(number) {
            return Math[number < 0 ? 'ceil' : 'floor'](number);
        };

        //Si el valor de latitud inicial es mayor o menor al primitido lo ajusta
        function latInicialMinMax() {
            var grad = document.getElementById("lat_inicial_grad");
            var min = document.getElementById("lat_inicial_min");
            var seg = document.getElementById("lat_inicial_seg");
            ajustarValoresLatitud(grad, min, seg);
        }

        //Si el valor de longitud inicial es mayor o menor al primitido lo ajusta
        function longInicialMinMax() {
            var grad = document.getElementById("long_inicial_grad");
            var min = document.getElementById("long_inicial_min");
            var seg = document.getElementById("long_inicial_seg");
            ajustarValoresLongitud(grad, min, seg);
        }

        //Si el valor de latitud final es mayor o menor al primitido lo ajusta
        function latFinalMinMax() {
            var grad = document.getElementById("lat_final_grad");
            var min = document.getElementById("lat_final_min");
            var seg = document.getElementById("lat_final_seg");
            ajustarValoresLatitud(grad, min, seg);
        }

        //Si el valor de longitud final es mayor o menor al primitido lo ajusta
        function longFinalMinMax() {
            var grad = document.getElementById("long_final_grad");
            var min = document.getElementById("long_final_min");
            var seg = document.getElementById("long_final_seg");
            ajustarValoresLongitud(grad, min, seg);
        }

        //Funcion auxuliar para ajustar los valores de latitud
        function ajustarValoresLatitud(grad, min, seg) {
            grad.value >= 60 ? (grad.value = 60, min.value = 0, seg.value = 0) : grad.value <= -60 ? (grad.value = -60, min.value = 0, seg.value = 0) : null;
            min.value = min.value > 59 ? 59 : min.value < 0 ? 0 : min.value;
            seg.value = seg.value > 59 ? 59 : seg.value < 0 ? 0 : seg.value;
        }

        //Funcion auxuliar para ajustar los valores de longitud
        function ajustarValoresLongitud(grad, min, seg) {
            grad.value >= 180 ? (grad.value = 180, min.value = 0, seg.value = 0) : grad.value <= -180 ? (grad.value = -180, min.value = 0, seg.value = 0) : null;
            min.value = min.value > 59 ? 59 : min.value < 0 ? 0 : min.value;
            seg.value = seg.value > 59 ? 59 : seg.value < 0 ? 0 : seg.value;
        }
    </script>
</body>

<!-- ********************************* CSS ********************************* -->

<style>
    :root {
        --naranja: #e16c2e;
        --naranja-medio: #f06a22;
        --naranja-oscuro: #a34e20;
        --amarillo: #cca22e;
        --amarillo-oscuro: #b58a13;
        --rojo: #d23131;
        --rojo-oscuro: #9a1010;
        --verde: #059411;
        --verde-oscuro: #03770c;
        --celeste: #2e8a8a;
        --celeste-oscuro: #136161;
        --gris-claro: #dfdfdf;
        --gris-oscuro: #979797;
        --color-footer: rgb(49, 48, 48);
    }

    html {
        box-sizing: border-box;
        font-size: 62.5%;
        font-family: 'Noto Sans KR', sans-serif;
    }

    *,
    *:before,
    *:after {
        box-sizing: inherit;
    }

    /** Global **/

    .contenedor {
        max-width: 120rem;
        width: 95%;
        margin: 0 auto;
        height: 100%;
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    h1 {
        font-size: 4rem;
        color: white;
        width: 100%;
        text-align: center;
    }

    h2 {
        font-size: 3.6rem;
        text-align: center;
        font-weight: 400;
    }

    h3 {
        font-size: 3.2rem;
        margin: 1rem 0 0 0;
    }

    h4 {
        font-size: 2.4rem;
        margin: 1rem 0;
    }

    table {
        width: 100%;
    }

    td,
    th {
        border: 0.2rem solid #dddddd;
        text-align: center;
        padding: 2rem;
        font-size: 1.6rem;
        font-weight: 600;
    }

    th {
        font-weight: 800;
    }

    .site-header {
        height: 100%;
        align-items: center;
        background-color: black;
        margin: 0 auto;
        max-width: 140rem;
        background-size: cover;
        background-position: cover;
        display: flex;
        flex-direction: column;
    }

    .boton {
        padding: 2rem 4rem;
        font-size: 2rem;
        font-weight: 700;
        text-transform: uppercase;
        text-decoration: none;
        color: white;
        text-align: center;
        width: 25rem;
        border: none;
        cursor: pointer;
        outline: none;
    }

    .boton-rojo {
        background-color: var(--rojo);
    }

    .boton-rojo:hover {
        background-color: var(--rojo-oscuro);
    }

    .boton-amarillo {
        background-color: var(--amarillo);
    }

    .boton-amarillo:hover {
        background-color: var(--amarillo-oscuro);
    }

    .boton-verde {
        background-color: var(--verde);
    }

    .boton-verde:hover {
        background-color: var(--verde-oscuro);
    }

    .boton-celeste {
        background-color: var(--celeste);
    }

    .boton-celeste:hover {
        background-color: var(--celeste-oscuro);
    }

    /* Ingreso de datos */
    legend {
        font-size: 2rem;
        color: #4f4f4f;
        margin-bottom: 1.2rem;
    }

    label {
        font-weight: 700;
        text-transform: uppercase;
        display: block;
        margin-bottom: 0.5rem;
        font-size: 1.3rem;
    }

    input:not([type='submit']),
    select {
        padding: 1rem;
        display: block;
        width: 100%;
        background-color: #e1e1e1;
        margin-bottom: 2rem;
        border: none;
        border-radius: 1rem;
    }

    input {
        width: auto;
        margin-right: 1rem;
        outline: none;
    }

    input:last-of-type {
        margin-right: 0;
    }

    #contenedor-tiempo-de-espera {
        display: flex;
        flex-direction: column;
        align-items: center;

    }

    #contenedor-tiempo-de-espera p:not([id=tiempo-de-espera]) {
        font-size: 2rem;
        margin-top: 3rem;
    }

    #tiempo-de-espera {
        font-size: 12rem;
        font-weight: 800;
        margin: 1rem 0;
    }

    .contenedor-inputs {
        display: flex
    }

    .datos-usuario {
        margin-top: 2rem;
        width: 80%;
    }

    .contenedor-datos {
        margin-bottom: 2rem;
    }

    .contenedor-datos:last-of-type {
        margin-bottom: 0;
    }

    .form-footer {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-top: 1rem;
    }

    @media (max-width: 768px) {
        .form-footer {
            flex-direction: column;
        }
    }

    .contenedor-tiempos {
        display: flex;
        flex-direction: column;
        min-width: 35rem;
        padding-right: 2rem;
    }

    .contenedor-tiempos p {
        font-size: 1.6rem;
    }

    @media (max-width: 768px) {
        .contenedor-tiempos {
            min-width: 100%;
        }
    }

    .contenedor-t_disponible,
    .contenedor-t_estimado {
        display: flex;
        justify-content: space-between;
        align-items: center;
    }

    #boton-iniciar {
        display: block;
        max-width: 30%;
    }

    @media (max-width: 768px) {
        #boton-iniciar {
            display: inline-block;
            min-width: 100%;
        }
    }

    /* Botón Detener, Cancelar y Tabla */
    #contenedor-tabla-boton {
        width: 80%;
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    .contenedor-tabla {
        margin-top: 2rem;
        width: 100%;
        height: 60rem;
        overflow-y: scroll;
    }

    .botones-detener-cancelar {
        display: flex;
        justify-content: space-between;
        flex-basis: calc(50% - 1.2rem);
    }

    #boton-detener,
    #boton-continuar,
    #boton-cancelar,
    #boton-volver-inicio {
        width: 100%;
        margin-top: 2rem;
        flex-basis: calc(50% - 1.2rem);
    }

    #boton-volver-inicio {
        flex-basis: 100%;
    }

    .fila-titulo {
        background: var(--gris-claro);
    }
</style>

</html>
)=====";