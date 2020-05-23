 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#if !__has_feature(objc_arc)
#error This file must be compiled with Arc. Use -fobjc-arc flag
#endif

#include "IPlugWebViewEditorDelegate.h"
#import <WebKit/WebKit.h>
#include "config.h"
#include "IPlugPluginBase.h"
#include "IPlugPaths.h"
#include "wdl_base64.h"

BEGIN_IPLUG_NAMESPACE
extern bool GetResourcePathFromBundle(const char* fileName, const char* searchExt, WDL_String& fullPath, const char* bundleID);
END_IPLUG_NAMESPACE

using namespace iplug;

WebViewEditorDelegate::WebViewEditorDelegate(int nParams)
: IEditorDelegate(nParams)
, IWebView()
{
}

WebViewEditorDelegate::~WebViewEditorDelegate()
{
}

void* WebViewEditorDelegate::OpenWindow(void* pParent)
{
  PLATFORM_VIEW* parentView = (__bridge PLATFORM_VIEW*) pParent;
  PLATFORM_RECT r = [parentView frame];
  
  void* pView = OpenWebView(0, 0, r.size.width, r.size.height);
  
  if(mEditorInitFunc)
    mEditorInitFunc();
  
  [parentView addSubview: (__bridge PLATFORM_VIEW*) pView];
  
  return pView;
}

void WebViewEditorDelegate::CloseWindow()
{
  CloseWebView();
}

void WebViewEditorDelegate::SendControlValueFromDelegate(int ctrlTag, double normalizedValue)
{
  WDL_String str;
  str.SetFormatted(50, "SCVFD(%i, %f)", ctrlTag, normalizedValue);
  EvaluateJavaScript(str.Get());
}

void WebViewEditorDelegate::SendControlMsgFromDelegate(int ctrlTag, int msgTag, int dataSize, const void* pData)
{
  WDL_String str;
  WDL_TypedBuf<char> base64;
  int sizeOfBase64 = 4 * std::ceil(((double) dataSize/3.));
  base64.Resize(sizeOfBase64);
  wdl_base64encode(reinterpret_cast<const unsigned char*>(pData), base64.GetFast(), dataSize);
  str.SetFormatted(50, "SCMFD(%i, %i, %i, %s)", ctrlTag, msgTag, dataSize, base64.GetFast());
  EvaluateJavaScript(str.Get());
}

void WebViewEditorDelegate::SendParameterValueFromDelegate(int paramIdx, double value, bool normalized)
{
  WDL_String str;
  str.SetFormatted(50, "SPVFD(%i, %f)", paramIdx, value);
  EvaluateJavaScript(str.Get());
}

void WebViewEditorDelegate::SendArbitraryMsgFromDelegate(int msgTag, int dataSize, const void* pData)
{
  WDL_String str;
  WDL_TypedBuf<char> base64;
  int sizeOfBase64 = 4 * std::ceil(((double) dataSize/3.));
  base64.Resize(sizeOfBase64);
  wdl_base64encode(reinterpret_cast<const unsigned char*>(pData), base64.GetFast(), dataSize);
  str.SetFormatted(50, "SAMFD(%i, %i, %s)", msgTag, dataSize, base64.GetFast());
  EvaluateJavaScript(str.Get());
}

//void WebViewEditorDelegate::LoadFileFromBundle(const char* fileName)
//{
//  IPluginBase* pPlug = dynamic_cast<IPluginBase*>(this);
//  WKWebView* webView = (__bridge WKWebView*) mWKWebView;
//
//  WDL_String fullPath;
//  WDL_String fileNameWeb("web/");
//  fileNameWeb.Append(fileName);
//
//  GetResourcePathFromBundle(fileNameWeb.Get(), "html", fullPath, pPlug->GetBundleID());
  
//  NSString* pPath = [NSString stringWithUTF8String:fullPath.Get()];
//
//  NSString* str = @"file:";
//  NSString* webroot = [str stringByAppendingString:[pPath stringByReplacingOccurrencesOfString:[NSString stringWithUTF8String:fileName] withString:@""]];
//  NSURL* pageUrl = [NSURL URLWithString:[webroot stringByAppendingString:[NSString stringWithUTF8String:fileName]] relativeToURL:nil];
//  NSURL* rootUrl = [NSURL URLWithString:webroot relativeToURL:nil];
//
//  [webView loadFileURL:pageUrl allowingReadAccessToURL:rootUrl];
//}

void WebViewEditorDelegate::Resize(int width, int height)
{
////  [NSAnimationContext beginGrouping]; // Prevent animated resizing
////  [[NSAnimationContext currentContext] setDuration:0.0];
//  [(__bridge WKWebView*) mWKWebView setFrame: MAKERECT(0.f, 0.f, (float) width, (float) height) ];
////  [NSAnimationContext endGrouping];
//  EditorResizeFromUI(width, height, true);
}
