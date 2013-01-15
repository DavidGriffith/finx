<?
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

include "builder.cfg";
include "HandleDB.php";
include "conf.php";
checkPermission();

if(!isset($_REQUEST['idServer']) OR !isset($_REQUEST['idUser']) )
{
 exit; 
}

if(detectInjection($_REQUEST['idServer']) OR detectInjection($_REQUEST['idUser']) )
{
 exit;
}

?>
   <table cellspacing="0" cellpadding="0" border="0" style="margin-top:-5px;">
   <tr>
   <?

   $countSession=0;
   
   $userList=userListById($_REQUEST['idUser']);
   $user=mysql_fetch_array($userList);
   
   
   
   if($user['identity']!="administrator")
   {
    $sessionList = sessionListUser($_REQUEST['idUser'], $_REQUEST['idServer']);
   }
   else
   {
    $sessionList = getSessionsList($_REQUEST['idServer']);
   }   
   
   
   //$sessionList = sessionListUser($_REQUEST['idUser'], $_REQUEST['idServer']);
   $session=mysql_fetch_array($sessionList);


   if(!$session)
   {
   ?>
       <td align="center">
       <img src="images/shared/empty.png" border="0" width="62" height="50">
      </td>
   <? 
   
   }
   
   
   while($session)
   {
     $countSession++; 
     if($session['icon']=="") $session['icon']="empty.png"
   ?>
       <td align="left">
        <table width="63" border="0" cellpadding="1" cellspacing="0">
        <tr>
         <td valign="top" align="center"> <img src="<?=$sessionShared?>/<?=$session['icon']?>" width="62" height="50" title="<?=$session['sessionName']?>" ></td>
        </tr>
        <tr> 
         <td valign="top" align="center"> 
           <span class="titleBlueSmall" style="cursor: pointer;" title="<?=$session['sessionName']?>"><?=getSessionTitle($session['sessionName'],7)?></span>
         </td>
        </tr>
        </table>  
      </td>
   <?
    if($countSession==5)
    {
     echo "</tr><tr>";
     $countSession=0;
    }   
    $session=@mysql_fetch_array($sessionList);
   } 
   ?>
    </tr>
   </table>
