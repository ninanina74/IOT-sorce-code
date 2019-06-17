<?php
header('Content-type: text/html; charset=utf8'); 
try {
    $sql = "SELECT A.* From Log A, Status B, Item C where A.item_id = C.item_id and A.status_id = B.status_id and B.status_name = '水洗結束'  ";

    if (!is_null($_GET['itemSerial']) && $_GET['itemSerial'] != "") {
        $sql = $sql."AND C.item_serial ='".$_GET['itemSerial']."'";
    }

    $sql = $sql . "order by A.create_time desc";

    include("DB.php");
    echo json_encode(query($sql));
    // echo json_encode($sql);
} catch (Exception $e) {
    echo $e->getMessage();
}
?>