<?php  
 //https://www.cloudways.com/blog/import-export-csv-using-php-and-mysql/
 if(isset($_POST["hum_export"])) 
 {  
      $connect = mysqli_connect("localhost", "root", "powerlab", "estacion");   
      header('Content-Type: text/csv; charset=utf-8');
      header('Content-Disposition: attachment; filename=humedad.csv');
      $output = fopen("php://output", "w");  
      fputcsv($output, array('ID', 'Fecha', 'Humedad'));  
      $query = "SELECT ID, FECHA, HUMEDAD FROM cosmic WHERE FECHA BETWEEN '".$_POST["Desde"]."' AND '".$_POST["Hasta"]."'  ";  
      $result = mysqli_query($connect, $query);  
      while($row = mysqli_fetch_assoc($result))  
      {  
           fputcsv($output, $row);  
      }  
      fclose($output); 
}  
 ?>