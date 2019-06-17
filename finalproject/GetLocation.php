<?php
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");
    $sql = "SELECT * FROM Location ORDER BY location_id";
    echo json_encode(query($sql));
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
