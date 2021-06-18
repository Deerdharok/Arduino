<?php
date_default_timezone_set('America/Mexico_City');
if(isset($_GET["num"])) {
   $num = strval($_GET["num"]); // obtener numero de HTTP GET
   $fecha= date("Y-m-d H:i:s",time());
   $times = strval( $fecha );
   $servername = "localhost";
   $username = "Arduino";
   $password = "12345";
   $dbname = "db_arduino";

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   $sql = "INSERT INTO entradas (nombre,fecha) VALUES ($num,'$times')";
   
   if ($conn->query($sql) === TRUE) {
      echo "Entrada registrada";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "valor no registrada";
}
?>