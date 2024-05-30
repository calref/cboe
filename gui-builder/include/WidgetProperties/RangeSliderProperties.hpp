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


#ifndef TGUI_GUI_BUILDER_RANGE_SLIDER_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_RANGE_SLIDER_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/RangeSlider.hpp>

struct RangeSliderProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto slider = widget->cast<tgui::RangeSlider>();
        if (property == "Minimum")
            slider->setMinimum(value.toFloat());
        else if (property == "Maximum")
            slider->setMaximum(value.toFloat());
        else if (property == "SelectionStart")
            slider->setSelectionStart(value.toFloat());
        else if (property == "SelectionEnd")
            slider->setSelectionEnd(value.toFloat());
        else if (property == "Step")
            slider->setStep(value.toFloat());
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto slider = widget->cast<tgui::RangeSlider>();
        pair.first["Minimum"] = {"Float", tgui::String::fromNumber(slider->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::String::fromNumber(slider->getMaximum())};
        pair.first["SelectionStart"] = {"Float", tgui::String::fromNumber(slider->getSelectionStart())};
        pair.first["SelectionEnd"] = {"Float", tgui::String::fromNumber(slider->getSelectionEnd())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(slider->getStep())};

        const auto renderer = slider->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColor())};
        pair.second["TrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColorHover())};
        pair.second["SelectedTrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTrackColor())};
        pair.second["SelectedTrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTrackColorHover())};
        pair.second["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pair.second["ThumbColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["TextureTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrack())};
        pair.second["TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrackHover())};
        pair.second["TextureThumb"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumb())};
        pair.second["TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumbHover())};
        pair.second["TextureSelectedTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTrack())};
        pair.second["TextureSelectedTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTrackHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_RANGE_SLIDER_PROPERTIES_HPP
