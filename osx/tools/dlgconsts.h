/*
 *  dlogconsts.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define	NUM_BUTTONS		15
#define	NUM_DLOG_B		53

#define ND	15
#define	NI	500
#define	NL	100

// These constants are for draw_dialog_graphic() to know where to find the graphic.
#define PICT_OLD			-1
#define PICT_BLANK			0
#define PICT_TER			1 // 28x36 terrain graphic
#define PICT_TER_ANIM		2
#define PICT_MONST			3
#define PICT_DLG			4 // 36x36 dialog graphic
#define PICT_TALK			5 // 32x32 facial graphic
#define PICT_SCEN			6 // 32x32 scenario icon
#define PICT_ITEM			7 // Item graphic (two sizes available)
#define PICT_PC				8 // Player character graphic
#define PICT_INFO_HELP		9
#define PICT_PC_HELP		10
#define PICT_HELP			11
#define PICT_COMBAT_AP		12
#define PICT_STAT			13
#define PICT_FIELD			14 // A field or an explosion
#define PICT_DLG_LARGE		15
#define PICT_SCEN_LARGE		16
/* The following three are modifiers to be added to the above seven constants
 * Usage for PICT_CUSTOM:
 * PICT_CUSTOM + PICT_XXX, where XXX is anything valid other than OLD or BLANK.
 * PICT_CUSTOM + PICT_MONST + PICT_WIDE_MONST, for a 2x1 monster
 * PICT_CUSTOM + PICT_MONST + PICT_TALL_MONST, for a 1x2 monster
 * PICT_CUSTOM + PICT_MONST + PICT_TALL_MONST + PICT_WIDE_MONST, for a 2x2 monster
 * Notes:
 *  - PICT_TALL_MONST and PICT_WIDE_MONST only apply to custom monsters - the preset
 * monster graphics have these settings hardcoded.
 *  - Also note that custom scenario graphics are not currently supported. This doesn't really
 * matter, though, since if they were they'd be handled like talk (facial) graphics.
 *  - Custom animated terrain graphics should use PICT_TER rather than PICT_TER_ANIM.
 */
#define PICT_CUSTOM				100
#define PICT_WIDE_MONSTER		20
#define PICT_TALL_MONSTER		40
// These constants are the total number of preset graphics of each type.
#define PICT_N_TER				252 // 336
#define PICT_N_TER_ANIM			13 // 18
#define PICT_N_MONST			172
#define PICT_N_DLG				32 // 44
#define PICT_N_TALK				83
#define PICT_N_SCEN				29
#define PICT_N_ITEM				122

// These constants are used to create the definition of the dialogs, and
// to parse a dialog resource when displaying it to the screen.
#define DLG_BUTTON				0
#define DLG_DEFAULT_BTN			1
// Not sure what 10 and 11 do, but they may be similar to above two
#define DLG_LED_BUTTON			2
// 8 and 9 are other text objects; not completely sure which is which
#define DLG_TEXT_BOLD			3
#define DLG_TEXT_PLAIN			4
#define DLG_OLD_PICTURE			5
#define DLG_HIDDEN_BUTTON		6 // This was recognized on load, but not used anywhere else; now it is ;)
#define DLG_TEXT_LARGE			7
#define DLG_TEXT_CLICKABLE		8
#define DLG_TEXT_DEFAULT		9  // Appears to be equivalent to DLG_TEXT_BOLD.
#define DLG_CUSTOM_BTN			10 // These appear to be buttons whose label is stored
#define DLG_CUSTOM_DEF_BTN		11 // in the same place as the text of text items.
#define DLG_PUSH_BTN			12 // for potential future use; a button like above whose label is below it
// And I don't know what 6 is for either; could it be a relic from the red push buttons of E3?
#define DLG_NEW_PICTURE			1000
// Add DLG_NEW_PICTURE to any PICT_ constant (other than PICT_OLD) to form a valid constant

#define DLG_KEY_LEFT	20
#define DLG_KEY_RIGHT	21
#define DLG_KEY_UP		22
#define DLG_KEY_DOWN	23
#define DLG_KEY_ESC		24
#define DLG_KEY_CTRL1	25
#define DLG_KEY_CTRL2	26
#define DLG_KEY_CTRL3	27
#define DLG_KEY_CTRL4	28
#define DLG_KEY_CTRL5	29
#define DLG_KEY_CTRL6	30
#define DLG_KEY_RETURN	31

#define DLG_BTN_SM		0 // 23x23 (PICT id 2000 / 2001)
#define DLG_BTN_REG		1 // 63x23 (PICT id 2002 / 2003)
#define DLG_BTN_LG		2 // 102x23 (PICT id 2004 / 2005)
#define DLG_BTN_HELP	3 // 16x13 (PICT id 2006 / 2007) white bubble w/ ? mark
#define DLG_BTN_LEFT	4 // 63x23 (PICT id 2008 / 2009) with left arrow
#define DLG_BTN_RIGHT	5 // 63x23 (PICT id 2010 / 2011) with right arrow
#define DLG_BTN_UP		6 // 63x23 (PICT id 2012 / 2013) with up arrow
#define DLG_BTN_DOWN	7 // 63x23 (PICT id 2014 / 2015) with down arrow
#define DLG_BTN_LED1	8 // 6x6 (PICT id 2016 / 2017)
#define DLG_BTN_LED2	9 // 14x10 (PICT id 2018 / 2019)
#define DLG_BTN_LED3	10 // 14x10 (PICT id 2020 / 2021)
#define DLG_BTN_DONE	11 // 63x23 (PICT id 2022 / 2023) says "Done"
#define DLG_BTN_TALL	12 // 63x40 (PICT id 2024 / 2025)
#define DLG_BTN_TRAIT	13 // 63x40 (PICT id 2026 / 2027) says "Race Good/Bad Traits"
#define DLG_BTN_PUSH	14 // 30x30 (PICT id 2028 / 2029) red round button
