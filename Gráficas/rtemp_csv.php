<?php

 function conectarBD(){ 
            $server = "localhost";
            $usuario = "root";
            $pass = "powerlab";
            $BD = "estacion";
            //variable que guarda la conexión de la base de datos
            $conexion = mysqli_connect($server, $usuario, $pass, $BD); 
            //Comprobamos si la conexión ha tenido exito
            if(!$conexion){ 
               echo 'Ha sucedido un error inexperado en la conexion de la base de datos<br>'; 
            } 
            //devolvemos el objeto de conexión para usarlo en las consultas  
            return $conexion; 
    }  
    /*Desconectar la conexion a la base de datos*/
    function desconectarBD($conexion){
            //Cierra la conexión y guarda el estado de la operación en una variable
            $close = mysqli_close($conexion); 
            //Comprobamos si se ha cerrado la conexión correctamente
            if(!$close){  
               echo 'Ha sucedido un error inexperado en la desconexion de la base de datos<br>'; 
            }    
            //devuelve el estado del cierre de conexión
            return $close;         
    }

    //Devuelve un array multidimensional con el resultado de la consulta
    function getArraySQL($sql){
        //Creamos la conexión
        $conexion = conectarBD();
        //generamos la consulta
        if(!$result = mysqli_query($conexion, $sql)) die();

        $rawdata = array();
        //guardamos en un array multidimensional todos los datos de la consulta
        $i=0;
        while($row = mysqli_fetch_array($result))
        {   
            //guardamos en rawdata todos los vectores/filas que nos devuelve la consulta
            $rawdata[$i] = $row;
            $i++;
        }
        //Cerramos la base de datos
        desconectarBD($conexion);
        //devolvemos rawdata
        return $rawdata;
    }

    //Sentencia SQL
$sql = "SELECT * from cosmic;";
//Array Multidimensional
$rawdata = getArraySQL($sql);

//Adaptar el tiempo
for($i=0;$i<count($rawdata);$i++){
    $FECHA = $rawdata[$i]["FECHA"];
    $date = new DateTime($FECHA);
    $rawdata[$i]["FECHA"]=$date->getTimestamp()*1000;
}

?>
<!DOCTYPE html>
<HTML>
<title>Historial temperatura — Estación #1</title>
<link rel="shortcut icon" href="logo.ico" />
<body bgcolor="006699">

<meta charset="utf-8"> 
<style>
 }
body {
  margin: 0;
}

ul {
  list-style-type: none;
  margin: 0;
  padding: 0;
  width: 15%;
  background: linear-gradient(to bottom, #00ffcc 0%, #0066ff 100%);
  position: fixed;
  height: 100%;
  overflow: auto;
}

li a {
  display: block;
  color: #000;
  padding: 20px 0px;
  text-decoration: none;
  text-align: center;
}

li a.active {
  color: white;
}

li a:hover:not(.active) {
  background-color: #555;
  color: white;
}

</style>

</head>
<ul>
  <li><a class="active" href="index.html"><img src="ITSON.png" alt="HOME" width="150" height="150"></a></li>
  <li><a href="est1.php">ESTACIÓN #1</a></li>
  <li><a href="est2.php">ESTACIÓN #2</a></li>
  <li><a href="est3.php">ESTACIÓN #3</a></li>
</ul>

<div style="margin-left:25%; text-align:center;padding:0px 0px;"> 
  <style>
.button {
  background-color: #4CAF50;
  border: none;
  color: white;
  padding: 15px 20px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  position: absolute;
  left: 215px;
  top: 10px;
  cursor: pointer;
}
.button {border-radius: 12px;}
</style>
<a href="temp_export.html" class="button">Exportar datos CSV</a>
</div>

<script src="https://code.highcharts.com/stock/highstock.js"></script>
<script src="https://code.highcharts.com/stock/modules/data.js"></script>
<script src="https://code.highcharts.com/stock/highcharts-more.js"></script>
<script src="https://code.highcharts.com/stock/modules/exporting.js"></script>
<script src="https://code.highcharts.com/modules/export-data.js"></script>
<div id="container" style="margin-left:25%;padding:80px 0px;width:875px;height:440px"></div>


<script type='text/javascript'>

Highcharts.getJSON('https://cdn.jsdelivr.net/gh/highcharts/highcharts@v7.0.0/samples/data/range.json', function (data) {

    Highcharts.setOptions({
    lang: {
            loading: 'Cargando...',
            months: ['Enero', 'Febrero', 'Marzo', 'Abril', 'Mayo', 'Junio', 'Julio', 'Agosto', 'Septiembre', 'Octubre', 'Noviembre', 'Diciembre'],
            weekdays: ['Domingo', 'Lunes', 'Martes', 'Miércoles', 'Jueves', 'Viernes', 'Sábado'],
            shortMonths: ['Ene', 'Feb', 'Mar', 'Abr', 'May', 'Jun', 'Jul', 'Ago', 'Sep', 'Oct', 'Nov', 'Dic'],
            exportButtonTitle: "Exportar",
            printButtonTitle: "Importar",
            rangeSelectorFrom: "Desde",
            rangeSelectorTo: "Hasta",
            rangeSelectorZoom: "Período",
            downloadPNG: 'Descargar imagen PNG',
            downloadJPEG: 'Descargar imagen JPEG',
            downloadPDF: 'Descargar imagen PDF',
            downloadSVG: 'Descargar imagen SVG',
            printChart: 'Imprimir',
            resetZoom: 'Reiniciar zoom',
            resetZoomTitle: 'Reiniciar zoom',
            thousandsSep: ",",
            decimalPoint: '.'
        }        
});

    Highcharts.stockChart('container', {

        chart: {
            type: 'spline',
            backgroundColor: '#CCCCCC'
        },

       rangeSelector: {
            allButtonsEnabled: true,
            buttons: [{
                type: 'month',
                count: 3,
                text: 'Dia',
                dataGrouping: {
                    forced: true,
                    units: [['day', [1]]]
                }
            }, {
                type: 'year',
                count: 1,
                text: 'Semana',
                dataGrouping: {
                    forced: true,
                    units: [['week', [1]]]
                }
            }, 

            {
                type: 'all',
                text: 'Mes',
                dataGrouping: {
                    forced: true,
                    units: [['month', [1]]]
                }
            }],
            buttonTheme: {
                width: 60
            },
            selected: 2
        },

        xAxis: {
                type: 'datetime',
                tickPixelInterval: 150
            },
            yAxis: {
                title: {
                    text: 'T (°C)'
                },
                
             },

        title: {
            text: 'Temperatura'
        },

        tooltip: {
            valueSuffix: '°C',
            valueDecimals: 2
        },

        series: [{
            name: 'Temperatura',
                color: '#FF0000',
                data: (function() {
                   var data = [];
                    <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    data.push([<?php echo $rawdata[$i]["FECHA"];?>,<?php echo $rawdata[$i]["TEMPERATURA"];?>]);
                    <?php } ?>
                return data;
                })()
        }]

    });
});


</script>
</html>