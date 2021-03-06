const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>TD3 TP7</title>
</head>

<body>
    <header class="site-header">
        <h1>Técnicas Digitales 3 - TP 7</h1>
    </header>

    <div class="contenedor">
        <div id="contenedor-tabla-boton">
            <div class="botones-detener-cancelar">
                <button type="button" class="boton boton-rojo" id="boton-detener"
                    onclick="detenerLecturas()">detener</button>
                <button type="button" class="boton boton-verde" id="boton-iniciar"
                    onclick="iniciarLecturas()">iniciar</button>
            </div>
            <div class="contenedor-tabla">
                <table id="dataTable">
                    <tr class="fila-titulo">
                        <th>N</th>
                        <th>ESCLAVO</th>
                        <th>SENSOR 1</th>
                        <th>SENSOR 2</th>
                    </tr>
                </table>
            </div>
        </div>
    </div>

    <!-- ********************************* SCRIPTS ********************************* -->

    <script type="text/javascript">
        //Variables para todo el código JavaScript
        var slave;
        var sensor1;
        var sensor2;
        var vect_datos = [];

        var intervalActualizarDatos;

        //Se ejecuta al cargar la página
        document.addEventListener("DOMContentLoaded", function () {
            configBotonesDisplay(0, 1); // botones: detener(0), iniciar(1)
        });

        // --------------- Pedido de datos y actualización de la información --------------

        // Se ejecuta cada 1 segundo para actualizar la página
        function actualizarDatos() {
            fetch("/consultar", { method: 'GET' })
                .then(function (respuesta) {
                    if (respuesta.ok) {
                        respuesta.json().then(function (datos) {
                            slave = datos.slave;
                            sensor1 = datos.sensor1;
                            sensor2 = datos.sensor2;
                            agregarFila();
                        })
                    }
                })
                .catch(function () {
                })
        }


        //Se agrega una nueva fila con datos
        function agregarFila() {
            var table = document.getElementById("dataTable");

            var rowCount = table.rows.length;
            var row = table.insertRow(1);

            var cell1 = row.insertCell(0);
            cell1.innerHTML = rowCount;

            var cell2 = row.insertCell(1);
            cell2.innerHTML = slave;

            var cell3 = row.insertCell(2);
            cell3.innerHTML = sensor1;

            var cell3 = row.insertCell(3);
            cell3.innerHTML = sensor2;
        }

        // --------------- Botones detener, continuar y cancelar --------------

        //Da la orden de detener las lecturas 
        function detenerLecturas() {
            clearInterval(intervalActualizarDatos);
            configBotonesDisplay(0, 1); // botones: detener(0), iniciar(1)
            enviarDatos("detener");
        }

        //Da la orden de iniciar las lecturas
        function iniciarLecturas() {
            intervalActualizarDatos = setInterval(actualizarDatos, 3000);
            configBotonesDisplay(1, 0); // botones: detener(1), iniciar(0)
            enviarDatos("iniciar");
        }

        // --------------- Funciones útiles --------------

        //Se envia al equipo ordenes para iniciar o detener las lecturas
        function enviarDatos(str) {
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

        //Se muestran o se esconden los botones de detner e iniciar
        function configBotonesDisplay(detener, iniciar) {
            var opt_detener = detener ? "inline-block" : "none";
            var opt_iniciar = iniciar ? "inline-block" : "none";

            document.getElementById("boton-detener").style.display = opt_detener;
            document.getElementById("boton-iniciar").style.display = opt_iniciar;
        }

    </script>
</body>

<!-- ********************************* CSS ********************************* -->

<style>
    :root {
        --rojo: #d23131;
        --rojo-oscuro: #9a1010;
        --verde: #059411;
        --verde-oscuro: #03770c;
        --gris: #dfdfdf;
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
        color: white;
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
        max-width: 100%;
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

    .boton-verde {
        background-color: var(--verde);
    }

    .boton-verde:hover {
        background-color: var(--verde-oscuro);
    }

    /* Botones Iniciar, Cancelar y Tabla */
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

    #boton-detener,
    #boton-iniciar {
        width: 200px;
        margin-top: 2rem;
        flex-basis: calc(50% - 1.2rem);
    }

    .fila-titulo {
        background: var(--gris);
    }
</style>

</html>
)=====";
