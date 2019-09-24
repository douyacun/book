/**
* 自定义会话管理器
* @author 刘宁
* @date 2017/04/08
**/
函数 session_set_save_handler() 的参数即为在会话生命周期内要调用的一组回调函数： open， read， write 以及 close。 还有一些回调函数被用来完成垃圾清理：destroy 用来删除会话， gc 用来进行周期性的垃圾收集。
因此，会话保存管理器对于 PHP 而言是必需的。 默认情况下会使用内置的文件会话保存管理器。

因此，会话保存管理器对于 PHP 而言是必需的。 默认情况下会使用内置的文件会话保存管理器。 可以通过 session_set_save_handler() 函数来设置自定义会话保存管理器。 一些 PHP 扩展也提供了内置的会话管理器，例如：sqlite， memcache 以及 memcached， 可以通过配置项 session.save_handler 来使用它们。
会话开始的时候，PHP 会调用 open 管理器，然后再调用 read 回调函数来读取内容，该回调函数返回已经经过编码的字符串。 然后 PHP 会将这个字符串解码，并且产生一个数组对象，然后保存至 $_SESSION 超级全局变量。
当 PHP 关闭的时候（或者调用了 session_write_close() 之后）， PHP 会对 $_SESSION 中的数据进行编码， 然后和会话 ID 一起传送给 write 回调函数。 write 回调函数调用完毕之后，PHP 内部将调用 close 回调函数。
销毁会话时，PHP 会调用 destroy 回调函数。

根据会话生命周期时间的设置，PHP 会不时地调用 gc 回调函数。 该函数会从持久化存储中删除超时的会话数据。 超时是指会话最后一次访问时间距离当前时间超过了 $lifetime 所指定的值。


PHP session works with database

//Database
CREATE TABLE `Session` (
  `Session_Id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `Session_Expires` datetime NOT NULL,
  `Session_Data` text COLLATE utf8_unicode_ci,
  PRIMARY KEY (`Session_Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SELECT * FROM mydatabase.Session;

<?php
/**
 * seesion 学习测试
 * @author 刘宁
 * @date   2017-04-12
 */
namespace SysSession;

class SysSession implements \SessionHandlerInterface
{
    private $link;

    public function open($savePath, $sessionName)
    {
        $link = mysqli_connect("192.168.1.237","root","a123456");
        mysqli_set_charset($link,'utf8');
        mysqli_select_db($link,'douyacun');
        if($link){
            $this->link = $link;
            return true;
        }else{
            return false;
        }
    }
    public function close()
    {
        mysqli_close($this->link);
        return true;
    }
    public function read($id)
    {
        // mysqli_select_db($this->link,'douyacun');
        $result = mysqli_query($this->link,"SELECT Session_Data FROM Session WHERE Session_Id = '".$id."' AND Session_Expires > '".date('Y-m-d H:i:s')."'");
        if($row = mysqli_fetch_assoc($result)){
            return $row['Session_Data'];
        }else{
            return "";
        }
    }
    public function write($id, $data)
    {
        $DateTime = date('Y-m-d H:i:s');
        $NewDateTime = date('Y-m-d H:i:s',strtotime($DateTime.' + 1 hour'));
        $result = mysqli_query($this->link,"REPLACE INTO Session SET Session_Id = '".$id."', Session_Expires = '".$NewDateTime."', Session_Data = '".$data."'");
        if($result){
            return true;
        }else{
            return false;
        }
    }
    public function destroy($id)
    {
        $result = mysqli_query($this->link,"DELETE FROM Session WHERE Session_Id ='".$id."'");
        if($result){
            return true;
        }else{
            return false;
        }
    }
    public function gc($maxlifetime)
    {
        $result = mysqli_query($this->link,"DELETE FROM Session WHERE ((UNIX_TIMESTAMP(Session_Expires) + ".$maxlifetime.") < ".$maxlifetime.")");
        if($result){
            return true;
        }else{
            return false;
        }
    }
}
?>

<?php
/**
 * session
 * @author 刘宁
 * @date   2017-04-12
 */
namespace app\blog\controller;
use SysSession\SysSession;
use think\Controller;
$handler = new SysSession();
session_set_save_handler($handler, true);
session_start();
$_SESSION['user_name'] = '刘宁';
