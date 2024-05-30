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


#ifndef TGUI_GUI_BUILDER_PICTURE_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_PICTURE_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Picture.hpp>

struct PictureProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto picture = widget->cast<tgui::Picture>();
        if (property == "IgnoreMouseEvents")
            picture->ignoreMouseEvents(parseBoolean(value, false));
        else if (property == "Texture")
        {
            const tgui::Texture& oldTexture = picture->getSharedRenderer()->getTexture();
            const tgui::String oldFilename = oldTexture.getId();
            const tgui::UIntRect oldPartRect = oldTexture.getPartRect();

            picture->getRenderer()->setProperty(property, value);

            const tgui::Texture& newTexture = picture->getSharedRenderer()->getTexture();
            const tgui::String newFilename = newTexture.getId();
            const tgui::UIntRect newPartRect = newTexture.getPartRect();
            const tgui::Vector2u imageSize = newTexture.getImageSize();
            if (((newFilename != oldFilename) || (newPartRect != oldPartRect)) && (imageSize.x != 0) && (imageSize.y != 0))
                picture->setSize(tgui::Vector2f{imageSize});
        }
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto picture = widget->cast<tgui::Picture>();
        pair.first["IgnoreMouseEvents"] = {"Bool", tgui::Serializer::serialize(picture->isIgnoringMouseEvents())};

        const auto renderer = picture->getSharedRenderer();
        pair.second["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_PICTURE_PROPERTIES_HPP
