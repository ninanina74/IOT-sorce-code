<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['statusName'] == null || $_REQUEST['statusName'] == "") {
        throw new Exception("必須填入狀態名稱");
    }

    include("DB.php");
    $statusName = $_REQUEST['statusName'];
    $enable = $_REQUEST['selectEnable'];
    $sql = "INSERT INTO Status (status_name, enable) VALUES ('$statusName', $enable)";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
