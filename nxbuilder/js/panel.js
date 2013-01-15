/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2004, 2005 NoMachine, http://www.nomachine.com.          */
/*                                                                        */
/* NXMANAGER, NX protocol compression and NX extensions to this software  */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rights reserved.                                                   */
/*                                                                        */
/**************************************************************************/

function selectTab(n,numPanel) {
// iterate all tab-panel pairs

    for(var i=0; i < numPanel; i++) {
        var panelDiv = window.document.getElementById("panelLogin"+i);
        var tab = document.getElementById("tab"+i);
        z = panelDiv.style.zIndex;
        // if this is the one clicked and it isn't in front, move it to the front
        if (i == n) {
            z = numPanel; 
            visibility="visible"; 
            display="block";
        }
        // in all other cases move it to the original position
        else { 
               z = (numPanel-i-1)
               visibility="hidden";
                display="none";
        }
        panelDiv.style.zIndex = z;
        panelDiv.style.visibility = visibility;
        panelDiv.style.display = display;
        tab.style.zIndex = z;
    }
}



function setlayer(i,layer){
    var tab = document.getElementById("tab"+i);
    tab.style.zIndex = layer;
}





function WindowWitdh() {
  var myWidth = 0, myHeight = 0;
  if( typeof( window.innerWidth ) == 'number' ) {
    //Non-IE
    myWidth = window.innerWidth;
    myHeight = window.innerHeight;
  } else if( document.documentElement &&
      ( document.documentElement.clientWidth || document.documentElement.clientHeight ) ) {
    //IE 6+ in 'standards compliant mode'
    myWidth = document.documentElement.clientWidth;
    myHeight = document.documentElement.clientHeight;
  } else if( document.body && ( document.body.clientWidth || document.body.clientHeight ) ) {
    //IE 4 compatible
    myWidth = document.body.clientWidth;
    myHeight = document.body.clientHeight;
  }
//window.alert( 'Width = ' + myWidth );
//window.alert( 'Height = ' + myHeight );
//window.alert( 'MaxWidht = ' + maxSize );
  return myWidth; 
}
