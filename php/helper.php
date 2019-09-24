<?php

/**
 * 发送post请求
 * @param string $url
 * @param array $requestData
 * @return mixed
 *
 */
function _httpPost($url="" ,$requestData=array()){

    $curl = curl_init();

    curl_setopt($curl, CURLOPT_URL, $url);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);

    //普通数据
    curl_setopt($curl, CURLOPT_POSTFIELDS, http_build_query($requestData));

    //发送json数据
    //$requestData = '{"name":"hello","age":122,"arr":{"arrid":44,"name":"world","test":[333,444,555,66,"xxdfads"]}}';
    //curl_setopt($curl, CURLOPT_HTTPHEADER, array('Content-Type: application/json', 'Content-Length:' . strlen($requestData)));
    //curl_setopt($curl, CURLOPT_POSTFIELDS, $requestData);
    //服务器端接收json数据  file_get_contents('php://input');

    $res = curl_exec($curl);
    curl_close($curl);

    return $res;
}


/**
 * 接收数据流
 */
$http_raw_post_data = isset($GLOBALS['HTTP_RAW_POST_DATA'])?$GLOBALS['HTTP_RAW_POST_DATA']:'';
if(empty($http_raw_post_data))
{
    $http_raw_post_data = simplexml_load_string(file_get_contents('php://input'));
}


?>