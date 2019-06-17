<?php
header('Content-type: text/html; charset=utf8'); 
try {
    if ($_REQUEST['changeVendor'] == null || $_REQUEST['changeVendor'] == "") {
        throw new Exception("必須填入廠商名稱");
    }

    include("DB.php");
    $vendorId = $_REQUEST['vendorId'];
    $changeVendor = $_REQUEST['changeVendor'];
    $changeEnable = $_REQUEST['changeEnable'];
    $sql = "UPDATE Vendor SET vendor_name = '$changeVendor', enable = $changeEnable WHERE vendor_id = $vendorId";
    insertOrUpdate($sql);
    echo json_encode("成功");
} catch (Exception $e) {
    echo json_encode($e->getMessage());
}
?>
