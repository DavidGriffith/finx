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


//init following variables to prevent notice output by php interpreter 
if(!isset($customCommandValue)) $customCommandValue="";

if(!isset($VncPortValue)) $VncPortValue="";
if(!isset($vncPasswordValue)) $vncPasswordValue="";
if(!isset($check_vncRememberPassword)) $check_vncRememberPassword="";




?>


<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Application</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
       <td><input name="runApplication" id="runApplication" onclick="enableCommand('disable')" value="console" <?=$check_runApplication0?> type="radio"></td><td><span class="textBlack">Run the console</span></td>
      </tr>
      <tr>
       <td><input name="runApplication" id="runApplication" onclick="enableCommand('disable')" value="default" <?=$check_runApplication1?> type="radio"></td><td><span class="textBlack">Run the default X client script on server</span></td>
      </tr>
      <tr>
       <td><input name="runApplication" id="runApplication" onclick="enableCommand('enable')" value="application" <?=$check_runApplication2?> type="radio"></td><td><span class="textBlack">Run the following command</span></td>
      </tr>
      <tr>
       <td></td><td><input name="customCommand" id="customCommand"  type="text" class="textBlack" size="38" <?=$dis_customCommand?> value="<?=$customCommandValue?>" ></td>
      </tr>
     </table>
    </div>
   <p class="textBlack">Options</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
      <td><input name="customOptions" id="customOptions" type="radio" <?=$check_customOptions0?> value="false" onclick="enableCommandOption('enable')"></td><td><span class="textBlack">Floating window</span></td>
     </tr>
     <tr>
      <td></td>
      <td>
       <table>
        <tr><td><input name="disableX" id="disableX" <?=$check_disableX?> <?=$dis_disableX?> value="false" type="checkbox" onclick="enableTaint();"></td><td><span class="textBlack">Disable X agent encoding</span></td></tr> 
        <tr><td><input id="taint" name="taint" value="false" type="checkbox" <?=$check_taint?> <?=$dis_taint?> ></td><td><span class="textBlack">Disable taint of X replies</span></tr></td>
       </table>
      </td>
     </tr>
     <tr>
      <td><input name="customOptions" id="customOptions" <?=$check_customOptions1?> type="radio"  value="true" onclick="enableCommandOption('disable')"></td><td><span class="textBlack">New virtual desktop</span></td>
     </tr>
     </table>
    </div>
      

      
  <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('customOption').style.display='none'; disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('customOption').style.display='none'; disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
 </table>
</div>

</div>
