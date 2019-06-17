<?php
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");
    $sql = "SELECT A.*, B.type_name, C.vendor_name FROM Item A, ItemType B, Vendor C WHERE A.type_id = B.type_id AND A.vendor_id = C.vendor_id ORDER BY item_id";
    echo json_encode(query($sql));
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
