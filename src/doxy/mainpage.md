
Blades of Exile Documentation			{#mainpage}
=============================

This is the documentation for parts of the Blades of Exile source code.
Currently it covers only the dialog engine.

Defining a Dialog
=================

Creating a new dialog consists of two basic steps: first, you have to
create the XML dialog definition, and then you need to write the code to
set it up and implement all the required callbacks.

The format of the XML dialog files is defined by the schema in
_rsrc/schemas/dialog.xsd_, but this document will cover what you need to
know in a more readable manner. A dialog definition consists of a
`<dialog>` tag containing any number of tags representing various dialog
controls. Each tag will be documented in some detail below.

Common Attributes
-----------------

The following attributes are allowed on all or most elements:

* `top`, `left`, `width`, `height` - Specifies the location and bounding
rect of the control within the dialog. All non-container controls
support these attributes, and in fact the `top` and `left` attributes
are required. Some controls may ignore the `width` and `height`
attributes. All of these must be non-negative integers.
* `relative` - Specifies how the location is computed; defaults to `"abs"`.
Must be one or two (space-separated) of the following;
if two are specified, they represent the mode used for calculating x and y respectively:
  * `abs` - Computed in global dialog space, relative to the top left corner.
  * `pos` - Computed relative to the reference widget's bottom right corner.
  * `pos-in` - Computed relative to the reference widget's top left corner.
  * `neg` - Computed relative to the reference widget's top left corner,
with the axes inverted (as if top and left were negative).
As a special case, if this is used with no reference widget,
the widget's size does not contribute to computation of the dialog's size,
and the widget is positioned relative to the dialog's bottom right corner.
  * `neg-in` - Computed relative to the reference widget's bottom right corner,
with the axes inverted.
* `anchor` - Specifies the `name` of the reference widget for this widget's location.
* `rel-anchor` - Set to `prev` or `next` to use the previous or next element in the XML ordering
as the reference widget for this widget's location.
This currently does not work for widgets in containers.
Mutually exclusive with `anchor`.
* `def-key` - Specifies the default keyboard shortcut for the
control. See **Keyboard Shortcuts** below for more information on the
format of this attribute.
* `font`, `size`, `color`, `colour` - Specifies text attributes of the
control. See **Text Formatting** below for details on accepted values.
* `name` - Give the control a unique identifier which you can use to
refer to it from the code. If omitted, the code will generate a random
identifier. All controls support this attribute.
* `fromlist` - Currently unused, but the intended use is to look up the
control's text value in a string list (in _rsrc/strings_)
* `framed` - Specifies whether the control should be drawn with a frame
around it, and what type of frame. Can be either `true` or `false`
* `outline` - Sets the type of frame to use for this control. Must be one
of `solid`, `inset`, `outset`, `double`. It defaults to `inset` and is
meaningless if `framed` is `false`.

The `<dialog>` tag
------------------

The `<dialog>` tag accepts the following attributes:

* `skin` - Specify the dialog's background pattern. Currently only
accepts `dark` or `light`. If omitted, a default is used which depends
on whether the dialog is used in the game or the scenario editor. As
such, this attribute should be omitted for most dialogs.
* `debug` - This attribute is completely ignored by the game, but if set
to `true`, the XSL stylesheet will draw the bounding rects of LEDs and
other debug information.
* `fore` - The default text colour. Generally this shouldn't be needed.
* `defbtn` - The ID (`name` attribute) of button triggered by Enter/Return.
This is an IDREF, so it must exist in the dialog.
* `escbtn` - The ID (`name` attribute) of button triggered by Escape.
This is an IDREF, so it must exist in the dialog.

The `<text>` tag
----------------

The `<text>` tag can be used either for a non-editable text message or
for a frame to group elements. Within a `<text>` tag, you can use a
`<br>` tag to insert a line break. See **Whitespace** for an explanation
of the intended whitespace collapsing behaviour.

The `<text>` tag accepts the following attributes:

* `framed` - See **Common Attributes** above. Defaults to `false`.
* `outline` - See **Common Attributes** above.
* `underline` - If true, the text will be underlined.
* `align` - `right` or `left`. Defaults to `left`.
* `fromlist`, `font`, `size`, `color`, `colour`, `def-key` -
See **Common Attributes** above.

The `<button>` tag
------------------

The `<button>` tag decribes a non-LED button. It can contain label text
as well as an optional empty `<key>` element. It does not support the
`<br>` tag. The `<key>` element was intended as a way of inserting a
readable form of the button's keyboard shortcut, but at present this is
unimplemented.

The `<button>` tag accepts the following attributes:

* `name` - This attribute is required on the `<button>` tag. See
**Common Attributes** above.
* `type` - Specifies the type of button. This attribute is required.
* `text-size` - Overrides the text size of the button's type
* `wrap` - Specifies whether to wrap the text on the button. Can be
either `true` or `false`; defaults to `false`.
* `fromlist`, `def-key` - See **Common Attributes** above.

The possible values for the `type` attribute are:

* `small` - A small 23x23 button.
* `regular` - A normal-sized 63x23 button.
* `large` - A large 102x23 button.
* `help` - A small 16x13 help button - a white bubble with a ? mark.
* `left` - A normal-sized 63x23 button with a left-pointing arrow.
* `right` - A normal-sized 63x23 button with a right-pointing arrow.
* `up` - A normal-sized 63x23 button with an up-pointing arrow.
* `down` - A normal-sized 63x23 button with a down-pointing arrow.
* `tiny` - A tiny 14x10 button, same size as an LED and with the same
label behaviour.
* `done` - A normal-sized 63x23 button with "Done" on it.
* `tall` - A tall 63x40 button.
* `trait` - A tall 63x40 button with "Race Good/Bad Traits" on it.
* `push` - A round red 30x30 push button. The label is centred below it.

(Naturally, they correspond perfectly with the values of the @ref eBtnType
enumeration.)

The `<led>` tag
---------------

The `<led>` tag describes an LED button. It can contain label text,
which is drawn to the right of the button graphic. This differs from
normal practice in the original Blades of Exile dialog engine, where LED
labels were typically left-aligned to the _left_ of their associated
LED. I've chosen to make this change because I feel it is easier to see
which LED a label is associated to, and because the behaviour is more
similar to the checkboxes and radio buttons found in normal GUIs.

The `<led>` tag accepts the following attributes:

* `name` - This attribute is required on the `<led>` tag. See **Common
Attributes** above.
* `state` - Specifies the starting state of the LED. Can be one of
`red`, `green`, or `off`; defaults to `off`.
* `font`, `size`, `color`, `colour` - See **Common Attributes** above. Note that, for an LED, omitting the size attribute gives a different result than any of the possible values.
* `label-pos` - defaults to `right`, `left` switches the horizontal order of the LED and its text

The `<group>` tag
-----------------

The `<group>` tag defines an LED group, a collection of LEDs that behave
like radio buttons rather than checkboxes. The tag can contain any
number of `<led>` elements.

The `<group>` tag accepts the following attributes:

* `fromlist` - See **Common Attributes** above. I don't remember exactly
how this would have applied to an LED group though; perhaps it would use
the string list to assign labels to its component LEDs in order of
declaration?

The `<pict>` tag
----------------

The `<pict>` tag defines a dialog picture. This covers a wide range of
things, from large pictures to small icons.

The `<pict>` tag accepts the following attributes:

* `type` - Specifies the type of graphic. This attribute is required.
* `num` - Specifies the graphic number. This attribute is required.
* `custom` - Specifies whether the picture is custom or preset. Can be
either `true` or `false`; defaults to `false`.
* `framed` - See **Common Attributes** above. Defaults to `true`.
* `outline` - See **Common Attributes** above.
* `size` - For certain types of graphics, this provides an additional
hint. Can be one of `small`, `wide`, `tall`, or `large`. See the list of
types below to determine if this attribute is relevant.
* `scaled` - For certain larger types of graphics, this determines
whether it should be scaled to fit within the provided bounds. It only
has an effect for types `full`, `dlog`, and `scen`, and the latter two
only if `size` is `large`. Can be either `true` or `false`, defaults to
`false`.
* `filled` - For transparent graphics, specifies whether to draw a solid
black fill behind the icon
* `color` - The colour of the solid fill. Defaults to black.
* `def-key` - See **Common Attributes** above.

The possible values for the `type` attribute are:

* `blank` - Draw no graphic, just fill the bounding rect with black.
* `ter` - A terrain graphic.
* `teranim` - An animated terrain graphic.
* `monst` - A monster graphic. The `size` attribute can be set to any of
its four accepted values to indicate the size of the monster.
* `dlog` - A dialog graphic. The `size` attribute can be `small` (the
default) or `large`.
* `talk` - A conversation graphic.
* `scen` - A scenario icon. The `size` attribute can be `small` (the
default) or `large`.
* `item` - An item graphic. The `size` attribute can be `large` (the
default) or `small`.
* `pc` - A PC graphic.
* `field` - A field graphic.
* `boom` - A boom graphic.
* `missile` - A missile graphic.
* `map` - A map graphic.
* `status` - A status icon.
* `full` - A full sheet. With this value and with `custom` left at
`false`, the `num` parameter must be one of the following:
	* 1100 - The inventory help frame.
	* 1200 - The stats area help frame.
	* 1300 - The action points help fram.
	* 1400 - The outdoor toolbar help frame.
	* 1401 - The combat toolbar help frame.
	* 1402 - The town toolbar help frame.

The `<field>` tag
-----------------

The `<field>` tag defines an editable text field. It can contain default
text as its contents, but does not support the `<br>` tag.

The `<field>` tag accepts the following attributes:

* `type` - Specifies the input type of the field. Can be one of `text`,
`int`, `uint`, or `real`; defaults to `text`.
* `tab-order` - Specifies the tab order of the field. This must be an
integer that is unique in the dialog.

The `<slider>` tag
------------------

The `<slider>` tag defines a slider control (presented with a scrollbar).

The `<slider>` tag accepts the following attributes:

* `style` - Specifies the slider style. Can be either `white` or `led`;
defaults to `led`.
* `vertical` - Specifies whether the slider should be shown horizontally
or vertically. Usually defaults to true, but if `width` is provided and
`height` is not, then it will default to false.
* `initial` - Specifies the initial value of the slider. Must be a value
from 0 to `max` inclusive. Defaults to 0.
* `max` - Specifies the maximum value of the slider. This attribute is required.
* `page-size` - Specifies how much the slider should scroll when clicking
between the thumb and the arrow button. Defaults to 10.
* `link` - If present, the control with this ID will have its text set
to the slider's value whenever it changes. It works best if the referenced
control appears before the slider control in the definition.

The `<stack>` tag
-----------------

The `<stack>` tag groups elements that represent a single entry in an array.
It can contain any elements except for nested `<stack>`, `<pane>`, or `<tilemap>` elements.

The `<stack>` tag accepts the following attributes:

* `pages` - The initial number of pages in the stack. This can also be
set programmatically.

In addition to nested widgets, the `<stack>` tag can contain `<page>` elements.
Each `<page>` element contains a group of widgets that will be visible on a
particular page. Unless changed programmatically, any widgets in a `<page>`
element will be visible if and if only the corresponding page is active.
Widgets not assigned to a `<page>` will always be visible.
A `<page>` can contain any elements except for nested `<stack>` or `<pane>` elements.

The `<page>` tag accepts the following attributes:

* `span` - If set to an integer greater than 1, the same page definition will be
used for a span of this many pages, as if the entire element had been duplicated
that many times.
* `default` - If set to true, this page will be used as the default definition for
any new pages added programmatically. Only one page may be the default.
* `template` - If set to a string, this can be used to apply the page definition to
a page programmatically.

The `<pane>` tag
----------------

The `<pane>` tag groups elements into a scrollable subpane.
It can contain any elements except for nested `<stack>`, `<pane>`, or `<tilemap>` elements.

The `<pane>` tag accepts the following attributes:

* `framed` - See **Common Attributes** above. Defaults to `false`.
* `outline` - See **Common Attributes** above.
* `style` - Same as for `<slider>`, see above. Applies to the pane's scrollbar.

The `<tilemap>` tag
-------------------

The `<tilemap>` tag represents a grid of identical elements based off the provided template.
It can contain any elements except for nested `<stack>`, `<pane>`, or `<tilemap>` elements.
Every child element of the tilemap will be instanced for each cell of the tilemap,
unless it is a `<mapgroup>` element (see below).

The `<tilemap>` tag accepts the following attributes:

* `framed` - See **Common Attributes** above. Defaults to `false`.
* `outline` - See **Common Attributes** above.
* `rows` - The number of rows to generate. Required.
* `cols` - The number of columns to generate. Required.
* `cellspacing` - If specified, adds a buffer of this size between each cell.
* `<mapgroup>` - Defines a combined LED group that will contain every instance of the LEDs it defines. which will be instanced for _each_ cell of the tilemap.

Keyboard Shortcuts
------------------

Keyboard shortcuts are specified with the `def-key` attribute.

The `def-key` attribute is a space separated list of values. The final value in this list
specifies the main key that triggers the control. It can be any of the following values:

* A single digit
* A lowercase letter
* One of the following special characters:

		` - = [ ] \ ; ' , . /
* `left` - The left arrow key
* `right` - The right arrow key
* `up` - The up arrow key
* `down` - The down arrow key
* `esc` - The escape key
* `enter` - The enter key.
* `return` - The enter key. (Note: Theoretically, `enter` should be
numpad enter while this is normal enter; I don't think this distinction
is implemented though.)
* `tab` - The tab key.
* `help` - The help key (on keyboards that have one) or the F1 key.
* `space` - The space key.

Preceding values in the `def-key` attribute specify the modifiers to be used with the key.
Any of the following values is allowed:

* `ctrl` - The primary accelerator key (either control or command).
* `shift` - The shift key
* `alt` - The alt or option key

The following special value is also allowed for the `def-key` attribute:

* `none` - No key shortcut (the default).

Text Formatting
---------------

Text formatting options in dialogs are actually quite limited. There are
only three possible attributes, with a very limited number of possible
values.

* `font` - This attribute can take one of the following values: `plain`,
`bold`, `dungeon`, `maidenword`. It defaults to `bold`. The latter two
options are fantasy-style scripts such as that used for the conversation
screen.
* `size` - The point size of the text. An integer, or one of three presets: `small` (10pt), `large` (12pt), and `title` (18pt).
* `color` or `colour` - This is the only truly unrestricted attribute,
accepting any HTML colour code. It understands three-digit hex codes (eg
`#333`, which is equivalent to `#333333`), six-digit hex codes, and the
sixteen standard HTML colour names. Both the British and American
spelling are supported for the attribute name.

Whitespace
----------

Whitespace collapse rules within Dialog XML elements are a little weird.
All tabs are stripped out, and then runs of newlines are replaced with a
single space, but runs of spaces are left as-is. Thus, you can use spaces
to do things like forcing indentation, while tabs can serve to indent
the XML itself.

Implementing a Dialog
=====================

Once the dialog is defined, it needs to be implemented! This consists of
a few steps - construct the dialog, attach handlers, possibly populate
the dialog's fields and other dynamic elements according to whatever the
dialog is supposed to display, and then run the dialog.

Use of `std::bind` for attaching the handlers is encouraged, and can be
used in conjunction with `std::ref` to provide state for the dialog.
Lambda expressions may also be useful for some of the simpler handlers.
Use of `dynamic_cast` will be required when dealing with LEDs or icons.
