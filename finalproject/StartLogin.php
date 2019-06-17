<?php
session_start();
header('Content-type: text/html; charset=utf8'); 
try {
    include("DB.php");

    if (is_null($_REQUEST['userAccount']) && $_REQUEST['userAccount'] == "") {
        echo json_encode("必須輸入帳號");
        return;
    }

    if (is_null($_REQUEST['userPassword']) && $_REQUEST['userPassword'] == "") {
        echo json_encode("必須輸入密碼");
        return;
    }
    
    $userAccount = $_REQUEST['userAccount'];
    $userPassword = $_REQUEST['userPassword'];
    $sql = "SELECT * FROM Employee WHERE enable = 1 AND account = '$userAccount' AND password = '$userPassword'";

    $result = query($sql);
    
    if (count($result) > 0) {        
        $_SESSION['account'] = $result[0]['account'];
        echo json_encode($result);
    } else {
        $_SESSION['account'] = "";
        echo json_encode(null);
    }
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
