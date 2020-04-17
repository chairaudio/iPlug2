/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @ingroup Controls
 * @copydoc IWebViewControl
 */

#include "IControl.h"

#define OBJC_OLD_DISPATCH_PROTOTYPES 1
#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc-runtime.h>

#define WKUserScriptInjectionTimeAtDocumentStart 0

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

// Helpers to avoid too much typing
id operator"" _cls(const char *s, std::size_t) { return (id)objc_getClass(s); }
SEL operator"" _sel(const char *s, std::size_t) { return sel_registerName(s); }
id operator"" _str(const char *s, std::size_t) {
  return objc_msgSend("NSString"_cls, "stringWithUTF8String:"_sel, s);
}

using dispatch_fn_t = std::function<void()>;

/**
 * @ingroup IControls */
class IWebViewControl : public IControl
{
public:
  IWebViewControl(const IRECT& bounds)
  : IControl(bounds)
  {
    // Delegate
    auto cls = objc_allocateClassPair((Class) "NSResponder"_cls, "AppDelegate", 0);
    class_addProtocol(cls, objc_getProtocol("NSTouchBarProvider"));
    class_addMethod(cls, "userContentController:didReceiveScriptMessage:"_sel,
                  (IMP)(+[](id self, SEL, id, id msg) {
                    auto w = (IWebViewControl *)objc_getAssociatedObject( self, "webview");
                    w->on_message((const char *)objc_msgSend(objc_msgSend(msg, "body"_sel), "UTF8String"_sel));
                  }),
                  "v@:@@");
    objc_registerClassPair(cls);

    auto delegate = objc_msgSend((id)cls, "new"_sel);
    objc_setAssociatedObject(delegate, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    id app = objc_msgSend("NSApplication"_cls, "sharedApplication"_sel);
    objc_msgSend(app, sel_registerName("setDelegate:"), delegate);
    
    // Webview
    auto config = objc_msgSend("WKWebViewConfiguration"_cls, "new"_sel);
    m_manager = objc_msgSend(config, "userContentController"_sel);
    m_webview = objc_msgSend("WKWebView"_cls, "alloc"_sel);
    if (0 /*debug*/)
    {
      objc_msgSend(objc_msgSend(config, "preferences"_sel),
                   "setValue:forKey:"_sel,
                   objc_msgSend("NSNumber"_cls, "numberWithBool:"_sel, 1),
                   "developerExtrasEnabled"_str);
    }
    objc_msgSend(m_webview, "initWithFrame:configuration:"_sel,
                 CGRectMake(bounds.L, bounds.T, bounds.W(), bounds.H()), config);
    objc_msgSend(m_manager, "addScriptMessageHandler:name:"_sel, delegate,
                 "external"_str);
    init(R"script(
                      window.external = {
                        invoke: function(s) {
                          window.webkit.messageHandlers.external.postMessage(s);
                        },
                      };
                     )script");
  }
  
  void OnAttached() override
  {
    GetUI()->AddSubView(m_webview);
//    navigate("https://github.com/zserge/webview");
//    navigate("data:text/html,%3Chtml%3Ehello%3C%2Fhtml%3E");
//    navigate("(data:text/html,hello)");
  }
  
  void Draw(IGraphics& g) override
  {
  }
  
  void dispatch(std::function<void()> f) {
    dispatch_async_f(dispatch_get_main_queue(), new dispatch_fn_t(f),
                     (dispatch_function_t)([](void *arg) {
                       auto f = static_cast<dispatch_fn_t *>(arg);
                       (*f)();
                       delete f;
                     }));
  }
  
  void navigate(const std::string url) {
    auto nsurl = objc_msgSend(
        "NSURL"_cls, "URLWithString:"_sel,
        objc_msgSend("NSString"_cls, "stringWithUTF8String:"_sel, url.c_str()));
    objc_msgSend(
        m_webview, "loadRequest:"_sel,
        objc_msgSend("NSURLRequest"_cls, "requestWithURL:"_sel, nsurl));
  }
  void init(const std::string js) {
    objc_msgSend(
        m_manager, "addUserScript:"_sel,
        objc_msgSend(objc_msgSend("WKUserScript"_cls, "alloc"_sel),
                     "initWithSource:injectionTime:forMainFrameOnly:"_sel,
                     objc_msgSend("NSString"_cls, "stringWithUTF8String:"_sel,
                                  js.c_str()),
                     WKUserScriptInjectionTimeAtDocumentStart, 1));
  }
  void eval(const std::string js) {
    objc_msgSend(
        m_webview, "evaluateJavaScript:completionHandler:"_sel,
        objc_msgSend("NSString"_cls, "stringWithUTF8String:"_sel, js.c_str()),
        nullptr);
  }

private:
  virtual void on_message(const std::string msg) {};

  id m_webview;
  id m_manager;
};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE

