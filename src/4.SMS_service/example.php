<?php
    include "class.http.php";
    include "class.EmmaSMS.php";

    if($_SERVER['REQUEST_METHOD'] == "GET") {

        $sms_id = "rodemeng";
        $sms_passwd = "iccad2177#";
        $sms_to = $_GET['sms_to'];
        $sms_from = $_GET['sms_from'];
        $sms_date = $_GET['sms_date'];
        $sms_msg = $_GET['sms_msg'];
        $sms_type = "L";    // 설정 하지 않는다면 80byte 넘는 메시지는 쪼개져서 sms로 발송, L 로 설정하면 80byte 넘으면 자동으로 lms 변환

        $sms = new EmmaSMS();
        $sms->login($sms_id, $sms_passwd);
        $ret = $sms->send($sms_to, $sms_from, $sms_msg, $sms_date, $sms_type);

        print_r($ret);
    }
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
    <title>[후이즈 호스팅] SMS 호스팅 - PHP UTF-8 모듈 테스트</title>
    <meta name="generator" content="whoisweb">
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
<style>
    legend { font-size:12px; font-weight:bold; font-family:tahoma }
    td { font-size:12px; font-family:tahoma }
</style>
</head>

<body>
<form method="get">
<fieldset style="width:300">
<legend>문자메시지</legend>
<table>
<tr>
    <td width="100">메시지</td>
    <td><textarea name="sms_msg" rows="5" cols="20" readonly>[후이즈 호스팅] SMS 호스팅 - PHP UTF-8 모듈 테스트입니다.</textarea></td>
</tr>
<tr>
    <td>받는 사람 번호</td>
    <td><input name="sms_to"></td>
</tr>
<tr>
    <td>보내는 사람 번호</td>
    <td><input name="sms_from" value="1588-4259"></td>
</tr>
<tr>
    <td>예약시간</td>
    <td>
    <!-- 예약시에는 20160105180000 (년월일시분초) 형태로 넣어주세요. -->
    <select name="sms_date">
    <option value="">즉시발송
    <option value="20160105180000">2016년 01월 05일 18시
    </select>
    </td>
</tr>
</table>
</fieldset>
<table>
<tr>
    <td><input type="submit" value="보내기"></td>
</tr>
</table>
</form>
</body>
</html>
