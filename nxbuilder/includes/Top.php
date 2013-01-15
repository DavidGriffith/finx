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

if(!isset($_SESSION['userName'])){session_start();} 


//require ("../conf.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
<title> NX Builder</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="expires" content="0">
<link rel="stylesheet" type="text/css" href="style/default.css" media="all" >
<link rel="stylesheet" type="text/css" href="style/general.css" media="all" >
<script src="js/browser.js"  language="javaScript" type="text/javascript"></script>
<script src="js/function.js" language="javaScript" type="text/javascript"></script>
<script src="js/pos.js"  language="javaScript" type="text/javascript"></script>
<script src="js/panel.js"    language="javaScript" type="text/javascript"></script>
<LINK REL="icon" HREF="<?=$shared?>/favicon.ico" TYPE="image/gif">
<STYLE>
.BgSlider
{
 margin-top: 13px;
 background-image: url('<?=$button?>/tick.png');
 background-repeat: repeat-x;
 background-position: top left; 
}

</STYLE>
</head>
<?
$initSlide="";
if((basename($_SERVER['PHP_SELF'])=="AddSession.php") || (basename($_SERVER['PHP_SELF'])=="ModifySession.php"))
{
  $initSlide='onload="initSlide()" ';
?>
<SCRIPT src="js/crossBrowser.js" type=text/javascript></SCRIPT>

<SCRIPT src="js/handler.js" type=text/javascript></SCRIPT>

<SCRIPT src="js/slide.js" type=text/javascript></SCRIPT>

<SCRIPT type=text/javascript>
<!--
function initSlide() 
{


<?

$enableSlide=false;
$enableSlideRdp=false;
$enableSlideVnc=false;


$defWin=0;
$defUnix=6;
$defJpgRdp=6;
$defJpgVnc=6;



if((basename($_SERVER['PHP_SELF'])=="AddSession.php") && isset($_POST['addSession_x']))
{
 if(trim($check_enableJpegQuality)=="checked")
 {
  $enableSlide=true;
 }

 if(trim($check_useRdpJpg)=="checked")
 {
  $enableSlideRdp=true;
 }
  
 if(trim($check_useVncJpg)=="checked")
 {
  $enableSlideVnc=true;
 }

 
 $defUnix=$_POST['jpegQuality']; 
 $defJpgRdp=$_POST['jpegQualityRdp'];
 $defJpgVnc=$_POST['jpegQualityVnc'];
 $defWin=$_POST['sliderDivWindows_value'];
 
  
  
}
elseif((basename($_SERVER['PHP_SELF'])=="ModifySession.php") && !isset($_POST['modifySession_x']))
{
 $id = $_POST['sessID'];
 $jpgQ=GetJpegQuality($id);
 
 if($jpgQ['jpegQuality'] != "")
 {
  $defUnix=$jpgQ['jpegQuality'];
 } 
 else 
 {
  $defUnix=6;
 }
     	  
 if($jpgQ['rdpJpgQuality'] != "")
 {
  $defJpgRdp=$jpgQ['rdpJpgQuality'];
 } 
 else 
 {
  $defJpgRdp=6;
 }
 if($jpgQ['vncJpg'] != "")
 {
  $defJpgVnc=$jpgQ['vncJpg'];
 } 
 else 
 {
  $defJpgVnc=6;
 }
 
 if(trim($check_enableJpegQuality)=="checked")
 {
  $enableSlide=true;
 }
 if(trim($check_useRdpJpg)=="checked")
 {
  $enableSlideRdp=true;
 }
  
 if(trim($check_useVncJpg)=="checked")
 {
  $enableSlideVnc=true;
 }
 
 $colorDeph=GetwinColorDepth($id);

 if( $colorDeph !="" )
 {
  if($colorDeph=='8')
  {
  $defWin=0; 
  }
  if($colorDeph=='15')
  {
  $defWin=3; 
  }
  if($colorDeph=='16')
  {
  $defWin=6; 
  }
  if($colorDeph=='24')
  {
  $defWin=9; 
  }
 } 
 else
 {
  $defWin=0; 
 }
 
}
elseif((basename($_SERVER['PHP_SELF'])=="ModifySession.php") && isset($_POST['modifySession_x']))
{

 if(trim($check_enableJpegQuality)=="checked")
 {
  $enableSlide=true;
 }
 
 if(trim($check_useRdpJpg)=="checked")
 {
  $enableSlideRdp=true;
 }
  
 if(trim($check_useVncJpg)=="checked")
 {
  $enableSlideVnc=true;
 }
 
 $defUnix=$_POST['jpegQuality'];
 $defWin=$_POST['sliderDivWindows_value'];
 $defJpgRdp=$_POST['jpegQualityRdp'];
 $defJpgVnc=$_POST['jpegQualityVnc'];
 
}

?>



  // slidebar for unix display
  sliderUnix = new Bs_Slider();
  sliderUnix.attachOnChange(changeUnix);
  sliderUnix.width         = 195;
  sliderUnix.height        = 0;
  sliderUnix.minVal        = 0;
  sliderUnix.maxVal        = 9;
  sliderUnix.valueInterval = 1;
  sliderUnix.arrowAmount   = 1;
 	sliderUnix.arrowKeepFiringTimeout = 500;
  sliderUnix.valueDefault  = <?=$defUnix?>;
  sliderUnix.imgDir   = '<?=$button?>/';
  sliderUnix.setBackgroundImage('','');
  sliderUnix.setSliderIcon('cursor.png', 11, 27);
  sliderUnix.setArrowIconLeft('', 0, 0);
  sliderUnix.setArrowIconRight('', 0, 0);
  sliderUnix.useInputField = 0;
  sliderUnix.styleValueFieldClass = 'sliderInput';
  sliderUnix.colorbar = new Object();
  sliderUnix.colorbar['color']           = 'ffffff';
  sliderUnix.colorbar['height']          = 0;
  sliderUnix.colorbar['widthDifference'] = 0;
  sliderUnix.colorbar['offsetLeft']      = 0;
  sliderUnix.colorbar['offsetTop']       = 0;
  sliderUnix.drawInto('sliderDivUnix');



  // slidebar for jpeg in windows rdp display
  sliderJpgRdp = new Bs_Slider();
  sliderJpgRdp.attachOnChange(changeJpgRdp);
  sliderJpgRdp.width         = 195;
  sliderJpgRdp.height        = 0;
  sliderJpgRdp.minVal        = 0;
  sliderJpgRdp.maxVal        = 9;
  sliderJpgRdp.valueInterval = 1;
  sliderJpgRdp.arrowAmount   = 1;
 	sliderJpgRdp.arrowKeepFiringTimeout = 500;
  sliderJpgRdp.valueDefault  = <?=$defJpgRdp?>;
  sliderJpgRdp.imgDir   = '<?=$button?>/';
  sliderJpgRdp.setBackgroundImage('','');
  sliderJpgRdp.setSliderIcon('cursor.png', 11, 27);
  sliderJpgRdp.setArrowIconLeft('', 0, 0);
  sliderJpgRdp.setArrowIconRight('', 0, 0);
  sliderJpgRdp.useInputField = 0;
  sliderJpgRdp.styleValueFieldClass = 'sliderInput';
  sliderJpgRdp.colorbar = new Object();
  sliderJpgRdp.colorbar['color']           = 'ffffff';
  sliderJpgRdp.colorbar['height']          = 0;
  sliderJpgRdp.colorbar['widthDifference'] = 0;
  sliderJpgRdp.colorbar['offsetLeft']      = 0;
  sliderJpgRdp.colorbar['offsetTop']       = 0;
  sliderJpgRdp.drawInto('sliderDivJpgRdp');


  // slidebar for jpeg in Vnc RFB display
  sliderJpgVnc = new Bs_Slider();
  sliderJpgVnc.attachOnChange(changeJpgVnc);
  sliderJpgVnc.width         = 195;
  sliderJpgVnc.height        = 0;
  sliderJpgVnc.minVal        = 0;
  sliderJpgVnc.maxVal        = 9;
  sliderJpgVnc.valueInterval = 1;
  sliderJpgVnc.arrowAmount   = 1;
 	sliderJpgVnc.arrowKeepFiringTimeout = 500;
  sliderJpgVnc.valueDefault  = <?=$defJpgVnc?>;
  sliderJpgVnc.imgDir   = '<?=$button?>/';
  sliderJpgVnc.setBackgroundImage('','');
  sliderJpgVnc.setSliderIcon('cursor.png', 11, 27);
  sliderJpgVnc.setArrowIconLeft('', 0, 0);
  sliderJpgVnc.setArrowIconRight('', 0, 0);
  sliderJpgVnc.useInputField = 0;
  sliderJpgVnc.styleValueFieldClass = 'sliderInput';
  sliderJpgVnc.colorbar = new Object();
  sliderJpgVnc.colorbar['color']           = 'ffffff';
  sliderJpgVnc.colorbar['height']          = 0;
  sliderJpgVnc.colorbar['widthDifference'] = 0;
  sliderJpgVnc.colorbar['offsetLeft']      = 0;
  sliderJpgVnc.colorbar['offsetTop']       = 0;
  sliderJpgVnc.drawInto('sliderDivJpgVnc');


  // slidebar for color depth in windows rdp display
  sliderWindows = new Bs_Slider();
  sliderWindows.attachOnChange(changeWindows);
  sliderWindows.width         = 195;
  sliderWindows.height        = 0;
  sliderWindows.minVal        = 0;
  sliderWindows.maxVal        = 9;
  sliderWindows.valueInterval = 3;
  sliderWindows.arrowAmount   = 3;
 	sliderWindows.arrowKeepFiringTimeout = 500;
  sliderWindows.valueDefault  = <?=$defWin?>;
  sliderWindows.imgDir   = '<?=$button?>/';
  sliderWindows.setBackgroundImage('','');
  sliderWindows.setSliderIcon('cursor.png', 11, 27);
  sliderWindows.setArrowIconLeft('', 0, 0);
  sliderWindows.setArrowIconRight('', 0, 0);
  sliderWindows.useInputField = 0;
  sliderWindows.styleValueFieldClass = 'sliderInput';
  sliderWindows.colorbar = new Object();
  sliderWindows.colorbar['color']           = 'ffffff';
  sliderWindows.colorbar['height']          = 0;
  sliderWindows.colorbar['widthDifference'] = 0;
  sliderWindows.colorbar['offsetLeft']      = 0;
  sliderWindows.colorbar['offsetTop']       = 0;
  sliderWindows.drawInto('sliderDivWindows');

 <?
  if(!$enableSlide)
  {
 ?>
  sliderUnix.setDisabled();
 <?
  }

  if(!$enableSlideRdp)
  {
 ?>
  sliderJpgRdp.setDisabled();
 <?
  }

  if(!$enableSlideVnc)
  {
 ?>
  sliderJpgVnc.setDisabled();
 <?
  }
 ?>


  
}


function changeUnix(sliderObj, val, newPos){ 
  document.addSession.jpegQuality.value = val;
}

function changeJpgRdp(sliderObj, val, newPos){ 
  document.addSession.jpegQualityRdp.value = val;
}

function changeJpgVnc(sliderObj, val, newPos){ 
  document.addSession.jpegQualityVnc.value = val;
}


function changeWindows(sliderObj, val, newPos){ 
var setValue="";

  if(val=='0')
  {
   setValue="8"
  }
  else if(val=='3')
  {
   setValue="15"
  }
  else if(val=='6')
  {
   setValue="16"
  }
  else if(val=='9')
  {
   setValue="24"
  }

  document.addSession.rdpQuality.value = setValue;
}

// -->
</SCRIPT>


<?
}
?>
<body <?=$initSlide?> >
