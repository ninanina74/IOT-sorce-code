<?php
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");
    $itemId = $_REQUEST['itemId'];
    $changeEnable = $_REQUEST['changeEnable'];
    $sql = "UPDATE Item SET enable = $changeEnable WHERE item_id = $itemId";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
