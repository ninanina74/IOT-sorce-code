<?php
header('Content-type: text/html; charset=utf8'); 
try {
    $sql = "SELECT B.location_name, C.status_name, D.device_name, E.item_serial, A.create_time FROM Log A, Location B, Status C, Device D, Item E WHERE A.location_id = B.location_id AND A.status_id = C.status_id AND A.device_id = D.device_id AND A.item_id = E.item_id";

    if ($_REQUEST['disabledPeriod'] == "No") {
        if (!is_null($_REQUEST['dateFrom']) && $_REQUEST['dateFrom'] != "") {
            $sql = $sql . " AND A.create_time >= " . $_REQUEST['dateFrom'];    
        }

        if (!is_null($_REQUEST['dateTo']) && $_REQUEST['dateTo'] != "") {
            $sql = $sql . " AND A.create_time <= " . $_REQUEST['dateTo'];    
        }
    }

    if ($_REQUEST['location'] != "ALL") {
        $sql = $sql . " AND A.location_id = " . $_REQUEST['location'];
    }

    if ($_REQUEST['status'] != "ALL") {
        $sql = $sql . " AND A.status_id = " . $_REQUEST['status'];
    }

    if ($_REQUEST['device'] != "ALL") {
        $sql = $sql . " AND A.device_id = " . $_REQUEST['device'];
    }

    include("DB.php");
    echo json_encode(query($sql));
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
