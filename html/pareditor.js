// $Id$

JSROOT.define(["jquery", "jquery-ui"], $ => {

   "use strict";

   if (typeof JSROOT != "object") {
      let e1 = new Error("pareditor.js requires JSROOT to be already loaded");
      e1.source = "pareditor.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
      let e1 = new Error("pareditor.js requires GO4 to be already loaded");
      e1.source = "pareditor.js";
      throw e1;
   }

   // only during transition
   JSROOT.loadScript("https://root.cern/js/6.3.2/style/jquery-ui.min.css");

   // ===========================================================================================

   GO4.ParameterEditor = function(dom, par) {
      JSROOT.BasePainter.call(this, dom);
      this.par = par;
      this.changes = ["dummy", "init"]; // TODO: put to common "base class" of condition and parameter editor
   }

   GO4.ParameterEditor.prototype = Object.create(JSROOT.BasePainter.prototype);

   GO4.ParameterEditor.prototype.checkResize = function() { }

   // TODO: put to common "base class" of condition and parameter editor
   // add identifier of changed element to list, make warning sign visible
   GO4.ParameterEditor.prototype.markChanged = function(key) {
      // first avoid duplicate keys:
      if (this.changes.indexOf(key) >= 0) return;
      this.changes.push(key);
      console.log("Mark changed :%s", key);
      this.selectDom().select(".buttonChangedParameter").style("display", null);// show warning sign
   }

   // TODO: put to common "base class" of condition and parameter editor
   GO4.ParameterEditor.prototype.clearChanges = function() {
      this.changes = []; //
      this.selectDom().select(".buttonChangedParameter").style("display", "none"); // hide warning sign
   }

   // scan changed value list and return optionstring to be send to server
   GO4.ParameterEditor.prototype.evaluateChanges = function(optionstring) {
      let dom = this.selectDom(),
          len = this.changes.length;
      for (let index = 0; index < len; index++) {
         //let cursor=changes.pop();
         let key = this.changes[index];
         console.log("Evaluate change key:%s", key);
         // here mapping of key to editor field:
         // key will be name of letiable which is class name of input field:
         let val = dom.select("." + key.toString()).property("value"),
             arraysplit = key.split("_"), opt = "";
         if (arraysplit.length > 1) {
            // found array with index after separator, reformat it:
            opt = arraysplit[0];
            if (arraysplit.length > 3) {
               // 3dim array:
               let ix = arraysplit[arraysplit.length - 3], //
                   iy = arraysplit[arraysplit.length - 2], //
                   iz = arraysplit[arraysplit.length - 1]; //
               opt += `[${ix}][${iy}][${iz}]`;
            } else if (arraysplit.length > 2) {
               // 2dim array:
               let ix = arraysplit[arraysplit.length - 2], //
                   iy = arraysplit[arraysplit.length - 1]; //
               opt += `[${ix}][${iy}]`;
            } else {
               // 1dim array:
               opt = arraysplit[0];
               let ix = arraysplit[arraysplit.length - 1]; //
               opt += `[${ix}]`;
            }
         } else {
            opt = key;
         }

         optionstring += `&${opt}=${val}`;
      }// for index
      console.log("Resulting option string:%s", optionstring);
      return optionstring;
   }

   /** @summary fill comments for parameter */
   GO4.ParameterEditor.prototype.fillComments = function() {
      if (this.xreq || !this.getItemName())
         return Promise.resolve(this); // avoid double requests

      let tr_nodes = this.selectDom().select(".par_values tbody").selectAll("tr").nodes();

      this.xreq = true;

      return GO4.httpRequest(this.getItemName() + "/h.json?more", 'object').then(res => {

         tr_nodes.forEach(raw_tr => {
            let tr = d3.select(raw_tr),
                name = tr.select("td").text(),
                title = null, arrayinfo = null, typeinfo = null;
            for (let i in res._childs) {
               let n = res._childs[i]._name;
               let arsplit = name.split("["); // remove array information at the end, if any
               if (arsplit[0] == n) {
                  //if ((name==n) || (name.indexOf(n)==0)) {
                  title = res._childs[i]._title;
                  arrayinfo = res._childs[i]._arraydim;
                  typeinfo = res._childs[i]._typename;
                  //console.log("found title="+title+", arrayinfo="+arrayinfo);
                  break;
               }
            }
            if (title !== null)
               tr.select("td.par_comment").text(title).style('white-space', 'nowrap'); // comments from class member declaration
            if (typeinfo !== null) {
               tr.select("td.par_class").text(typeinfo).style('white-space', 'nowrap'); // member type

               let par_table = d3.select(raw_tr.parentNode.parentNode);

               if (par_table.classed("par_arraytable")) {

                  // if we are inside array table, indicate that we are an array
                  par_table.select('td.par_comment').text("Array").style('white-space', 'nowrap');

                  // put type information of array to subtable header
                  par_table.selectAll('td.par_class').text(arrayinfo ? `${typeinfo} [${arrayinfo}]` : typeinfo).style('white-space', 'nowrap');
               }
            }

         });

         return this; // return painter

      }).finally(() => { this.xreq = false; });
   }

   GO4.ParameterEditor.prototype.fillMemberTable = function() {
      let editor = this,
          par = this.par,
          dom = this.selectDom();

      dom.selectAll(".par_values tbody").html("");
      let found_title = false;
      for (let key in par) {
         if (typeof par[key] == 'function') continue;
         if (key == 'fTitle') { found_title = true; continue; }
         if (!found_title) continue;
         let value = (par[key] != null ? (par[key] instanceof Array ? par[key] : par[key].toString()) : "null");
         let classname = "";
         if (value instanceof Array) {
            // here put array table with clickable header:
            // (thanks to Jonathan at http://mediaformations.com/accordion-tables-with-jquery/ for this idea!)
            let arraytableclass = key.toString() + "_array";
            let isTooBig = false;
            dom.select(".par_values > tbody").append("tr").html(`<td style='width:100%; padding:0px' colspan='4' > <table class='${arraytableclass} par_arraytable'><thead><tr><td class='par_key'> <bf>[+]</bf>${key.toString()}</td><td class='par_class'></td><td class='par_value' >Click to expand</td><td class='par_comment'></td></tr></thead><tbody></tbody></table></td>`);
            for (let i = 0; i < value.length; i++) {
               if (value[i] instanceof Array) {
                  let subvalue = value[i];
                  for (let j = 0; j < subvalue.length; j++) {
                     if (subvalue[j] instanceof Array) {
                        let subsubvalue = subvalue[j];
                        // here supress display of 3d arrays if too
                        // large:
                        if (subsubvalue.length * subvalue.length * value.length > 1000) {
                           isTooBig = true;
                           break;
                        }
                        else {
                           for (let k = 0; k < subsubvalue.length; k++) {
                              // decode 3dim array
                              classname = key.toString() + `_${i}_${j}_${k}`;
                              dom.select("." + arraytableclass + " tbody").append("tr")
                                 .html(`<td class='par_key'>${key.toString()}[${i}][${j}][${k}]</td>
                                        <td class='par_class'></td>
                                        <td class='par_value'><input type='text' size='10'  value='${subsubvalue[k]}' class='${classname}'/></td>
                                        <td class='par_comment'></td>`);

                           } // for k
                        }
                     }
                     else {
                        // decode 2dim array
                        classname = key.toString() + `_${i}_${j}`;
                        dom.select("." + arraytableclass + " tbody").append("tr")
                           .html(`<td class='par_key'>${key.toString()}[${i}][${j}]</td>
                                  <td class='par_class'></td>
                                  <td class='par_value'><input type='text' size='10' value='${subvalue[j]}' class='${classname}'/></td>
                                  <td class='par_comment'></td>`);

                     }
                  } // for j
               }
               else {
                  // decode 1dim array
                  classname = key.toString() + "_" + i;
                  dom.select("." + arraytableclass + " tbody").append("tr")
                     .html(`<td class='par_key'>${key.toString()}[${i}]</td>
                            <td class='par_class'></td>
                            <td class='par_value'><input type='text' size='10' value='${value[i]}' class='${classname}'/></td>
                            <td class='par_comment'></td>`);
               }
            } // for i
            //
            if (isTooBig) {
               dom.select("." + arraytableclass + " tbody")
                  .append("tr").html(
                     `<td class='par_key'>${key.toString()}</td>
                      <td colspan='3'> Sorry, Array dimension [${value.length}][${subvalue.length}][${subsubvalue.length}] too big to display!</td>`);
            }

            dom.select("table." + arraytableclass + " thead tr").on("click",
               function() {
                  let prnt = d3.select(this.parentNode);
                  while (!prnt.empty() && !prnt.classed('par_arraytable')) prnt = d3.select(prnt.node().parentNode);

                  let disp = prnt.select('tbody').style('display');
                  prnt.select('tbody').style('display', disp == 'none' ? null : 'none');

                  // $(this).parents('table.par_arraytable').children('tbody').toggle();

                  let txt = prnt.select('td').text();
                  if (txt.indexOf("[+]") >= 0)
                     txt = txt.replace("[+]", "[-]");
                  else
                     txt = txt.replace("[-]", "[+]");
                  prnt.select('td').text(txt);

                  txt = prnt.select('td.par_value').text();
                  if (txt.indexOf("expand") != -1)
                     txt = txt.replace("expand", "shrink");
                  else
                     txt = txt.replace("shrink", "expand");
                  prnt.select('td.par_value').text(txt);
               }
            );
            dom.select("table." + arraytableclass).select('tbody').style('display', 'none');

         } else {
            classname = key.toString();
            dom.select(".par_values > tbody").append("tr")
               .html(`<td class='par_key'>${key.toString()}</td>
                      <td class='par_class'></td>
                      <td class='par_value'><input type='text' size='10' value='${value}' class='${classname}'/></td>
                      <td class='par_comment'></td>`);
         }

      }
      // here set callbacks; referred classname must be evaluated dynamically in function!:
      dom.select(" .par_values tbody").selectAll("input").on("change", function() { editor.markChanged(d3.select(this).attr('class')); });
      dom.select(".par_values tbody").selectAll("td").classed("par_membertable_style", true);
      dom.selectAll(".par_values > thead th").classed("par_memberheader_style", true);
      dom.selectAll(".par_arraytable thead td").classed("par_arrayheader_style", true);

      this.clearChanges();
   }

   GO4.ParameterEditor.prototype.fillEditor = function() {
      let dom = this.selectDom();
          //width = dom.node().clientWidth,
          //height = dom.node().clientHeight;

      dom.select(".par_name").text(this.par.fName);
      dom.select(".par_type").text(this.par._typename);

      // dom.select("div").style("width", (width-4) + "px").style("height", (height-4) + "px");

      dom.select(".buttonGetParameter")
         .style('background-image', "url(" + GO4.source_dir + "icons/right.png)")
         .on("click", () => {
            console.log("update item = " + this.getItemName());
            if (JSROOT.hpainter)
               JSROOT.hpainter.display(this.getItemName());
            else
               console.log("hpainter not found");
         });

      dom.select(".buttonSetParameter")
         .style('background-image', "url(" + GO4.source_dir + "icons/left.png)")
         .on("click", () => {
            let options = this.evaluateChanges(""); // do not need to use name here
            console.log("set parameter " + this.getItemName() + ", options=" + options);
            GO4.ExecuteMethod(this, "UpdateFromUrl", options).then(() => {
               console.log("set parameter done.");
               this.clearChanges();
            }).catch(err => {
               console.log("Set parameter FAILED.", err);
            });
         })

      dom.select(".buttonChangedParameter")
         .style('background-image', "url(" + GO4.source_dir + "icons/info1.png)");

      this.fillMemberTable();
   }

   GO4.ParameterEditor.prototype.redrawObject = function(obj) {
      console.log('redraw parameter!!!');
      if (obj._typename != this.par._typename) return false;
      this.par = JSROOT.clone(obj);
      this.redraw(); // no need to redraw complete pad
      return true;
   }

   GO4.ParameterEditor.prototype.setItemName = function(name, opt, hpainter) {
      JSROOT.BasePainter.prototype.setItemName.call(this, name, opt, hpainter);
      this.fillComments();
   }

   GO4.ParameterEditor.prototype.redraw = function() {
      console.log("ParemeterEditor Redraw...");
      this.fillMemberTable();
      this.fillComments();
   }

   GO4.ParameterEditor.prototype.drawEditor = function() {

      let sel = this.selectDom(),
          h = sel.node().clientHeight,
          w = sel.node().clientWidth;

      if ((h < 10) && (w > 10)) sel.style("height", Math.round(w * 0.4)+"px");

      return JSROOT.httpRequest(GO4.source_dir + "html/pareditor.htm", "text").then(code => {
         sel.html(code);
         this.setTopPainter();
         this.fillEditor();
         return this.fillComments();
      });
   }

   GO4.drawParameter = function(dom, par /*, option */) {
      let editor = new GO4.ParameterEditor(dom, par);
      return editor.drawEditor();
   }

   return GO4;

});
