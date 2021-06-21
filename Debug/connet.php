<?php

if(trim($_POST['password'])!=trim($_POST['repassword'])){
 exit('两次密码不一致，请返回上一页');
} 
 //$_POST用户名和密码
 $username = $_POST['username'];
 $password = md5($_POST['password']);
 //连接mysql
 $con = mysqli_connect('localhost','root','root');
 //验证mysql连接是否成功
if(mysqli_errno($con)){
 echo "连接mysql失败：".mysqli_error($con);
 exit;
}
//设置数据库字符集
 mysqli_set_charset($con,'utf8');
 //查看数据库emp
 mysqli_select_db($con,'dvwa');
 //查看表sgbf用户名与密码和传输值是否相等
 $sql = "select * from users where user='$username' and password='$password'";
 //result必需规定由 mysqli_query()、mysqli_store_result() 或 mysqli_use_result() 返回的结果集标识符。
 $result = mysqli_query($con,$sql);
 $num = mysqli_num_rows($result);// 函数返回结果集中行的数量
 if($num){
  
 echo '登录成功';
 }else{
 echo'登录失败';
 }
 mysqli_close($con);
 ?>