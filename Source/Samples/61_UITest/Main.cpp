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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/IO/FileSystem.h>

#include "Main.h"
#include "CheckBoxGroup.h"
#include "TabGroup.h"
#include "SpriteAnimBox.h"
#include "LineBatcher.h"
#include "SlideBar.h"
#include "LineComponent.h"
#include "DrawTool.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
URHO3D_DEFINE_APPLICATION_MAIN(Main)

//=============================================================================
//=============================================================================
Main::Main(Context* context) :
    Sample(context)
{
    CheckBoxGroup::RegisterObject(context);
    TabGroup::RegisterObject(context);
    SpriteAnimBox::RegisterObject(context);
    LineBatcher::RegisterObject(context);
    SlideBar::RegisterObject(context);
    SliderVariable::RegisterObject(context);
    ConnectorLine::RegisterObject(context);
    LineTest::RegisterObject(context);
    DrawTool::RegisterObject(context);
}

void Main::Setup()
{
    engineParameters_["WindowTitle"]  = GetTypeName();
    engineParameters_["LogName"]      = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"]   = false;
    engineParameters_["Headless"]     = false;
    engineParameters_["WindowWidth"]  = 1280; 
    engineParameters_["WindowHeight"] = 720;
}

void Main::Start()
{
    // Execute base class startup
    Sample::Start();

    // Set mouse visible
    String platform = GetPlatform();
    if (platform != "Android" && platform != "iOS")
        GetSubsystem<Input>()->SetMouseVisible(true);

    // change background color
    colorBackground_ = Color(0.3f, 0.3f, 0.9f);
    GetSubsystem<Renderer>()->GetDefaultZone()->SetFogColor(colorBackground_);

    // Create the UI content
    CreateGUI();
    CreateInstructions();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}

void Main::CreateGUI()
{
    CreateCheckboxGroup();
    CreateTabGroup();
    CreateSpriteAnimBox();
    CreateConnectorLine();
    CreateSliderVariable();
    CreateDrawTool();
}

void Main::CreateCheckboxGroup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();
    root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    // create group
    CheckBoxGroup *ckboxGroup = root->CreateChild<CheckBoxGroup>();
    ckboxGroup->SetPosition(20, 50);
    ckboxGroup->SetSize(300, 300);
    ckboxGroup->SetColor(Color(0.2f,0.2f,0.2f));

    for (int i = 0; i < 4; ++i)
    {
        String desc = String("checkbox ") + String(i+1) + String(" - (click text)"); 
        ckboxGroup->CreateCheckboxDesc(desc);
    }

    ckboxGroup->SetEnabled(true);
    SubscribeToEvent(E_CHECKGROUPTOGGLED, URHO3D_HANDLER(Main, HandleCheckboxSelected));
}

void Main::CreateTabGroup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();
    root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    Text* gtext1 = ui->GetRoot()->CreateChild<Text>();
    gtext1->SetPosition(370, 20);
    gtext1->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    gtext1->SetText("Tab group");

    // create tab group
    TabGroup *tabgroup = root->CreateChild<TabGroup>();
    tabgroup->SetPosition(350, 40);
    tabgroup->SetSize(300, 180);
    tabgroup->SetLayoutBorder(IntRect(10,10,10,10));
    tabgroup->SetColor(Color(Color(0,0,0,0)));

    // then create tabs
    const IntVector2 tabSize(60, 25);
    const IntVector2 tabBodySize(280, 150);

    for (int i = 0; i < 4; ++i)
    {
        String tabLabel = String("tab ") + String(i+1);

        TabElement *tabElement = tabgroup->CreateTab(tabSize, tabBodySize);
        tabElement->tabText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 8);
        tabElement->tabText_->SetText(tabLabel);

        tabElement->tabButton_->SetColor(Color(0.3f,0.7f,0.3f));
        tabElement->tabBody_->SetColor(Color(0.3f,0.7f,0.3f));

        Text *bodyText = tabElement->tabBody_->CreateChild<Text>();
        bodyText->SetAlignment(HA_CENTER, VA_CENTER);
        bodyText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        String btext = "Tab body " + String(i+1);
        bodyText->SetText(btext);
    }

    tabgroup->SetEnabled(true);
    SubscribeToEvent(E_TABSELECTED, URHO3D_HANDLER(Main, HandleTabSelected));
}

void Main::CreateSpriteAnimBox()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();

    SpriteAnimBox* animbox = root->CreateChild<SpriteAnimBox>();

    animbox->Init(IntVector2(300, 180), true, true);
    animbox->SetColor(Color(0,0,0,0));
    animbox->SetPosition(20, 270);

    animbox->AddSprite("Urho2D/GoldIcon/1.png");
    animbox->AddSprite("Urho2D/GoldIcon/2.png");
    animbox->AddSprite("Urho2D/GoldIcon/3.png");
    animbox->AddSprite("Urho2D/GoldIcon/4.png");
    animbox->AddSprite("Urho2D/GoldIcon/5.png");

    animbox->SetFPS(20.0f);
    animbox->SetReady();
}

void Main::CreateConnectorLine()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();

    Text *text = root->CreateChild<Text>();
    text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    text->SetText("Control Lines - drag control boxes to adjust");
    text->SetPosition(120, 450);

    IntVector2 points[5] =
    {
        { 100, 480 },
        { 200, 530 },
        { 300, 500 },
        { 400, 550 },
        { 500, 500 },
    };
    PODVector<IntVector2> pointList;
    pointList.Resize(5);
    pointList[0] = points[0];
    pointList[1] = points[1];
    pointList[2] = points[2];
    pointList[3] = points[3];
    pointList[4] = points[4];

    LineTest *lineTest1 = root->CreateChild<LineTest>();
    lineTest1->SetPoints(pointList, STRAIGHT_LINE, Color::BLUE, 8.0f);

    pointList[0] += IntVector2(0, 100);
    pointList[1] += IntVector2(0, 100);
    pointList[2] += IntVector2(0, 100);
    pointList[3] += IntVector2(0, 100);
    pointList[4] += IntVector2(0, 100);

    LineTest *lineTest2 = root->CreateChild<LineTest>();
    lineTest2->SetPoints(pointList, CURVE_LINE, Color::RED, 4.0f);
}

void Main::CreateSliderVariable()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();

    // ui callback helper 
    // you can use a typical event handler instead, e.g. HandleMessage(StringHash eventType, VariantMap& eventData);
    // and write the event sender in SlideBar
    class UICallbackHelper : public UIElement
    {
        URHO3D_OBJECT(UICallbackHelper, UIElement);
    public:
        UICallbackHelper(Context *context) : UIElement(context){}
        virtual ~UICallbackHelper(){}
        void SetBackgroundColor(const Color &color) { colorBackground_ = color; }
        void RedColorHandler(Variant &var)
        {
            colorBackground_.r_ = var.GetFloat();
            GetSubsystem<Renderer>()->GetDefaultZone()->SetFogColor(colorBackground_);
        }

    protected:
        Color colorBackground_;
    };

    UICallbackHelper *colorChangedHelper = new UICallbackHelper(context_);
    root->AddChild(colorChangedHelper);
    colorChangedHelper->SetBackgroundColor(colorBackground_);

    // create sliders
    SlideBar *slideBar = root->CreateChild<SlideBar>();
    slideBar->SetPosition(350, 270);
    slideBar->SetSize(250, 70);

    const IntVector2 barsize(280, 50);
    slideBar->CreateBar(barsize);

    slideBar->SetColor(Color(0.2f,0.2f,0.2f));
    slideBar->SetSliderColor(Color(0.8f,0.3f,0.3f));

    slideBar->GetHeaderText()->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    slideBar->GetHeaderText()->SetText("Slider Value (drag)");
    //slideBar->GetHeaderElement()->SetVisible(false);

    slideBar->SetRange((Variant)0.0f, (Variant)1.0f);
    slideBar->SetCurrentValue((Variant)0.0f );
    slideBar->SetSensitivity(0.005f);
    slideBar->SetEnabled(true);

    // assign callback
    slideBar->SetValChangedCallback(colorChangedHelper, (ValChangedCallback)&UICallbackHelper::RedColorHandler);
}

void Main::CreateDrawTool()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect(84,87,85,88);

    // texture draw tool
    DrawTool *drawtoolTexture = root->CreateChild<DrawTool>();

    if ( drawtoolTexture->Create( IntVector2(500, 330), uiTex2d, rect, false) )
    {
        drawtoolTexture->SetPosition(700, 5);
        drawtoolTexture->SetColor(Color(0.2f,0.2f,0.2f));
        drawtoolTexture->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        drawtoolTexture->SetHeaderText("Drawtool Texture (right mouse click to draw)");
    }

    // linebatcher draw tool
    DrawTool *drawtoolLineBatcher = root->CreateChild<DrawTool>();

    if ( drawtoolLineBatcher->Create( IntVector2(500, 330), uiTex2d, rect, true) )
    {
        drawtoolLineBatcher->SetPosition(700, 365);
        drawtoolLineBatcher->SetColor(Color(0.2f,0.2f,0.2f));
        drawtoolLineBatcher->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        drawtoolLineBatcher->SetHeaderText("Drawtool LineBatcher");
    }
}

void Main::CreateInstructions()
{
}

void Main::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
}

void Main::HandleCheckboxSelected(StringHash eventType, VariantMap& eventData)
{
    using namespace CheckGroupToggled;
}

void Main::HandleTabSelected(StringHash eventType, VariantMap& eventData)
{
    using namespace TabSelected;
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
}
