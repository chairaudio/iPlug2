 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#if !__has_feature(objc_arc)
#error This file must be compiled with Arc. Use -fobjc-arc flag
#endif

#import <WebKit/WebKit.h>

#include "IPlugWebView.h"
#include "IPlugPaths.h"

namespace iplug {
extern bool GetResourcePathFromBundle(const char* fileName, const char* searchExt, WDL_String& fullPath, const char* bundleID);
}

using namespace iplug;

@interface ScriptHandler : NSObject <WKScriptMessageHandler, WKNavigationDelegate>
{
  IWebView* mWebView;
}
@end

@implementation ScriptHandler

-(id) initWithIWebView:(IWebView*) webView
{
  self = [super init];
  
  if(self)
    mWebView = webView;
  
  return self;
}

- (void) userContentController:(nonnull WKUserContentController*) userContentController didReceiveScriptMessage:(nonnull WKScriptMessage*) message
{
  if([[message name] isEqualToString:@"callback"])
  {
    NSDictionary* dict = (NSDictionary*) message.body;
    NSData* data = [NSJSONSerialization dataWithJSONObject:dict options:NSJSONWritingPrettyPrinted error:nil];
    NSString* jsonString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    mWebView->OnMessageFromWebView([jsonString UTF8String]);
  }
}

- (void) webView:(WKWebView*) webView didFinishNavigation:(WKNavigation*) navigation
{
  mWebView->OnWebContentLoaded();
}

@end

IWebView::IWebView(bool opaque)
: mOpaque(opaque)
{
}

IWebView::~IWebView()
{
  CloseWebView();
}

void* IWebView::OpenWebView(void* pParent, float x, float y, float w, float h, float scale)
{  
  WKWebViewConfiguration* webConfig = [[WKWebViewConfiguration alloc] init];
  WKPreferences* preferences = [[WKPreferences alloc] init];
  
  WKUserContentController* controller = [[WKUserContentController alloc] init];
  webConfig.userContentController = controller;

  ScriptHandler* scriptHandler = [[ScriptHandler alloc] initWithIWebView: this];
  [controller addScriptMessageHandler: scriptHandler name:@"callback"];
  [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
  webConfig.preferences = preferences;
  
  WKWebView* webView = [[WKWebView alloc] initWithFrame: MAKERECT(x, y, w, h) configuration:webConfig];
  

#if defined OS_IOS
  [webView.scrollView setScrollEnabled:NO];
  if(!mOpaque)
  {
    webView.backgroundColor = [UIColor clearColor];
    webView.scrollView.backgroundColor = [UIColor clearColor];
    webView.opaque = NO;
  }
#endif

#if defined OS_MAC
  if(!mOpaque)
    [webView setValue:[NSNumber numberWithBool:YES]  forKey:@"drawsTransparentBackground"];
  
  [webView setAllowsMagnification:NO];
#endif
  
  [webView setNavigationDelegate:scriptHandler];
    
//#ifdef OS_MAC
//  [webView setAutoresizingMask: NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin ];
//#else
//  [webView setAutoresizingMask: UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleTopMargin];
//#endif
//  [parentView setAutoresizesSubviews:YES];
  
  mWebConfig = (__bridge void*) webConfig;
  mWKWebView = (__bridge void*) webView;
  mScriptHandler = (__bridge void*) scriptHandler;
  
  OnWebViewReady();

  return (__bridge void*) webView;
}

void IWebView::CloseWebView()
{
  mWKWebView = nullptr;
  mWKWebView = nullptr;
  mScriptHandler = nullptr;
}

void IWebView::LoadHTML(const WDL_String& html)
{
  WKWebView* webView = (__bridge WKWebView*) mWKWebView;
  [webView loadHTMLString:[NSString stringWithUTF8String:html.Get()] baseURL:nil];
}

void IWebView::LoadURL(const char* url)
{
  WKWebView* webView = (__bridge WKWebView*) mWKWebView;
  
  NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url] relativeToURL:nil];
  NSURLRequest* req = [[NSURLRequest alloc] initWithURL:nsurl];
  [webView loadRequest:req];
}

void IWebView::LoadFile(const char* fileName, const char* bundleID)
{
  WKWebView* webView = (__bridge WKWebView*) mWKWebView;

  WDL_String fullPath;
  WDL_String fileNameWeb("web/");
  fileNameWeb.Append(fileName);

  GetResourcePathFromBundle(fileNameWeb.Get(), "html", fullPath, bundleID);
  
  NSString* pPath = [NSString stringWithUTF8String:fullPath.Get()];

  NSString* str = @"file:";
  NSString* webroot = [str stringByAppendingString:[pPath stringByReplacingOccurrencesOfString:[NSString stringWithUTF8String:fileName] withString:@""]];
  NSURL* pageUrl = [NSURL URLWithString:[webroot stringByAppendingString:[NSString stringWithUTF8String:fileName]] relativeToURL:nil];
  NSURL* rootUrl = [NSURL URLWithString:webroot relativeToURL:nil];

  [webView loadFileURL:pageUrl allowingReadAccessToURL:rootUrl];
}

void IWebView::EvaluateJavaScript(const char* scriptStr, completionHandlerFunc func)
{
  WKWebView* webView = (__bridge WKWebView*) mWKWebView;
  
  if (![webView isLoading])
  {
    [webView evaluateJavaScript:[NSString stringWithUTF8String:scriptStr] completionHandler:^(NSString *result, NSError *error) {
      if(error != nil)
        NSLog(@"Error %@",error);
      else if(func)
      {
        const WDL_String str {[result UTF8String]};
        func(str);
      }
    }];
  }
}

void IWebView::EnableScroll(bool enable)
{
#ifdef OS_IOS
  WKWebView* webView = (__bridge WKWebView*) mWKWebView;
  [webView.scrollView setScrollEnabled:enable];
#endif
}

void IWebView::SetWebViewBounds(float x, float y, float w, float h, float scale)
{
//  [NSAnimationContext beginGrouping]; // Prevent animated resizing
//  [[NSAnimationContext currentContext] setDuration:0.0];
  [(__bridge WKWebView*) mWKWebView setFrame: MAKERECT(x, y, (float) w, (float) h) ];
//  [NSAnimationContext endGrouping];
}

