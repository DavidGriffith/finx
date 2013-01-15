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


<?php
include ("HandleDB.php");
include ("conf.php");

if($_POST['type']=='0')
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
    <table cellspacing="0" cellpadding="0" border="0" align="right" style="width:355px; margin-top:0px;" id="contentSetup">
    <tr>
     <td height="15" colspan="2">
     NX Builder 1.5.0 and 2.1.0 users can proceed with upgrading the NX Builder backend.<br>
     All other users do not need to upgrade their NX Builder backend. 
     If you already have a DB created using NX Builder 3.x.x, you should modify the  builder.cfg file 
     appropriately.</td>
    </tr>
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
      <input name="run" style="margin-top:15px;" type="image" width="82" height="17" src="<?=$button?>/b_run.png" border="0" onmouseout="this.src='<?=$button?>/b_run.png'" onmouseover="this.src='<?=$button?>/b_runOver.png'">
     </td>
   </tr>  
   </table>

<?
}
?>   
