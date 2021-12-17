// $Id$

(function() {

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

  if (typeof GO4 == "object") {
      var e1 = new Error("GO4 already defined when loading go4.js");
      e1.source = "go4.js";
      throw e1;
   }

   GO4 = {};

   GO4.version = "6.1.4";

   // use location to load all other scripts when required
   GO4.source_dir = function() {
      var scripts = document.getElementsByTagName('script');

      for (var n in scripts) {
         if (scripts[n]['type'] != 'text/javascript') continue;

         var src = scripts[n]['src'];
         if ((src == null) || (src.length == 0)) continue;

         var pos = src.indexOf("html/go4.js");
         if (pos<0) continue;
         if ((src.indexOf("JSRootCore") >= 0) || (src.indexOf("JSRoot.core") >= 0)) continue;
         console.log('Set GO4.source_dir to ' + src.substr(0, pos));
         return src.substr(0, pos);
      }
      return "";
   }();

   let BasePainter = JSROOT.BasePainter || JSROOT.TBasePainter;

   if (!BasePainter.prototype.get_main_id) {
      GO4.id_counter = 1;
      // method removed from JSROOT v6, is not required there, therefore reintroduce it here
      BasePainter.prototype.get_main_id = function() {
         var elem = this.selectDom();
         if (elem.empty()) return "";
         var id = elem.attr("id");
         if (!id) {
            id = "go4_element_" + GO4.id_counter++;
            elem.attr("id", id);
         }
         return id;
      }
   }

   if (!BasePainter.prototype.getItemName)
      BasePainter.prototype.getItemName = BasePainter.prototype.GetItemName;

   if (typeof JSROOT.httpRequest == 'function')
      GO4.httpRequest = JSROOT.httpRequest;
   else
      GO4.httpRequest = function(url, kind, post_data) {
         return new Promise((resolveFunc,rejectFunc) => {
            let req = JSROOT.NewHttpRequest(url,kind, (res) => {
               if (res === null)
                  rejectFunc(Error(`Fail to request ${url}`));
               else
                  resolveFunc(res);
            });

            req.send(post_data || null);
         });
      }

   GO4.ExecuteMethod = function(item, method, options, callback) {
      var prefix = "";
      if (item.getItemName())
         prefix = item.getItemName() + "/"; // suppress / if item name is empty
      prefix += "exe.json\?method=";

      var fullcom = prefix + method + (options || "&"); // send any arguments otherwise ROOT refuse to process it

      GO4.httpRequest(fullcom, 'text')
         .then(() => callback(true))
         .catch(() => callback(false))
         .finally(() => console.log('Command is completed ' + prefix + method));
   }

   // ==================================================================================

   GO4.DrawAnalysisRatemeter = function(divid, itemname) {

      function CreateHTML() {
         var elem = d3.select('#'+divid);

         if (elem.size() == 0) return null;
         if (elem.select(".event_rate").size() > 0) return elem;

         var html = "<div style='padding-top:2px'>";
         html += "<img class='go4_logo' style='vertical-align:middle;margin-left:5px;margin-right:5px;' src='go4sys/icons/go4logorun4.gif' alt='logo'></img>";
         html += "<label class='event_source' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>file.lmd</label> ";
         html += "<label class='event_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; background-color: grey'; padding-left:3px; padding-right:3px;>---</label> Ev/s ";
         html += "<label class='aver_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> Ev/s ";
         html += "<label class='run_time' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> s ";
         html += "<label class='total_events' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> Events ";
         html += "<label class='analysis_time' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>time</label>";
         html += "</div>";

         elem.style('overflow','hidden')
             .style('padding-left','5px')
             .style('display', 'inline-block')
             .style('white-space', 'nowrap')
             .html(html);

         // use height of child element
         var brlayout = JSROOT.hpainter ? JSROOT.hpainter.brlayout : null,
             sz = $('#'+divid + " div").height() + 4; // use jquery to get height

         if (brlayout)
            if (JSROOT._)
               brlayout.adjustSeparators(null, sz, true);
            else
               brlayout.AdjustSeparator(null, sz, true);
         return elem;
      }

      var xreq = false, was_running = null;

      function UpdateRatemeter() {
         if (xreq) return;

         let elem = CreateHTML();
         if (!elem) return;

         xreq = true;
         GO4.httpRequest(itemname+"/root.json.gz", 'object').then(res => {
            elem.select(".event_rate").style('background-color', res.fbRunning ? 'lightgreen' : 'red');
            if (was_running != res.fbRunning)
               elem.select(".go4_logo").attr("src", res.fbRunning ? 'go4sys/icons/go4logorun4.gif' : 'go4sys/icons/go4logo_t.png');

            was_running = res.fbRunning;

            elem.select(".event_source").text(res.fxEventSource == "" ? "<source>" : res.fxEventSource);
            elem.select(".event_rate").text(res.fdRate.toFixed(1));
            elem.select(".aver_rate").text((res.fdTime > 0 ? res.fuCurrentCount / res.fdTime : 0).toFixed(1));
            elem.select(".run_time").text(res.fdTime.toFixed(1));
            elem.select(".total_events").text(res.fuCurrentCount);
            elem.select(".analysis_time").text(res.fxDateString == "" ? "<datime>" : res.fxDateString);
         }).catch(() => {
            elem.select(".event_rate").style('background-color','grey');
         }).finally(() => {
            xreq = false;
         });
      }

      CreateHTML();

      setInterval(UpdateRatemeter, 2000);
   }


   GO4.MakeMsgListRequest = function(hitem, item) {
      var arg = "&max=2000";
      if ('last-id' in item) arg+= "&id="+item['last-id'];
      return 'exe.json.gz?method=Select' + arg;
   }

   GO4.AfterMsgListRequest = function(hitem, item, obj) {
      if (!item) return;

      if (!obj) {
         delete item['last-id'];
         return;
      }
      // ignore all other classes
      if (obj._typename != 'TList') return;

      obj._typename = "TGo4MsgList";

      if (obj.arr.length>0) {
         var duplicate = (('last-id' in item) && (item['last-id'] == obj.arr[0].fString));

         item['last-id'] = obj.arr[0].fString;

         // workaround for snapshot, it sends always same messages many times
         if (duplicate) obj.arr.length = 1;

         // add clear function for item
         if (!('clear' in item))
            item['clear'] = function() { delete this['last-id']; }
      }
   }


   GO4.MsgListPainter = function(divid, lst) {
      BasePainter.call(this, divid);
      if (this.SetDivId) this.SetDivId(divid); // old
      this.lst = lst;
      return this;
   }

   GO4.MsgListPainter.prototype = Object.create( BasePainter.prototype );

   GO4.MsgListPainter.prototype.redrawObject = function(obj) {
      // if (!obj._typename != 'TList') return false;
      this.lst = obj;
      this.Draw();
      return true;
   }

   if (!JSROOT._)
      GO4.MsgListPainter.prototype.RedrawObject = GO4.MsgListPainter.prototype.redrawObject;

   GO4.MsgListPainter.prototype.Draw = function() {
      if (!this.lst) return;

      var frame = JSROOT._ ? this.selectDom() : this.select_main();

      var main = frame.select("div");
      if (main.empty())
         main = frame.append("div")
                     .style('max-width','100%')
                     .style('max-height','100%')
                     .style('overflow','auto');

      var old = main.selectAll("pre");
      var newsize = old.size() + this.lst.arr.length - 1;

      // in the browser keep maximum 2000 entries
      if (newsize > 2000)
         old.select(function(d,i) { return i < newsize - 2000 ? this : null; }).remove();

      for (var i=this.lst.arr.length-1;i>0;i--)
         main.append("pre").style('margin','3px').html(this.lst.arr[i].fString);

   }

   GO4.DrawMsgList = function(divid, lst, opt) {

      var painter = new GO4.MsgListPainter(divid, lst);

      painter.Draw();

      if (JSROOT._) {
         painter.setTopPainter();
         return Promise.resolve(painter);
      }
      // (re) set painter to first child element
      painter.SetDivId(divid); // old
      return painter.DrawingReady();
   }

   GO4.drawAnalysisTerminal = function(hpainter, itemname) {
      var url = "", mdi, frame;
      // FIXME: only for short backward compatibility with jsroot5
      if (JSROOT._) {
         url = hpainter.getOnlineItemUrl(itemname);
         mdi = hpainter.getDisplay();
         if (mdi) frame = mdi.findFrame(itemname, true);
      } else {
         url = hpainter.GetOnlineItemUrl(itemname);
         mdi = hpainter.GetDisplay();
         if (mdi) frame = mdi.FindFrame(itemname, true);
      }

      if (!url || !frame) return null;

      var divid = d3.select(frame).attr('id');

      var h = $("#"+divid).height(), w = $("#"+divid).width();
      if ((h<10) && (w>10)) $("#"+divid).height(w*0.7);

      var player = new BasePainter(divid);
      player.url = url;
      player.hpainter = hpainter;
      player.itemname = itemname;
      player.draw_ready = true;
      player.needscroll = false;

      player.LogReady = function(p) {
         if (p) this.log_painter = p;
         if(this.needscroll) {
            this.ClickScroll();
            this.needscroll = false;
         }
         this.draw_ready = true;
      }

   if (JSROOT._)
      player.cleanup = function() {
         if (this.log_painter) {
            this.log_painter.cleanup();
            delete this.log_painter;
         }
         if (this.interval) {
            clearInterval(this.interval);
            delete this.interval;
         }
         BasePainter.prototype.cleanup.call(this);
      }
   else
      player.Cleanup = function() {
         if (this.log_painter) {
            this.log_painter.Cleanup();
            delete this.log_painter;
         }
         if (this.interval) {
            clearInterval(this.interval);
            delete this.interval;
         }
         BasePainter.prototype.Cleanup.call(this);
      }


      player.ProcessTimer = function() {
         var subid = "anaterm_output_container";
         if ($("#" + subid).length == 0) {
            // detect if drawing disappear
            return JSROOT._ ? this.cleanup() : this.Cleanup();
         }
         if (!this.draw_ready) return;

         var msgitem = this.itemname.replace("Control/Terminal", "Status/Log");

         this.draw_ready = false;

         if (this.log_painter)
            this.hpainter.display(msgitem, "update:divid:" + subid).then(() => this.LogReady());
         else if (JSROOT._)
            this.hpainter.display(msgitem, "divid:" + subid).then(p => this.LogReady(p));
         else
            this.hpainter.display(msgitem, "divid:" + subid, () => this.LogReady());
      }

      player.ClickCommand = function(kind) {
         var pthis = this;
         var command = this.itemname.replace("Terminal", "CmdExecute");
         if (JSROOT._)
            this.hpainter.executeCommand(command, null, kind). then(() => { pthis.needscroll = true; });
         else
            this.hpainter.ExecuteCommand(command, function() { pthis.needscroll = true; }, kind);
      }

      player.ClickClear = function() {
         document.getElementById("anaterm_output_container").firstChild.innerHTML = "";
      }

      player.ClickScroll = function() {
         //  inner frame created by hpainter has the scrollbars, i.e. first child
         var disp = $("#anaterm_output_container").children(":first");
         disp.scrollTop(disp[0].scrollHeight - disp.height());
      }


      player.fillDisplay = function(id) {
          if (JSROOT._) {
             this.setTopPainter();
          } else {
             this.SetDivId(id); // old
          }
          this.interval = setInterval(this.ProcessTimer.bind(this), 2000);

          id = "#" + id; // to use in jQuery

          $(id + " .go4_clearterm")
              .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
              .click(this.ClickClear.bind(this))
              .children(":first") // select first button element, used for images
              .css('background-image', "url(" + GO4.source_dir + "icons/clear.png)");

          $(id + " .go4_endterm")
            .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
            .click(this.ClickScroll.bind(this))
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/shiftdown.png)");

          $(id + " .go4_printhistos")
             .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
             .click(this.ClickCommand.bind(this,"@PrintHistograms()"))
             .children(":first") // select first button element, used for images
             .css('background-image', "url(" + GO4.source_dir + "icons/hislist.png)");

          $(id + " .go4_printcond")
             .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
             .click(this.ClickCommand.bind(this,"@PrintConditions()"))
             .children(":first") // select first button element, used for images
             .css('background-image', "url(" + GO4.source_dir + "icons/condlist.png)");

          var pthis = this;

          $("#go4_anaterm_cmd_form").submit(
              function(event) {
                 var command = pthis.itemname.replace("Terminal", "CmdExecute");
                 var cmdpar = document.getElementById("go4_anaterm_command").value;
                 console.log("submit command - " + cmdpar);
                 if (JSROOT._)
                    pthis.hpainter.executeCommand(command,  null, cmdpar).then(() => { pthis.needscroll = true; });
                 else
                    pthis.hpainter.ExecuteCommand(command,  function() { pthis.needscroll = true; }, cmdpar);
                 event.preventDefault();
              });

         $(id + " .go4_executescript")
            .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/macro_t.png)");
      }

      player.CheckResize = player.checkResize = function() {}

      $("#"+divid).load(GO4.source_dir + "html/terminal.htm", "", () => player.fillDisplay(divid));

      return player;
   }

   function drawPictureObjects(divid, pic, k) {
      if (!divid || !pic.fxNames)
         return Promise.resolve(false);
      
      let arr = pic.fxNames ? pic.fxNames.arr : null;
      if (!arr || (k >= arr.length))
         return Promise.resolve(false);
      
      let n = pic.fxNames.arr[k], itemname = "";
         
      JSROOT.hpainter.forEachItem(item => {
         if (item._name == n.fString) itemname = JSROOT.hpainter.itemFullName(item); 
      });
      
      if (!itemname) {
         console.log('not found object with name', n.fString);
         return drawPictureObjects(divid, pic, k+1);
      }

      // console.log('Want to display item', itemname, 'on', divid);
      
      let opt = k > 0 ? "same" : "";
      
      return JSROOT.hpainter.display(itemname, opt + "divid:" + divid).then(() => drawPictureObjects(divid, pic, k+1));  
   }
   
   function drawSubPictures(pad_painter, pic, nsub) {
      let arr = pic && pic.fxSubPictures ? pic.fxSubPictures.arr : null;
      if (!arr || nsub >= arr.length)
         return Promise.resolve(pad_painter);
         
      let subpic = pic.fxSubPictures.arr[nsub];
         
      let subpad_painter = pad_painter.getSubPadPainter(1 + subpic.fiPosY*pic.fiNDivX + subpic.fiPosX);
      
      return drawPicture(subpad_painter, subpic).then(() => drawSubPictures(pad_painter, pic, nsub+1));  
   }
   
   function drawPicture(pad_painter, pic) {
      if (!pad_painter) 
         return Promise.resolve(false);
         
      let need_divide = pic.fiNDivX * pic.fiNDivY > 1;
      
      if (need_divide && !pad_painter.divide) {
         console.log('JSROOT version without PadPainter.divide');
         return Promise.resolve(false); 
      }   

      let prev_name = pad_painter.selectCurrentPad(pad_painter.this_pad_name);

      if (need_divide)
         return pad_painter.divide(pic.fiNDivX, pic.fiNDivY).then(() => drawSubPictures(pad_painter, pic, 0)).then(() => {
            pad_painter.selectCurrentPad(prev_name);
            return pad_painter;
         });

      let divid = pad_painter.selectDom().attr('id');
      if (!divid) {
         divid = "go4picture_div_" + GO4.id_counter++;
         pad_painter.selectDom().attr('id', divid);
         console.error('Drawing must be done on element with id, force ', divid);
      }
      
      return drawPictureObjects(divid, pic, 0).then(() => {
         pad_painter.selectCurrentPad(prev_name);
         return pad_painter; 
      });  
   }

   GO4.drawGo4Picture = function(dom, pic) {
      if (!JSROOT._ || !JSROOT.hpainter) return null;
      
      let painter = new JSROOT.ObjectPainter(dom, pic);

      return JSROOT.require('gpad').then(() => JSROOT.Painter.ensureTCanvas(painter, false)).then(() => {  
         let pad_painter = painter.getPadPainter();
         
         painter.removeFromPadPrimitives();
         
         return drawPicture(pad_painter, pic);
      }).then(() => painter); // return dummy painter
   }

   // ==============================================================================

   var canvsrc = GO4.source_dir + 'html/go4canvas.js;';
   var jsrp = JSROOT._ ? JSROOT.Painter : JSROOT;

   jsrp.addDrawFunc({ name: "TGo4WinCond",  script: canvsrc + GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond', opt: ";editor" });
   jsrp.addDrawFunc({ name: "TGo4PolyCond", script: canvsrc + GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond', opt: ";editor" });
   jsrp.addDrawFunc({ name: "TGo4ShapedCond", script: canvsrc + GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond', opt: ";editor" });
   jsrp.addDrawFunc({ name: "TGo4CondArray", script: canvsrc + GO4.source_dir + 'html/condition.js', func: 'GO4.drawCondArray', opt: ";editor" });
   jsrp.addDrawFunc({ name: "TGo4Marker", script: canvsrc, func: 'GO4.drawGo4Marker' });

   jsrp.addDrawFunc({ name: "TGo4AnalysisWebStatus", script: GO4.source_dir + 'html/analysiseditor.js', func: 'GO4.drawGo4AnalysisStatus', opt: "editor" });

   jsrp.addDrawFunc({ name: "TGo4MsgList", func: GO4.DrawMsgList });
   jsrp.addDrawFunc({ name: "TGo4Picture", func: GO4.drawGo4Picture, icon: GO4.source_dir + "icons/picture.png" });

   jsrp.addDrawFunc({ name: "TGo4MbsEvent", noinspect: true });
   jsrp.addDrawFunc({ name: "TGo4EventElement", noinspect: true });

})(); // factory
