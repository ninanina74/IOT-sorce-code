<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['locationName'] == null || $_REQUEST['locationName'] == "") {
        throw new Exception("必須填入地點名稱");
    }

    include("DB.php");
    $locationName = $_REQUEST['locationName'];
    $enable = $_REQUEST['selectEnable'];
    $sql = "INSERT INTO Location (location_name, enable) VALUES ('$locationName', $enable)";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
