<?php
/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2005, 2011 NoMachine, http://www.nomachine.com.           */
/*                                                                        */
/* NXBUILDER, NX protocol compression and NX extensions to this software  */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/
header("Cache-Control: no-store, no-cache, must-revalidate");  
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          


 session_start();
 session_unset();
 session_destroy();


if(isset($_REQUEST['logOff_x']))
{
 session_start();
 session_unset();
 session_destroy();
 @header("Location: nxbuilder.php");
 exit;
}


session_start();

unset($_SESSION['error']);
unset($_SESSION['userName']);
unset($_SESSION['password']);


include("Messages.php");


$existCFG= include ("builder.cfg");
    
if($existCFG!=1)
{
 $_SESSION['error'] = 'builder.cfg not found'; 
 @header("Location: nxbuilder.php");
 exit;    
}
else if(isset($_POST['guestUser']))
{

  
  $id = "administrator";
  $query="SELECT * from users WHERE userName='guest' AND identity='guest'";
  $result=mysql_query($query,$dbconn);
  $row = mysql_fetch_array ($result);

  if($row)
  {
    $_SESSION['identity'] = 'guest'; 
    $_SESSION['userName'] = 'Guest Builder';
    $_SESSION['password'] = 'guest';  
    $_SESSION['credentialsNX']=$row['credentialsNX'];
    $_SESSION['idUser']=$row['id'];
    unset($_SESSION['error']);
    mysql_close($dbconn);
    @header('Location:ServerList.php');
    exit;
  }  
  else
  {
    $_SESSION['error']="No Guest user is enabled";
    mysql_close($dbconn);
    @header('Location:nxbuilder.php');
    exit;
  }
  
  
  
  
}
else if(isset($_POST['userName']) && isset($_POST['password']))
{
  
  $userName = $_POST['userName'];
  $password = $_POST['password'];
  $userName = addslashes($userName);
  $passwd = md5($password);


       
  if(!empty($userName) && !empty($_POST['password']))
  {

    $id = "administrator";
    $query="SELECT * from users WHERE userName='$userName' AND password='$passwd' AND identity='$id'";
    $result=mysql_query($query,$dbconn);
    $row = mysql_fetch_array ($result);



    
    if($row)
    {
      $_SESSION['userName'] = $_POST['userName'];
      $_SESSION['password'] = $_POST['password'];
      $_SESSION['credentialsNX']=$row['credentialsNX'];
      $_SESSION['identity'] = "administrator";
      $_SESSION['idUser']=$row['id'];
      unset($_SESSION['error']);
      mysql_close($dbconn);
      @header('Location:nxbuilder.php');
      exit;
    }
    else
    {
      $query1="SELECT * from users WHERE userName='$userName' AND password='$passwd'";
      $result1=mysql_query($query1,$dbconn);

      $row1 = mysql_fetch_array ($result1);
      if($row1)
      {
        $_SESSION['userName'] = $_POST['userName'];
        $_SESSION['password'] = $_POST['password'];
        $_SESSION['credentialsNX']=$row1['credentialsNX'];
        $_SESSION['identity'] = $row1['identity'];
        $_SESSION['idUser']=$row1['id'];
        unset($_SESSION['error']);
        mysql_close($dbconn);
        @header('Location:ServerList.php');
        exit;
      }
      else
      {
        
        $_SESSION['error']= $errorLogin;
        @header('Location:nxbuilder.php');
        exit;
      }
    }
    
  }
  else
  {
    /*if (trim($_POST['userName']) == "")
      $_SESSION['authentication']='emptyLogin';
    else
      $_SESSION["authentication"]= "emptyPassword";*/
       if(empty($userName)) { 
       $_SESSION['error'] = $emptyLogin;
       }
       if(empty($_POST['password'])) { 
       $_SESSION['error'] = $emptyPassword;
       }
       if(empty($userName) && empty($_POST['password'])) { 
       $_SESSION['error']  = $emptyLogPass; 
       }
    @header('Location:nxbuilder.php');
    exit;
  }
}
//user and psw are empty
else
{
 if(empty($userName)) 
 { 
  $_SESSION['error'] = $emptyLogin;
 }
 if(empty($_POST['password'])) 
 { 
  $_SESSION['error'] = $emptyPassword;
 }
 if(empty($userName) && empty($_POST['password'])) 
 { 
  $_SESSION['error']  = $emptyLogPass;
 }
 
 @header('Location:nxbuilder.php');
 exit;
}

?>
