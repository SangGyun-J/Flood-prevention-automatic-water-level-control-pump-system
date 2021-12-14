<?php
    include "class.http.php";
    include "Services_JSON.php";
    include "class.EmmaSMS.php";

    $sms = new EmmaSMS();
    $sms_id = "smstest";
    $sms_passwd = "smstest";
    $sms->login($sms_id, $sms_passwd);    // $sms->login( [고객 ID], [고객 패스워드]);
    $point = $sms->point();

    if(preg_match("/[0-9]/",$point)) {
        echo "남은 건수는 : ".number_format($point)."건 입니다.";
    } else {
        echo "[에러] ".$sms->errMsg;
    }
?>