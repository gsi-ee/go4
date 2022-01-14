// $Id$

JSROOT.define([], () => {

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
      let dom = this.selectDom(),
          len = this.changes.length;
      for (let index = 0; index < len ; index++) {
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
         } else if (key == "polygon") {
            let npoints = dom.select(".cut_points").property("value");
            optionstring += "&npolygon="+npoints;
            let values = dom.selectAll(".cut_values input").nodes();
            // TODO: set display of polygon points
            if (values.length != npoints*2)
               return console.error('mismatch', values.length, npoints*2);

            // always copy last point!
            values[npoints*2-2].value = values[0].value;
            values[npoints*2-1].value = values[1].value;

            for(let i = 0; i < npoints; ++i) {
               let x = values[i*2].value,
                   y = values[i*2+1].value;
               optionstring += `&x${i}=${x}&y${i}=${y}`;
            }
         } else if (key == "ellinpts") {
            let val = dom.select(".cond_ellipse_points").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellicx") {
            let val = dom.select(".cond_ellipse_cx").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellicy") {
            let val = dom.select(".cond_ellipse_cy").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellia1") {
            let val = dom.select(".cond_ellipse_a1").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellia2") {
            let val = dom.select(".cond_ellipse_a2").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellishape") {
            let val = dom.select(".cond_ellipse_iscircle").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "ellith") {
            let val = dom.select(".cond_ellipse_theta").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "resultmode") {
            let val = dom.select(".cond_execmode").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "invertmode") {
            let val = dom.select(".cond_invertmode").property("value");
            optionstring += `&${key}=${val}`;
         } else if (key == "visible") {
            let arg = dom.select(".cond_visible").property("checked") ? 1 : 0;
            optionstring += `&${key}=${arg}`;
         } else if (key == "labeldraw") {
            let arg = dom.select(".cond_label").property("checked") ? 1 : 0;
            this.cond.fbLabelDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "limitsdraw"){
            let arg = dom.select(".cond_limits").property("checked") ? 1 : 0;
            this.cond.fbLimitsDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "intdraw") {
            let arg = dom.select(".cond_integr").property("checked") ? 1 : 0;
            this.cond.fbIntDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "xmeandraw") {
            let arg = dom.select(".cond_xmean").property("checked") ? 1 : 0;
            this.cond.fbXMeanDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "xrmsdraw") {
            let arg = dom.select(".cond_xrms").property("checked") ? 1 : 0;
            this.cond.fbXRMSDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "ymeandraw") {
            let arg = dom.select(".cond_ymean").property("checked") ? 1 : 0;
            this.cond.fbYMeanDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "yrmsdraw") {
            let arg = dom.select(".cond_yrms").property("checked") ? 1 : 0;
            this.cond.fbYRMSDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key == "xmaxdraw") {
            let arg = dom.select(".cond_maxx").property("checked") ? 1 : 0;
            this.cond.fbXMaxDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key=="ymaxdraw") {
            let arg = dom.select(".cond_maxy").property("checked") ? 1 : 0;
            this.cond.fbYMaxDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else if (key=="cmaxdraw") {
            let arg = dom.select(".cond_max").property("checked") ? 1 : 0;
            this.cond.fbCMaxDraw = arg;
            optionstring += `&${key}=${arg}`;
         } else {
            console.log(`Warning: evaluateChanges found unknown key: ${key}`);
         }
      }
      console.log(`Resulting option string: ${optionstring}`);
      return optionstring;
   }

   GO4.ConditionEditor.prototype.checkResize = function() {}

   GO4.ConditionEditor.prototype.changePolygonDimension = function() {
      // this only changes display of condition, not condition itself!
      // note that condition is still changed in analysis only by evaluateChanges
      // local condition copy is unchanged until we can display it somewhere.

      if(!this.isPolyCond()) return;
      let id = "#" + this.getDomId(),
          dom = this.selectDom(),
          oldpoints = this.cond.fxCut.fNpoints,
          npoints = dom.select(".cut_points").property("value");

      //if(npoints==oldpoints) return; // no dimension change, do nothing - disabled, error if we again go back to original condition dimension
      if (this.cond.fxCut) {
         let body = dom.select(".cut_values tbody").html(""); // clear old contents
         if (npoints > oldpoints) {
            // insert last but one point into table:
            // first points are unchanged:
            for (let i = 0; i < oldpoints - 1; i++) {
               let x = this.cond.fxCut.fX[i], y = this.cond.fxCut.fY[i];
               body.append("tr").html(`<td><input type="text" value="${x}"/></td><td><input type="text" value="${y}"/></td>`);
            }
            // inserted points will reproduce values of last but one point:
            let insx = this.cond.fxCut.fX[oldpoints - 2],
                insy = this.cond.fxCut.fY[oldpoints - 2];
            for (let i = oldpoints - 1; i < npoints - 1; i++)
               body.append("tr").html(`<td><input type="text" value="${insx}"/></td><td><input type="text" value="${insy}"/></td>`);
            // final point is kept as last point of old polygon, should
            // match first point for closed tcutg:
            let lastx = this.cond.fxCut.fX[oldpoints - 1],
                lasty = this.cond.fxCut.fY[oldpoints - 1];
            body.append("tr").html(`<td><input type="text" value="${lastx}" disabled/></td><td><input type="text" value="${lasty}" disabled/></td>`);
         } else {
            // remove last but one point from table:
            for (let i = 0; i < npoints - 1; i++) {
               let x = this.cond.fxCut.fX[i], y = this.cond.fxCut.fY[i];
               body.append("tr").html(`<td><input type="text" value="${x}"/></td><td><input type="text" value="${y}"/></td>`);
            }
            // final point is kept as last point of old polygon, should
            // match first point for closed tcutg:
            let lastx = this.cond.fxCut.fX[oldpoints - 1],
                lasty = this.cond.fxCut.fY[oldpoints - 1];
            body.append("tr").html(`<td><input type="text" value="${lastx}" disabled/></td><td><input type="text" value="${lasty}" disabled/> </td>`);
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

      d3.select(btns[indx]).attr('disabled', (action=="enable") ? null : "true");

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
      } else if (action == "enable") {
         tabs.forEach((tab,i) => d3.select(tab).style('display', i==indx ? null : "none"));
      }
   }


   GO4.ConditionEditor.prototype.refreshEditor = function() {
      let editor = this,
          cond = this.cond,
          dom = this.selectDom();

      dom.select(".cond_name").text(cond.fName);
      dom.select(".cond_type").text(cond._typename);

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
         this.changeTab("enable", 1); // enable/disable by tab index
         this.changeTab("disable", 0); // enable/disable by tab index
         if (this.cond.fxCut) {
            let numpoints = this.cond.fxCut.fNpoints;
            dom.select(".cut_points").property("value", numpoints);
            let body = dom.select(".cut_values tbody").html("").on("change", () => editor.markChanged("polygon"));

            for(let i = 0; i < numpoints; i++) {
               let x = this.cond.fxCut.fX[i];
               let y = this.cond.fxCut.fY[i];
               let row = body.append("tr").html(`<td><input type="text" value="${x}"/></td><td><input type="text" value="${y}"/> </td>`);
               if ((i == numpoints-1) && (numpoints > 1)) row.selectAll("input").property("disabled", true); // disable last row
            }
         }
         if(this.isEllipseCond()) {
            this.changeTab( "enable", 2 ); // enable/disable by tab index
            let numpoints = this.cond.fiResolution;
            dom.select(".cond_ellipse_points").property("value", numpoints);
            dom.select(".cond_ellipse_cx").property("value", cond.fdCenterX).on("change", () => this.markChanged("ellicx"));
            dom.select(".cond_ellipse_cy").property("value", cond.fdCenterY).on("change", () => this.markChanged("ellicy"));
            dom.select(".cond_ellipse_a1").property("value", cond.fdRadius1).on("change", () => this.markChanged("ellia1"));
            dom.select(".cond_ellipse_a2").property("value", cond.fdRadius2).property('disabled', cond.fbIsCircle).on("change", () => this.markChanged("ellia2"));
            dom.select(".cond_ellipse_theta").property("value", cond.fdTheta).property('disabled', cond.fbIsCircle).on("change", () => {
               this.markChanged("ellith");
               dom.select(".cond_ellipse_theta_slider").property("value", dom.select(".cond_ellipse_theta").property("value") % 360);
            });

            let options = dom.select(".cond_ellipse_iscircle").node().options;
            for (let i = 0; i < options.length; i++)
               options[i].selected = (options[i].value == cond.fiShapeType);

            dom.select(".cond_ellipse_theta_slider")
                .property("value",cond.fdTheta)
                .attr("disbaled", cond.fbIsCircle)
                .on("change", () => {
                  this.markChanged("ellith");
                  dom.select(".cond_ellipse_theta").property("value", dom.select(".cond_ellipse_theta_slider").property("value"));
               });
         }

      } else {
         this.changeTab( "enable", 0 );
         this.changeTab( "disable", 1 ); // enable/disable by tab index
         this.changeTab( "disable", 2 ); // enable/disable by tab index
      }

      dom.select(".cond_counts").text(cond.fiCounts);
      dom.select(".cond_true").text(cond.fiTrueCounts);
      dom.select(".cond_percent").text((cond.fiCounts > 0 ? 100. * cond.fiTrueCounts / cond.fiCounts : 0.).toFixed(2) + "%");

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

      dom.select(".cond_integr")
         .property('checked', cond.fbIntDraw)
         .on("click", function() { cond.fbIntDraw = this.checked; editor.markChanged("intdraw")});

      dom.select(".cond_maxx")
         .property('checked', cond.fbXMaxDraw)
         .on("click", function() { cond.fbXMaxDraw = this.checked; editor.markChanged("xmaxdraw")});

      dom.select(".cond_max")
         .property('checked', cond.fbCMaxDraw)
         .on("click", function() { cond.fbCMaxDraw = this.checked; editor.markChanged("cmaxdraw")});

      dom.select(".cond_maxy")
         .property('checked', cond.fbYMaxDraw)
         .on("click", function() { cond.fbYMaxDraw = this.checked; editor.markChanged("ymaxdraw")});

      dom.select(".cond_xmean")
         .property('checked', cond.fbXMeanDraw)
         .on("click", function() { cond.fbXMeanDraw = this.checked; editor.markChanged("xmeandraw")});

      dom.select(".cond_xrms")
         .property('checked', cond.fbXRMSDraw)
         .on("click", function() { cond.fbXRMSDraw = this.checked; editor.markChanged("xrmsdraw")});

      dom.select(".cond_ymean")
         .property('checked', cond.fbYMeanDraw)
         .on("click", function() { cond.fbYMeanDraw = this.checked; editor.markChanged("ymeandraw")});

      dom.select(".cond_yrms")
         .property('checked', cond.fbYRMSDraw)
         .on("click", function() { cond.fbYRMSDraw = this.checked; editor.markChanged("yrmsdraw")});

      editor.clearChanges();
   }
  //--------- end refreshEditor


  //////////////////////////////////////////////////////////
   GO4.ConditionEditor.prototype.fillEditor = function() {
      this.setTopPainter();

      let editor = this,
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
         dom.select(".cond_ellipse_iscircle").on("change", () => {
            cond.fiShapeType = parseInt(dom.select(".cond_ellipse_iscircle").property("value"));
            let flags;
            switch(cond.fiShapeType) {
               case 2: flags = [true, true, true, null]; break; // circle
               case 3: flags = [null, null, null, null]; break; // ellipse
               case 4: flags = [null, null, null, true]; break; // box
               default: flags = [true, true, true, null]; // free style
            }
            dom.select(".cond_ellipse_a2").attr('disabled', flags[0]);
            dom.select(".cond_ellipse_theta").attr('disabled', flags[1]);
            dom.select(".cond_ellipse_theta_slider").attr('disabled', flags[2]);
            dom.select(".cond_ellipse_points").attr('disabled', flags[3]);
            editor.markChanged("ellishape");
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

      dom.select(".cut_points").on("change", () => this.changePolygonDimension());

      dom.select(".cond_ellipse_points").on("change", () => this.markChanged("ellinpts"));

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
