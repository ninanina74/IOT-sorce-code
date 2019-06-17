<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['vendorName'] == null || $_REQUEST['vendorName'] == "") {
        throw new Exception("必須填入廠商名稱");
    }

    include("DB.php");
    $vendorName = $_REQUEST['vendorName'];
    $enable = $_REQUEST['selectEnable'];
    $sql = "INSERT INTO Vendor (vendor_name, enable) VALUES ('$vendorName', $enable)";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
