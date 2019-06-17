<?php
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");
    $itemTypeId = $_REQUEST['itemTypeId'];
    $changeEnable = $_REQUEST['changeEnable'];
    $sql = "UPDATE ItemType SET enable = $changeEnable WHERE type_id = $itemTypeId";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
