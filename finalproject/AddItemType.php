<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['itemType'] == null || $_REQUEST['itemType'] == "") {
        throw new Exception("必須填入物品種類");
    }

    include("DB.php");
    $itemType = $_REQUEST['itemType'];
    $selectEnable = $_REQUEST['selectEnable'];
    $sql = "INSERT INTO ItemType (type_name, enable) VALUES ('$itemType', $selectEnable)";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
