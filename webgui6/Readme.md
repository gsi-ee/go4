# Full-functional TWebCanvas for ROOT6

This library completes implementation of TWebCanvas in ROOT.
It provides many kinds of interactive features like mouse-events forwarding, update of objects attributes,
invocation of menu commands.

To enable usage of this web-based canvas in ROOT6, one should just add following lines in rootlogon.C script

```
{
   gPluginMgr->AddHandler("TGuiFactory", "webfull", "TWebGuiFactoryFull", "WebGuiFull", "TWebGuiFactoryFull()");
   gEnv->SetValue("Gui.Factory", "webfull");
}
```
