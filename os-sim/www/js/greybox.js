/* Greybox Redux
 * Required: http://jquery.com/
 * Written by: John Resig
 * Based on code by: 4mir Salihefendic (http://amix.dk)
 * License: LGPL (read more in LGPL.txt)
 * 2009-05-1 modified by jmalbarracin. Added GB_TYPE. Fixed  total document width/height
 * 2009-06-4 modified by jmalbarracin. Support of width %
 */

var GB_DONE = false;
var GB_TYPE = ''; // empty or "w"
var GB_HEIGHT = 400;
var GB_WIDTH = 400;
var GB_SCROLL_DIFF = 22;

function GB_show(caption, url, height, width) {
  GB_HEIGHT = height || 400;
  GB_WIDTH = width || 400;
  if(!GB_DONE) {
    $(document.body)
      .append("<div id='GB_overlay" + GB_TYPE + "'></div><div id='GB_window'><div id='GB_caption'></div>"
        + "<img src='../pixmaps/theme/close.png' alt='Close'/></div>");
    $("#GB_window img").click(GB_hide);
    $("#GB_overlay" + GB_TYPE).click(GB_hide);
    $(window).resize(GB_position);
    GB_DONE = true;
  }

  $("#GB_frame").remove();
  $("#GB_window").append("<iframe id='GB_frame' src='"+url+"'></iframe>");

  $("#GB_caption").html(caption);
  $("#GB_overlay" + GB_TYPE).show();
  GB_position();

  $("#GB_window").show();
}

function GB_hide() {
  //$('body').removeClass("noscroll").addClass("autoscroll");
  $("#GB_window,#GB_overlay" + GB_TYPE).hide();
  if (typeof(GB_onclose) == "function") GB_onclose();
}

function GB_position() {
  var de = document.documentElement;
  // total document width
  var w = document.body.scrollWidth
  if (self.innerWidth > w) w = self.innerWidth;
  if (de && de.clientWidth > w) w = de.clientWidth;
  if (document.body.clientWidth > w) w = document.body.clientWidth;
  // total document height
  var h = document.body.scrollHeight
  if ((self.innerHeight+window.scrollMaxY) > h) h = self.innerHeight+window.scrollMaxY;
  if (de && de.clientHeight > h) h = de.clientHeight;
  if (document.body.clientHeight > h) h = document.body.clientHeight;
  //alert(h+';'+document.body.scrollHeight+';'+self.innerHeight+';'+de.clientHeight+';'+document.body.clientHeight+';'+window.scrollMaxY)
  //
  //$('body').removeClass("autoscroll").addClass("noscroll");
  $("#GB_overlay" + GB_TYPE).css({width:(w+GB_SCROLL_DIFF)+"px",height:(h+GB_SCROLL_DIFF)+"px"});
  var sy = document.documentElement.scrollTop || document.body.scrollTop;
  var ww = (typeof(GB_WIDTH) == "string" && GB_WIDTH.match(/\%/)) ? GB_WIDTH : GB_WIDTH+"px";
  var wp = (typeof(GB_WIDTH) == "string" && GB_WIDTH.match(/\%/)) ? w*(GB_WIDTH.replace(/\%/,''))/100 : GB_WIDTH;
  $("#GB_window").css({ width: ww, height: GB_HEIGHT+"px",
    left: ((w - wp)/2)+"px", top: (sy+32)+"px" });
  $("#GB_frame").css("height",GB_HEIGHT - 44 +"px");
}
