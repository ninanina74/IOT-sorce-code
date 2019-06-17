<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['itemSerial'] == null || $_REQUEST['itemSerial'] == "") {
        throw new Exception("必須填入物品序號");
    }

    include("DB.php");
    $itemSerial = $_REQUEST['itemSerial'];
    $selectItemType = $_REQUEST['selectItemType'];
    $selectVendor = $_REQUEST['selectVendor'];
    $selectEnable = $_REQUEST['selectEnable'];
    $sql = "INSERT INTO Item (item_serial, type_id, vendor_id, enable) VALUES ('$itemSerial', $selectItemType, $selectVendor, $selectEnable)";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
