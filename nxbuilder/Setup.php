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
/* NX and NoMachine are trademarks of Medialogic S.p.A.                  */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/


session_start();

unset($_SESSION['error']);
unset($_SESSION['userName']);
unset($_SESSION['password']);
unset($_SESSION['identity']);

if( isset($_POST['skip_x']) || isset($_POST['builder_x']) )
{
 $_SESSION['userName'] = "nxbuilder";
 $_SESSION['password'] = "nxbuilder";
 $_SESSION['identity'] = "administrator";
 header("Location: nxbuilder.php");
 exit;
}


include ("HandleDB.php");

$shared="images/shared";
$serverShared="images/shared/servers";
$sessionShared="images/shared/sessions";

$button="images/buttons";
$sharedSkin="images/shared";
$toolbarSkin="images/toolbar";



include ("Messages.php");
$enableLogin="";
$enableUpdateImage=0;
$keyMessage="";
$error=0;
$imageResult="";

if(isset($_POST['path']) || isset($_POST['path2']) )
{

 $enableUpdateImage=1;
 
 $upIcon=updateIcon($_SESSION['hostDB_old'],$_SESSION['nameDB_old'],$_SESSION['userDB_old'],$_SESSION['passDB_old'], $_POST['path'], $_POST['path2']); 
 
 if($upIcon)
 {
  $keyMessage="UpdateImageSuccessifull";
  $imageResult="1";
 }
 else  
 {
  $keyMessage="UpdateImageFailed"; 
  $imageResult="0";
 }

}


if(isset($_REQUEST['operation']) && !isset($_REQUEST['detail_x']))
{

 if(trim($_POST['dbHost'])=="")
 {
  $keyMessage="EmptyHost";
  $error=1;
  logger("ERROR", $Message[$keyMessage]);
 }
 elseif(trim($_POST['dbName'])=="")
 {
  $keyMessage="EmptyDbName";
  logger("ERROR", $Message[$keyMessage]);
  $error=1;
 }
 elseif(trim($_POST['dbAdminUsername'])=="")
 {
  $keyMessage="EmptyAdministrator";
  $error=1;
  logger("ERROR", $Message[$keyMessage]);
 }
 

 if($error!=1)
 {
  $fileName= "builder.cfg";

  $content=
  '<?
  //
  // Specify the connection parameters for accessing the MySQL DB.
  //
  $dbHost = "'.$_REQUEST['dbHost'].'";
  $dbName = "'.$_REQUEST['dbName'].'";
  $dbAdminUsername = "'.$_REQUEST['dbAdminUsername'].'";
  $dbAdminPasswd = "'.$_REQUEST['dbAdminPasswd'].'";

  $dbconn = mysql_connect($dbHost, $dbAdminUsername, $dbAdminPasswd);

  @mysql_select_db($dbName) or die("Cannot connect to DB"); 
 
 
  $absPath="'.$_SERVER['DOCUMENT_ROOT'].dirname($_SERVER['PHP_SELF']).'";
  $absHost="http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF']).'"; 
 
  ?>';

  if (!$handle = @fopen($fileName, "w+"))
  {
   $keyMessage='createFile';
   $error=1;  
   logger("ERROR", $Message[$keyMessage]);
   //echo "error: cannot create file 'builder.cfg'";
   //exit;
  }

  if (!@fwrite($handle, $content))
  {
   $keyMessage='writeFile';
   $error=1;
   logger("ERROR", $Message[$keyMessage]);
   //echo "error: cannot write in this file : ($fileName)";
   //exit;
  }
  @fclose($handle); 
 } 

if($error!=1)
{  
 switch($_REQUEST['operation'])
 {
  case('create'):
  
   if($error!=1)
   {
    if(!createDB($_POST['dbHost'], $_POST['dbAdminUsername'],$_POST['dbAdminPasswd'],$_POST['dbName'], "1")) 
    {
     $keyMessage='errorCreateDb';
     $error=1;
    } 
    else
    {
     $keyMessage='CreatedDb';
     $error=0;
     $enableLogin="1";
    } 
   }   
  break;
  
  case('upgrade'):
  
     //make backup old DB
   $error=0; 
    
   if(trim($_POST['dbHost_old'])=="")
   {
    $keyMessage="EmptyHostOld";
    $error=1;
   }
   elseif(trim($_POST['dbName_old'])=="")
   {
    $keyMessage="EmptyDbNameOld";
    $error=1;
   }
   elseif(trim($_POST['dbAdminUsername_old'])=="")
   {
    $keyMessage="EmptyAdministratorOld";
    $error=1;
   }

    
     
   $crlf="\r\n";
     
   $link = mysql_connect($_POST['dbHost_old'], $_POST['dbAdminUsername_old'], $_POST['dbAdminPasswd_old']);
     
   if($link)
   {  
    $dbname = $_POST['dbName_old'];
     
    $database = mysql_select_db($dbname); 
     
    if($database) 
    { 
     $dump_buffer="";
     
     $tables = mysql_query("show tables from $dbname");
     $num_tables = mysql_num_rows($tables);
     
     if($num_tables == 0)
     {
      $keyMessage="TablesNotFound";
      $error=1;
     }
     else
     {
      $dump_buffer.= "# DatabaseBackup $crlf";
      $dump_buffer.= "# Backup made:$crlf";
      $dump_buffer.= "# ".date("F j, Y, g:i a")."$crlf";
      $dump_buffer.= "# Database: $dbname$crlf";
      $dump_buffer.= "# Backed up tables : $dbname $crlf";
      
      $i = 0;
      while($i < $num_tables)
      {
       $table = mysql_tablename($tables, $i);

       $dump_buffer.= "# --------------------------------------------------------$crlf";
       $dump_buffer.= "$crlf#$crlf";
       $dump_buffer.= "# Table structure for table '$table'$crlf";
       $dump_buffer.= "#$crlf$crlf";
       $db = $table;
       $dump_buffer.= get_table_def($table, $crlf,$dbname).";$crlf";
       $dump_buffer.= "$crlf#$crlf";
       $dump_buffer.= "# Dumping data for table '$table'$crlf";
       $dump_buffer.= "#$crlf$crlf";
       $tmp_buffer="";
       get_table_content($dbname, $table, 0, 0, 'my_handler', $dbname);
       $dump_buffer.=$tmp_buffer;
       
       $i++;
       $dump_buffer.= "$crlf";
      }
      
      
      
      $f_name="tmp/".$dbname.".sql";
      
      //check if file exist
      $suffix=1;
      while(file_exists($f_name))
      {
       $f_name= $f_name="tmp/".$dbname."-(".$suffix.").sql";
       $suffix++;
      }
      
      
      if (!$handle = fopen($f_name, "w"))
      {
        $keyMessage='ErrorCreateDump'; 
        $error=1;
      }
 
      if (!fwrite($handle, $dump_buffer))
      {
        $keyMessage='ErrorWriteDump'; 
        $error=1;
      }
      else
      {
        $keyMessage='WriteDumpConfirm';
      }     
      
      fclose($handle);
    }//end else
    
    if($error!=1)
    {
     $_SESSION['hostDB_old']=$_POST['dbHost_old'];
     $_SESSION['nameDB_old']=$_POST['dbName_old'];
     $_SESSION['userDB_old']=$_POST['dbAdminUsername_old'];
     $_SESSION['passDB_old']=$_POST['dbAdminPasswd_old'];
     
     if(!updateDB($_POST['dbHost'], $_POST['dbAdminUsername'],$_POST['dbAdminPasswd'],$_POST['dbName'] ,$_POST['dbHost_old'], $_POST['dbAdminUsername_old'],$_POST['dbAdminPasswd_old'],$_POST['dbName_old']))
     { 
      $keyMessage='ErrorUpdateDb';
     }  
     else
     {
      $keyMessage='UpdateDb';
      $enableUpdateImage=1;
     } 
    }
   }//end if($database)
   else
   {
    $error=1;
    $keyMessage='WrongDbName';   
   }  
  }// end if($link) 
  else
  {
   $error=1;
   $keyMessage='WrongDbCredit';
  }


  break;  
 
 } //end switch
} //end switch if($error!=1) 

}// end if(isset($_REQUEST['operation']))

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
<title> NX Builder</title>

<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<link rel="stylesheet" type="text/css" href="style/default.css" media="all" >
<link rel="stylesheet" type="text/css" href="style/general.css" media="all" >
<script src="js/browser.js"  language=javaScript type="text/javascript"></script>
<script src="js/function.js"  language=javaScript type="text/javascript"></script>
<script src="js/panel.js"  language=javaScript type="text/javascript"></script>
<link REL="icon" HREF="<?=$sharedSkin?>/favicon.ico" TYPE="image/gif">
</head>
<body style="background-color:#FFFAFF;">

<center>


<? 
 include("includes/Toolbardisable.php");
 $classLogin="logintable";

?>

<?
//if($keyMessage=="")
//{

?>
<form name="configure" id="configure" ecntype="multipart/form-data" action="<?=$_SERVER['PHP_SELF']?>" method="post" style="margin: 0px; padding: 0px;">

<?

if($imageResult!="")
{
?>
<table border="0" width="610" height="250" align="center" cellspacing="0" cellpadding="0" class="<?=$classLogin?>">
<tr>
  <td width="255" valign="top">
 &nbsp;
  </td>
  <td  valign="top">
   
    <table align="left" cellpadding="0" cellspacing="0"  width="100%" >
    <tr>
     <td height="15">

       <table cellSpacing="0" cellPadding="0" border="0" height="76">
       <tr>
         <td align="left" class="titleBlue">Manage your NX Builder backend</td>
       </tr>       
       <tr>
         <td align="left" class="titleBlue">Insert the existing absolute paths of the NXBuilder server and session icons to update the NXBuilder.</td>
       </tr>
       </table>     

       <table align="right" style="width:355px; margin-top:0px; margin-right: 0px;" id="contentSetup">
       <tr>
        <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
       </tr>
       </table>
     </td>
    </tr>   
    <tr>
     <td align="center">
<?
if($keyMessage!="")
{

?>
    <table  align="center" cellpadding="1" cellspacing="0" style="margin-top:15px; margin-bottom:15px;">       
    <tr>
      <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$Message[$keyMessage]?></td>
    </tr>   
    </table>

<?
}     
?>      
     
     </td>
    </tr>
    <tr>
     <td>
       <input type="image" style="margin-top:15px;" name="builder" width="82" height="17"  src="<?=$button?>/b_login.png" border="0" onmouseout="this.src='<?=$button?>/b_login.png'" onmouseover="this.src='<?=$button?>/b_loginOver.png'">
     </td>
    </tr> 
    </table>
  </td>
</tr>
</table>   
<?
}
elseif($enableUpdateImage==1)
{
?>
<table border="0" width="610" height="250" align="center" cellspacing="0" cellpadding="0" class="<?=$classLogin?>">
<tr>
  <td width="255" valign="top">
 &nbsp;
  </td>
  <td  valign="top">
   
    <table align="left" cellpadding="0" cellspacing="0"  width="100%" >
    <tr>
     <td height="15" colspan="2">

       <table cellSpacing="0" cellPadding="0" border="0" height="76">
       <tr>
         <td align="left" class="titleBlue">Manage your NX Builder backend</td>
       </tr>       
       <tr>
         <td align="left" class="titleBlue">Insert the existing absolute paths of the NXBuilder server and session icons to update the NXBuilder.</td>
       </tr>       
       <tr>
         <td align="left" class="titleBlue">Update existing icons</td>
       </tr>
       </table>     
 
       <table align="right" style="width:355px; margin-top:0px; margin-right: 0px;" id="contentSetup">
       <tr>
        <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
       </tr>
       <tr>
        <td  class="textblack" width="150">Absolute path existing NXBuilder server icons:</td>
        <td align="left"> <input type="text" class="textblack" style="width:200px;" name="path" id="path"></td>
       </tr>
       <tr>
        <td  class="textblack" width="150">Absolute path existing NXBuilder session icons:</td>
        <td align="left"> <input type="text" class="textblack" style="width:200px;" name="path2" id="path2"></td>
       </tr>
       </table>
     </td>
    </tr>   
    <tr>
     <td colspan="2" align="center">
<?
if($keyMessage!="")
{

?>
    <table border="0" align="center"  cellpadding="1" cellspacing="0" style="margin-top:15px; marin-bottom:15px;">       
    <tr>
      <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$Message[$keyMessage]?></td>
    </tr>   
    </table>
    
<?
}     
?>    
    
     </td>
    </tr>
    <tr>
     <td>
      <input name="continue" style="margin-top:15px;" type="image" width="82" height="17" src="<?=$button?>/b_continue.png" border="0" onmouseout="this.src='<?=$button?>/b_continue.png'" onmouseover="this.src='<?=$button?>/b_continueOver.png'">
     </td>
     <td>
       <input name="skip" type="image" style="margin-top:15px;" width="82" height="17" alt="skip" src="<?=$button?>/b_skip.png" border="0" onmouseout="this.src='<?=$button?>/b_skip.png'" onmouseover="this.src='<?=$button?>/b_skipOver.png'">
     </td>
    </tr> 
    </table>
  </td>
</tr>
</table>   

<?
}
else
{
?>

<table border="0" width="610" height="250" align="center" cellspacing="0" cellpadding="0" class="<?=$classLogin?>">
<tr>
  <td width="255" valign="top">
 &nbsp;
  </td>
  <td  valign="top">
   
    <table align="left" cellpadding="0" cellspacing="0"  width="100%" >
    <tr>
     <td height="15" colspan="2">

       <table cellSpacing="0" cellPadding="0" border="0" height="76">
       <tr>
         <td align="left" class="titleBlue">Manage your NX Builder backend</td>
       </tr>
       <tr>
         <td align="left" class="subTitleBlue">Choose 'Create DB' to set a new DB or 'Upgrade DB' to update the NX Builder backend.</td>
       </tr>
       </table>     
     
     
     </td>
    </tr>   
   </table>

    <div id="contentSetup" >

<?
 if((isset($_POST['operation']))&&($_POST['operation']=="upgrade"))
 {
?> 
    <table align="right" style="width:355px; margin-top:0px; margin-right: 0px;" id="contentSetup">
    <tr>
     <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
    </tr>
    <tr>
     <td height="15" colspan="2" >Data for new DB</td>
    </tr>
    <tr>
     <td  class="textblack" width="150">Bind-address:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbHost" value="<?if (isset($_POST['dbHost'])) {echo $_POST['dbHost'];} else echo 'localhost'; ?>" ></td>
    </tr>
    <tr>
     <td class="textblack" width="150">DB name:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbName" value="<?if (isset($_POST['dbName'])) {echo $_POST['dbName'];} else echo 'nxbuilderDB';?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Administrator:</td>
     <td align="left"><input type="text" class="textblack" style="width:200px;" name="dbAdminUsername"  value="<?if (isset($_POST['dbAdminUsername'])) {echo $_POST['dbAdminUsername'];}?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Password:</td>
     <td align="left"><input type="password" class="textblack" style="width:200px;" name="dbAdminPasswd" size="40" ></td>
    </tr>

    <tr>
     <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
    </tr>
    <tr>
     <td height="15" colspan="2" >Data existing DB</td>
    </tr>
    <tr>
     <td  class="textblack" width="150">Bind-address:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbHost_old" value="<?if (isset($_POST['dbHost_old'])) {echo $_POST['dbHost_old']; } else echo 'localhost'; ?>" ></td>
    </tr>
    <tr>
     <td class="textblack" width="150">DB name:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbName_old" value="<?if (isset($_POST['dbName_old'])) {echo $_POST['dbName_old'];} else echo 'nxbuilderDB';?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Administrator:</td>
     <td align="left"><input type="text" class="textblack" style="width:200px;" name="dbAdminUsername_old"  value="<?if (isset($_POST['dbAdminUsername_old'])) {echo $_POST['dbAdminUsername_old'];}?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Password:</td>
     <td align="left"><input type="password" class="textblack" style="width:200px;" name="dbAdminPasswd_old" size="40" ></td>
    </tr>
    <tr>
     <td colspan="2">
      <table cellpadding="0" cellspacing="0" border="0" width="350">
      <tr>
        <td>Create DB&nbsp;<input type="radio" name="operation" value="create" onclick="javascript: getSetupForm('setupRequest.php','0');"></td>
        <td>Upgrade DB&nbsp;<input type="radio" name="operation" value="upgrade" onclick="javascript: getSetupForm('setupRequest.php','1');" checked></td>
      </tr>
      </table>  
     </td> 
    </tr>    
    <tr>
     <td height="15" colspan="2" align="center" style="border-bottom:1px solid <?=$borderGrey?>;">&nbsp;
     </td>
    </tr>
    <tr>
     <td colspan="2" align="center">
<?
if($keyMessage!="")
{

?>

    <table border="0" align="center"  cellpadding="1" cellspacing="0" style="margin-top:15px; marin-bottom:15px;">       
    <tr>
      <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$Message[$keyMessage]?></td>
     <?
      if($error==1)
      {
     ?>
      <td width="50"><input type="image" name="detail" width="82" height="17" src="<?=$button?>/b_detail.png" border="0" onmouseout="this.src='<?=$button?>/b_detail.png'" onmouseover="this.src='<?=$button?>/b_detailOver.png'"></td>
     <?
      }
     ?>
    </tr>   
    </table>

<?
}     
?>      
     </td>
    </tr> 
     
    <tr>
     <td colspan="2" align="center">
      <input name="run" style="margin-top:15px;" type="image" width="82" height="17" src="<?=$button?>/b_run.png" border="0" onmouseout="this.src='<?=$button?>/b_run.png'" onmouseover="this.src='<?=$button?>/b_runOver.png'">
     </td>
   </tr>  
   </table> 
<? 
 }
 else
 {
?>    
    
     
    <table align="right" style="width:355px; margin-top:0px; margin-right: 0px;" id="contentSetup">
    <tr>
     <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
    </tr>
    <tr>
     <td  class="textblack" width="150">Bind-address:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbHost" value="<?if (isset($_POST['dbHost'])) {echo $_POST['dbHost'];} else echo 'localhost'; ?>" ></td>
    </tr>
    <tr>
     <td class="textblack" width="150">DB name:</td>
     <td align="left"> <input type="text" class="textblack" style="width:200px;" name="dbName" value="<?if (isset($_POST['dbName'])) {echo $_POST['dbName'];} else echo 'nxbuilderDB';?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Administrator:</td>
     <td align="left"><input type="text" class="textblack" style="width:200px;" name="dbAdminUsername"  value="<?if (isset($_POST['dbAdminUsername'])) {echo $_POST['dbAdminUsername'];}?>"></td>
    </tr>
    <tr>
     <td class="textblack" width="150" >Password:</td>
     <td align="left"><input type="password" class="textblack" style="width:200px;" name="dbAdminPasswd" size="40" ></td>
    </tr>
    <tr>
     <td colspan="2">
      <table cellpadding="0" cellspacing="0" border="0" width="350">
      <tr>
        <td>Create DB&nbsp;<input checked type="radio" name="operation" value="create" onclick="javascript: getSetupForm('setupRequest.php','0');"></td>
        <td>Upgrade DB&nbsp;<input type="radio" name="operation" value="upgrade" onclick="javascript: getSetupForm('setupRequest.php','1');"></td>
      </tr>
      </table>  
     </td> 
    </tr>    
    <tr>
     <td height="15" colspan="2" align="center" style="border-bottom:1px solid <?=$borderGrey?>;">&nbsp;
     </td>
    <tr>
     <td colspan="2" align="center">
<?
if($keyMessage!="")
{

?>
    <table border="0" align="center"  cellpadding="1" cellspacing="0" style="margin-top:15px; marin-bottom:15px;">       
    <tr>
      <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$Message[$keyMessage]?></td>
     <?
      if($error==1)
      {
     ?>
      <td width="50"><input type="image" name="detail" width="82" height="17" src="<?=$button?>/b_detail.png" border="0" onmouseout="this.src='<?=$button?>/b_detail.png'" onmouseover="this.src='<?=$button?>/b_detailOver.png'"></td>
     <?
      }
     ?>
    </tr>   
    </table>
<?
}     
?>      
     </td>
    </tr> 
    <tr>
     <td colspan="2" align="center">
     <?
       if($enableLogin!="1")
       {
     ?>
      <input name="run" style="margin-top:15px;" type="image" width="82" height="17" src="<?=$button?>/b_run.png" border="0" onmouseout="this.src='<?=$button?>/b_run.png'" onmouseover="this.src='<?=$button?>/b_runOver.png'">
     <?
       }
       else
       {
     ?>  
      <input type="image" style="margin-top:15px;" name="builder" width="82" height="17"  src="<?=$button?>/b_login.png" border="0" onmouseout="this.src='<?=$button?>/b_login.png'" onmouseover="this.src='<?=$button?>/b_loginOver.png'">       
     <?  
       }
     ?>        
     </td>
   </tr>  
   </table>
  
<?
 }
?> 
  </div>  
  
  </td>
</tr>
</table>
<?
}
?>

</form>
<?
if(isset($_POST['detail_x']))
{
?>
<div id="detailPanelLog" class="box_pop_up"  style="position:absolute;width: 590px; left: 100px; top:50px; display: block; z-index: 100">
<table width="100%" cellpadding="0" padding="0" border="0" class="bg">
<tr>
  <td id="handlePanelLog" class="handle" style="margin:0px; padding:0px; width:560px; height: 26px; display:block; cursor: move; line-height:26px; text-align:left; text-indent:15px;" >  
	  <span id="customTitle"  class="txtHeaderTable">Details</span>
  </td>
	 <td>
	  <a  onclick="javascript: document.getElementById('detailPanelLog').style.display='none';">
    <img style="margin-right: 10px; margin-top: 3px;" src="<?=$button?>/b_close.png" onmouseout="this.src='<?=$button?>/b_close.png'" onmouseover="this.src='<?=$button?>/b_closeOver.png'">
   </a>
  </td>
 </tr>
 </table>
 <table width="100%" height="300" cellpadding="0" padding="0" border="0" style="background-color: #ffffff;">
 <tr>
  <td valign="top" align="left">
   
    <div class="textblack"  style="background-color: #ffffff; text-align: left; padding: 0px 5px 5px 5px">
<?
 $fd = @fopen ("tmp/setup.log", "r");
 
 if(!$fd)
 {
  echo "Log file not found. Please check ownership and permission of directory '".$_SERVER['DOCUMENT_ROOT'].dirname($_SERVER['PHP_SELF'])."/tmp'";
 }
 else
 {
 while (!feof ($fd)) 
 {
    $buffer = @fgets($fd, 4096);
    $buffer=str_replace("ERROR", "<b>ERROR</b>", $buffer);
   if($buffer!="")
   {     
    echo $buffer."<br>";
   } 
 }
 @fclose ($fd);
 }
?> 
    </div><!--[if lte IE 6.5]><iframe></iframe><![endif]-->	
   </td>
 </tr>
 </table>  
</div>
<script language="javascript" src="js/drag.js"></script>
<script language="javascript">
	var limitW=(document.body.clientWidth-document.getElementById('detailPanelLog').clientWidth)-5;
	var limitH=(document.body.clientHeight-document.getElementById('detailPanelLog').clientHeight)-5;

	var cusUnixHandle = document.getElementById("handlePanelLog");
	var cusUnixRoot   = document.getElementById("detailPanelLog");
	Drag.init(cusUnixHandle, cusUnixRoot, 0, limitW, 0, limitH);
</script>

<?
}

?>

<? include("includes/Footer.php") ?>
