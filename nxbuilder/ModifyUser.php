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

if (!isset($_SESSION['userName'])) session_start();
if (!isset($_REQUEST['selUser']) && !isset($_REQUEST['hiddenId'] )) header("Location: UserList.php");

$messageType="";
$userCheck="";
$idFormUser=$_REQUEST['selUser'];

if(isset($_POST['hiddenId']))
{
 $idFormUser=$_POST['hiddenId'];
}

include ("HandleDB.php");
include ("conf.php");
checkPermission();
include ("Messages.php");


$error=false;
if(isset($_POST['modUser_x']))
{

  if ((isset($_POST["userName"]) and isset($_POST["password"]) and isset($_POST["repeatPassword"])) || ($_POST['userType']=="guest") )
  {
    if((trim($_POST["userName"]) != "" and trim($_POST["password"]) != "")|| ($_POST['userType']=="guest") )
    {
      if (($_POST["password"] == $_POST["repeatPassword"])|| ($_POST['userType']=="guest") )
      {

        if (isset($_POST['credentialsNX'])) $credentialsNX='2';
        elseif(isset($_POST['logUserGuest']))  $credentialsNX='1';
        else $credentialsNX='0'; 
          
        if($_POST['userType']=="guest")
        {
         $_POST['userName']="guest";
        } 
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
         
       $resUpdate=UpdateUserList($_POST['hiddenId'] ,$_POST['userName'],$_POST['password'], $_POST['userType'],$credentialsNX);
       
       if($resUpdate==-1)
       {
        $messageType="ErrorModifyNoAdmin";
        $error=true;
       }
       elseif($resUpdate==0)
       {
        $messageType="ErrorModify";
        $error=true;
       }

       if(!$error)
       {
        ModifyUserSession($_POST["hiddenId"],$sessionListIns);
        @header('Location: UserList.php'); 
        $messageType="ModifiedUser";
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

if($error)
{
 //set default for form by Post values
  
 $selAdm="";
 $selUsr=""; 
 $selGuest="";
 
 if($_POST['userType']=="administrator")
 {
  $selAdm=" selected";
 }   
 elseif($_POST['userType']=="user")
 {
  $selUsr=" selected";
 } 
 elseif($_POST['userType']=="guest")
 {
  $selGuest=" selected";
 } 
  
 $usernameValue=$_POST['userName'];
 
 
 
 $passwordValue="****default****";
 $repeatPasswordValue="****default****"; 
 
 
 if(isset($_POST['credentialsNX']))
 {
  $check_credentialsNX=" checked";
  $dis_logUserGuest =" disabled";
 }
 elseif(isset($_POST['logUserGuest']))
 {
  $check_logUserGuest =" checked";
  $dis_credentialsNX =" disabled";
 }



     

}
else
{
 //set default for form by DB values


 $queryUser=userListById($idFormUser);
 $user=mysql_fetch_array($queryUser);
 
 
 $usernameValue=$user['userName'];
 
 $selAdm="";
 $selUsr=""; 
 $selGuest="";

 $dis_userName="";
 $dis_password="";
 $dis_repeatPassword="";


 $check_credentialsNX="";
 $check_logUserGuest="";
 $dis_logUserGuest="";
 $dis_credentialsNX="";
 
 if($user['credentialsNX']=="2")
 {
  $check_credentialsNX=" checked";
  $dis_logUserGuest =" disabled";
 }
 elseif($user['credentialsNX']=="1")
 {
  $check_logUserGuest =" checked";
  $dis_credentialsNX =" disabled";
 }
 elseif($user['credentialsNX']=="0")
 {
  $check_logUserGuest =" ";
  $check_credentialsNX=" ";
  
 }

 
 if($user['identity']=="administrator")
 {
  $selAdm=" selected";
 }   
 elseif($user['identity']=="user")
 {
  $selUsr=" selected";
 } 
 elseif($user['identity']=="guest")
 {
  $selGuest=" selected";
  
  $dis_userName=" disabled";
  $dis_password=" disabled";
  $dis_repeatPassword=" disabled";  
  $usernameValue="";
  $dis_credentialsNX =" disabled";  
 } 
  
 

  
 
 $passwordValue="****default****";
 $repeatPasswordValue="****default****"; 
 
 
 

 
}
include ("includes/Top.php");
?>
<center>

<script>

<!--

function checkFieldsEnabled()
{
 var selectUserType = document.getElementById('userType');
 var normalUser = document.getElementById('normalUser');
 var administratorUser = document.getElementById('administratorUser');
 var serverList = document.getElementById('server');

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
  if(document.getElementById('server').value)
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

/*  if(operation=="1")
  {
    alert('id '+ theSelFrom.id)
    if(theSelFrom.id=='sessionUser')
    {
     alert('ppp')
     var sel2=document.getElementById('sessionUser');
   //  sel2[0].text="";
   //   sel2.remove(0)
      alert('lenght: '+sel2.options.length)
 while (sel2.options.length) 
 {
     sel2.remove(0);
 }      
    }  
  }*/
  
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
<? include("includes/Toolbar.php");?>
<!--SectionIcon Start-->
<form name="modify" ecntype="multipart/form-data" action="<?=$_SERVER['PHP_SELF']?>" method="post">
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-addUser.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Modify User</td>
</tr>
</table>
<!--SectionIcon Stop-->

<table  cellSpacing="0" cellPadding="0" width="98%" style="min-width:900px; margin-top:20px;">
 <tr valign="top">
  <!-- start left box -->
  <td width="48%" align="left"> 
  
    <table width="98%" cellspacing="0" cellpadding="0" height="26" class="bgHeaderTable">
     <tr>
       <td class="txtHeaderTable">&nbsp;User Details</td>
     </tr>
    </table>
    <table width="98%" height="160" bgcolor="#F8F6E9" cellspacing="0" cellpadding="4" border="0" class="bgBoxInternal">
    <tr>
       <td width="35%" class="txtLabel" align="right">User Type:</td>
       <td colspan="3">
        <select class="txtLabel" name="userType" id="userType" onchange="DisableGuestDetails()">
       <?
        if($selGuest==" selected")
        {
       ?> 
         <option value="guest" <?=$selGuest?> >Guest Builder User</option>        
       <? 
        }
        else
        {
       ?>
         <option value="administrator" <?=$selAdm?>  >Administrator</option>
         <option value="user" <?=$selUsr?>>User</option>
       <?
        }
       ?>
        </select>
       </td>
     </tr>
     <tr>
       <td width="35%" class="txtLabel" align="right">Username:</td>
       <td colspan="3" align="left"><input type="text" name="userName" id="userName" size="40" <?=$dis_userName?> class="textBlack" value="<?=$usernameValue?>" ></td>   
     </tr>
     <tr>
       <td width="35%" class="txtLabel" align="right">Password:</td>
       <td colspan="3" align="left"><input type="password" name="password" id="password" size="40" <?=$dis_password?> class="textBlack" value="<?=$passwordValue?>"></td>
     </tr>
     <tr>
       <td width="35%" class="txtLabel" align="right" >Confirm password:</td>
       <td colspan="3" align="left"><input type="password" name="repeatPassword" <?=$dis_repeatPassword?> id="repeatPassword" size="40" class="textBlack" value="<?=$repeatPasswordValue?>"></td>
     </tr>
     <tr>
       <td><img src="<?=$shared?>/empty.png" width="1" height="1"></td>
       <td width="15"  align="left"><input style="margin:0px;padding:0px;cursor:pointer" type="checkbox" <?=$check_credentialsNX?> <?=$dis_credentialsNX ?> name="credentialsNX" id="credentialsNX" value="1" onclick="javascript: checkBox(this);"></td>
       <td colspan="2" class="txtLabel">Use Web credentials to access NX</td>
     
     </tr>
     <tr>
       <td><img src="<?=$shared?>/empty.png" width="1" height="1"></td>
       <td width="15" align="left"><input style="margin:0px;padding:0px;cursor:pointer" type="checkbox" <?=$check_logUserGuest?> <?=$dis_logUserGuest?> name="logUserGuest" id="logUserGuest" value="0" onclick="javascript: checkBox(this);"></td>
       <td colspan="2" class="txtLabel">Login to NX Server as a Guest User</td>
     </tr>
    </table>
  
  
  
  </td>
  <!-- start right box -->
  <td width="48%" align="right"> 
   <table border="0" width="98%" cellspacing="0" cellpadding="0" height="26" class="bgHeaderTable">
   <tr>
     <td height="26" class="txtHeaderTable">&nbsp;Deployed Sessions</td>
   </tr>
  </table>
  <table width="98%" height="160" cellspacing="0" cellpadding="4" class="bgBoxInternal">
   <tr>
    <td width="35%" class="textBlack">NX Server:</td>
    <td class="textBlack"> 
     <select class="textBlack" style="width:200px;" name="sever" id="server" onchange="javascript: setSession(this);">
<?
/*if(isset($_POST['selUser']))
{
 $keyPost="serverList".$_POST['selUser'];
 
 $serverList=getServer($_POST[$keyPost]);
}
else
{
 $serverList=getServer();
}
*/

$serverList=getServer();

if(isset($_POST['selUser']))
{
 $keyPost="serverList".$_POST['selUser'];
 
 $idSel=$_POST[$keyPost];
}
else
{
 $idSel=$_POST['sever'];
}

$hiddenSession="";

if(!$serverList)
{
echo "<option value=\"empty\">Servers Not Availble</option>";
}
else
{
  $server=mysql_fetch_array($serverList);
  $idFirstServerForHidden=$server['id'];
  while($server) 
  {
   $selected=""; 
   if($server['id']==$idSel)
   {
    $selected=" selected";
   }
   $value=getSessionChoose($idFormUser ,$server['id']);
   $hiddenSession.='<input type="hidden" name="sess-'.$server['id'].'" id="sess-'.$server['id'].'" value="'.$value.'">'."\n";
   
?>
       <option value="<?=$server['id']?>" <?=$selected?> ><?=$server['hostName']?></option>
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
  
        <td class="txtLabel" align="center">
        <?php
        
          $resultList=getSessionsByUser($idFormUser, $idSel);
         
          
          $result=getSessionsNotAssigned($idSel, $resultList);
          $row = mysql_fetch_array($result);          
        ?>      
          <select name="sessionServer" id="lp" class="textblack" multiple size=6  style="width:150px;" ondblclick="moveOptions(this.form.sessionServer, this.form.sessionUser, '0');">
       <?
          while ($row)
          {
           echo '<option  value="'.$row['id'].'">'.$row['sessionName'].'</option>'."\n";
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

       </td>                      
       <td>
         <select name="sessionUser" id="sessionUser" class="textblack" multiple="multiple" size="6" style="width: 150px;" ondblclick="moveOptions(this.form.sessionUser,this.form.sessionServer, '0');">
      <?
       $resultList=getSessionsByUser($idFormUser, $idSel);
       $sessAssigned=mysql_fetch_array($resultList);
          while ($sessAssigned)
          {
           echo '<option  value="'.$sessAssigned['id'].'">'.$sessAssigned['sessionName'].'</option>'."\n";
           $sessAssigned=mysql_fetch_array($resultList);
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
          $result=getSessionsList($idFirstServerForHidden);
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
  <td width="100" align="center"><input type="image" name="modUser" id="modUser"  width="82" height="17" border="0" src="<?=$button?>/b_modify.png"  onmouseout="this.src='<?=$button?>/b_modify.png'" onmouseover="this.src='<?=$button?>/b_modifyOver.png'" style="cursor: pointer; margin-right:10px;"></td>
</tr>
</table>
<input type="hidden" name="hiddenId" value="<?=$idFormUser?>">
<?=$hiddenSession?>



</form>

<script>

checkFieldsEnabled();

</script>

<? include("includes/Footer.php") ?>
