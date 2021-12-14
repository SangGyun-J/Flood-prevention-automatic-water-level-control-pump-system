<<?php
function send(){
    $url = "http://www.all-tafp.org/EmmaSMS_PHP_UTF-8_JSON/example.php?sms_msg=메세지테스트용입니다.&sms_to=010-8338-2385&sms_from=010-8338-2385&sms_date=";
    $ch = curl_init();

    curl_setopt($ch,CURLOPT_URL,$url);
    curl_setopt($ch,CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch,CURLOPT_NOSIGNAL, 1);

    $data = curl_exec($ch);
    $curl_errno = curl_errno($ch);
    $curl_error = curl_error($ch);

    curl_close($ch);

  }

 ?>
