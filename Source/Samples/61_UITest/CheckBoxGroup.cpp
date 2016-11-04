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
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "CheckBoxGroup.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void CheckText::RegisterObject(Context* context)
{
    context->RegisterFactory<CheckText>();
}

void CheckText::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, 
                          int button, int buttons, int qualifiers, Cursor* cursor)
{
    using namespace Toggled;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    SendEvent(E_PRESSED, eventData);
}

void CheckBoxGroup::RegisterObject(Context* context)
{
    context->RegisterFactory<CheckBoxGroup>();
    CheckText::RegisterObject(context);
}

CheckBoxGroup::CheckBoxGroup(Context *context) 
    : BorderImage(context)
{
}

CheckBoxGroup::~CheckBoxGroup()
{
}

void CheckBoxGroup::InitInternal()
{
    if (titleText_ == NULL)
    {
        // default settings
        SetLayoutMode(LM_VERTICAL);

        // title
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        headerElement_ = CreateChild<UIElement>();
        headerElement_->SetLayoutMode(LM_VERTICAL);
        headerElement_->SetLayoutBorder(IntRect(20,4,4,4));
        headerElement_->SetMaxHeight(30);

        titleText_ = headerElement_->CreateChild<Text>();
        titleText_->SetText("Checkbox Group");
        titleText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        titleText_->SetVerticalAlignment(VA_CENTER);
    }
}

CheckboxDesc* CheckBoxGroup::CreateCheckboxDesc(const String& desc)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // init
    InitInternal();
 
    // create checkbox and desc
    CheckboxDesc chkDesc;

    UIElement *element = CreateChild<UIElement>();
    chkDesc.element_ = element;
    element->SetFocusMode(FM_FOCUSABLE_DEFOCUSABLE);
    element->SetLayoutMode(LM_HORIZONTAL);
    element->SetLayoutSpacing(10);
    element->SetMaxHeight(30);
    element->SetLayoutBorder(IntRect(40,0,0,0));

    chkDesc.checkBox_ = element->CreateChild<CheckBox>();
    chkDesc.checkBox_->SetStyleAuto();
    chkDesc.checkBox_->SetImageRect(IntRect(208, 96, 224, 112));
    chkDesc.checkBox_->SetChecked(childList_.Size() == 0);

    chkDesc.desc_ = element->CreateChild<CheckText>();
    chkDesc.desc_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 8);
    chkDesc.desc_->SetText(desc);

    UIElement::SetSize(internalSize_);
    childList_.Push(chkDesc);

    return &childList_.Back();
}

CheckboxDesc* CheckBoxGroup::GetCheckboxDesc(unsigned idx)
{
    CheckboxDesc *desc = NULL;

    if (idx < childList_.Size())
    {
        desc = &childList_[idx];
    }

    return desc;
}

Text* CheckBoxGroup::GetTitleTextElement() 
{
    if (titleText_ == NULL)
    {
        InitInternal();
    }
     
    return titleText_; 
}

void CheckBoxGroup::SetEnabled(bool enabled)
{
    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        if (enabled)
        {
            SubscribeToEvent(childList_[i].checkBox_, E_TOGGLED, URHO3D_HANDLER(CheckBoxGroup, HandleCheckbox));

            childList_[i].desc_->SetEnabled(true);
            SubscribeToEvent(childList_[i].desc_, E_PRESSED, URHO3D_HANDLER(CheckBoxGroup, HandlePressed));
        }
        else
        {
            childList_[i].desc_->SetEnabled(false);
            UnsubscribeFromEvent(childList_[i].checkBox_, E_TOGGLED);
            UnsubscribeFromEvent(childList_[i].desc_, E_PRESSED);
        }
    }
}

void CheckBoxGroup::SetSize(int width, int height)
{
    SetSize(IntVector2(width, height));
}

void CheckBoxGroup::SetSize(const IntVector2& size)
{
    internalSize_ = size;
    UIElement::SetSize(internalSize_);
}

void CheckBoxGroup::HandlePressed(StringHash eventType, VariantMap& eventData)
{
    using namespace Pressed;
    UIElement *element = (UIElement*)eventData[P_ELEMENT].GetVoidPtr();
    int selectedIdx = -1;

    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        CheckBox *chkbox = childList_[i].checkBox_;

        if (childList_[i].desc_ == element && !chkbox->IsChecked())
            selectedIdx = i;

        chkbox->SetCheckedInternal(childList_[i].desc_ == element);
    }

    if (selectedIdx >= 0)
    {
        SendGroupToggleEvent(selectedIdx);
    }
}

void CheckBoxGroup::HandleCheckbox(StringHash eventType, VariantMap& eventData)
{
    using namespace Toggled;

    CheckBox *element = (CheckBox*)eventData[P_ELEMENT].GetVoidPtr();
    bool checked = eventData[P_STATE].GetBool();
    int checkedIdx = -1;

    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        CheckBox *chkbox = childList_[i].checkBox_;

        if (chkbox == element && !chkbox->IsChecked())
            checkedIdx = i;

        chkbox->SetCheckedInternal(chkbox == element);
    }

    if (checkedIdx >= 0)
    {
        SendGroupToggleEvent(checkedIdx);
    }
}

void CheckBoxGroup::SendGroupToggleEvent(int idx)
{
    using namespace CheckGroupToggled;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    eventData[P_INDEX] = idx;
    SendEvent(E_CHECKGROUPTOGGLED, eventData);
}



