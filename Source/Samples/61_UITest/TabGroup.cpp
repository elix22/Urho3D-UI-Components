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

#include "TabGroup.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
void TabGroup::RegisterObject(Context* context)
{
    context->RegisterFactory<TabGroup>(UI_CATEGORY);
}

TabGroup::TabGroup(Context *context) : BorderImage(context)
{
    SetLayoutMode(LM_VERTICAL);
    SetLayoutSpacing(0);
}

TabGroup::~TabGroup()
{
    childList_.Clear();
}

void TabGroup::InitInternal(const IntVector2 &tabSize, const IntVector2 &bodySize)
{
    if (tabContainer_ == NULL && bodyContainer_ == NULL)
    {
        // default settings
        SetLayoutMode(LM_VERTICAL);
        SetLayoutSpacing(0);
        SetClipChildren(true);

        tabContainer_ = CreateChild<UIElement>();
        tabContainer_->SetLayoutMode(LM_HORIZONTAL);
        tabContainer_->SetLayoutSpacing(2);
        tabContainer_->SetMaxHeight(tabSize.y_);

        bodyContainer_ = CreateChild<UIElement>();
    }
}

TabElement* TabGroup::CreateTab(const IntVector2 &tabSize, const IntVector2 &bodySize)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    TabElement tabElement;

    // init containers
    InitInternal(tabSize, bodySize);

    // button
    tabElement.tabButton_ = new CheckBox(context_);
    tabContainer_->AddChild(tabElement.tabButton_);

    tabElement.tabButton_->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    tabElement.tabButton_->SetStyle("TabButton");

    // setting the style doesn't seem to take(bug?) - manually set image rect and checked offset
    tabElement.tabButton_->SetImageRect(IntRect(208, 64, 224, 80));
    tabElement.tabButton_->SetCheckedOffset(IntVector2(0,16));

    tabElement.tabButton_->SetFixedSize(tabSize);
    tabElement.tabButton_->SetChecked(childList_.Size() == 0);

    tabElement.tabText_ = tabElement.tabButton_->CreateChild<Text>();
    tabElement.tabText_->SetAlignment(HA_CENTER, VA_CENTER);

    // body
    tabElement.tabBody_ = new BorderImage(context_);
    bodyContainer_->AddChild(tabElement.tabBody_);

    tabElement.tabBody_->SetStyle("TabBody");
    tabElement.tabBody_->SetImageRect(IntRect(192, 80, 208, 96));
    tabElement.tabBody_->SetSize(bodySize);
    tabElement.tabBody_->SetVisible(childList_.Size() == 0);

    UIElement::SetSize(internalSize_);
    childList_.Push(tabElement);

    return &childList_.Back();
}

TabElement* TabGroup::GetTabElement(unsigned idx)
{
    TabElement *element = NULL;

    if (idx < childList_.Size())
        element = &childList_[idx];

    return element;
}

void TabGroup::SetEnabled(bool enabled)
{
    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        if (enabled)
        {
            SubscribeToEvent(childList_[i].tabButton_, E_TOGGLED, URHO3D_HANDLER(TabGroup, HandleTabToggled));
        }
        else
        {
            UnsubscribeFromEvent(childList_[i].tabButton_, E_TOGGLED);
        }

    }
}

void TabGroup::SetSize(int width, int height)
{
    SetSize(IntVector2(width, height));
}

void TabGroup::SetSize(const IntVector2& size)
{
    internalSize_ = size;
    UIElement::SetSize(internalSize_);
}

void TabGroup::HandleTabToggled(StringHash eventType, VariantMap& eventData)
{
    using namespace Toggled;

    CheckBox *element = (CheckBox*)eventData[P_ELEMENT].GetVoidPtr();
    bool checked = eventData[P_STATE].GetBool();
    int checkedIdx = -1;

    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        CheckBox *chkbox = childList_[i].tabButton_;

        if (chkbox == element)
            checkedIdx = i;

        childList_[i].tabBody_->SetVisible(chkbox == element);

        chkbox->SetCheckedInternal(chkbox == element);
    }

    if (checkedIdx >= 0)
    {
        SendTabSelectedEvent(checkedIdx);
    }
}

void TabGroup::SendTabSelectedEvent(int idx)
{
    using namespace TabSelected;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    eventData[P_INDEX] = idx;
    SendEvent(E_TABSELECTED, eventData);
}


