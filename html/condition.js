// $Id$

JSROOT.define(["jquery", "jquery-ui"], $ => {

   "use strict";

   if (typeof JSROOT != "object") {
      let e1 = new Error("condition.js requires JSROOT to be already loaded");
      e1.source = "condition.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
      let e1 = new Error("condition.js requires GO4 to be already loaded");
      e1.source = "condition.js";
      throw e1;
   }

   // only during transition
   JSROOT.loadScript("https://root.cern/js/6.3.2/style/jquery-ui.min.css");

   // =========================================================================================

   GO4.ConditionEditor = function(dom, cond) {
      JSROOT.BasePainter.call(this, dom);
      this.cond = cond;
      this.changes = ["dummy", "init"];
      this.clearChanges();
   }

   GO4.ConditionEditor.prototype = Object.create(JSROOT.BasePainter.prototype);

   GO4.ConditionEditor.prototype.isPolyCond = function() {
       return ((this.cond._typename == "TGo4PolyCond") || (this.cond._typename == "TGo4ShapedCond"));
   }

   GO4.ConditionEditor.prototype.isEllipseCond = function() {
       return (this.cond._typename == "TGo4ShapedCond");
   }

   // add identifier of changed element to list, make warning sign visible
   GO4.ConditionEditor.prototype.markChanged = function(key) {
      // first avoid duplicate keys:
      for (let index = 0; index < this.changes.length; index++) {
         if (this.changes[index]== key) return;
      }
      this.changes.push(key);
      console.log("Mark changed :%s", key);
      this.selectDom().select(".buttonChangeLabel").style("display", null);// show warning sign
   }

   // clear changed elements' ist, make warning sign invisible
   GO4.ConditionEditor.prototype.clearChanges = function() {
      for (let index = 0; index < this.changes.length ; index++) {
         let removed = this.changes.pop();
         console.log("Clear changes removed :%s", removed);
      }
      this.selectDom().select(".buttonChangeLabel").style("display", "none"); // hide warning sign
   }

   // scan changed value list and return optionstring to be send to server
   GO4.ConditionEditor.prototype.evaluateChanges = function(optionstring) {
      let id = "#" + this.getDomId(),
          dom = this.selectDom(),
          index, len = this.changes.length;
      for (index = 0; index < len ; index++) {
         //let cursor=changes.pop();
         let key = this.changes[index];
         //console.log("Evaluate change key:%s", key);

         // here mapping of key to editor field:
         if(key=="limits")
         {
            let xmin = dom.select(".cond_xmin").property("value"),
                xmax = dom.select(".cond_xmax").property("value");
            optionstring += `&xmin=${xmin}&xmax=${xmax}`;
            this.cond.fLow1 = xmin;
            this.cond.fUp1 = xmax;
            if (this.cond.fiDim == 2) {
               let ymin = dom.select(".cond_ymin").property("value"),
                   ymax = dom.select(".cond_ymax").property("value");
               this.cond.fLow2 = ymin;
               this.cond.fUp2 = ymax;
               optionstring += `&ymin=${ymin}&ymax=${ymax}`;
            }
         }
         else if(key=="polygon")
         {
            let npoints=$(id+" .cut_points")[0].value;
            optionstring +="&npolygon="+npoints;
            // TODO: set display of polygon points
            for(let i=0; i<npoints; ++i)
            {
               let x = $(id + " .cut_values input").eq(2*i)[0].value;
               let y = $(id + " .cut_values input").eq(2*i+1)[0].value;
               optionstring += `&x${i}=${x}&y${i}=${y}`;
            }
         }
         else if (key=="ellinpts"){
            let val=$(id+" .cond_ellipse_points")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="ellicx"){
            let val=$(id+" .cond_ellipse_cx")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="ellicy"){
            let val=$(id+" .cond_ellipse_cy")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="ellia1"){
            let val=$(id+" .cond_ellipse_a1")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="ellia2"){
            let val=$(id+" .cond_ellipse_a2")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="ellishape"){
            let arg=$(id+" .cond_ellipse_iscircle")[0].value;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="ellith"){
            let val=$(id+" .cond_ellipse_theta")[0].value;
            optionstring +="&"+key+"="+val;
         }
         else if (key=="resultmode"){
            let selected = dom.select(".cond_execmode").node().value;
            optionstring += "&"+key+"="+selected;
         }
         else if (key=="invertmode"){
            let selected = dom.select(".cond_invertmode").node().value;
            optionstring += "&"+key+"="+selected;
         }
         else if (key == "visible"){
            let arg = dom.select(".cond_visible").property("checked") ? "1" : "0";
            optionstring += `&${key}=${arg}`;
         } else if (key == "labeldraw"){
            let arg = dom.select(".cond_label").property("checked") ? "1" : "0";
            this.cond.fbLabelDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "limitsdraw"){
            let arg = dom.select(".cond_limits").property("checked") ? "1" : "0";
            this.cond.fbLimitsDraw = arg;
            optionstring += `&${key}=${arg}`;
         }
         else if (key=="intdraw"){
            let checked=$(id+" .cond_integr")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbIntDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="xmeandraw"){
            let checked=$(id+" .cond_xmean")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbXMeanDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="xrmsdraw"){
            let checked=$(id+" .cond_xrms")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbXRMSDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="ymeandraw"){
            let checked=$(id+" .cond_ymean")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbYMeanDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="yrmsdraw"){
            let checked=$(id+" .cond_yrms")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbYRMSDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="xmaxdraw"){
            let checked=$(id+" .cond_maxx")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbXMaxDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="ymaxdraw"){
            let checked=$(id+" .cond_maxy")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbYMaxDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else if (key=="cmaxdraw"){
            let checked=$(id+" .cond_max")[0].checked;
            let arg= (checked ? "1" : "0");
            this.cond.fbCMaxDraw=arg;
            optionstring +="&"+key+"="+arg;
         }
         else{
            console.log("Warning: evaluateChanges found unknown key:%s", key);
         }

      }// for index
      console.log("Resulting option string:%s", optionstring);
      return optionstring;
   }

   GO4.ConditionEditor.prototype.checkResize = function() {}

   GO4.ConditionEditor.prototype.changePolygonDimension = function() {
      // this only changes display of condition, not condition itself!
      // note that condition is still changed in analysis only by evaluateChanges
      // local condition copy is unchanged until we can display it somewhere.

      if(!this.isPolyCond()) return;
      let id = "#" + this.getDomId(),
          oldpoints = this.cond.fxCut.fNpoints,
          npoints = $(id+" .cut_points")[0].value;

      //if(npoints==oldpoints) return; // no dimension change, do nothing - disabled, error if we again go back to original condition dimension
      if (this.cond.fxCut) {

         $(id + " .cut_values tbody").html(""); // clear old contents
         if (npoints > oldpoints) {
            // insert last but one point into table:
            // first points are unchanged:
            for (let i = 0; i < oldpoints - 1; i++) {
               let x = this.cond.fxCut.fX[i];
               let y = this.cond.fxCut.fY[i];
               $(id + " .cut_values tbody")
               .append(
                     "<tr><td><input type='text' value='"
                     + x
                     + "'/></td>  <td> <input type='text' value='"
                     + y + "'/>  </td></tr>");
               console.log("i:" + i + ", X=" + x + " Y=" + y);
            }
            // inserted points will reproduce values of last but one point:
            let insx = this.cond.fxCut.fX[oldpoints - 2];
            let insy = this.cond.fxCut.fY[oldpoints - 2];
            for (let i = oldpoints - 1; i < npoints - 1; i++) {
               $(id + " .cut_values tbody")
               .append(
                     "<tr><td><input type='text' value='"
                     + insx
                     + "'/></td>  <td> <input type='text' value='"
                     + insy + "'/>  </td></tr>");
            }
            // final point is kept as last point of old polygon, should
            // match first point for closed tcutg:
            let lastx = this.cond.fxCut.fX[oldpoints - 1];
            let lasty = this.cond.fxCut.fY[oldpoints - 1];
            $(id + " .cut_values tbody").append(
                  "<tr><td><input type='text' value='" + lastx
                  + "'/></td>  <td> <input type='text' value='"
                  + lasty + "'/>  </td></tr>");
         }
         else
         {
            // remove last but one point from table:
            for (let i = 0; i < npoints - 1; i++) {
               let x = this.cond.fxCut.fX[i];
               let y = this.cond.fxCut.fY[i];
               $(id + " .cut_values tbody")
               .append(
                     "<tr><td><input type='text' value='"
                     + x
                     + "'/></td>  <td> <input type='text' value='"
                     + y + "'/>  </td></tr>");
            }
            // final point is kept as last point of old polygon, should
            // match first point for closed tcutg:
            let lastx = this.cond.fxCut.fX[oldpoints - 1];
            let lasty = this.cond.fxCut.fY[oldpoints - 1];
            $(id + " .cut_values tbody").append(
                  "<tr><td><input type='text' value='" + lastx
                  + "'/></td>  <td> <input type='text' value='"
                  + lasty + "'/>  </td></tr>");
            console.log("i:" + (npoints - 1) + ", X=" + lastx + " Y=" + lasty);

         }
      }
      this.markChanged("polygon");
   }

   /** @summary enable/disable tab by index */
   GO4.ConditionEditor.prototype.changeTab = function(action, indx) {

      let dom = this.selectDom(),
          btns = dom.select('.tabs_header').selectAll("button").nodes(),
          tabs = dom.selectAll('.tabs_body>div').nodes();
      if (btns.length != tabs.length)
         return console.error('mismatch in tabs sizes', btns.length, tabs.length);

      d3.select(btns[indx]).attr('disabled', action=="enable" ? null : "true");

      // if that tab selected, find any other suitable
      if ((action == "disable") && !d3.select(tabs[indx]).style("display")) {
         let best = -1;
         btns.forEach((btn,k) => {
            if (!d3.select(btn).attr('disabled') && (k != indx) && (best < 0)) best = k;
         });

         if (best >= 0) {
            d3.select(tabs[indx]).style("display", "none");
            d3.select(tabs[best]).style("display", null);
         }
      }
   }


   GO4.ConditionEditor.prototype.refreshEditor = function() {
      let editor = this,
          id = "#" + this.getDomId(),
          cond = this.cond,
          dom = this.selectDom();

      $(id+" .cond_name").text(cond.fName);
      $(id+" .cond_type").text(cond._typename);

      dom.select(".cond_execmode").node().value = cond.fbEnabled ? 0 : (cond.fbResult ? 1 : 2);

      dom.select(".cond_invertmode").node().value = cond.fbTrue ? 0 : 1;

      dom.select(".cond_xmin").property("value", cond.fLow1).on("change", () => this.markChanged("limits"));
      dom.select(".cond_xmax").property("value", cond.fUp1).on("change", () => this.markChanged("limits"));
      if (cond.fiDim==2) {
         dom.select(".cond_ymin").property("value", cond.fLow2).on("change", () => this.markChanged("limits"));
         dom.select(".cond_ymax").property("value", cond.fUp2).on("change", () => this.markChanged("limits"));
      } else {
         dom.select(".cond_ymin").attr('disabled', true);
         dom.select(".cond_ymax").attr('disabled', true);
      }

      if(this.isPolyCond()) {
         this.changeTab( "disable", 0 ); // enable/disable by tab index
         this.changeTab( "enable", 1 ); // enable/disable by tab index
         if (this.cond.fxCut) {
            let numpoints = this.cond.fxCut.fNpoints;
            $(id+" .cut_points").val(numpoints); //.change(function(){ editor.markChanged("polygon")});
            $(id + " .cut_values tbody").html("");

            for(let i = 0; i < numpoints; i++) {
               let x = this.cond.fxCut.fX[i];
               let y = this.cond.fxCut.fY[i];
               $(id + " .cut_values tbody").append("<tr><td><input type='text' value='" + x + "'/></td>  <td> <input type='text' value='" + y + "'/>  </td></tr>");
            }
            $(id + " .cut_values tbody").change(function(){ editor.markChanged("polygon")});

         }
         if(this.isEllipseCond()) {
            this.changeTab( "enable", 2 ); // enable/disable by tab index
            let numpoints = this.cond.fiResolution;
            $(id+" .cond_ellipse_points").val(numpoints);
            $(id+" .cond_ellipse_cx").val(cond.fdCenterX).change(function(){ editor.markChanged("ellicx")});
            $(id+" .cond_ellipse_cy").val(cond.fdCenterY).change(function(){ editor.markChanged("ellicy")});
            $(id+" .cond_ellipse_a1").val(cond.fdRadius1).change(function(){ editor.markChanged("ellia1")});
            $(id+" .cond_ellipse_a2").val(cond.fdRadius2).prop('disabled', cond.fbIsCircle).change(function(){ editor.markChanged("ellia2")});
            $(id+" .cond_ellipse_theta").val(cond.fdTheta).prop('disabled', cond.fbIsCircle).change(function(){
               editor.markChanged("ellith");
               $(id+" .cond_ellipse_theta_slider").slider( "option", "value", $(this)[0].value % 360);
            });

            let options = $(id+" .cond_ellipse_iscircle")[0].options;
            for (let i = 0; i < options.length; i++)
               options[i].selected = (options[i].value == cond.fiShapeType);

            $(id+" .cond_ellipse_iscircle").selectmenu('refresh', true).selectmenu("option", "width", "80%");

            $(id + " .cond_ellipse_theta_slider")
            .slider({
               min : 0,
               max : 360,
               step : 1,
               value: cond.fdTheta,
               disabled: cond.fbIsCircle,
               change : function(event, ui) {
                  editor.markChanged("ellith");
                  $(id + " .cond_ellipse_theta").val(ui.value);
               },
               stop : function(event, ui) {
                  editor.markChanged("ellith");
               }
            });

         }

      } else {
         this.changeTab( "enable", 0 );
         this.changeTab( "disable", 1 ); // enable/disable by tab index
         this.changeTab( "disable", 2 ); // enable/disable by tab index
      }

      $(id+" .cond_counts").text(cond.fiCounts);
      $(id+" .cond_true").text(cond.fiTrueCounts);
      $(id+" .cond_percent").text((cond.fiCounts > 0 ? 100. * cond.fiTrueCounts / cond.fiCounts : 0.).toFixed(2) + "%");

      // todo: get keywords from condition class definition
      // problem: static letiables are not streamed by default

      dom.select(".cond_visible")
      .property('checked', cond.fbVisible)
      .on("click", function() { cond.fbVisible = this.checked; editor.markChanged("visible")});

      dom.select(".cond_limits")
      .property('checked', cond.fbLimitsDraw)
      .on("click", function() { cond.fbLimitsDraw = this.checked; editor.markChanged("limitsdraw")});

      dom.select(".cond_label")
      .property('checked', cond.fbLabelDraw)
      .on("click", function() { cond.fbLabelDraw = this.checked; editor.markChanged("labeldraw")});

      $(id+" .cond_integr")
      .prop('checked', cond.fbIntDraw)
      .click(function() { cond.fbIntDraw = this.checked; editor.markChanged("intdraw")});

      $(id+" .cond_maxx")
      .prop('checked', cond.fbXMaxDraw)
      .click(function() { cond.fbXMaxDraw = this.checked; editor.markChanged("xmaxdraw")});

      $(id+" .cond_max")
      .prop('checked', cond.fbCMaxDraw)
      .click(function() { cond.fbCMaxDraw = this.checked; editor.markChanged("cmaxdraw")});

      $(id+" .cond_maxy")
      .prop('checked', cond.fbYMaxDraw)
      .click(function() { cond.fbYMaxDraw = this.checked; editor.markChanged("ymaxdraw")});

      $(id+" .cond_xmean")
      .prop('checked', cond.fbXMeanDraw)
      .click(function() { cond.fbXMeanDraw = this.checked; editor.markChanged("xmeandraw")});

      $(id+" .cond_xrms")
      .prop('checked', cond.fbXRMSDraw)
      .click(function() { cond.fbXRMSDraw = this.checked; editor.markChanged("xrmsdraw")});

      $(id+" .cond_ymean")
      .prop('checked', cond.fbYMeanDraw)
      .click(function() { cond.fbYMeanDraw = this.checked; editor.markChanged("ymeandraw")});

      $(id+" .cond_yrms")
      .prop('checked', cond.fbYRMSDraw)
      .click(function() { cond.fbYRMSDraw = this.checked; editor.markChanged("yrmsdraw")});

      editor.clearChanges();
   }
  //--------- end refreshEditor


  //////////////////////////////////////////////////////////
   GO4.ConditionEditor.prototype.fillEditor = function() {
      this.setTopPainter();

      let id = "#" + this.getDomId(),
          editor = this,
          cond = this.cond,
          dom = this.selectDom();

      // assign tabs buttons handlers
      dom.select('.tabs_header').selectAll("button").on("click", function() {
         let btn = d3.select(this);
         dom.selectAll('.tabs_body>div').each(function() {
            let tab = d3.select(this);
            tab.style('display', tab.attr('id') == btn.attr("for") ? null : "none");
         });
      });

      dom.select(".cond_execmode").on("change", () => this.markChanged("resultmode"));

      dom.select(".cond_invertmode").on("change", () => this.markChanged("invertmode"));

      if(this.isEllipseCond()) {
         $(id+" .cond_ellipse_iscircle").selectmenu({
            change : function(event, ui) {
               cond.fiShapeType = ui.item.value;
               if(cond.fiShapeType == 2) {
                  // circle
                  $(id+" .cond_ellipse_a2").prop('disabled', true);
                  $(id+" .cond_ellipse_theta").prop('disabled', true);
                  $(id + " .cond_ellipse_theta_slider").slider("disable");
                  $(id + "cond_ellipse_points").prop('disabled', false);
               } else if(cond.fiShapeType == 3) {
                  // ellipse
                  $(id+" .cond_ellipse_a2").prop('disabled', false);
                  $(id+" .cond_ellipse_theta").prop('disabled', false);
                  $(id + " .cond_ellipse_theta_slider").slider("enable");
                  $(id + "cond_ellipse_points").prop('disabled', false);
               } else if(cond.fiShapeType == 4) {
                  // box
                  $(id+" .cond_ellipse_a2").prop('disabled', false);
                  $(id+" .cond_ellipse_theta").prop('disabled', false);
                  $(id + " .cond_ellipse_theta_slider").slider("enable");
                  $(id + "cond_ellipse_points").prop('disabled', true);
               } else {
                  // free style
                  $(id+" .cond_ellipse_a2").prop('disabled', true);
                  $(id+" .cond_ellipse_theta").prop('disabled', true);
                  $(id + " .cond_ellipse_theta_slider").slider("disable");
                  $(id + "cond_ellipse_points").prop('disabled', false);
               }

               editor.markChanged("ellishape");
            }
         })

      }

      dom.select(".buttonGetCondition")
         .style('background-image', "url(" + GO4.source_dir + "icons/right.png)")
         .on("click", () => {
            if (JSROOT.hpainter) JSROOT.hpainter.display(editor.getItemName());
                            else console.log("hierarhy painter object not found!");
         });

      dom.select(".buttonSetCondition")
         .style('background-image', "url(" + GO4.source_dir + "icons/left.png)")
         .on("click", () => {
            let options = editor.evaluateChanges(""); // complete option string from all changed elements
            console.log("set - condition "+ editor.getItemName()+ ", options="+options);
            GO4.ExecuteMethod(editor,"UpdateFromUrl",options,function(result) {
               console.log(result ? "set condition done. " : "set condition FAILED.");
               if(result) editor.clearChanges();
            });
         });

      dom.select(".buttonChangeLabel")
         .style('background-image', "url(" + GO4.source_dir + "icons/info1.png)");

      dom.select(".buttonDrawCondition")
         .style('background-image', "url(" + GO4.source_dir + "icons/chart.png)")
         .on("click", () => {
            // TODO: implement correctly after MDI is improved, need to find out active frame and location of bound histogram

            if (JSROOT.hpainter) {
               editor.evaluateChanges("");

               if (JSROOT.hpainter.updateOnOtherFrames(editor, editor.cond)) return;

               JSROOT.hpainter.drawOnSuitableHistogram(editor, editor.cond, editor.cond.fiDim==2);

               return;
            }

            //if (JSROOT.hpainter){
            //let onlineprop = JSROOT.hpainter.getOnlineProp(editor.getItemName());
            //let baseurl = onlineprop.server + onlineprop.itemname + "/";
            let baseurl = editor.getItemName() + "/",
                drawurl = baseurl + "draw.htm",
                editorurl = baseurl + "draw.htm?opt=editor";
            console.log("draw condition to next window with url="+drawurl);
            //window.open(drawurl);
            window.open(drawurl,'_blank');

         });

      dom.select(".buttonClearCondition")
         .style('background-image', "url(" + GO4.source_dir + "icons/clear.png)")
         .on("click", ()=> {
            let options = "&resetcounters=1";
            GO4.ExecuteMethod(editor, "UpdateFromUrl",options,function(result) {
               console.log(result ? "reset condition counters done. " : "reset condition counters FAILED.");
               if (result) {
                   if(JSROOT.hpainter) JSROOT.hpainter.display(editor.getItemName());
                               else console.log("hpainter object not found!");
               }
            })
          });

      $(id+" .cut_points").spinner({
         min: 0,
         max: 1000,
         step: 1,
         //spin: function( event, ui ) {console.log("cut spin has value:"+ui.value);},
         change: function( event, ui ) {editor.changePolygonDimension();//console.log("cut changed.");
         },
         stop: function( event, ui ) {editor.changePolygonDimension();//console.log("cut spin stopped.");
         }
      });

      $(id+" .cond_ellipse_points").spinner({
         min: 0,
         max: 1000,
         step: 1,
         //spin: function( event, ui ) {console.log("cut spin has value:"+ui.value);},
         change: function( event, ui ) {editor.markChanged("ellinpts");console.log("ellipse points changed.");
         },
         stop: function( event, ui ) {editor.markChanged("ellinpts");console.log("ellipse points stopped.");
         }
      });

      this.refreshEditor();
   }

   GO4.ConditionEditor.prototype.drawEditor = function() {
      return JSROOT.httpRequest(GO4.source_dir + "html/condeditor.htm", "text").then(src => {
         this.selectDom().html(src);
         this.fillEditor();
         return this;
      });
   }

   GO4.ConditionEditor.prototype.redrawObject = function(obj/*, opt */) {
      if (obj._typename != this.cond._typename) return false;
      this.cond = JSROOT.clone(obj); // does this also work with polygon condition?
      this.refreshEditor();
      return true;
   }

}) // function
