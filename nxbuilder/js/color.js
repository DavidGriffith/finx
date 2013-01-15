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

function getScrollY()
{
 var scrOfX = 0,scrOfY=0;
 if(typeof(window.pageYOffset)=='number')
 {
  scrOfY=window.pageYOffset;
  scrOfX=window.pageXOffset;
 }
 else if(document.body&&(document.body.scrollLeft||document.body.scrollTop))
 {
  scrOfY=document.body.scrollTop;scrOfX=document.body.scrollLeft;
 }
 else if(document.documentElement&&(document.documentElement.scrollLeft||document.documentElement.scrollTop))
 {
  scrOfY=document.documentElement.scrollTop;scrOfX=document.documentElement.scrollLeft;
 }return scrOfY;
}

document.write("<style>.color{text-align:center;visibility:hidden;display:none;position:absolute;background-color:#FFF;border:solid 1px #CCC;padding:4px;z-index:999;filter:progid:DXImageTransform.Microsoft.Shadow(color=#D0D0D0,direction=135);}.o5582brd{border-bott6om:solid 1px #DFDFDF;border-right:solid 1px #DFDFDF;padding:0;width:12px;height:14px;}a.o5582n66,.o5582n66,.o5582n66a{font-family:arial,tahoma,sans-serif;text-decoration:underline;font-size:9px;color:#666;border:none;}.o5582n66,.o5582n66a{text-align:center;text-decoration:none;}a:hover.o5582n66{text-decoration:none;color:#FFA500;cursor:pointer;}.a01p3{padding:1px 4px 1px 2px;background:whitesmoke;border:solid 1px #DFDFDF;}</style>");
function gett6op6()
{
 csBrHt=0;
 if(typeof(window.innerWidth)=='number')
 {
  csBrHt=window.innerHeight;
 }
 else if(document.documentElement&&(document.documentElement.clientWidth||document.documentElement.clientHeight))
 {
  csBrHt=document.documentElement.clientHeight;
 }
 else if(document.body&&(document.body.clientWidth||document.body.clientHeight))
 {
  csBrHt=document.body.clientHeight;
 }
 ctop=((csBrHt/2)-132)+getScrollY();return ctop;
}

function getLeft6()
{
 var csBrWt=0;if(typeof(window.innerWidth)=='number')
 {
  csBrWt=window.innerWidth;
 }
 else if(document.documentElement&&(document.documentElement.clientWidth||document.documentElement.clientHeight))
 {
  csBrWt=document.documentElement.clientWidth;
 }
 else if(document.body&&(document.body.clientWidth||document.body.clientHeight))
 {
  csBrWt=document.body.clientWidth;}cleft=(csBrWt/2)-125;return cleft;
 }
 
 var nocol1="",clos1="&#67;&#76;&#79;&#83;&#69;";

 function setCCbldID6(objID,val)
 {
  document.getElementById(objID).value=val;
 }

 function setCCbldSty6(objID,prop,val)
 {
  switch(prop)
  {
   case "bc":
    if(objID!='none')
    {
     document.getElementById(objID).style.backgroundColor=val;
    }
    break;
    
   case "vs":
    document.getElementById(objID).style.visibility=val;
    break;
 
   case "ds":
    document.getElementById(objID).style.display=val;
    break;
    
   case "tp":
    document.getElementById(objID).style.top=val;
    break;
   
   case "lf":
    document.getElementById(objID).style.left=val;
    break;
   }
  }
  
  function putColor(OBjElem,Samp,pigMent)
  {
   if(pigMent!='x')
   {
    setCCbldID6(OBjElem,pigMent);
    //setCCbldSty6(Samp,'bc',pigMent);
   }
  
   setCCbldSty6('colorPick','vs','hidden');
   setCCbldSty6('colorPick','ds','none');
   /*setManually(OBjElem,pigMent);*/
   
  }
    
  function showColor(OBjElem,Sam)
  {
   var objX=new Array('00','33','66','99','CC','FF');
   var c=0;var z='"'+OBjElem+'","'+Sam+'",""';
   var xl='"'+OBjElem+'","'+Sam+'","x"';
   var mid='';
    mid+='<div class=\"bg\" style=\"padding: 5px\"><table class="bg" border="0" cellpadding="0" cellspacing="0" >';
    mid+="<tr><td colspan='18' style='margin:0;padding:0px;height:16px;' class='bg' align=\"right\"><img src=\"images/buttons/b_close.png\" onclick='javascript: putColor("+xl+")' onmouseout=\"this.src='images/buttons/b_close.png'\" onmouseover=\"this.src='images/buttons/b_closeOver.png'\" width=\"13\" height=\"13\" border=\"0\"></td></tr><tr>";
   var br=1;
   
   for(o=0;o<6;o++)
   {
    mid+='</tr><tr>';
     for(y=0;y<6;y++)
     {
      if(y==3)
      {
       mid+='</tr><tr>';
      }
      
      for(x=0;x<6;x++)
      {
       var grid='';grid=objX[o]+objX[y]+objX[x];
       var b="'"+OBjElem+"', '"+Sam+"','#"+grid+"'";mid+='<td class="o5582brd" style="background-color:#'+grid+'"><a class="o5582n66"  href="javascript:onclick=putColor('+b+');" ><div style="width:12px;height:14px;"></div></a></td>';
       c++;
      }
     }
    }
    mid+='</tr></table>';
    var objX=new Array('0','3','6','9','C','F');
    var c=0;var z='"'+OBjElem+'","'+Sam+'",""';
    var xl='"'+OBjElem+'","'+Sam+'","x"';mid+='<table class="bg" border="0" cellpadding="0" cellspacing="0" ><tr>';
    var br=0;
    
    for(y=0;y<6;y++)
    {
     for(x=0;x<6;x++)
     {
      if(br==18)
      {
       br=0;
       mid+='</tr><tr>';
      }
      
      br++;
      var grid='';
      grid=objX[y]+objX[x]+objX[y]+objX[x]+objX[y]+objX[x];
      var b="'"+OBjElem+"', '"+Sam+"','#"+grid+"'";mid+='<td class="o5582brd" style="background-color:#'+grid+'"><a class="o5582n66"  href="javascript:onclick=putColor('+b+');" ><div style="width:12px;height:14px;"></div></a></td>';
      c++;
     }
    }
    

    mid+='</tr></table></div>';setCCbldSty6('colorPick','tp','100px');
    document.getElementById('colorPick').style.top=gett6op6();
    document.getElementById('colorPick').style.left=getLeft6();
    setCCbldSty6('colorPick','vs','visible');
    setCCbldSty6('colorPick','ds','block');
    document.getElementById('colorPick').innerHTML=mid;
   }
