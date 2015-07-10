
Blades of Exile New Scenario Format
===================================

This document describes, in as much detail as possible, the format of _.boes_ files. Parts
of the format are specified by the various XSD schemas in this directory, but the schemas
are not a perfect representation of the format - they are a little more restrictive than
the game itself accepts, and omit a few complex restrictions that the game enforces.

The Scenario Package
--------------------

A Blades of Exile scenario is a tarball (ustar) compressed with gzip. The tarball contains
the following files:

* scenario/
	* header.exs - This file has no function except to allow you to open a scenario that
	has been unpacked with the scenario editor. It contains the scenario and Blades of Exile
	version in binary format.
	* scenario.xml - Contains most of the global data for the scenario.
	* scenario.spec - Contains the definitions of the scenario special nodes.
	* terrain.xml - Contains the definitions of scenario terrain types.
	* items.xml - Contains the definitions of scenario items.
	* monsters.xml - Contains the definitions of scenario monsters.
	* towns/
		* town*N*.xml - Contains most of the data for town _N_.
		* town*N*.spec - Contains the definitions of the special nodes for town _N_.
		* town*N*.map - Contains the map data for town _N_.
		* talk*N*.xml - Contains the dialogue nodes associated with town _N_.
	* out/
		* out*X*~*Y*.xml - Contains most of the data for outdoors sector (_X_,_Y_).
		* out*X*~*Y*.spec - Contains the definitions of the special nodes for outdoors
		sector (_X_,_Y_).
		* out*X*~*Y*.map - Contains the map data for outdoors sector (_X_,_Y_).
	* graphics/ - (optional)
		* sheet*X*.png - (optional) Custom graphics sheet ID _X_.
		* objects.png, tinyobj.png, fields.png, trim.png, booms.png, missiles.png,
		dlogpics.png, staticons.png, teranim.png, talkportraits.png, pcs.png, vehicle.png,
		termap.png - (optional) Overrides the default graphic sheets of the same name.
	* sounds/ - (optional)
		* SND*X*.wav - (optional) A custom sound with ID _X_. If _X_ < 100, the sound will
		override the default sound with the same ID.
	* dialogs/ - (optional) - Contains complex dialog definitions used by the scenario.
	This is currently unused.

If you have a _scenario_ directory with the proper structure, it can be turned into a
valid scenario with the following shell command:

	tar -zcf scenname.boes scenario

Special Nodes
-------------

Scenario special nodes are stored in _.spec_ files in a special "pseudo-assembly" format.
The grammar is as follows:

	«node-file» -> («comment» | «const-def» [«comment»] | «node-def» [«comment»])*
	«const-def» -> def «identifier» = «integer»
	«node-def» -> @«opcode» [= «integer»] «data-def»*
	«data-def» -> «data-def-unary» | «data-def-binary» | «data-def-ternary»
	«data-def-unary» -> goto = «value»
	«data-def-binary» -> (sdf | pic) = «value» [, «value»]
	«data-def-ternary» -> (msg | ex1 | ex2) = «value» [, «value» [, «value»]]
	«value» -> «integer» | «identifier»

An `«identifier»` is a sequence of letters, underscores, hyphens, and dollar signs (digits
not permitted). An `«integer»` is a sequence of digits. An `«opcode»` is a special
identifier; valid opcodes are listed in the file
[specials-opcodes.txt](../strings/specials-opcodes.txt).
In addition, `nop` is also a valid opcode.

A `«comment»` is any sequence of characters starting with the `#` character and ending
with a newline.

Note that, although the format supports defined constants using `def`, the scenario never
writes defined constants.

Map Data
--------

Map data is stored in _.map_ files in a two-dimensional format. Each line in the file
represents one row of terrain in the town or outdoor sector. Individual tiles within a
single row are comma-separated. A tile consists of an integer, giving the terrain type on
that space, followed by an optional set of modifiers. Most modifiers consist of a special
character followed by an integer. A few modifiers consist of only a special character.
There can be multiple modifiers of the same type on a single tile.

The list of valid tile modifiers follows:

* `^ < > v` - Gives the position of a town entrance from one specific direction. (The
fourth one there is lowercase V.) Takes no argument. Only valid in town.
* `*` - Specifies that the tile is a wandering monster arrival point. Argument is which
point (0-3).
* `:` - Indicates a special node is attached to the space. Argument is the node number.
* `!` - Indicates a sign is attached to the space. Argument is which sign.
* `@` - When used outdoors, indicates a town entrance; argument is the town number. When
used in town, indicates a placed item; argument is the index of the placed item entry.
* `&` - Indicates an overlay such as a field, object, or special spot. The argument is a
number indicating the type of field (valid values are listed below). It can be used
outdoors, but the only valid argument is `9`, indicating a special spot.
* `$` - When used in town, indicates a placed creature; argument is the index of the
placed creature entry. When used outdoors, indicates a placed special encounter group;
argument is the index of the special encounter. (Note: Placed outdoor special encounters
are not yet supported.)
* `h` - Indicates a horse that belongs to the party. Argument is the index of the horse in
the global list.
* `H` - As above, except the horse does not belong to the party.
* `b` - Indicates a boat that belongs to the party. Argument is the index of the boat in
the global list.
* `B` - As above, except the boat does not belong to the party.

General XML notes
-----------------

All XML documents used in the scenario format require a `boes` attribute on their root
node, which should be set to `2.0.0`. This is a way to indicate the _format version_; if
the format ever changes, the value of this attribute will also be changed.

Some elements that contain text content are automatically wrapped in a `CDATA` declaration
by the scenario editor. The rest are never wrapped in a `CDATA` declaration. As a result,
while the game will correctly load `CDATA` declarations in any text element, it's only
safe to use them in elements where the scenario editor would use one.

There are some types that are common:

* Booleans - a boolean is one of the strings "`true`" or "`false`".
* Points - a point is an element with two attributes, `x` and `y`. It has no content.
* Rects - a rect is an element with four attributes: `top`, `left`, `bottom`, and `right`.
It has no content
* Timers - a timer is an element whose content is the number of the node to call, with an
attribute `freq` specifying how many moves to wait between calls.

Scenario Header
---------------

The _scenario.xml_ file stores most of the global scenario data. The following toplevel
tags are required:

* `<title>` - Contains the scenario name.
* `<icon>` - Contains an integer specifying the scenario icon.
* `<id>` - Contains the scenario's campaign ID.
* `<version>` - Contains the scenario version.
* `<language>` - For future expansion. Currently always contains "en-US".
* `<author>` - Contains information about the scenario author. Requires two sub-elements:
`<name>` and `<email>`.
* `<text>` - Contains basic textual information about the scenario. Can contain any of the
following:
	* `<teaser>` - (max 2) A short string to be displayed in the scenario selection
	dialog.
	* `<icon>` - Overrides the toplevel `<icon>` just for the intro dialog.
	* `<intro-msg>` - (max 6) A string to be shown in the intro dialog. If none are
	present, or all are empty, no intro dialog is shown. The scenario editor wraps the
	contents of this element in a `CDATA` declaration.
* `<ratings>` - Contains the scenario rating. The following two subtags are required:
	* `<content>` - The content rating. Must be one of `G`, `PG`, `R`, `NC-17`.
	* `<difficulty>` - The difficulty rating. Must be an integer from 1 to 4.
* `<flags>` - Contains various scenario flags.
* `<creator>` - Contains information about the program that wrote this scenario. Requires
the following subtags:
	* `<type>` - Must contain the string `oboe`. If you fork OBoE and change the scenario
	format, you would use a different string here to let mainline OBoE know that it's
	incompatible.
	* `<version>` - The version of OBoE that the scenario was last edited with.
	* `<os>` - The operating system on which the scenario was last edited.
* `<game>` - Contains the bulk of global information about the scenario.
* `<editor>` - Contains several details of the scenario that aren't relevant to gameplay,
but are instead used by the scenario editor to enhance your editing experience.

The `<game>` element can have the following child tags, most of which are optional:

* `<num-towns>` - (required) Indicates how many towns the scenario has.
* `<out-width>`, `<out-height>` - (required) Indicates how many outdoor sectors the
scenario has.
* `<start-town>` - (required) Specifies which town the player should start in.
* `<town-start>` - (required) Specifies the player's initial position in the start town,
as a point.
* `<outdoor-start>` - (required) Specifies which outdoor section the player should start
in, as a point.
* `<sector-start>` - (required) Specifies the player's initial position in the starting
sector.
* `<on-init>` - Indicates the number of a scenario special node to call when the scenario
first starts.
* `<store-items>` - (max 3) Specifies a rectangle from which the player can retrieve items
after completing the scenario, as a rectangle with an extra `town` attribute that
specifies which town the area
* `<town-flag>` - (max 10) Specifies a rule to add the value of an SDF to a town upon
entry. The SDF is specified with the `add-x` and `add-y` attributes, and the town number
with the `town` attribute.
* `<special-item>` - (max unbounded) Defines one special item. Special items must appear
in order, starting with special item ID 0. Special items have the following attributes and
subtags:
	* `start-with=` - Boolean indicating whether the party starts the scenario with this
	item.
	* `usable=` - Boolean indicating whether the item can be used.
	* `special=` - (required only if `usable="true"`) Indicates the special node to call
	when the special item is used.
	* `<name>` - (required) The name of the special item.
	* `<description>` - (required) The special item description. The scenario editor wraps
	the contents of this element in a `CDATA` declaration.
* `<quest>` - (max unbounded) Defines one quest. Quests must appear in order, starting
with quest ID 0. Quests have the following attributes and subtags:
	* `start-with=` - Boolean indicating whether the party starts the scenario with this
	quest.
	* `<deadline>` - (optional) Specifies the quest deadline (in days) as its content.
	Supports two optional attributes - `waive-if`, an integer specifying the event whose
	occurrence waives the deadline, and `relative`, a boolean indicating whether the
	deadline is relative to the day the quest is started.
	* `<reward>` - (optional) An empty element specifying the reward for completing the
	quest, with its two possible attributes `gold` and `xp`.
	* `<bank>` - (optional, max 2) Specifies that the quest belongs to one or two job
	banks.
	* `<name>` - (required) - The name of the quest.
	* `<description>` - (required) The quest description. The scenario editor wraps the
	contents of this element in a `CDATA` declaration.
* `<shop>` - (max unbounded) Defines one shop. Shops must appear in order, starting with
shop ID 0. Shops require the following subtags:
	* `<name>` - The name of the shop, used by the scenario editor only.
	* `<type>` - The type of the shop. Must be one of `live`, `dead`, `rand`.
	* `<prompt>` - The shopping prompt. Must be one of `shop`, `heal`, `mage`, `priest`,
	`spell`, `alch`, `train`.
	* `<face>` - The talking portrait to use in the shop.
	* `<entries>` - The actual contents of the shop. It can contain any of the following
	elements in any order, but no more than 30 in total:
		* `<item>` - An ordinary item. The content specifies which item (taken from the
		scenario's item list). By default, there's an infinite amount of this item; the
		attribute `quantity` can specify a different amount, or you can explicitly specify
		`quantity="infinite"`. You can also use the `chance` attribute to make the item
		appear only sometimes (defaults to 100). Note: If you specify the `chance`
		attribute to be something other than 100, the shop's type needs to be set to
		`rand` for it to work properly.
		* `<mage-spell>` - A mage spell. The content specifies which item (0-61).
		* `<priest-spell>` - A mage spell. The content specifies which item (0-61).
		* `<recipe>` - A mage spell. The content specifies which item (0-19).
		* `<skill>` - A mage spell. The content specifies which item (0-20).
		* `<treasure>` - A random item of a specific treasure class (0-4). Note: If you
		use this type of entry, the shop's type needs to be set to `rand` for it to work
		properly.
		* `<class>` - A random item of a specific special class (1-100). Note: If you use
		this type of entry, the shop's type needs to be set to `rand` for it to work
		properly.
		* `<heal>` - Healing. Must be one of the following:
			* `0` - Restore health to full.
			* `1` - Remove poison.
			* `2` - Remove disease.
			* `3` - Remove acid.
			* `4` - Remove paralysis.
			* `5` - Uncurse all carried items.
			* `6` - Revive petrified character.
			* `7` - Revive dead character.
			* `8` - Revive obliterated (dust) character.
			* `9` - Remove dumbfounding.
		* `<special>` - An item that calls a special node when purchase. Requires the
		following subtags:
			* `<name>` - The name of the entry.
			* `<description>` - A description of the entry. The scenario editor wraps the
			contents of this element in a `CDATA` declaration.
			* `<node>` - The scenario special node to call.
			* `<quantity>` - How many times the item can be purchase (an integer or the
			special value `infinite`).
			* `<cost>` - How much the item costs to purchase.
			* `<icon>` - The item icon to use.
* `<timer>` - (max 20) A scenario timer.
* `<string>` - (max unbounded) A scenario string. The scenario editor wraps the contents
of this element in a `CDATA` declaration.
* `<journal>` - (max unbounded) A journal string. The scenario editor wraps the contents
of this element in a `CDATA` declaration.

The `<editor>` element can have the following child tags:

* `<default-ground>` - (required) The default ground used for new areas. Currently this is
set to 0 or 2 at scenario creation, and no way to change it is offered in the scenario
editor UI.
* `<last-out-section>` - (required) The most-recently-edited outdoor sector, as a point.
* `<last-town>` - (required) The number of the most-recently-edited town.
* `<graphics>` - Stores the classification of your custom graphics. For each classified
graphic, a `<pic>` child element is used, with an `index` property specifying the number
of the graphic (_sheet_ * 100 + _row_ * 10 + _col_). The content of this element specifies
the type of graphic as an integer; the following values are valid:
	* 0 or 11 - This specifies that the graphic is either unused or part of another
	graphic that starts in an earlier slot. However, this is also the default for any
	slots that don't have a `<pic>` element referencing them.
	* 1 - Specifies that the slot contains a non-animated terrain graphic.
	* 2 - Specifies that the slot contains the first from of an animated terrain graphic.
	* 15 - Specifies that the slot contains six terrain map icons.
	* 3 - Specifies that the slot contains the first frame of a single-space monster.
	* 23 - Specifies that the slot contains the left half of the first frame of a wide
	two-space monster.
	* 43 - Specifies that the slot contains the top half of the first frame of a tall
	two-space monster.
	* 63 - Specifies that the slot contains the top left quadrant of the first frame of a
	four-space monster.
	* 4 - Specifies that the slot contains the left half of a dialog graphic.
	* 13 - Specifies that the slot contains the left half of the top left quadrant of a
	dialog graphic.
	* 5 - Specifies that the slot contains the left half of a talk portrait.
	* 7 - Specifies that the slot contains a full-space item graphic.
	* 10 - Specifies that the slot contains the first frame of an animated explosion.
	* 12 - Specifies that the slot contains the first two frames of a missile animation.
* `<storage>` - (max 10) Defines one quick item placement shortcut. It has the following
subtags:
	* `<on-terrain>` - Specifies the terrain type to which this shortcut applies.
	* `<is-property>` - A boolean specifying whether the items in this shortcut should
	belong to someone other than the party.
	* `<item>` - (max 10) Specifies which item to place. It has a required `chance`
	attribute to give the chance of placing this item.
* `<sound>` - (max unbounded) Gives a name for a custom sound (ID 100 or greater). These
names are used when showing the Pick Sound dialog in various places. The required `id`
attribute specifies which sound it applies to.

(...more to come...)
