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

checkAjaxPermission();

//$_REQUEST['idServer']="16' OR '1'='1";

if(!isset($_REQUEST['idServer'])) exit;

$QueryString="SELECT * from servers WHERE id='".$_REQUEST['idServer']."' ";
$queryServers=@mysql_query($QueryString);
$server=@mysql_fetch_array($queryServers);

if($server && !detectInjection($_REQUEST['idServer']))
{
?>
          <table id="hostDescription"  border="0" cellpadding="2" cellspacing="0">
          <tr>  
           <td class="txtLabel" style="padding: 5px; width:84px;" align="right"> Host: </td>
           <td class="txtValue" align="left" nowrap="nowrap"><?=$server['hostName']?></td>
           <td class="txtLabel" align="left" nowrap="nowrap"> Port:  &nbsp; <?=$server['port']?></td>
          </tr>
          <tr>  
           <td class="txtLabel" style="padding: 5px;" align="right" valign="top"> Description: </td>
           <td class="txtValue" colspan="2" align="left"> 
            <?=formatDesc($server['description'])?>
           </td>
          </tr>
          <tr>  
           <td class="txtLabel" style="padding: 5px;" align="right" nowrap="nowrap"> O.S.: </td>
           <td class="txtValue" colspan="2" align="left"><?=$server['OS']?></td>
          </tr>
          <tr>  
           <td class="txtLabel" style="padding: 5px;" align="right" nowrap="nowrap"> CPU: </td>
           <td class="txtValue" colspan="2" align="left"><?=$server['CPU']?></td>
          </tr>
          <tr>  
           <td class="txtLabel" style="padding: 5px;" align="right" nowrap="nowrap"> Memory: </td>
           <td class="txtValue" colspan="2" align="left"><?=$server['memory']?></td>
          </tr>
          <tr>  
           <td class="txtLabel" style="padding: 5px;" align="right" nowrap="nowrap"> Disk: </td>
           <td class="txtValue" colspan="2" align="left"><?=$server['disk']?></td>
          </tr>
          </table>
<?
}
else
echo "data not available.";
?>
