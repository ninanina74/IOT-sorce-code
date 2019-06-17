<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['changeLocation'] == null || $_REQUEST['changeLocation'] == "") {
        throw new Exception("必須填入地點名稱");
    }

    include("DB.php");
    $locationId = $_REQUEST['locationId'];
    $changeLocation = $_REQUEST['changeLocation'];
    $changeEnable = $_REQUEST['changeEnable'];
    $sql = "UPDATE Location SET location_name = '$changeLocation', enable = $changeEnable WHERE location_id = $locationId";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
