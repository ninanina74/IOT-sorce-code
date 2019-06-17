<?php
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");
    $sql = "SELECT * FROM Device ORDER BY device_id";
    echo json_encode(query($sql));
} catch (Exception $e) {
    echo $e->getMessage();
}
?>