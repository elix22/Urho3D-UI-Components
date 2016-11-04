//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Spline.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <SDL/SDL_log.h>

#include "SlideBar.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void SliderVariable::RegisterObject(Context* context)
{
    context->RegisterFactory<SliderVariable>();
}

SliderVariable::SliderVariable(Context *context)
    : BorderImage(context)
    , processCaller(NULL)
    , pfnValChangedCallback(NULL)
    , sensitivity_(0.1f)
{
}

SliderVariable::~SliderVariable()
{
}


bool SliderVariable::CreateBar(const IntVector2 &size)
{
    SetLayoutBorder(IntRect(5,5,5,5));
    SetLayoutMode(LM_VERTICAL);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    variableText_ = CreateChild<Text>();
    variableText_->SetVerticalAlignment(VA_CENTER);
    variableText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);

    SetSize(size);

    return true;
}

void SliderVariable::SetRange(Variant &vmin, Variant &vmax)
{
    float minVal, maxVal;
    varMin_ = vmin;
    varMax_ = vmax;

    switch (vmin.GetType())
    {
    case VAR_INT:
        minVal = (float)vmin.GetInt();
        maxVal = (float)vmax.GetInt();
        break;

    case VAR_FLOAT:
        minVal = vmin.GetFloat();
        maxVal = vmax.GetFloat();
        break;

    default:
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }
}

void SliderVariable::SetCurrentValue(Variant &val)
{
    varCurrentValue_ = val;

    switch (val.GetType())
    {
    case VAR_INT:
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
        break;

    case VAR_FLOAT:
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
        break;

    default:
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }
}

void SliderVariable::ValueUpdate(float delta)
{
    int ival;
    float fval;

    switch (varMax_.GetType())
    {
    case VAR_INT:
        ival = varCurrentValue_.GetInt() + (int)(delta * sensitivity_);

        if ( ival < varMin_.GetInt() )
        {
            varCurrentValue_ = varMin_;
        }
        else if ( ival > varMax_.GetInt())
        {
            varCurrentValue_ = varMax_;
        }
        else
        {
            varCurrentValue_ = ival;
        }
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
        break;

    case VAR_FLOAT:
        fval = varCurrentValue_.GetFloat() + delta * sensitivity_;

        if (fval < varMin_.GetFloat())
        {
            varCurrentValue_ = varMin_;
        }
        else if (fval > varMax_.GetFloat())
        {
            varCurrentValue_ = varMax_;
        }
        else
        {
            varCurrentValue_ = fval;
        }
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
        break;
    }
}

void SliderVariable::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                                const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    ValueUpdate((float)deltaPos.x_ );
}

//=============================================================================
//=============================================================================
void SlideBar::RegisterObject(Context* context)
{
    context->RegisterFactory<SlideBar>();
}

SlideBar::SlideBar(Context *context)
    : BorderImage(context)
    , processCaller(NULL)
    , pfnValChangedCallback(NULL)
    , storedSize_(IntVector2::ZERO)
    , sensitivity_(0.1f)
{
    SetPriority(1);
    SetBringToFront(true);
}

SlideBar::~SlideBar()
{
}

void SlideBar::InitInternal(const IntVector2 &size)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    if (headerElement_ == NULL && sliderElement_ == NULL)
    {
        SetLayoutMode(LM_VERTICAL);
        SetLayoutBorder(IntRect(3,3,3,3));

        headerElement_ = CreateChild<UIElement>();
        headerElement_->SetLayoutBorder(IntRect(4,4,4,4));
        headerElement_->SetHeight(25);

        headerText_ = headerElement_->CreateChild<Text>();
        headerText_->SetAlignment(HA_LEFT, VA_CENTER);

        sliderElement_ = CreateChild<BorderImage>();
        sliderElement_->SetLayoutMode(LM_HORIZONTAL);
        sliderElement_->SetLayoutBorder(IntRect(4,4,0,0));
        sliderElement_->SetMaxHeight(25);

        variableText_ = sliderElement_->CreateChild<Text>();
        variableText_->SetVerticalAlignment(VA_CENTER);
        variableText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    }
}

void SlideBar::CreateOutputConnector()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");

    // connector
    outputConnector_ = GetSubsystem<UI>()->GetRoot()->CreateChild<ConnectorLine>();
    outputConnector_->SetParent(this);
    outputConnector_->Create(uiTex2d, ConnectorLine::GetBoxRect());
    outputConnector_->SetEnabled(true);
    outputConnector_->SetFixedSize(ConnectorLine::GetBoxSize());
    outputConnector_->GetTailImage()->SetFixedSize(ConnectorLine::GetBoxSize());
}

BorderImage* SlideBar::CreateBar(const IntVector2 &size)
{
    InitInternal(size);

    SetSize(size);

    return sliderElement_;
}

void SlideBar::SetSize(int width, int height)
{
    SetSize(IntVector2(width, height));
}

void SlideBar::SetSize(const IntVector2& size)
{
    storedSize_ = size;

    UIElement::SetSize(size);
}

void SlideBar::FrameSetColor(const Color& color)
{
    UIElement::SetColor(color);
}

void SlideBar::SetBodyColor(const Color& color)
{
}

void SlideBar::SetSliderColor(const Color& color)
{
    if (sliderElement_)
    {
        sliderElement_->SetColor(color);
    }
}

void SlideBar::SetEnabled(bool enable)
{
    if (enabled_ != enable)
    {
        enabled_ = enable;
        SetEnabled(enabled_);
    }
}

void SlideBar::SetVariantRange(Variant &vmin, Variant &vmax)
{
    float minVal, maxVal;
    varMin_ = vmin;
    varMax_ = vmax;

    switch (vmin.GetType())
    {
    case VAR_INT:
        minVal = (float)vmin.GetInt();
        maxVal = (float)vmax.GetInt();
        break;

    case VAR_FLOAT:
        minVal = vmin.GetFloat();
        maxVal = vmax.GetFloat();
        break;

    default:
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }
}

void SlideBar::SetCurrentValue(Variant &val)
{
    varCurrentValue_ = val;

    switch (val.GetType())
    {
    case VAR_INT:
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
        break;

    case VAR_FLOAT:
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
        break;

    default:
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }
}

void SlideBar::ValueUpdate(float delta)
{
    int ival;
    float fval;

    switch (varMax_.GetType())
    {
    case VAR_INT:
        ival = varCurrentValue_.GetInt() + (int)(delta * sensitivity_);

        if ( ival < varMin_.GetInt() )
        {
            varCurrentValue_ = varMin_;
        }
        else if ( ival > varMax_.GetInt())
        {
            varCurrentValue_ = varMax_;
        }
        else
        {
            varCurrentValue_ = ival;
        }
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
        break;

    case VAR_FLOAT:
        fval = varCurrentValue_.GetFloat() + delta * sensitivity_;

        if (fval < varMin_.GetFloat())
        {
            varCurrentValue_ = varMin_;
        }
        else if (fval > varMax_.GetFloat())
        {
            varCurrentValue_ = varMax_;
        }
        else
        {
            varCurrentValue_ = fval;
        }
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
        break;
    }

    if (processCaller && pfnValChangedCallback)
    {
        (processCaller->*pfnValChangedCallback)(varCurrentValue_);
    }
}

void SlideBar::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                          const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    ValueUpdate((float)deltaPos.x_ );
}

