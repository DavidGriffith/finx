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
?>

<table width="100%" cellspacing="0" cellpadding="0" border="0" style="min-width:900px;">
<tr>
 <td><IMG width="100%" height="1" src="images/shared/empty.png" border="0"></td>
</tr>
<tr>
 <td valign="top" class="bgtoolbar" id="bgToolbarId">
  <table border="0" width="100%" cellspacing="0" cellpadding="0">
  <tr>
<?



if($_SESSION['identity']=="administrator")
{
?>
<?
    if((basename($_SERVER['PHP_SELF'])=="ServerListAdmin.php") || (basename($_SERVER['PHP_SELF'])=="AddServer.php"))
    {
?>
   <td width="79" height="57"><a href="ServerListAdmin.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_serverListActive.png" border="0" ></a></td>
<?
    }
    else
    {
?>   
   <td width="79" height="57"><a href="ServerListAdmin.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_serverList.png" border="0" onmouseout="this.src='<?=$toolbarSkin?>/b_serverList.png'" onmouseover="this.src='<?=$toolbarSkin?>/b_serverListOver.png'" onclick="this.src='<?=$toolbarSkin?>/b_serverListActive.png'"></a></td>
<?
    }

    if((basename($_SERVER['PHP_SELF'])=="SessionList.php") || (basename($_SERVER['PHP_SELF'])=="AddSession.php"))
    {
?>
   <td width="79" height="57"><a href="SessionList.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_sessionListActive.png" border="0" ></a></td>
<?
    }
    else
    {
?>   
   <td width="79" height="57"><a href="SessionList.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_sessionList.png" border="0" onmouseout="this.src='<?=$toolbarSkin?>/b_sessionList.png'" onmouseover="this.src='<?=$toolbarSkin?>/b_sessionListOver.png'" onclick="this.src='<?=$toolbarSkin?>/b_sessionListActive.png'"></a></td>
<?
    }

    if((basename($_SERVER['PHP_SELF'])=="UserList.php") || (basename($_SERVER['PHP_SELF'])=="AddUser.php"))
    {
?>
    <td width="79" height="57"><a href="UserList.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_userListActive.png" border="0" ></a></td>
<?
    }
    else
    {
?>
   <td width="79" height="57"><a href="UserList.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_userList.png" border="0" onmouseout="this.src='<?=$toolbarSkin?>/b_userList.png'" onmouseover="this.src='<?=$toolbarSkin?>/b_userListOver.png'" onclick="this.src='<?=$toolbarSkin?>/b_userListActive.png'"></a></td>
<?  
   }

    if(basename($_SERVER['PHP_SELF'])=="Configuration.php")
    {
?>
    <td width="79" height="57"><a href="Configuration.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_configurationActive.png" border="0" ></a></td>
<?  
    }
    else
    {
?>
   <td width="79" height="57"><a href="Configuration.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_configuration.png" border="0" onmouseout="this.src='<?=$toolbarSkin?>/b_configuration.png'" onmouseover="this.src='<?=$toolbarSkin?>/b_configurationOver.png'" onclick="this.src='<?=$toolbarSkin?>/b_configurationActive.png'"></a></td>
<?  
   }

}
else
{
    if(basename($_SERVER['PHP_SELF'])=="ServerList.php")
    {
?>
   <td width="79" height="57"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_serverListActive.png" border="0" ></td>
<?
    }
    else
    {
?>   
   <td width="79" height="57"><a href="ServerList.php"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_serverList.png" border="0" onmouseout="this.src='<?=$toolbarSkin?>/b_serverList.png'" onmouseover="this.src='<?=$toolbarSkin?>/b_serverListOver.png'" onclick="this.src='<?=$toolbarSkin?>/b_serverListActive.png'"></a></td>
<?
    }
?> 
  
   <td width="79" height="57"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_sessionListDsb.png" border="0" ></td>
   <td width="79" height="57"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_userListDsb.png" border="0" ></td>  
   <td width="79" height="57"><IMG width="79" height="57" src="<?=$toolbarSkin?>/b_configurationDsb.png" border="0"></td>

<?
}
?>
<!-- logo -->

   <td valign="top" style="padding-top:10px;padding-left:0px;padding-right:8px;padding-bottom:10px;" align="right"><img width="172" height="16" border="0" src="<?=$toolbarSkin?>/nomachine_logo.png"></td>

<!-- right margin spacer -->

   <td width="2"><img width="2" height="57" border="0" src="images/shared/empty.png"></td>
  </tr>
  </table>
 </td>
</tr>

<tr>
 <td background="<?=$toolbarSkin?>/bg_up.png" valign="top"><img width="100%" height="3" border="0" src="images/shared/empty.png"></td>
</tr>

<tr nowrap="nowrap">
 <td align="right" valign="middle" id="bgToolbarBottomId" class="bgHeader1" >
  <table cellspacing="0" cellpadding="0" border="0">
  
<!-- start small buttons -->

  <tr valign="middle">
   <td height="25"  align="right" style="padding-top:0px;" valign="middle"><a href="nxbuilder.php" class="linkTxtHeader1"  id="bgToolbarBottomFontId">Logged to the NX Builder</a></td>
   <td width="35" height="25" align="left"><a href="loggOff.php" style="text-decoration: none; border: 0px; "><img width="25" height="25" border="0" src="<?=$toolbarSkin?>/butt_green.png" onmouseout="this.src='<?=$toolbarSkin?>/butt_green.png'" onmouseover="this.src='<?=$toolbarSkin?>/butt_green_over.png'"></a></td>
  </tr>

<!-- end small buttons -->

  </table>
 </td>
</tr>
<tr>
 <td style="background-image: url('<?=$toolbarSkin?>/bg_bottom.png'); background-repeat: repeat-x;"><img width="100%" height="1" border="0" src="<?=$sharedSkin?>/empty.png"></td>
</tr>
<tr>
 <td width="100%">
  <table cellSpacing="0" cellPadding="0" border="0" width="100%">
  <tr>
   <td width="100%"><IMG width="901" height="1" src="images/shared/empty.png" border="0"></td>
  </tr>
  <tr>
   <td width="100%" align="right" >
    <table cellSpacing="0" cellPadding="0" border="0" class="bgHeader2" width="100%" id="bgTableAreaId">
    <tr>
     <td  nowrap  ><span class="txtHeader2" id="colorFontAreaId" style="padding-left:20px;">Administration</span></td>
     <td width="100%"><IMG width="1" height="48" src="images/shared/empty.png" border="0"></td>
    </tr>
    </table>
   </td>
  </tr>
  </table>
 </td>
</tr>
</table>

