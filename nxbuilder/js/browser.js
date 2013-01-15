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


browserVersion=parseInt(navigator.appVersion);

browserName=navigator.appName;

//alert('bName: '+browserName+' bVers: '+browserVersion);
//alert('navigator: ');
//alert('plat: '+navigator.platform)
var sLinkRel = '';
var cssPath='style';

if(navigator.userAgent.indexOf("Windows") != -1)
{
  if (navigator.appCodeName.indexOf("Explorer") != -1 ||
      navigator.appName.indexOf("Explorer")     != -1)
  {
    //alert("Loading win_ie.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_ie.css" type="text/css">';
  }
  else if (navigator.appCodeName.indexOf("Mozilla") != -1 ||
           navigator.appName.indexOf("Mozilla")     != -1)
  {
    //alert("Loading win_mz.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_mz.css" type="text/css">';
  }
  else if (navigator.appCodeName.indexOf("Netscape") != -1 ||
           navigator.appName.indexOf("Netscape")     != -1)
  {
    //alert("Loading win_ns.css for "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_ns.css" type="text/css">';
  }
  else
  {
    //alert("Loading win_ie.css for others");
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_ie.css" type="text/css">';
  }
}
else if(navigator.userAgent.indexOf("Unix")    != -1 ||
        navigator.userAgent.indexOf("Linux")   != -1 ||
        navigator.userAgent.indexOf("Solaris") != -1)
{
  if (navigator.appCodeName.indexOf("Konqueror") != -1 ||
           navigator.appName.indexOf("Konqueror")     != -1)
  {
    //alert("Loading lin_kq.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/lin_kq.css" type="text/css">';
  }
  
  else if (navigator.appCodeName.indexOf("Mozilla") != -1 ||
      navigator.appName.indexOf("Mozilla")     != -1)
  {
    //alert("Loading lin_mz.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/lin_mz.css" type="text/css">';
  }
  
  
  else if (navigator.appCodeName.indexOf("Netscape") != -1 ||
           navigator.appName.indexOf("Netscape")     != -1)
  {
    //alert("Loading lin_ns.css for "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/lin_ns.css" type="text/css">';
  }
  else
  {
    //alert("Loading win_ie.css for others");
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_ie.css" type="text/css">';
  }
}

else if (navigator.userAgent.indexOf("Mac") != -1)
{
  if (navigator.appCodeName.indexOf("Explorer") != -1 ||
      navigator.appName.indexOf("Explorer")     != -1)
  {
    //alert("Loading win_ie.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/mac_ie.css" type="text/css">';
  }
  else if (navigator.appCodeName.indexOf("Mozilla") != -1 ||
           navigator.appName.indexOf("Mozilla")     != -1)
  {
    //alert("Loading win_mz.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/lin_mz.css" type="text/css">';
  }
  else if (navigator.appCodeName.indexOf("Netscape") != -1 ||
           navigator.appName.indexOf("Netscape")     != -1)
  {
    //alert("Loading win_mz.css for "+navigator.appCodeName+" - "+navigator.appName);
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/mac_ns.css" type="text/css">';
  }
  else
  {
    //alert("Loading win_ie.css for others");
    sLinkRel += '<link rel=stylesheet href="'+cssPath+'/mac_mz.css" type="text/css">';
  }
}
else
{
  //alert("Loading win_ie.css for others");
  sLinkRel += '<link rel=stylesheet href="'+cssPath+'/win_ie.css" type="text/css">';
}
document.write( sLinkRel );
//alert ("Loaded browser.js successfully: \nLoaded: \n"+sLinkRel);
// -->
