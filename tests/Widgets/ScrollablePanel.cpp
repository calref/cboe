/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tests.hpp"
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

TEST_CASE("[ScrollablePanel]")
{
    tgui::ScrollablePanel::Ptr panel = tgui::ScrollablePanel::create();
    panel->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(panel->getWidgetType() == "ScrollablePanel");
    }

    SECTION("Position and Size")
    {
        panel->setPosition(40, 30);
        panel->setSize(150, 100);

        SECTION("Without outline")
        {
            REQUIRE(panel->getPosition() == tgui::Vector2f(40, 30));
            REQUIRE(panel->getSize() == tgui::Vector2f(150, 100));
            REQUIRE(panel->getFullSize() == panel->getSize());
            REQUIRE(panel->getWidgetOffset() == tgui::Vector2f(0, 0));
            REQUIRE(panel->getChildWidgetsOffset() == tgui::Vector2f(0, 0));
        }

        SECTION("With outline")
        {
            panel->getRenderer()->setBorders({1, 2, 3, 4});
            panel->getRenderer()->setPadding({5, 6, 7, 8});

            REQUIRE(panel->getPosition() == tgui::Vector2f(40, 30));
            REQUIRE(panel->getSize() == tgui::Vector2f(150, 100));
            REQUIRE(panel->getFullSize() == panel->getSize());
            REQUIRE(panel->getWidgetOffset() == tgui::Vector2f(0, 0));
            REQUIRE(panel->getChildWidgetsOffset() == tgui::Vector2f(6, 8));
        }

        SECTION("Child widgets")
        {
            auto childWidget = tgui::ClickableWidget::create();
            childWidget->setPosition(60, 50);
            panel->add(childWidget);

            REQUIRE(childWidget->getPosition() == tgui::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == tgui::Vector2f(100, 80));

            panel->getRenderer()->setBorders({1, 2, 3, 4});
            REQUIRE(childWidget->getPosition() == tgui::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == tgui::Vector2f(101, 82));
        }
    }

    SECTION("VerticalScrollbarPolicy")
    {
        REQUIRE(panel->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(panel->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(panel->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(panel->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("HorizontalScrollbarPolicy")
    {
        REQUIRE(panel->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(panel->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(panel->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(panel->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("VerticalScrollAmount")
    {
        REQUIRE(panel->getVerticalScrollAmount() == 0);
        panel->setVerticalScrollAmount(10);
        REQUIRE(panel->getVerticalScrollAmount() == 10);
    }

    SECTION("HorizontalScrollAmount")
    {
        REQUIRE(panel->getHorizontalScrollAmount() == 0);
        panel->setHorizontalScrollAmount(10);
        REQUIRE(panel->getHorizontalScrollAmount() == 10);
    }

    SECTION("Scrollbar width")
    {
        auto scrollbar = tgui::Scrollbar::create();
        REQUIRE(panel->getScrollbarWidth() == scrollbar->getDefaultWidth());

        tgui::Texture image("resources/Texture1.png");
        tgui::ScrollbarRenderer scrollbarRenderer;
        scrollbarRenderer.setTextureTrack(image);
        panel->getRenderer()->setScrollbar(scrollbarRenderer.getData());
        REQUIRE(panel->getScrollbarWidth() == image.getImageSize().x);

        panel->getRenderer()->setScrollbarWidth(15);
        REQUIRE(panel->getScrollbarWidth() == 15);
    }

    SECTION("Events / Signals")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;

        panel->setPosition(40, 30);
        panel->setSize(150, 100);

        panel->onMousePress(&mouseCallback, std::ref(mousePressedCount));
        panel->onMouseRelease(&mouseCallback, std::ref(mouseReleasedCount));
        panel->onClick(&mouseCallback, std::ref(clickedCount));

        SECTION("isMouseOnWidget")
        {
            REQUIRE(!panel->isMouseOnWidget({39, 29}));
            REQUIRE(panel->isMouseOnWidget({40, 30}));
            REQUIRE(panel->isMouseOnWidget({115, 80}));
            REQUIRE(panel->isMouseOnWidget({189, 129}));
            REQUIRE(!panel->isMouseOnWidget({190, 130}));

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            panel->onMouseEnter(&genericCallback, std::ref(mouseEnteredCount));
            panel->onMouseLeave(&genericCallback, std::ref(mouseLeftCount));

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(panel);

            parent->mouseMoved({40, 40});
            REQUIRE(mouseEnteredCount == 0);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({70, 55});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({219, 154});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({220, 155});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 1);
        }

        SECTION("mouse click")
        {
            auto parent = tgui::Panel::create({300, 200});

            parent->setPosition(60, 55);
            parent->add(panel);

            parent->leftMouseReleased({175, 135});

            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);

            SECTION("mouse press")
            {
                parent->leftMousePressed({175, 135});

                REQUIRE(mousePressedCount == 1);
                REQUIRE(mouseReleasedCount == 1);
                REQUIRE(clickedCount == 0);
            }

            parent->leftMouseReleased({175, 135});

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 1);
        }
    }

    testWidgetRenderer(panel->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = panel->getRenderer();

        tgui::ScrollbarRenderer scrollbarRenderer;
        scrollbarRenderer.setTrackColor(tgui::Color::Red);
        scrollbarRenderer.setThumbColor(tgui::Color::Blue);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
            REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{40, 50, 60}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Padding{5, 6, 7, 8}));
            REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
        }

        SECTION("functions")
        {
            renderer->setBackgroundColor({10, 20, 30});
            renderer->setBorderColor({40, 50, 60});
            renderer->setBorders({1, 2, 3, 4});
            renderer->setPadding({5, 6, 7, 8});
            renderer->setScrollbar(scrollbarRenderer.getData());
            renderer->setScrollbarWidth(15);
        }

        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(40, 50, 60));
        REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

        REQUIRE(renderer->getBackgroundColor() == tgui::Color(10, 20, 30));
        REQUIRE(renderer->getBorderColor() == tgui::Color(40, 50, 60));
        REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getPadding() == tgui::Padding(5, 6, 7, 8));

        REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
        REQUIRE(renderer->getScrollbar()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Red);
        REQUIRE(renderer->getScrollbar()->propertyValuePairs["ThumbColor"].getColor() == tgui::Color::Blue);
    }

    SECTION("Saving and loading from file")
    {
        panel = tgui::ScrollablePanel::create({400, 300});
        panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);

        auto widget = tgui::ClickableWidget::create();
        widget->setPosition({20, 10});
        widget->setSize({150, 100});
        panel->add(widget);

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(panel->saveWidgetsToFile("ScrollablePanelWidgetFile1.txt"));

            panel->setSize(200, 100);
            REQUIRE_NOTHROW(panel->loadWidgetsFromFile("ScrollablePanelWidgetFile1.txt"));
            REQUIRE(panel->getSize() == tgui::Vector2f(200, 100)); // The Panel itself is not saved, only its children

            REQUIRE_NOTHROW(panel->saveWidgetsToFile("ScrollablePanelWidgetFile2.txt"));
            REQUIRE(compareFiles("ScrollablePanelWidgetFile1.txt", "ScrollablePanelWidgetFile2.txt"));
        }

        SECTION("Save entire panel")
        {
            testSavingWidget("ScrollablePanel", panel, false);
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 150, panel)

        tgui::ScrollablePanelRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Yellow);
        renderer.setBorderColor(tgui::Color::Red);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({5, 6, 7, 8});
        renderer.setOpacity(0.7f);
        panel->setRenderer(renderer.getData());

        panel->setSize({180, 140});
        panel->setPosition({10, 5});

        auto picture = tgui::Picture::create("resources/image.png");
        picture->setSize({150, 100});
        picture->setPosition({50, 55});
        panel->add(picture);

        picture = tgui::Picture::create("resources/image.png");
        picture->setSize({150, 100});
        picture->setPosition({250, 300});
        panel->add(picture);

        SECTION("Content size larger than panel (scrollbars needed)")
        {
            TEST_DRAW("ScrollablePanel.png")

            panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
            TEST_DRAW("ScrollablePanel_NoHorizontalScrollbar.png")

            panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
            TEST_DRAW("ScrollablePanel_NoScrollbars.png")

            panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
            TEST_DRAW("ScrollablePanel_NoVerticalScrollbar.png")
        }

        SECTION("Content size smaller than panel (no scrollbars needed)")
        {
            panel->setContentSize({90, 80});

            TEST_DRAW("ScrollablePanel_SmallContentSize.png")

            panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
            TEST_DRAW("ScrollablePanel_SmallContentSize_VerticalScrollbar.png")

            panel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
            TEST_DRAW("ScrollablePanel_SmallContentSize_BothScrollbars.png")

            panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
            TEST_DRAW("ScrollablePanel_SmallContentSize_HorizontalScrollbar.png")
        }
    }
}
