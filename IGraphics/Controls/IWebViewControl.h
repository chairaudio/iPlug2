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
#include "IPlugWebView.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** @ingroup IControls */
class IWebViewControl : public IControl, public IWebView
{
public:
  IWebViewControl(const IRECT& bounds, bool opaque = true)
  : IControl(bounds)
  , IWebView(opaque)
  {
  }
  
  void OnAttached() override
  {
    GetUI()->AddPlatformView(mRECT, OpenWebView(GetUI()->GetWindow(), mRECT.L, mRECT.T, mRECT.W(), mRECT.H(), GetUI()->GetTotalScale()));
  }
  
  void Draw(IGraphics& g) override
  {
     /* NO-OP */
  }

  void OnWebViewReady() override
  {
    WDL_String html{ "<meta name=\"viewport\" content=\"initial-scale=1.0\"/><p contentEditable=\"true\">Hello</p>" };
    LoadHTML(html);
//    LoadURL("https://google.com");
  }

  void OnRescale() override
  {
    SetWebViewBounds(mRECT.L, mRECT.T, mRECT.W(), mRECT.H(), GetUI()->GetTotalScale());
  }

  void OnResize() override
  {
    SetWebViewBounds(mRECT.L, mRECT.T, mRECT.W(), mRECT.H(), GetUI()->GetTotalScale());
  }
};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE

