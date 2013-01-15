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


include ("HandleDB.php");
checkPermission();
$messageType="";

$check1="";
$check2="";


if(isset($_POST['default_x']))
{
 $updateGuest=resetSkinGuest();
}

if(isset($_POST['modify_x']))
{
                                                      
 $up=updateTemplate($_POST['c5'],$_POST['c7'],$_POST['c8'],$_POST['c9'], $_POST['c10'], $_POST['c11']);
 $rewrite=rewriteCss();
}


if(!isset($_POST['default_x']))
{
 $querySettings=getColorTemplate(); 
}
else
{
 $querySettings=getColorDefaultTemplate();  
}


$settings=mysql_fetch_array($querySettings);

 
$c1=$settings['bgtoolbar'];
$c2=$settings['txtHeader1'];
$c3=$settings['bgHeader2'];
$c4=$settings['txtHeader2'];
$c5=$settings['bgMiniBar'];
$c6a=$settings['tabOver'];
$c6b=$settings['tabBorder'];
$c7=$settings['txtHeaderBox'];
$c8=$settings['bgBox'];
$c9=$settings['labelStyle'];
$c10=$settings['txtLabel'];
$c11=$settings['border'];


if($c9=="bold")
{
 $LabelButtonBold="Normal";
}
else
{
 $LabelButtonBold="Bold";
}

include ("conf.php");

/*if(isset($_POST['default_x']) )
{
 $css="defaultGuest";
}
*/


include ("Messages.php");
include ("includes/Top.php");


?>
<script language="javascript" src="js/drag.js"></script>
<script  src="js/color.js" type="text/javascript"></script>
<center>

<!--[if IE 6]>
<script type="text/javascript">
function calcWidth() {
document.getElementById('min_width').style.width = (document.body.clientWidth < 901 ? '900px' : '100%')
}
onresize = calcWidth;
</script>
<![endif]-->
<? include("includes/Toolbar.php");?>

 <script type="text/javascript">
function $(v) { return(document.getElementById(v)); }
function agent(v) { return(Math.max(navigator.userAgent.toLowerCase().indexOf(v),0)); }
function xy(e,v) { return(v?(agent('msie')?event.clientY+document.body.scrollTop:e.pageY):(agent('msie')?event.clientX+document.body.scrollTop:e.pageX)); }

function dragOBJ(d,e) {

	function drag(e) { if(!stop) { d.style.top=(tX=xy(e,1)+oY-eY+'px'); d.style.left=(tY=xy(e)+oX-eX+'px'); } }

	var oX=parseInt(d.style.left),oY=parseInt(d.style.top),eX=xy(e),eY=xy(e,1),tX,tY,stop;

	document.onmousemove=drag; document.onmouseup=function(){ stop=1; document.onmousemove=''; document.onmouseup=''; };

}

function setWeight()
{
 var bold=document.getElementById('bold');   
 var boldButton=document.getElementById('boldButton');

 if(bold.value=="normal")
 {
  bold.value="bold"  
  boldButton.value="Normal";
 }
 else
 {
  bold.value="normal"   
  boldButton.value="Bold";
 }
  
}

</script> 

<div id='colorPick' class="bg borderAll" style="padding: 0px; position: absolute; top: 0px; left: 0px; " onmousedown="dragOBJ(this,event)" class='color'></div>


<div id="min_width">
<!--SectionIcon Start-->
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/configuration.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left"><span id="titleIconId">Configuration</span></td>
</tr>
</table>
<!--SectionIcon Stop-->

<form name="configuration" id="configuration" method="post" action="<?=$_SERVER['PHP_SELF']?>" enctype="multipart/form-data" >
<table  cellSpacing="0" cellPadding="0" width="98%" style="min-width:900px; margin:0px;margin-top:15px;">
 <tr valign="top">
  <!-- start left box -->
  <td width="48%" align="left"> 
  
    <table id="table1" width="100%" cellspacing="0"  cellpadding="0" style="margin:0px;margin-top:15px;">
     <tr>
       <td height="26" id="tabHeader1" class="bgHeaderTable" ><span class="txtHeaderTable" id="colorSectIdSx">&nbsp;Background and border color</span></td>
     </tr>
    </table>
    <table id="bgTableSx" width="100%" height="350"  cellspacing="0" cellpadding="2" border="0" class="bgBox borderAll">
    <tr>
     <td align="center" style="padding:10px;" valign="top"><img src="<?=$shared?>/screenshot.png" width="401"></td>
    </tr>
    <tr> 
     <td valign="top" align="center">

       <table cellspacing="0" cellpadding="2" border="0">
       <tr>
        <td>
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet1.png" width="11" title="Title font color" ></td>
          <td><input type=button value=... onclick="showColor('colorSect');"></td>
          <td><INPUT TYPE="text"  id="colorSect"  name="c7"  value="<?=$c7?>"  size="7" ></td>
         </tr>
         </table> 
        </td>
        <td>   
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet2.png" width="11" title="Label style" ></td>
          <td colspan="2">
           <input type=button value="<?=$LabelButtonBold?>" style="width:96px" id="boldButton" onclick="setWeight();">
           <input type=hidden value="<?=$c9?>" name="c9" id="bold">
          </td>
         </tr>
         </table>
        </td>

        <td>
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet3.png" width="11" title="Font color" ></td>
          <td><input type=button value="..." onclick="showColor('labelColor');"> </td>
          <td><INPUT TYPE="text" size="7"   value="<?=$c10?>" id="labelColor"  name="c10"></td>     
         </tr>
         </table>
         </td>
        </tr>
        <td>
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet4.png" width="11" title="Statusbar color" ></td>
          <td><input type=button  value="..." onclick="showColor('bgMessage');" class="t_2black"></td>
          <td><INPUT TYPE="text" id="bgMessage" name="c5"  value="<?=$c5?>" size="7"  class="t_2black"></td>       
         </tr>
         </table>
        </td>
        <td>
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet5.png" width="11" title="Border color" ></td>
          <td><input type=button value=... onclick="showColor('borderColor');"></td>
          <td><INPUT TYPE="text" size="7"  value="<?=$c11?>"  id="borderColor"  name="c11"></td>
         </tr>
         </table>
        </td>
        <td>
         <table cellspacing="0" cellpadding="2" border="0">
         <tr>
          <td><img src="<?=$shared?>/bullet6.png" width="11" title="Panels color" ></td>
          <td><input type=button  value="..." onclick="showColor('bgTable');"></td>
          <td><INPUT TYPE="text" id="bgTable" name="c8"  value="<?=$c8?>" size="7" ></td>
         </tr>
         </table>
        </td>
       </tr> 
       </table>

     </td>
    </tr>
    <tr>
     <td colspan="2" valign="top">
      
      <table cellspacing="0" cellpadding="2" border="0" width="100%" style="border-top: 1px solid #ccc; margin-top: 5px;">
      <tr>
       <td ><img src="<?=$shared?>/empty.png" width="100%" height="1"></td>
      </tr>
      <tr>
       <td><img src="<?=$shared?>/empty.png" width="100%" height="5"></td>
      </tr>
      <tr>                
       <td align="center">
        <input type="image" name="default" src="<?=$button?>/b_default.png" width="82" height="17"  border="0" onmouseout="this.src='<?=$button?>/b_default.png'" onmouseover="this.src='<?=$button?>/b_defaultOver.png'">
       </td>
      </tr>      
      </table> 

     </td>
    </tr>
    </table>

   
  
  
  
  </td>
  <!-- start right box -->
  <td width="48%" align="right"> 
  <table id="table2" border="0" width="99%" cellspacing="0" cellpadding="0" style="margin:0px;margin-top:15px; ">
   <tr>
     <td height="26" id="tabHeader2" class="bgHeaderTable" ><span class="txtHeaderTable" id="colorSectIdDx">&nbsp;Embeddable code</span></td>
   </tr>
  </table>
  <table id="bgTableDx" width="99%" height="350"  cellspacing="0" cellpadding="4" class="borderAll bgBox">
   <tr>
    <td class="txtLabel" width="60%">Embedded code preview</td><td align="left"><img style="cursor: pointer;" src="<?=$button?>/b_preview.png" onclick="javascript: setPreview()" onmouseout="this.src='<?=$button?>/b_preview.png'" onmouseover="this.src='<?=$button?>/b_previewOver.png'"></td>
   </tr>
   <tr>
    <td colspan="2" class="txtLabel">Source Code:</td>
   </tr>
   <tr>
    <td colspan="2"><div style="width:100%; height: 2px;" class="border_top">&nbsp;</div></td>
   </tr>
   <tr>
    <td colspan="2">
     
    <style type="text/css">
     .num 
     {
     /*float: left;*/
     color: grey;
     text-align: right;
     margin-right: 6pt;
     padding-right: 6pt;
     border-right: 1px solid gray;
     }
    </style>
    <?
    
    include_once("builder.cfg");
    function highlight_num($file)
    {
   
     highlight_file($file);
    //overflow: auto;
    }
    ?>
    <div id="code" style="min-width:440px; width: 440px; height: 250px; overflow: scroll; background: white; padding: 2px;">
      
    <?
    
    
    highlight_num('printCodeGuest.php');  
    ?>
    </div>
    <script>
    
    var testo=document.getElementById('code').innerHTML;
    
    document.getElementById('code').innerHTML="";
  
    <?php 
     echo "var app='&lt;?php'; ";
     echo "app+=' include (\"".$_SERVER['DOCUMENT_ROOT'].dirname($_SERVER['PHP_SELF'])."/builder.cfg\");'; ";
     echo "app+=' include (\"\$absPath/HandleDB.php\");'; ";
     echo "app+=' include (\"\$absPath/conf.php\");'; ";
     echo "app=app + ' ?&gt;'; ";
    ?> 
    testo= "<code>"+app+"</code> "+testo;
    
    document.getElementById('code').innerHTML=testo;
    
       
    </script>   
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



<table id="gestTolbarBottom" class="bgMiniBar"  cellSpacing="0" cellPadding="0" width="98%" height="27" style="min-width:900px; margin-top:20px;">
<tr>
  <td width="100%" align="right"><input type="image" name="modify" id="modify"  width="82" height="17" border="0" src="<?=$button?>/b_modify.png"  onmouseout="this.src='<?=$button?>/b_modify.png'" onmouseover="this.src='<?=$button?>/b_modifyOver.png'" style="margin-right:10px;"></td>
</tr>
</table>
</form>
 




  <div id="preview" class="box_preview" style="left: 20px; top: 107px;">
   <table cellpadding="0" width="100%" padding="0" border="0" class="bgHeaderTable">
   <tr>
     <td id="previewHandle" class="handlePreview" >  
   	  <span id="customTitle"  class="txtHeaderTable">Preview</span>
     </td>
   	 <td width="20">
   	  <a  onclick="javascript:document.getElementById('preview').style.display='none';">
       <img src="<?=$button?>/b_close.png" onmouseout="this.src='<?=$button?>/b_close.png'" onmouseover="this.src='<?=$button?>/b_closeOver.png'">
      </a>
     </td>
    </tr>
    </table>
    <div class="contentPanel bgBoxInternal">
        <? include ("preview.php"); ?>
    </div>
   </div>





</div>

 <script language="javascript">
	
 var limitW=document.body.clientWidth-document.getElementById('preview').clientWidth;
 var limitH=document.body.clientHeight-document.getElementById('preview').clientHeight;


	var handleDiv = document.getElementById("previewHandle");
	var rootDiv   = document.getElementById("preview");
	Drag.init(handleDiv, rootDiv, 0, limitW, 0, limitH);

  
	
 </script>
<div class="borderAllGuest" id="result" style="z-index: 1000; display:none; position:absolute; background-color:white;"></div>	
<? include("includes/Footer.php") ?>

