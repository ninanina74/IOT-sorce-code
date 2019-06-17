<?php
header('Content-type: text/html; charset=utf8'); 
try 
{
    if ($_REQUEST['changeStatus'] == null || $_REQUEST['changeStatus'] == "") 
    {
        throw new Exception("必須填入狀態名稱");
    }

    include("DB.php");
    $statusId = $_REQUEST['statusId'];
    $changeStatus = $_REQUEST['changeStatus'];
    $changeEnable = $_REQUEST['changeEnable'];
    $sql = "UPDATE Status SET status_name = '$changeStatus', enable = $changeEnable WHERE status_id = $statusId";
    insertOrUpdate($sql);
    echo json_encode("成功");
} 
catch (Exception $e) 
{
    echo json_encode($e->getMessage());
}
?>
