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


<div id="border_box">
 <div class="content_box_pop_up">
   <p class="textBlack">HTTP Proxy</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
       <td><span class="textBlack">Host</span></td><td><input name="proxyHost" type="text" value="<?=$proxyHostValue?>" class="textBlack"></td>
       <td><span class="textBlack">Port</span></td><td><input name="proxyPort" type="text" value="<?=$proxyPortValue?>" size="4" class="textBlack"></td>
     </table>
    </div>
    <p class="textBlack">Authentication</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
       <td><span class="textBlack">Username</span></td><td><input name="userNameProxy"  value="<?=$userNameProxyValue?>" type="text" class="textBlack"></td>
      </tr>
      <tr>
       <td><span class="textBlack">Password</span></td><td><input name="passwordProxy" value="<?=$passwordProxyValue?>" type="password" class="textBlack"></td>
      </tr>
      <tr>
       <td align="right"><input value="true" name="rememberProxyAuth" <?=$check_rememberProxyAuth?> type="checkbox"></td><td><span class="textBlack">Remember my password</span></td>
      </tr>
     </table>
    </div>
   <table>
     <tr>
      <td>
       <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('proxyOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
      </td>
      <td>
       <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('proxyOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
      </td>
     </tr>
    </table>
 </div>
</div>
