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
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/

header("Cache-Control: no-store, no-cache, must-revalidate");  
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          

if (!isset($_SESSION['userName'])) session_start();


include ("HandleDB.php");
include ("conf.php");
include ("Messages.php");
checkPermission();


$messageType="";
$error=false;

if(isset($_POST['addUser_x']))
{

  if ((isset($_POST["userName"]) and isset($_POST["password"]) and isset($_POST["repeatPassword"]) ) || ($_POST['userType']=="guest") )
  {
    if((trim($_POST["userName"]) != "" and trim($_POST["password"]) != "") || ($_POST['userType']=="guest"))
    {
      if( ($_POST["password"] == $_POST["repeatPassword"]) || ($_POST['userType']=="guest"))
      {

        if (isset($_POST['credentialsNX'])) $credentialsNX='2';
        elseif(isset($_POST['logUserGuest']))  $credentialsNX='1';
        else $credentialsNX='0';  


        // access mode to NX server 
        // 0: client get login box
        // 1: access as guest user
        // 2: access with web credentials 

        $sessionListIns=array();
        
        $list=getServer();
        
        while($serverListIns=mysql_fetch_array($list))
        {
         $ind="sess-".$serverListIns['id'];
         if(isset($_POST[$ind]))
         {
          $split=explode(",",$_POST[$ind]);
          
          for($i=0; $i<count($split); $i++)
          {
           if($split[$i]!="")
           {
            $sessionListIns[]=$split[$i];
           }
          } 
         }
        }
       
       if($_POST['userType']=="guest")
       {
        $_POST['userName']="guest";
        $_POST['password']="";
       }
        
       if(!InsertUserList($_POST['userName'],md5($_POST['password']), $_POST['userType'],$credentialsNX))
       {
        $messageType="ErrorAddUser";
        $error=true;
       }
       
       InsertUserSession($_POST["userName"],md5($_POST['password']),$sessionListIns);
       
       if(!$error)
       {
        @header('Location: UserList.php'); 
        $messageType="AddedUser";
       } 
      }
      else
      {
        $error=true;
        $messageType="ErrorConfirmPassword";
      }
    }
    else
    {
      $error=true;
      
      if(trim($_POST["userName"]) == "")
        $messageType="ErrorEmptyUsername";
      else
        $messageType="ErrorEmptyPassword";
    }
  }
  else
  {
    $error=true;
    $messageType="ErrorUserUnknown";
  }
}

$dis_userName="";
$dis_password="";
$dis_repeatPassword="";

$check_logUserGuest=" ";
$check_credentialsNX=" ";
$dis_credentialsNX=" ";
$dis_logUserGuest=" "; 
if($error)
{



 if($_POST['userType']=="guest")
 {
  $dis_userName=" disabled";
  $dis_password=" disabled";
  $dis_repeatPassword=" disabled"; 
 }
 



 if(isset($_POST['credentialsNX']))
 {
  $check_credentialsNX=" checked";
  $check_logUserGuest=" ";
  $dis_logUserGuest=" disabled";
 }
 elseif(isset($_POST['logUserGuest']))
 {
  $check_logUserGuest=" checked";
  $check_credentialsNX=" ";
  $dis_credentialsNX=" disabled";
 }
 else
 {
  $check_logUserGuest=" ";
  $check_credentialsNX=" ";
  $dis_credentialsNX=" ";
  $dis_logUserGuest=" "; 
 }

}

include ("includes/Top.php");
?>

<center>

<script>

<!--


function DisableGuestDetails()
{
 var selectUserType = document.getElementById('userType');
 var userName = document.getElementById('userName');
 var password = document.getElementById('password');
 var confirmPsw = document.getElementById('repeatPassword');
 var credentials = document.getElementById('credentialsNX');
 var logUserGuest = document.getElementById('logUserGuest');
 var normalUser = document.getElementById('normalUser');
 var administratorUser = document.getElementById('administratorUser');
 var serverList = document.getElementById('server');
 if(selectUserType.value == "guest") 
 {
   userName.disabled="disabled";
   password.disabled="disabled";
   confirmPsw.disabled="disabled";
   credentials.disabled="disabled";
   logUserGuest.disabled=false;
   normalUser.style.display=" block";
   administratorUser.style.display=" none";
   serverList.disabled=false;
   
 }
 else
 {

  userName.disabled="";
  password.disabled="";
  confirmPsw.disabled="";
  credentials.disabled="";
  logUserGuest.disabled="";
 
  if(selectUserType.value != "administrator") 
  {  
    normalUser.style.display=" block";
    administratorUser.style.display=" none";
    serverList.disabled=false;
  }
  else if(selectUserType.value == "administrator") 
  {
    normalUser.style.display="none";
    administratorUser.style.display="block";
    serverList.disabled=true;
  }
  
  
  
 }

}


var NS4 = (navigator.appName == "Netscape" && parseInt(navigator.appVersion) < 5);


function addHidden(theValue, id)
{
   
  var hiddenSesion= document.getElementById(id);
   
   if(hiddenSesion.value=="")
   {
    hiddenSesion.value+=theValue;
   }
   else
   {
     hiddenSesion.value+=","+theValue;
   }


}

function removeHidden(theValue, id)
{
  var hiddenSesion= document.getElementById(id);
   
  var session=hiddenSesion.value.split(",");
  
  session = hiddenSesion.value.split(",");
  hiddenSesion.value="";
	 for (i=0;i<session.length;i++) 
  {
   if((session[i]!=theValue) && (session[i]!=""))
   {
     if(hiddenSesion.value=="")
     {
      hiddenSesion.value+=session[i];
     }
     else
     {
       hiddenSesion.value+=","+session[i];
     }      
   } 
 	};
}



function addOption(theSel, theText, theValue)
{
  var newOpt = new Option(theText, theValue);
  var selLength = theSel.length;
  theSel.options[selLength] = newOpt;
}

function deleteOption(theSel, theIndex)
{ 
  var selLength = theSel.length;
  if(selLength>0)
  {
    theSel.options[theIndex] = null;
  }
}


function setHidden()
{ 
 var serverVal=document.getElementById('server').value;
 var sessionUser=document.getElementById('sessionUser');
 var hiddenSession=document.getElementById('sess-'+serverVal);
 var i; 
 hiddenSession.value="";

   
 for(i=0; i<sessionUser.length; i++)
 {
 
  if(i==0)
  {
   hiddenSession.value+=sessionUser[i].value;
  }
  else
  {
    hiddenSession.value+=","+sessionUser[i].value;
  } 
 } 
}


function moveOptions(theSelFrom, theSelTo, operation)
{
  
  var selLength = theSelFrom.length;
  var selectedText = new Array();
  var selectedValues = new Array();
  var selectedCount = 0;

  if(operation=="1")
  {
    for(var i=0; i< theSelFrom.length; i++)
    {
     theSelFrom[i].selected=true;
    }
  }
  
  var i;
  
  for(i=selLength-1; i>=0; i--)
  {
    if(theSelFrom.options[i].selected)
    {
      selectedText[selectedCount] = theSelFrom.options[i].text;
      selectedValues[selectedCount] = theSelFrom.options[i].value;
      deleteOption(theSelFrom, i);
      selectedCount++;
    }
  }
  

  for(i=selectedCount-1; i>=0; i--)
  {
    addOption(theSelTo, selectedText[i], selectedValues[i]);
  }
  setHidden();
  if(NS4) history.go(0);
}

function checkSession(choose) 
{
		 var s=new String;
		 var session = new Array();
		 var cont=0;
		 var resCheck= false;

		 idServer=document.getElementById('server').value;
 
	  var inputHidden=document.getElementById('sess-'+idServer);
  
   s=inputHidden.value;
   session = s.split(",");

		 
		 for (i=0;i<session.length;i++) 
   {
		   if((session[i]==choose) && (session[i]!=""))
		   {
		     resCheck= true;
		     break;
		   } 
			};

			return  resCheck;			 
};

function setSession(chooser) 
{
 
 var selectRight = document.getElementById("sessionUser");
 
 while (selectRight.options.length) 
 {
     selectRight.remove(0);
 }
 
 var session = new Object()
 
<? 

   $serverList=getServer();
 
   while($server=mysql_fetch_array($serverList))
   {  
    $paramIdServer=$server['id'];
echo'session['.$server['id'].'] = ['."\n";          
    
    $queryStringSession="SELECT * FROM  sessions  WHERE idServer = '$paramIdServer' ";         //"Select * from sessions ";

    
    $result = mysql_query($queryStringSession);
    $row = mysql_fetch_array($result);  
    $num=mysql_num_rows($result);   
    $cont=1;
    
    
    while($row)
    {
     if($cont<$num)
     {
?>
         {value:"<?=$row['id']?>", text:"<?=$row['sessionName']?>"},<? 
     }
     else
     {
?>     
         {value:"<?=$row['id']?>", text:"<?=$row['sessionName']?>"}
<?     
     }

     $cont++;
     $row = mysql_fetch_array($result);    
    
    
    }
                    echo "];"."\n";
  }      

?>

 var newElem;
 var where = (navigator.appName == "Microsoft Internet Explorer") ? -1 : null;
 var sessionChooser = chooser.form.elements["sessionServer"];
 
 while (sessionChooser.options.length) 
 {
     sessionChooser.remove(0);
 }
 var choice = chooser.options[chooser.selectedIndex].value;

 
 var sessionElem = session[choice];

 for (var i = 0; i < sessionElem.length; i++) 
 {
  if(checkSession(sessionElem[i].value)==false) //,serverSelected
  {
   newElem = document.createElement("option");
   newElem.text = sessionElem[i].text;
   newElem.value = sessionElem[i].value;
   sessionChooser.add(newElem, where);
  }
 }


 var sessionElemRight = document.getElementById('sessionUser');
 
 var indexServer = document.getElementById('server').value;

	var inputHidden=document.getElementById('sess-'+indexServer);
	
	var s=new String;
	var sessionHidden = new Array();
	
 s=inputHidden.value;
 
 sessionHidden = s.split(",");
 
 
 var sessionElem = session[indexServer];
  
 for (var k = 0; k < sessionHidden.length; k++) 
 {
 
  for (var i = 0; i < sessionElem.length; i++) 
  {
   if(sessionElem[i].value==sessionHidden[k]) 
   {
    newElem = document.createElement("option");
    newElem.text = sessionElem[i].text;
    newElem.value = sessionElem[i].value;
    sessionElemRight.add(newElem, where);
   }
  }
 }
}

//-->
</script>
<!--[if IE 6]>
<script type="text/javascript">
function calcWidth() {
document.getElementById('min_width').style.width = (document.body.clientWidth < 901 ? '900px' : '100%')
}
onresize = calcWidth;
</script>
<![endif]-->
<? include("includes/Toolbar.php");?>
<div id="min_width">
<!--SectionIcon Start-->
<form name="create" ecntype="multipart/form-data" action="AddUser.php" method="post" >
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-addUser.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Add User</td>
</tr>
</table>
<!--SectionIcon Stop-->


<table  cellSpacing="0" cellPadding="0" width="98%" style="min-width:900px; margin-top:20px;">
 <tr valign="top">
  <!-- start left box -->
  <td width="48%" align="left"> 
  
    <table width="98%" cellspacing="0" cellpadding="0"  height="26" class="bgHeaderTable">
     <tr>
       <td class="txtHeaderTable">&nbsp;User Details</td>
     </tr>
    </table>
    <table width="98%" height="170" cellspacing="0" cellpadding="4" border="0" class="bgBoxInternal">
    <tr>
       <td width="35%" class="txtLabel" align="right">User Type:</td>
       <td colspan="3">
        <select class="textBlack" name="userType" id="userType" onchange="DisableGuestDetails()">
         <option value="user"  <?if( isset($_POST['userType']) && ($_POST['userType']=="user")) echo " selected"; ?> >User</option>
         <option value="guest" <?if( isset($_POST['userType']) && ($_POST['userType']=="guest")) echo " selected"; ?> >Guest Builder User</option>
         <option value="administrator" <?if( isset($_POST['userType']) && ($_POST['userType']=="administrator")) echo " selected"; ?> >Administrator</option>
        </select>
       </td>
     </tr>
     <tr> 
       <td width="35%" class="txtLabel" align="right">Username:</td>
       <td colspan="3" align="left"><input type="text" name="userName" id="userName"  size="40"  <?=$dis_userName?> class="textBlack" value="<?=$_POST['userName']?>" ></td>
     </tr>
     <tr>
       <td width="35%" class="txtLabel" align="right">Password:</td>
       <td colspan="3" align="left"><input type="password" name="password" id="password" size="40"  <?=$dis_password?> class="textBlack" value=""></td>
     </tr>
     <tr>
       <td width="35%" class="txtLabel" align="right" >Confirm password:</td>
       <td colspan="3" align="left"><input type="password" name="repeatPassword" id="repeatPassword" <?=$dis_repeatPassword?> size="40" class="textBlack" value=""></td>
     </tr>
     <tr>
       <td><img src="<?=$shared?>/empty.png" width="1" height="1"></td>
       <td width="15"  align="left"><input style="margin:0px;padding:0px;cursor:pointer" type="checkbox" <?=$check_credentialsNX?> <?=$dis_credentialsNX?> name="credentialsNX" id="credentialsNX" value="2" onclick="javascript: checkBox(this);"></td>
       <td colspan="2" class="txtLabel">Log to NX Server using web credentials.</td> 
     
     </tr>
     <tr>
       <td><img src="<?=$shared?>/empty.png" width="1" height="1"></td>
       <td width="15" align="left"><input style="margin:0px;padding:0px;cursor:pointer" type="checkbox" <?=$check_logUserGuest?> <?=$dis_logUserGuest?>  name="logUserGuest" id="logUserGuest" value="1" onclick="javascript: checkBox(this);"></td>
       <td colspan="2" class="txtLabel">Log to NX Server as a Guest User.</td>
     </tr>
    </table>
  
  
  
  </td>
  <!-- start right box -->
  <td width="48%" align="right"> 

  <table border="0" width="98%" cellspacing="0" cellpadding="0"  height="26" class="bgHeaderTable">
   <tr>
     <td height="26" class="txtHeaderTable">&nbsp;Deployed Sessions</td>
   </tr>
  </table>
  
  <table width="98%" height="170" bgcolor="#F8F6E9" cellspacing="0" cellpadding="4" class="bgBoxInternal">
   <tr>
    <td width="35%" class="txtLabel">NX Server:</td>
    <td class="textBlack"> 
     <select class="txtLabel" style="width:200px;" name="sever" id="server" onchange="javascript: setSession(this);">
<?
$serverList=getServer();

$hiddenSession="";
$keyHidden=0;

if(!$serverList)
{
echo "<option value=\"empty\">Servers Not Availble</option>";
}
else
{
  $hiddenSession="";
  $server=mysql_fetch_array($serverList);
  $paramIdServer=$server['id'];
  while($server) 
  {
   
   
   
   if(!isset($_POST[$keyHidden])) 
   {
    $conc="";
   }
   else
   {
    $conc=$_POST[$keyHidden];
   }  
  
   $keyHidden='sess-'.$server['id'];
  
   $hiddenSession.='<input type="hidden" name="'.$keyHidden.'" id="'.$keyHidden.'" value="'.$conc.'">'."\n";
   
?>
       <option value="<?=$server['id']?>"><?=$server['hostName']?></option>
<?
   $server=mysql_fetch_array($serverList);
  }
}
?>
     </select>
    </td>
   </tr>
   <tr>
    <td colspan="2">


    <div id="normalUser" style="display:block">
     <table cellSpacing="0" cellPadding="4" border="0" id="admin">
      <tr>
        <td colspan="3" class="txtLabel">NX Sessions:</td>
      </tr>
      <tr>
        <?php
        
          $result=getSessionsList($paramIdServer);
          $row = mysql_fetch_array($result);

          $chosen=array();
          $keyHidden='sess-'.$paramIdServer;
          
          
          if(isset($_POST[$keyHidden]))
          {
           $chosen=explode(",", $_POST[$keyHidden]);
          }


        ?>  
        <td class="txtLabel" align="center">
      
          <select name="sessionServer" class="textBlack"  id="lp"  multiple="multiple"  size="6"  style="width:150px;" ondblclick="moveOptions(this.form.sessionServer, this.form.sessionUser, '0');">
       <? 
           
          while ($row)
          {
           if(!in_array($row['id'], $chosen))
           {
            echo '<option  value="'.$row['id'].'">'.$row['sessionName'].'</option>'."\n";
           } 
           $row = mysql_fetch_array($result);
          }
          
          
          
          
       ?>  
         </select>
        </td>
        <td class="txtLabel" align="center">

        <span id="b1"><img src="<?=$button?>/b_removePartial.png" style="cursor: pointer;" id="remove" name="left" value=" " onclick="javascript: moveOptions(document.forms[0].sessionUser,document.forms[0].sessionServer, '0');" onmouseout="this.src='<?=$button?>/b_removePartial.png';" onmouseover="this.src='<?=$button?>/b_removePartialOver.png';"></span>
        <span id="b2"><img src="<?=$button?>/b_addPartial.png" style="cursor: pointer;" id="add" name="left" value=" " onclick="javascript: moveOptions(document.forms[0].sessionServer,document.forms[0].sessionUser, '0');" onmouseout="this.src='<?=$button?>/b_addPartial.png';" onmouseover="this.src='<?=$button?>/b_addPartialOver.png';"></span><BR>
        <span id="b3"><img src="<?=$button?>/b_removeAll.png" style="cursor: pointer;" id="allRemove" name="left" value=" " onclick="javascript: moveOptions(document.forms[0].sessionUser,document.forms[0].sessionServer, '1');" onmouseout="this.src='<?=$button?>/b_removeAll.png';" onmouseover="this.src='<?=$button?>/b_removeAllOver.png';"></span>
        <span id="b4"><img src="<?=$button?>/b_addAll.png" style="cursor: pointer;" id="allAdd" name="left" value=" " onclick="javascript: moveOptions(document.forms[0].sessionServer,document.forms[0].sessionUser, '1');" onmouseout="this.src='<?=$button?>/b_addAll.png';" onmouseover="this.src='<?=$button?>/b_addAllOver.png';"></span>

   <!--
        <input style="cursor: pointer;" src="<?=$button?>/b_removePartial.png" id="remove" name="left" value=" " onclick="moveOptions(this.form.sessionUser,this.form.sessionServer, '0');" onmouseout="this.src='url(<?=$button?>/b_removePartial.png)';" onmouseover="this.src='url(<?=$button?>/b_removePartialOver.png)';" type="image">
        <input style="cursor: pointer;" src="<?=$button?>/b_addPartial.png" id="add" name="right" value=" " onclick="moveOptions(this.form.sessionServer, this.form.sessionUser, '0');" onmouseout="this.style.backgroundImage='url(<?=$button?>/b_addPartial.png)';" onmouseover="this.src='url(<?=$button?>/b_addPartialOver.png)';" type="image" ><br>
        <input style="cursor: pointer;" src="<?=$button?>/b_removeAll.png" id="allRemove" name="left" value=" " onclick="moveOptions(this.form.sessionUser, this.form.sessionServer,'1');" onmouseout="this.src='url(<?=$button?>/b_removeAll.png)';" onmouseover="this.src='url(<?=$button?>/b_removeAllOver.png)';" type="image">
        <input style="cursor: pointer;" src="<?=$button?>/b_addAll.png" id="allAdd" name="right" value=" " onclick="moveOptions(this.form.sessionServer, this.form.sessionUser, '1');" onmouseout="this.src='url(<?=$button?>/b_addAll.png)';" onmouseover="this.src='url(<?=$button?>/b_addAllOver.png)';" type="image">
    -->    
       </td>                      
       <td>
      
         <select name="sessionUser" id="sessionUser" class="textBlack" multiple="multiple" size="6" style="width: 150px;" ondblclick="moveOptions(this.form.sessionUser,this.form.sessionServer, '0');">
         <?
          $result=getSessionsList($paramIdServer);
          $row = mysql_fetch_array($result);
           
          while ($row)
          {
           if(in_array($row['id'], $chosen))
           {
            echo '<option  value="'.$row['id'].'">'.$row['sessionName'].'</option>'."\n";
           } 
           $row = mysql_fetch_array($result);
          }           
           
          ?>         
         </select>
      
       </td> 
       </tr> 
      </table>    
    </div>
   <div id="administratorUser" style="display:none;">
     <table cellSpacing="0" cellPadding="4" border="0" id="admin">
      <tr>
        <td colspan="3" class="txtLabel">NX Sessions:</td>
      </tr>
      <tr>

        <td class="txtLabel" align="center">
      
          <select class="textBlack"   multiple="multiple"  size="6"  style="width:150px;" disabled>
         </select>

        </td>
        <td class="txtLabel" align="center">

        <img src="<?=$button?>/b_removePartialDsb.png" style="cursor: pointer;" >
        <img src="<?=$button?>/b_addPartialDsb.png" style="cursor: pointer;" ><BR>
        <img src="<?=$button?>/b_removeAllDsb.png" style="cursor: pointer;" >
        <img src="<?=$button?>/b_addAllDsb.png" style="cursor: pointer;" >

       </td>                      
       <td>
      
         <select class="textBlack" multiple="multiple" size="6" style="width: 150px;" disabled>
         <?
          $result=getSessionsList($paramIdServer);
          $row = mysql_fetch_array($result);
           
          while ($row)
          {

            echo '<option  value="'.$row['id'].'">'.$row['sessionName'].'</option>'."\n";

           $row = mysql_fetch_array($result);
          }           
           
          ?>         
         </select>
      
       </td> 
       </tr> 
      </table>    
    </div>
    
    
    
    </td>
   </tr>
  </table>
  </td>
 </tr>
</table>
<? if($messageType!="") { ?>
<div class="message">
<p class="t_2orange" style="margin:0px;padding:0px;line-height:27px;">
 <img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
 <?=$Message[$messageType]?>
</p></div>
<? } ?>

<table cellSpacing="0" cellPadding="0" width="98%" height="27" class="bgMiniBar">
<tr>
  <td width="100%"></td>
  <td width="100" align="center"><input type="image" name="addUser" id="addUser"  width="82" height="17" border="0" src="<?=$button?>/b_add.png" onmouseout="this.src='<?=$button?>/b_add.png'" onmouseover="this.src='<?=$button?>/b_addOver.png'" style="cursor: pointer; margin-right:10px;"></td>
</tr>
</table>
<?=$hiddenSession?>
</form>

</div>

<? include("includes/Footer.php") ?>
