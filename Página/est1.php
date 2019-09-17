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
$sql = "SELECT * FROM `cosmic` ORDER BY `id` DESC LIMIT 1;";
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
<html>
<head>
<title>Estacion #1</title>
<link rel="shortcut icon" href="logo.ico" />
<body bgcolor="66CCCC">

<style>
body {
  margin: 0;
}
 h1 { 
  display: block;
  font-size: 2em;
  margin-top: 0em;
  margin-bottom: 0em;
  margin-left: 0;
  margin-right: 0;
  font-weight: bold;
  color: white;
  text-align: center;
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
  background-color: #003366;
  color: white;
}
div.gallery {

    }

    div.gallery:hover {
    }

    div.gallery img {
      width: 100%;
      height: 100%;    }

    div.desc {
      padding: 15px;
      text-align: center;
      color: white;
      font-size: 120%;
    }


    .responsive {
      margin: 20px;
      padding: 0 6px;
      float: left;
      width: 25%;
    }
</style>
</head>
<body>

<ul>
  <li><a class="active" href="index.html"><img src="ITSON.png" alt="HOME" width="150" height="150"></a></li>
  <li><a href="est1.php">ESTACIÓN #1</a></li>
  <li><a href="">ESTACIÓN #2</a></li>
  <li><a href="">ESTACIÓN #3</a></li>
</ul>

<div style="margin-left:85%; text-align:center;padding:0px 0px;"> <iframe src="https://www.zeitverschiebung.net/clock-widget-iframe-v2?language=es&size=small&timezone=America%2FHermosillo" width="100%" height="90" frameborder="0" seamless></iframe> </div>

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
  left: 205px;
  top: 0px;
  cursor: pointer;
}
.button {border-radius: 12px;}
</style>
<a href="est1_export.html" class="button">Exportar datos CSV</a>
</div>


<div style="margin-left:25%;padding:0px 0px;height:auto;"> 
  <h1>Estación #1</h1>
  <div class="responsive">
      <div class="gallery">
        <a target="" href="rtemp.php">
          <img src="temp.jpg" alt="Temperatura" width="600" height="400">
        <title>Temperaturas</title>
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    Temperatura: <?php echo $rawdata[$i]["TEMPERATURA"];?> °C
                    <?php } ?> 
        </b> 
                    </div>
      </div>
    </div>

    <div class="responsive">
      <div class="gallery">
        <a target="" href="rh.php">
          <img src="hum.jpg" alt="Humedad Relativa" width="600" height="400">
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    Humedad: <?php echo $rawdata[$i]["HUMEDAD"];?>%
                    <?php } ?> 
        </b> 
                    </div>
      </div>
    </div>


    <div class="responsive">
      <div class="gallery">
        <a target="" href="rp.php">
          <img src="presion.jpg" alt="Presión" width="600" height="400">
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    Presión: <?php echo $rawdata[$i]["PRESION"];?> hPa
                    <?php } ?> 
        </b> 
                    </div>
      </div>
    </div>

    <div class="responsive">
      <div class="gallery">
        <a target="" href="rlluvia.php">
          <img src="lluvia.jpg" alt="Nivel de lluvia" width="600" height="400">
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    Nivel de lluvia: <?php echo $rawdata[$i]["LLUVIA"];?> mm
                    <?php } ?>
        </b> 
                    </div>
      </div>
    </div>


    <div class="responsive">
      <div class="gallery">
        <a target="" href="rvel.php">
          <img src="viento.jpg" alt="Velocidad del viento" width="600" height="400">
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    Viento: <?php echo $rawdata[$i]["VEL_VIENTO"];?> m/s | <?php echo $rawdata[$i]["DIR_VIENTO"];?>° 
                    <?php } ?>
        </b> 
                    </div>
      </div>
    </div>

    <div class="responsive">
      <div class="gallery">
        <a target="" href="rpm.php">
          <img src="pm.jpg" alt="Particulas suspendidas" width="600" height="400">
        </a>
        <div class="desc"><b>
          <?php
                        for($i = 0 ;$i<count($rawdata);$i++){
                    ?>
                    PM2.5: <?php echo $rawdata[$i]["PM2.5"];?> | PM10: <?php echo $rawdata[$i]["PM10"];?>
                    <?php } ?>
        </b> 
                    </div>
      </div>
    </div>


    <div class="clearfix"></div>
</div
>
</body>
</html>
