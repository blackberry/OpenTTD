/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file table/settings.h Settings to save in the savegame and config file. */

/* Callback function used in _settings[] as well as _company_settings[] */
static size_t ConvertLandscape(const char *value);


/****************************
 * OTTD specific INI stuff
 ****************************/

/**
 * Settings-macro usage:
 * The list might look daunting at first, but is in general easy to understand.
 * We have two types of list:
 * 1. SDTG_something
 * 2. SDT_something
 * The 'G' stands for global, so this is the one you will use for a
 * SettingDescGlobVarList section meaning global variables. The other uses a
 * Base/Offset and runtime variable selection mechanism, known from the saveload
 * convention (it also has global so it should not be hard).
 * Now there are a lot of types. Easy ones are:
 * - VAR:  any number type, 'type' field specifies what number. eg int8 or uint32
 * - BOOL: a boolean number type
 * - STR:  a string or character. 'type' field specifies what string. Normal, string, or quoted
 * A bit more difficult to use are MMANY (meaning ManyOfMany) and OMANY (OneOfMany)
 * These are actually normal numbers, only bitmasked. In MMANY several bits can
 * be set, in the other only one.
 * If nothing fits you, you can use the GENERAL macros, but it exposes the
 * internal structure somewhat so it needs a little looking. There are _NULL()
 * macros as well, these fill up space so you can add more settings there (in
 * place) and you DON'T have to increase the savegame version.
 *
 * While reading values from openttd.cfg, some values may not be converted
 * properly, for any kind of reasons.  In order to allow a process of self-cleaning
 * mechanism, a callback procedure is made available.  You will have to supply the function, which
 * will work on a string, one function per setting. And of course, enable the callback param
 * on the appropriate macro.
 */

#define NSD_GENERAL(name, def, cmd, guiflags, min, max, interval, many, str, strval, proc, load)\
	{name, (const void*)(size_t)(def), {(byte)cmd}, {(uint16)guiflags}, min, max, interval, many, str, strval, proc, load}

/* Macros for various objects to go in the configuration file.
 * This section is for global variables */
#define SDTG_GENERAL(name, sdt_cmd, sle_cmd, type, flags, guiflags, var, length, def, min, max, interval, full, str, strval, proc, from, to)\
	{NSD_GENERAL(name, def, sdt_cmd, guiflags, min, max, interval, full, str, strval, proc, NULL), SLEG_GENERAL(sle_cmd, var, type | flags, length, from, to)}

#define SDTG_VAR(name, type, flags, guiflags, var, def, min, max, interval, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_NUMX, SL_VAR, type, flags, guiflags, var, 0, def, min, max, interval, NULL, str, strval, proc, from, to)

#define SDTG_BOOL(name, flags, guiflags, var, def, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_BOOLX, SL_VAR, SLE_BOOL, flags, guiflags, var, 0, def, 0, 1, 0, NULL, str, strval, proc, from, to)

#define SDTG_LIST(name, type, length, flags, guiflags, var, def, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_INTLIST, SL_ARR, type, flags, guiflags, var, length, def, 0, 0, 0, NULL, str, strval, proc, from, to)

#define SDTG_STR(name, type, flags, guiflags, var, def, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_STRING, SL_STR, type, flags, guiflags, var, lengthof(var), def, 0, 0, 0, NULL, str, strval, proc, from, to)

#define SDTG_OMANY(name, type, flags, guiflags, var, def, max, full, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_ONEOFMANY, SL_VAR, type, flags, guiflags, var, 0, def, 0, max, 0, full, str, strval, proc, from, to)

#define SDTG_MMANY(name, type, flags, guiflags, var, def, full, str, strval, proc, from, to)\
	SDTG_GENERAL(name, SDT_MANYOFMANY, SL_VAR, type, flags, guiflags, var, 0, def, 0, 0, 0, full, str, strval, proc, from, to)

#define SDTG_NULL(length, from, to)\
	{{"", NULL, {0}, {0}, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, NULL}, SLEG_NULL(length, from, to)}

#define SDTG_END() {{NULL, NULL, {0}, {0}, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, NULL}, SLEG_END()}

/* Macros for various objects to go in the configuration file.
 * This section is for structures where their various members are saved */
#define SDT_GENERAL(name, sdt_cmd, sle_cmd, type, flags, guiflags, base, var, length, def, min, max, interval, full, str, strval, proc, load, from, to)\
	{NSD_GENERAL(name, def, sdt_cmd, guiflags, min, max, interval, full, str, strval, proc, load), SLE_GENERAL(sle_cmd, base, var, type | flags, length, from, to)}

#define SDT_VAR(base, var, type, flags, guiflags, def, min, max, interval, str, strval, proc, from, to)\
	SDT_GENERAL(#var, SDT_NUMX, SL_VAR, type, flags, guiflags, base, var, 1, def, min, max, interval, NULL, str, strval, proc, NULL, from, to)

#define SDT_BOOL(base, var, flags, guiflags, def, str, strval, proc, from, to)\
	SDT_GENERAL(#var, SDT_BOOLX, SL_VAR, SLE_BOOL, flags, guiflags, base, var, 1, def, 0, 1, 0, NULL, str, strval, proc, NULL, from, to)

#define SDT_LIST(base, var, type, flags, guiflags, def, str, strval, proc, from, to)\
	SDT_GENERAL(#var, SDT_INTLIST, SL_ARR, type, flags, guiflags, base, var, lengthof(((base*)8)->var), def, 0, 0, 0, NULL, str, strval, proc, NULL, from, to)

#define SDT_STR(base, var, type, flags, guiflags, def, str, strval, proc, from, to)\
	SDT_GENERAL(#var, SDT_STRING, SL_STR, type, flags, guiflags, base, var, lengthof(((base*)8)->var), def, 0, 0, 0, NULL, str, strval, proc, NULL, from, to)

#define SDT_CHR(base, var, flags, guiflags, def, str, strval, proc, from, to)\
	SDT_GENERAL(#var, SDT_STRING, SL_VAR, SLE_CHAR, flags, guiflags, base, var, 1, def, 0, 0, 0, NULL, str, strval, proc, NULL, from, to)

#define SDT_OMANY(base, var, type, flags, guiflags, def, max, full, str, strval, proc, from, to, load)\
	SDT_GENERAL(#var, SDT_ONEOFMANY, SL_VAR, type, flags, guiflags, base, var, 1, def, 0, max, 0, full, str, strval, proc, load, from, to)

#define SDT_MMANY(base, var, type, flags, guiflags, def, full, str, proc, strval, from, to)\
	SDT_GENERAL(#var, SDT_MANYOFMANY, SL_VAR, type, flags, guiflags, base, var, 1, def, 0, 0, 0, full, str, strval, proc, NULL, from, to)

#define SDT_NULL(length, from, to)\
	{{"", NULL, {0}, {0}, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, NULL}, SLE_CONDNULL(length, from, to)}


#define SDTC_VAR(var, type, flags, guiflags, def, min, max, interval, str, strval, proc, from, to)\
	SDTG_GENERAL(#var, SDT_NUMX, SL_VAR, type, flags, guiflags, _settings_client.var, 1, def, min, max, interval, NULL, str, strval, proc, from, to)

#define SDTC_BOOL(var, flags, guiflags, def, str, strval, proc, from, to)\
	SDTG_GENERAL(#var, SDT_BOOLX, SL_VAR, SLE_BOOL, flags, guiflags, _settings_client.var, 1, def, 0, 1, 0, NULL, str, strval, proc, from, to)

#define SDTC_LIST(var, type, flags, guiflags, def, str, strval, proc, from, to)\
	SDTG_GENERAL(#var, SDT_INTLIST, SL_ARR, type, flags, guiflags, _settings_client.var, lengthof(_settings_client.var), def, 0, 0, 0, NULL, str, strval, proc, from, to)

#define SDTC_STR(var, type, flags, guiflags, def, str, strval, proc, from, to)\
	SDTG_GENERAL(#var, SDT_STRING, SL_STR, type, flags, guiflags, _settings_client.var, lengthof(_settings_client.var), def, 0, 0, 0, NULL, str, strval, proc, from, to)

#define SDTC_OMANY(var, type, flags, guiflags, def, max, full, str, strval, proc, from, to)\
	SDTG_GENERAL(#var, SDT_ONEOFMANY, SL_VAR, type, flags, guiflags, _settings_client.var, 1, def, 0, max, 0, full, str, strval, proc, from, to)

#define SDT_END() {{NULL, NULL, {0}, {0}, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, NULL}, SLE_END()}

static bool CheckInterval(int32 p1);
static bool InvalidateDetailsWindow(int32 p1);

static const SettingDesc _company_settings[] = {
SDT_BOOL(CompanySettings, engine_renew,        0, SGF_PER_COMPANY, false,                        STR_CONFIG_SETTING_AUTORENEW_VEHICLE, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, engine_renew_months, SLE_INT16, 0, SGF_PER_COMPANY, 6, -12, 12, 0, STR_CONFIG_SETTING_AUTORENEW_MONTHS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, engine_renew_money, SLE_UINT, 0, SGF_PER_COMPANY | SGF_CURRENCY, 100000, 0, 2000000, 0, STR_CONFIG_SETTING_AUTORENEW_MONEY, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(CompanySettings, renew_keep_length,        0, SGF_PER_COMPANY, false,                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(CompanySettings, vehicle.servint_ispercent,        0, SGF_PER_COMPANY, false,                        STR_CONFIG_SETTING_SERVINT_ISPERCENT, STR_NULL, CheckInterval, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, vehicle.servint_trains, SLE_UINT16, 0, SGF_PER_COMPANY | SGF_0ISDISABLED, 150, 5, 800, 0, STR_CONFIG_SETTING_SERVINT_TRAINS, STR_NULL, InvalidateDetailsWindow, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, vehicle.servint_roadveh, SLE_UINT16, 0, SGF_PER_COMPANY | SGF_0ISDISABLED, 150, 5, 800, 0, STR_CONFIG_SETTING_SERVINT_ROAD_VEHICLES, STR_NULL, InvalidateDetailsWindow, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, vehicle.servint_ships, SLE_UINT16, 0, SGF_PER_COMPANY | SGF_0ISDISABLED, 360, 5, 800, 0, STR_CONFIG_SETTING_SERVINT_SHIPS, STR_NULL, InvalidateDetailsWindow, 0, SL_MAX_VERSION),
SDT_VAR(CompanySettings, vehicle.servint_aircraft, SLE_UINT16, 0, SGF_PER_COMPANY | SGF_0ISDISABLED, 100, 5, 800, 0, STR_CONFIG_SETTING_SERVINT_AIRCRAFT, STR_NULL, InvalidateDetailsWindow, 0, SL_MAX_VERSION),
SDT_END()
};
static const SettingDesc _currency_settings[] = {
SDT_VAR(CurrencySpec, rate, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1, 0, UINT16_MAX, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_CHR(CurrencySpec, separator,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, ".",                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(CurrencySpec, to_euro, SLE_INT32, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 0, MIN_YEAR, MAX_YEAR, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_STR(CurrencySpec, prefix, SLE_STRBQ, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_STR(CurrencySpec, suffix, SLE_STRBQ, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, " credits",                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_END()
};
static const uint GAME_DIFFICULTY_NUM = 18;
static uint16 _old_diff_custom[GAME_DIFFICULTY_NUM];

/* Most of these strings are used both for gameopt-backward compatability
 * and the settings tables. The rest is here for consistency. */
static const char *_locale_currencies = "GBP|USD|EUR|YEN|ATS|BEF|CHF|CZK|DEM|DKK|ESP|FIM|FRF|GRD|HUF|ISK|ITL|NLG|NOK|PLN|RON|RUR|SIT|SEK|YTL|SKK|BRL|EEK|custom";
static const char *_locale_units = "imperial|metric|si";
static const char *_town_names = "english|french|german|american|latin|silly|swedish|dutch|finnish|polish|slovak|norwegian|hungarian|austrian|romanian|czech|swiss|danish|turkish|italian|catalan";
static const char *_climates = "temperate|arctic|tropic|toyland";
static const char *_autosave_interval = "off|monthly|quarterly|half year|yearly";
static const char *_roadsides = "left|right";
static const char *_savegame_date = "long|short|iso";
#ifdef ENABLE_NETWORK
static const char *_server_langs = "ANY|ENGLISH|GERMAN|FRENCH|BRAZILIAN|BULGARIAN|CHINESE|CZECH|DANISH|DUTCH|ESPERANTO|FINNISH|HUNGARIAN|ICELANDIC|ITALIAN|JAPANESE|KOREAN|LITHUANIAN|NORWEGIAN|POLISH|PORTUGUESE|ROMANIAN|RUSSIAN|SLOVAK|SLOVENIAN|SPANISH|SWEDISH|TURKISH|UKRAINIAN|AFRIKAANS|CROATIAN|CATALAN|ESTONIAN|GALICIAN|GREEK|LATVIAN";
#endif /* ENABLE_NETWORK */

static const SettingDesc _gameopt_settings[] = {
	/* In version 4 a new difficulty setting has been added to the difficulty settings,
	 * town attitude towards demolishing. Needs special handling because some dimwit thought
	 * it funny to have the GameDifficulty struct be an array while it is a struct of
	 * same-sized members
	 * XXX - To save file-space and since values are never bigger than about 10? only
	 * save the first 16 bits in the savegame. Question is why the values are still int32
	 * and why not byte for example?
	 * 'SLE_FILE_I16 | SLE_VAR_U16' in "diff_custom" is needed to get around SlArray() hack
	 * for savegames version 0 - though it is an array, it has to go through the byteswap process */
SDTG_GENERAL("diff_custom", SDT_INTLIST, SL_ARR, SLE_FILE_I16 | SLE_VAR_U16, SLF_NOT_IN_CONFIG, 0, _old_diff_custom, 17, 0, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, 0, 3),
SDTG_GENERAL("diff_custom", SDT_INTLIST, SL_ARR, SLE_UINT16, SLF_NOT_IN_CONFIG, 0, _old_diff_custom, 18, 0, 0, 0, 0, NULL, STR_NULL, STR_NULL, NULL, 4, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.diff_level, SLE_UINT8, 0, 0, 3, 0, 3,        0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_OMANY(GameSettings, locale.currency, SLE_UINT8, SLF_NO_NETWORK_SYNC, 0, 0,       CUSTOM_CURRENCY_ID, _locale_currencies,            STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION, NULL),
SDT_OMANY(GameSettings, locale.units, SLE_UINT8, SLF_NO_NETWORK_SYNC, 0, 1,       2, _locale_units,            STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION, NULL),
SDT_OMANY(GameSettings, game_creation.town_name, SLE_UINT8, 0, 0, 0,       255, _town_names,            STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION, NULL),
SDT_OMANY(GameSettings, game_creation.landscape, SLE_UINT8, 0, 0, 0,       3, _climates,            STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION, ConvertLandscape),
SDT_VAR(GameSettings, game_creation.snow_line, SLE_UINT8, 0, 0, DEF_SNOWLINE_HEIGHT * TILE_HEIGHT, MIN_SNOWLINE_HEIGHT * TILE_HEIGHT, MAX_SNOWLINE_HEIGHT * TILE_HEIGHT,        0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_NULL(1, 0, 22),
SDTC_OMANY(       gui.autosave, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1,       4, _autosave_interval,            STR_NULL, STR_NULL, NULL, 23, SL_MAX_VERSION),
SDT_OMANY(GameSettings, vehicle.road_side, SLE_UINT8, 0, 0, 1,       1, _roadsides,            STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION, NULL),
SDT_END()
};
extern char _config_language_file[MAX_PATH];

static const SettingDescGlobVarList _misc_settings[] = {
SDTG_MMANY("display_opt", SLE_UINT8,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _display_opt, (1 << DO_SHOW_TOWN_NAMES | 1 << DO_SHOW_STATION_NAMES | 1 << DO_SHOW_SIGNS | 1 << DO_FULL_ANIMATION | 1 << DO_FULL_DETAIL | 1 << DO_SHOW_WAYPOINT_NAMES),                        "SHOW_TOWN_NAMES|SHOW_STATION_NAMES|SHOW_SIGNS|FULL_ANIMATION||FULL_DETAIL|WAYPOINTS", STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_BOOL("news_ticker_sound",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _news_ticker_sound, true,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_BOOL("fullscreen",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _fullscreen, false,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("graphicsset", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, BaseGraphics::ini_set, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("soundsset", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, BaseSounds::ini_set, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("musicset", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, BaseMusic::ini_set, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("videodriver", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _ini_videodriver, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("musicdriver", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _ini_musicdriver, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("sounddriver", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _ini_sounddriver, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("blitter", SLE_STRQ,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _ini_blitter, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("language", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _config_language_file, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_LIST("resolution", SLE_INT, 2, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _cur_resolution, "640,480",                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("screenshot_format", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _screenshot_format_name, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("savegame_format", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _savegame_format, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_BOOL("rightclick_emulate",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _rightclick_emulate, false,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#ifdef WITH_FREETYPE
SDTG_STR("small_font", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.small_font, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_STR("medium_font", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.medium_font, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_STR("large_font", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.large_font, NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_VAR("small_size", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.small_size, 8, 0, 72, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_VAR("medium_size", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.medium_size, 10, 0, 72, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_VAR("large_size", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.large_size, 16, 0, 72, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_BOOL("small_aa",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.small_aa, false,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_BOOL("medium_aa",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.medium_aa, false,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef WITH_FREETYPE
SDTG_BOOL("large_aa",                 SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _freetype.large_aa, false,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
SDTG_VAR("sprite_cache_size", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _sprite_cache_size, 4, 1, 64, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_VAR("player_face", SLE_UINT32,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _company_manager_face, 0, 0, 0xFFFFFFFF, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_VAR("transparency_options", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _transparency_opt, 0, 0, 0x1FF, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_VAR("transparency_locks", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _transparency_lock, 0, 0, 0x1FF, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_VAR("invisibility_options", SLE_UINT,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _invisibility_opt, 0, 0, 0xFF, 0,        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("keyboard", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _keyboard_opt[0], NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_STR("keyboard_caps", SLE_STRB,          SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _keyboard_opt[1], NULL,                               STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_END()
};
/* Begin - Callback Functions for the various settings */
static bool v_PositionMainToolbar(int32 p1);
static bool v_PositionStatusbar(int32 p1);
static bool PopulationInLabelActive(int32 p1);
static bool RedrawScreen(int32 p1);
static bool RedrawSmallmap(int32 p1);
static bool InvalidateStationBuildWindow(int32 p1);
static bool InvalidateBuildIndustryWindow(int32 p1);
static bool CloseSignalGUI(int32 p1);
static bool InvalidateTownViewWindow(int32 p1);
static bool DeleteSelectStationWindow(int32 p1);
static bool UpdateConsists(int32 p1);
static bool TrainAccelerationModelChanged(int32 p1);
static bool RoadVehAccelerationModelChanged(int32 p1);
static bool TrainSlopeSteepnessChanged(int32 p1);
static bool RoadVehSlopeSteepnessChanged(int32 p1);
static bool DragSignalsDensityChanged(int32);
static bool TownFoundingChanged(int32 p1);
static bool DifficultyReset(int32 level);
static bool DifficultyChange(int32);
static bool DifficultyNoiseChange(int32 i);
static bool MaxNoAIsChange(int32 i);
static bool CheckRoadSide(int p1);
static bool CheckFreeformEdges(int32 p1);
static bool ChangeDynamicEngines(int32 p1);
static bool StationCatchmentChanged(int32 p1);
static bool InvalidateVehTimetableWindow(int32 p1);
static bool InvalidateCompanyLiveryWindow(int32 p1);
static bool InvalidateNewGRFChangeWindows(int32 p1);
static bool InvalidateIndustryViewWindow(int32 p1);
static bool RedrawTownAuthority(int32 p1);
extern bool UpdateNewGRFConfigPalette(int32 p1);

#ifdef ENABLE_NETWORK
static bool UpdateClientName(int32 p1);
static bool UpdateServerPassword(int32 p1);
static bool UpdateRconPassword(int32 p1);
static bool UpdateClientConfigValues(int32 p1);
#endif /* ENABLE_NETWORK */
/* End - Callback Functions for the various settings */

/* Some settings do not need to be synchronised when playing in multiplayer.
 * These include for example the GUI settings and will not be saved with the
 * savegame.
 * It is also a bit tricky since you would think that service_interval
 * for example doesn't need to be synched. Every client assigns the
 * service_interval value to the v->service_interval, meaning that every client
 * assigns his value. If the setting was company-based, that would mean that
 * vehicles could decide on different moments that they are heading back to a
 * service depot, causing desyncs on a massive scale. */
const SettingDesc _settings[] = {
SDT_VAR(GameSettings, difficulty.max_no_competitors, SLE_UINT8, 0, 0, 0,       0, MAX_COMPANIES - 1, 1, STR_NULL, STR_NULL, MaxNoAIsChange, 97, SL_MAX_VERSION),
SDT_NULL(1, 97, 109),
SDT_VAR(GameSettings, difficulty.number_towns, SLE_UINT8, 0, SGF_NEWGAME_ONLY, 2,       0, 4, 1, STR_NULL, STR_NUM_VERY_LOW, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.industry_density, SLE_UINT8, 0, SGF_NEWGAME_ONLY, ID_END - 1,       0, ID_END - 1, 1, STR_NULL, STR_FUNDING_ONLY, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.max_loan, SLE_UINT32, 0, SGF_NEWGAME_ONLY | SGF_SCENEDIT_TOO | SGF_CURRENCY, 300000,       100000, 500000, 50000, STR_NULL, STR_NULL, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.initial_interest, SLE_UINT8, 0, SGF_NEWGAME_ONLY | SGF_SCENEDIT_TOO, 2,       2, 4, 1, STR_NULL, STR_NULL, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.vehicle_costs, SLE_UINT8, 0, SGF_NEWGAME_ONLY | SGF_SCENEDIT_TOO, 0,       0, 2, 1, STR_NULL, STR_SEA_LEVEL_LOW, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.competitor_speed, SLE_UINT8, 0, 0, 2,       0, 4, 1, STR_NULL, STR_AI_SPEED_VERY_SLOW, DifficultyChange, 97, SL_MAX_VERSION),
SDT_NULL(1, 97, 109),
SDT_VAR(GameSettings, difficulty.vehicle_breakdowns, SLE_UINT8, 0, 0, 1,       0, 2, 1, STR_NULL, STR_DISASTER_NONE, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.subsidy_multiplier, SLE_UINT8, 0, 0, 2,       0, 3, 1, STR_NULL, STR_SUBSIDY_X1_5, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.construction_cost, SLE_UINT8, 0, SGF_NEWGAME_ONLY | SGF_SCENEDIT_TOO, 0,       0, 2, 1, STR_NULL, STR_SEA_LEVEL_LOW, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.terrain_type, SLE_UINT8, 0, SGF_NEWGAME_ONLY, 1,       0, 3, 1, STR_NULL, STR_TERRAIN_TYPE_VERY_FLAT, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.quantity_sea_lakes, SLE_UINT8, 0, SGF_NEWGAME_ONLY, 0,       0, 4, 1, STR_NULL, STR_SEA_LEVEL_VERY_LOW, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.economy, SLE_UINT8, 0, 0, 0,       0, 1, 1, STR_NULL, STR_ECONOMY_STEADY, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.line_reverse_mode, SLE_UINT8, 0, 0, 0,       0, 1, 1, STR_NULL, STR_REVERSE_AT_END_OF_LINE_AND_AT_STATIONS, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.disasters, SLE_UINT8, 0, 0, 0,       0, 1, 1, STR_NULL, STR_CONFIG_SETTING_OFF, DifficultyChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.town_council_tolerance, SLE_UINT8, 0, 0, 0,       0, 2, 1, STR_NULL, STR_CITY_APPROVAL_PERMISSIVE, DifficultyNoiseChange, 97, SL_MAX_VERSION),
SDT_VAR(GameSettings, difficulty.diff_level, SLE_UINT8, 0, 0, 3,       0, 3, 0, STR_NULL, STR_NULL, DifficultyReset, 97, SL_MAX_VERSION),
SDT_OMANY(GameSettings, game_creation.town_name, SLE_UINT8, 0, SGF_NO_NETWORK, 0,             255, _town_names,     STR_NULL, STR_NULL, NULL, 97, SL_MAX_VERSION, NULL),
SDT_OMANY(GameSettings, game_creation.landscape, SLE_UINT8, 0, SGF_NO_NETWORK, 0,             3, _climates,     STR_NULL, STR_NULL, NULL, 97, SL_MAX_VERSION, ConvertLandscape),
SDT_VAR(GameSettings, game_creation.snow_line, SLE_UINT8, 0, SGF_NO_NETWORK, DEF_SNOWLINE_HEIGHT * TILE_HEIGHT,       MIN_SNOWLINE_HEIGHT * TILE_HEIGHT, MAX_SNOWLINE_HEIGHT * TILE_HEIGHT, 0, STR_NULL, STR_NULL, NULL, 97, SL_MAX_VERSION),
SDT_OMANY(GameSettings, vehicle.road_side, SLE_UINT8, 0, SGF_NO_NETWORK, 1,             1, _roadsides,     STR_NULL, STR_NULL, CheckRoadSide, 97, SL_MAX_VERSION, NULL),
SDT_BOOL(GameSettings, construction.build_on_slopes,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_BUILDONSLOPES, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.command_pause_level, SLE_UINT8, 0, SGF_MULTISTRING | SGF_NO_NETWORK, 1,       0, 3, 1, STR_CONFIG_SETTING_COMMAND_PAUSE_LEVEL, STR_CONFIG_SETTING_COMMAND_PAUSE_LEVEL_NO_ACTIONS, NULL, 154, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.terraform_per_64k_frames, SLE_UINT32, 0, 0, 64 << 16,       0, 1 << 30, 1, STR_NULL, STR_NULL, NULL, 156, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.terraform_frame_burst, SLE_UINT16, 0, 0, 4096,       0, 1 << 30, 1, STR_NULL, STR_NULL, NULL, 156, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.clear_per_64k_frames, SLE_UINT32, 0, 0, 64 << 16,       0, 1 << 30, 1, STR_NULL, STR_NULL, NULL, 156, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.clear_frame_burst, SLE_UINT16, 0, 0, 4096,       0, 1 << 30, 1, STR_NULL, STR_NULL, NULL, 156, SL_MAX_VERSION),
SDT_BOOL(GameSettings, construction.autoslope,        0, 0, true,                              STR_CONFIG_SETTING_AUTOSLOPE, STR_NULL, NULL, 75, SL_MAX_VERSION),
SDT_BOOL(GameSettings, construction.extra_dynamite,        0, 0, true,                              STR_CONFIG_SETTING_EXTRADYNAMITE, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.max_bridge_length, SLE_UINT16, 0, SGF_NO_NETWORK, 64,       1, 2048, 1, STR_CONFIG_SETTING_MAX_BRIDGE_LENGTH, STR_NULL, NULL, 159, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.max_tunnel_length, SLE_UINT16, 0, SGF_NO_NETWORK, 64,       1, 2048, 1, STR_CONFIG_SETTING_MAX_TUNNEL_LENGTH, STR_NULL, NULL, 159, SL_MAX_VERSION),
SDT_NULL(1, 0, 158),
SDT_BOOL(GameSettings, construction.signal_side,        SLF_NO_NETWORK_SYNC, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_SIGNALSIDE, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, station.never_expire_airports,        0, SGF_NO_NETWORK, false,                              STR_CONFIG_SETTING_NEVER_EXPIRE_AIRPORTS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.town_layout, SLE_UINT8, 0, SGF_MULTISTRING, TL_ORIGINAL,       TL_BEGIN, NUM_TLS - 1, 1, STR_CONFIG_SETTING_TOWN_LAYOUT, STR_CONFIG_SETTING_TOWN_LAYOUT_DEFAULT, TownFoundingChanged, 59, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.allow_town_roads,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_ALLOW_TOWN_ROADS, STR_NULL, NULL, 113, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.found_town, SLE_UINT8, 0, SGF_MULTISTRING, TF_FORBIDDEN,       TF_BEGIN, TF_END - 1, 1, STR_CONFIG_SETTING_TOWN_FOUNDING, STR_CONFIG_SETTING_TOWN_FOUNDING_FORBIDDEN, TownFoundingChanged, 128, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.allow_town_level_crossings,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_ALLOW_TOWN_LEVEL_CROSSINGS, STR_NULL, NULL, 143, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.train_acceleration_model, SLE_UINT8, 0, SGF_MULTISTRING, 0,       0, 1, 1, STR_CONFIG_SETTING_TRAIN_ACCELERATION_MODEL, STR_CONFIG_SETTING_ORIGINAL, TrainAccelerationModelChanged, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.roadveh_acceleration_model, SLE_UINT8, 0, SGF_MULTISTRING, 0,       0, 1, 1, STR_CONFIG_SETTING_ROAD_VEHICLE_ACCELERATION_MODEL, STR_CONFIG_SETTING_ORIGINAL, RoadVehAccelerationModelChanged, 139, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.train_slope_steepness, SLE_UINT8, 0, 0, 3,       0, 10, 1, STR_CONFIG_SETTING_TRAIN_SLOPE_STEEPNESS, STR_NULL, TrainSlopeSteepnessChanged, 133, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.roadveh_slope_steepness, SLE_UINT8, 0, 0, 7,       0, 10, 1, STR_CONFIG_SETTING_ROAD_VEHICLE_SLOPE_STEEPNESS, STR_NULL, RoadVehSlopeSteepnessChanged, 139, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.forbid_90_deg,        0, 0, false,                              STR_CONFIG_SETTING_FORBID_90_DEG, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.max_train_length, SLE_UINT8, 0, 0, 7,       1, 64, 1, STR_CONFIG_SETTING_TRAIN_LENGTH, STR_NULL, NULL, 159, SL_MAX_VERSION),
SDT_NULL(1, 0, 158),
SDT_VAR(GameSettings, vehicle.smoke_amount, SLE_UINT8, 0, SGF_MULTISTRING, 1,       0, 2, 0, STR_CONFIG_SETTING_SMOKE_AMOUNT, STR_CONFIG_SETTING_NONE, NULL, 145, SL_MAX_VERSION),
SDT_NULL(1, 0, 158),
SDT_BOOL(GameSettings, pf.roadveh_queue,        0, 0, true,                              STR_CONFIG_SETTING_ROAD_VEHICLE_QUEUEING, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.new_pathfinding_all,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 0, 86),
SDT_BOOL(GameSettings, pf.yapf.ship_use_yapf,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 28, 86),
SDT_BOOL(GameSettings, pf.yapf.road_use_yapf,        0, 0, true,                              STR_NULL, STR_NULL, NULL, 28, 86),
SDT_BOOL(GameSettings, pf.yapf.rail_use_yapf,        0, 0, true,                              STR_NULL, STR_NULL, NULL, 28, 86),
SDT_VAR(GameSettings, pf.pathfinder_for_trains, SLE_UINT8, 0, SGF_MULTISTRING, 2,       1, 2, 1, STR_CONFIG_SETTING_PATHFINDER_FOR_TRAINS, STR_CONFIG_SETTING_PATHFINDER_NPF, NULL, 87, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.pathfinder_for_roadvehs, SLE_UINT8, 0, SGF_MULTISTRING, 2,       1, 2, 1, STR_CONFIG_SETTING_PATHFINDER_FOR_ROAD_VEHICLES, STR_CONFIG_SETTING_PATHFINDER_NPF, NULL, 87, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.pathfinder_for_ships, SLE_UINT8, 0, SGF_MULTISTRING, 2,       0, 2, 1, STR_CONFIG_SETTING_PATHFINDER_FOR_SHIPS, STR_CONFIG_SETTING_PATHFINDER_OPF, NULL, 87, SL_MAX_VERSION),
SDT_BOOL(GameSettings, vehicle.never_expire_vehicles,        0, SGF_NO_NETWORK, false,                              STR_CONFIG_SETTING_NEVER_EXPIRE_VEHICLES, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.max_trains, SLE_UINT16, 0, 0, 500,       0, 5000, 0, STR_CONFIG_SETTING_MAX_TRAINS, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.max_roadveh, SLE_UINT16, 0, 0, 500,       0, 5000, 0, STR_CONFIG_SETTING_MAX_ROAD_VEHICLES, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.max_aircraft, SLE_UINT16, 0, 0, 200,       0, 5000, 0, STR_CONFIG_SETTING_MAX_AIRCRAFT, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.max_ships, SLE_UINT16, 0, 0, 300,       0, 5000, 0, STR_CONFIG_SETTING_MAX_SHIPS, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDTG_BOOL(NULL,              0, SGF_NO_NETWORK, _old_vds.servint_ispercent, false,                        STR_NULL, STR_NULL, NULL, 0, 119),
SDTG_VAR(NULL,       SLE_UINT16, 0, SGF_0ISDISABLED, _old_vds.servint_trains, 150, 5, 800, 0, STR_NULL, STR_NULL, NULL, 0, 119),
SDTG_VAR(NULL,       SLE_UINT16, 0, SGF_0ISDISABLED, _old_vds.servint_roadveh, 150, 5, 800, 0, STR_NULL, STR_NULL, NULL, 0, 119),
SDTG_VAR(NULL,       SLE_UINT16, 0, SGF_0ISDISABLED, _old_vds.servint_ships, 360, 5, 800, 0, STR_NULL, STR_NULL, NULL, 0, 119),
SDTG_VAR(NULL,       SLE_UINT16, 0, SGF_0ISDISABLED, _old_vds.servint_aircraft, 150, 5, 800, 0, STR_NULL, STR_NULL, NULL, 0, 119),
SDT_BOOL(GameSettings, order.no_servicing_if_no_breakdowns,        0, 0, true,                              STR_CONFIG_SETTING_NOSERVICE, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, vehicle.wagon_speed_limits,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_WAGONSPEEDLIMITS, STR_NULL, UpdateConsists, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, vehicle.disable_elrails,        0, SGF_NO_NETWORK, false,                              STR_CONFIG_SETTING_DISABLE_ELRAILS, STR_NULL, SettingsDisableElrail, 38, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.freight_trains, SLE_UINT8, 0, SGF_NO_NETWORK, 1,       1, 255, 1, STR_CONFIG_SETTING_FREIGHT_TRAINS, STR_NULL, UpdateConsists, 39, SL_MAX_VERSION),
SDT_NULL(1, 67, 158),
SDT_VAR(GameSettings, vehicle.plane_speed, SLE_UINT8, 0, SGF_NO_NETWORK, 4,       1, 4, 0, STR_CONFIG_SETTING_PLANE_SPEED, STR_NULL, NULL, 90, SL_MAX_VERSION),
SDT_BOOL(GameSettings, vehicle.dynamic_engines,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_DYNAMIC_ENGINES, STR_NULL, ChangeDynamicEngines, 95, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.plane_crashes, SLE_UINT8, 0, SGF_MULTISTRING, 2,       0, 2, 1, STR_CONFIG_SETTING_PLANE_CRASHES, STR_CONFIG_SETTING_PLANE_CRASHES_NONE, NULL, 138, SL_MAX_VERSION),
SDT_NULL(1, 0, 158),
SDTC_BOOL(       gui.sg_full_load_any,        0, 0, true,                              STR_NULL, STR_NULL, NULL, 22, 92),
SDT_BOOL(GameSettings, order.improved_load,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_IMPROVEDLOAD, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, order.selectgoods,        0, 0, true,                              STR_CONFIG_SETTING_SELECTGOODS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.sg_new_nonstop,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 22, 92),
SDT_NULL(1, 0, 158),
SDT_VAR(GameSettings, station.station_spread, SLE_UINT8, 0, 0, 12,       4, 64, 0, STR_CONFIG_SETTING_STATION_SPREAD, STR_NULL, InvalidateStationBuildWindow, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, order.serviceathelipad,        0, 0, true,                              STR_CONFIG_SETTING_SERVICEATHELIPAD, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, station.modified_catchment,        0, 0, true,                              STR_CONFIG_SETTING_CATCHMENT, STR_NULL, StationCatchmentChanged, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, order.gradual_loading,        0, SGF_NO_NETWORK, true,                              STR_CONFIG_SETTING_GRADUAL_LOADING, STR_NULL, NULL, 40, SL_MAX_VERSION),
SDT_BOOL(GameSettings, construction.road_stop_on_town_road,        0, 0, true,                              STR_CONFIG_SETTING_STOP_ON_TOWN_ROAD, STR_NULL, NULL, 47, SL_MAX_VERSION),
SDT_BOOL(GameSettings, construction.road_stop_on_competitor_road,        0, 0, true,                              STR_CONFIG_SETTING_STOP_ON_COMPETITOR_ROAD, STR_NULL, NULL, 114, SL_MAX_VERSION),
SDT_BOOL(GameSettings, station.adjacent_stations,        0, 0, true,                              STR_CONFIG_SETTING_ADJACENT_STATIONS, STR_NULL, NULL, 62, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.station_noise_level,        0, SGF_NO_NETWORK, false,                              STR_CONFIG_SETTING_NOISE_LEVEL, STR_NULL, InvalidateTownViewWindow, 96, SL_MAX_VERSION),
SDT_BOOL(GameSettings, station.distant_join_stations,        0, 0, true,                              STR_CONFIG_SETTING_DISTANT_JOIN_STATIONS, STR_NULL, DeleteSelectStationWindow, 106, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.inflation,        0, 0, true,                              STR_CONFIG_SETTING_INFLATION, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.raw_industry_construction, SLE_UINT8, 0, SGF_MULTISTRING, 0,       0, 2, 0, STR_CONFIG_SETTING_RAW_INDUSTRY_CONSTRUCTION_METHOD, STR_CONFIG_SETTING_RAW_INDUSTRY_CONSTRUCTION_METHOD_NONE, InvalidateBuildIndustryWindow, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.industry_platform, SLE_UINT8, 0, 0, 1,       0, 4, 0, STR_CONFIG_SETTING_INDUSTRY_PLATFORM, STR_NULL, NULL, 148, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.multiple_industry_per_town,        0, 0, false,                              STR_CONFIG_SETTING_MULTIPINDTOWN, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_NULL(1, 0, 140),
SDT_BOOL(GameSettings, economy.bribe,        0, 0, true,                              STR_CONFIG_SETTING_BRIBE, STR_NULL, RedrawTownAuthority, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.exclusive_rights,        0, 0, true,                              STR_CONFIG_SETTING_ALLOW_EXCLUSIVE, STR_NULL, RedrawTownAuthority, 79, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.fund_roads,        0, 0, true,                              STR_CONFIG_SETTING_ALLOW_FUND_ROAD, STR_NULL, RedrawTownAuthority, 160, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.give_money,        0, 0, true,                              STR_CONFIG_SETTING_ALLOW_GIVE_MONEY, STR_NULL, NULL, 79, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.snow_line_height, SLE_UINT8, 0, 0, DEF_SNOWLINE_HEIGHT,       MIN_SNOWLINE_HEIGHT, MAX_SNOWLINE_HEIGHT, 0, STR_CONFIG_SETTING_SNOWLINE_HEIGHT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_NULL(4, 0, 143),
SDT_VAR(GameSettings, game_creation.starting_year, SLE_INT32, 0, SGF_NOCOMMA, DEF_START_YEAR,       MIN_YEAR, MAX_YEAR, 1, STR_CONFIG_SETTING_STARTING_YEAR, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_NULL(4, 0, 104),
SDT_BOOL(GameSettings, economy.smooth_economy,        0, 0, true,                              STR_CONFIG_SETTING_SMOOTH_ECONOMY, STR_NULL, InvalidateIndustryViewWindow, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.allow_shares,        0, 0, false,                              STR_CONFIG_SETTING_ALLOW_SHARES, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.feeder_payment_share, SLE_UINT8, 0, 0, 75,       0, 100, 0, STR_CONFIG_SETTING_FEEDER_PAYMENT_SHARE, STR_NULL, NULL, 134, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.town_growth_rate, SLE_UINT8, 0, SGF_MULTISTRING, 2,       0, 4, 0, STR_CONFIG_SETTING_TOWN_GROWTH, STR_CONFIG_SETTING_TOWN_GROWTH_NONE, NULL, 54, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.larger_towns, SLE_UINT8, 0, SGF_0ISDISABLED, 4,       0, 255, 1, STR_CONFIG_SETTING_LARGER_TOWNS, STR_NULL, NULL, 54, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.initial_city_size, SLE_UINT8, 0, 0, 2,       1, 10, 1, STR_CONFIG_SETTING_CITY_SIZE_MULTIPLIER, STR_NULL, NULL, 56, SL_MAX_VERSION),
SDT_BOOL(GameSettings, economy.mod_road_rebuild,        0, 0, true,                              STR_CONFIG_SETTING_MODIFIED_ROAD_REBUILD, STR_NULL, NULL, 77, SL_MAX_VERSION),
SDT_NULL(1, 0, 106),
SDT_BOOL(GameSettings, ai.ai_in_multiplayer,        0, 0, true,                              STR_CONFIG_SETTING_AI_IN_MULTIPLAYER, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, ai.ai_disable_veh_train,        0, 0, false,                              STR_CONFIG_SETTING_AI_BUILDS_TRAINS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, ai.ai_disable_veh_roadveh,        0, 0, false,                              STR_CONFIG_SETTING_AI_BUILDS_ROAD_VEHICLES, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, ai.ai_disable_veh_aircraft,        0, 0, false,                              STR_CONFIG_SETTING_AI_BUILDS_AIRCRAFT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, ai.ai_disable_veh_ship,        0, 0, false,                              STR_CONFIG_SETTING_AI_BUILDS_SHIPS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, ai.ai_max_opcode_till_suspend, SLE_UINT32, 0, SGF_NEWGAME_ONLY, 10000,       5000, 250000, 2500, STR_CONFIG_SETTING_AI_MAX_OPCODES, STR_NULL, NULL, 107, SL_MAX_VERSION),
SDT_VAR(GameSettings, vehicle.extend_vehicle_life, SLE_UINT8, 0, 0, 0,       0, 100, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.dist_local_authority, SLE_UINT8, 0, 0, 20,       5, 60, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.reverse_at_signals,        0, 0, false,                              STR_CONFIG_SETTING_REVERSE_AT_SIGNALS, STR_NULL, NULL, 159, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.wait_oneway_signal, SLE_UINT8, 0, 0, 15,       2, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.wait_twoway_signal, SLE_UINT8, 0, 0, 41,       2, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.town_noise_population[0], SLE_UINT16, 0, 0, 800,       200, 65535, 0, STR_NULL, STR_NULL, NULL, 96, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.town_noise_population[1], SLE_UINT16, 0, 0, 2000,       400, 65535, 0, STR_NULL, STR_NULL, NULL, 96, SL_MAX_VERSION),
SDT_VAR(GameSettings, economy.town_noise_population[2], SLE_UINT16, 0, 0, 4000,       800, 65535, 0, STR_NULL, STR_NULL, NULL, 96, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.wait_for_pbs_path, SLE_UINT8, 0, 0, 30,       2, 255, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.reserve_paths,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.path_backoff_interval, SLE_UINT8, 0, 0, 20,       1, 255, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.opf.pf_maxlength, SLE_UINT16, 0, 0, 4096,       64, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.opf.pf_maxdepth, SLE_UINT8, 0, 0, 48,       4, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_max_search_nodes, SLE_UINT, 0, 0, 10000,       500, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_firstred_penalty, SLE_UINT, 0, 0, 10 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_firstred_exit_penalty, SLE_UINT, 0, 0, 100 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_lastred_penalty, SLE_UINT, 0, 0, 10 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_station_penalty, SLE_UINT, 0, 0, 1 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_slope_penalty, SLE_UINT, 0, 0, 1 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_curve_penalty, SLE_UINT, 0, 0, 1 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_depot_reverse_penalty, SLE_UINT, 0, 0, 50 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_pbs_cross_penalty, SLE_UINT, 0, 0, 3 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_rail_pbs_signal_back_penalty, SLE_UINT, 0, 0, 15 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_buoy_penalty, SLE_UINT, 0, 0, 2 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_water_curve_penalty, SLE_UINT, 0, 0, 1 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_road_curve_penalty, SLE_UINT, 0, 0, 1 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_crossing_penalty, SLE_UINT, 0, 0, 3 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_road_drive_through_penalty, SLE_UINT, 0, 0, 8 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 47, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_road_dt_occupied_penalty, SLE_UINT, 0, 0, 8 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 130, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.npf_road_bay_occupied_penalty, SLE_UINT, 0, 0, 15 * NPF_TILE_LENGTH,       0, 100000, 0, STR_NULL, STR_NULL, NULL, 130, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.npf.maximum_go_to_depot_penalty, SLE_UINT, 0, 0, 20 * NPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 131, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.yapf.disable_node_optimization,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.max_search_nodes, SLE_UINT, 0, 0, 10000,       500, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_BOOL(GameSettings, pf.yapf.rail_firstred_twoway_eol,        0, 0, false,                              STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_firstred_penalty, SLE_UINT, 0, 0, 10 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_firstred_exit_penalty, SLE_UINT, 0, 0, 100 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_lastred_penalty, SLE_UINT, 0, 0, 10 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_lastred_exit_penalty, SLE_UINT, 0, 0, 100 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_station_penalty, SLE_UINT, 0, 0, 10 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_slope_penalty, SLE_UINT, 0, 0, 2 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_curve45_penalty, SLE_UINT, 0, 0, 1 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_curve90_penalty, SLE_UINT, 0, 0, 6 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_depot_reverse_penalty, SLE_UINT, 0, 0, 50 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_crossing_penalty, SLE_UINT, 0, 0, 3 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_look_ahead_max_signals, SLE_UINT, 0, 0, 10,       1, 100, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_look_ahead_signal_p0, SLE_INT, 0, 0, 500,       -1000000, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_look_ahead_signal_p1, SLE_INT, 0, 0, -100,       -1000000, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_look_ahead_signal_p2, SLE_INT, 0, 0, 5,       -1000000, 1000000, 0, STR_NULL, STR_NULL, NULL, 28, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_pbs_cross_penalty, SLE_UINT, 0, 0, 3 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_pbs_station_penalty, SLE_UINT, 0, 0, 8 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_pbs_signal_back_penalty, SLE_UINT, 0, 0, 15 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_doubleslip_penalty, SLE_UINT, 0, 0, 1 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 100, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_longer_platform_penalty, SLE_UINT, 0, 0, 8 * YAPF_TILE_LENGTH,       0, 20000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_longer_platform_per_tile_penalty, SLE_UINT, 0, 0, 0 * YAPF_TILE_LENGTH,       0, 20000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_shorter_platform_penalty, SLE_UINT, 0, 0, 40 * YAPF_TILE_LENGTH,       0, 20000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.rail_shorter_platform_per_tile_penalty, SLE_UINT, 0, 0, 0 * YAPF_TILE_LENGTH,       0, 20000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_slope_penalty, SLE_UINT, 0, 0, 2 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_curve_penalty, SLE_UINT, 0, 0, 1 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_crossing_penalty, SLE_UINT, 0, 0, 3 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 33, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_stop_penalty, SLE_UINT, 0, 0, 8 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 47, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_stop_occupied_penalty, SLE_UINT, 0, 0, 8 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 130, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.road_stop_bay_occupied_penalty, SLE_UINT, 0, 0, 15 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 130, SL_MAX_VERSION),
SDT_VAR(GameSettings, pf.yapf.maximum_go_to_depot_penalty, SLE_UINT, 0, 0, 20 * YAPF_TILE_LENGTH,       0, 1000000, 0, STR_NULL, STR_NULL, NULL, 131, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.land_generator, SLE_UINT8, 0, SGF_MULTISTRING, 1,       0, 1, 0, STR_CONFIG_SETTING_LAND_GENERATOR, STR_CONFIG_SETTING_LAND_GENERATOR_ORIGINAL, NULL, 30, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.oil_refinery_limit, SLE_UINT8, 0, 0, 32,       12, 48, 0, STR_CONFIG_SETTING_OIL_REF_EDGE_DISTANCE, STR_NULL, NULL, 30, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.tgen_smoothness, SLE_UINT8, 0, SGF_MULTISTRING, 1,       0, 3, 0, STR_CONFIG_SETTING_ROUGHNESS_OF_TERRAIN, STR_CONFIG_SETTING_ROUGHNESS_OF_TERRAIN_VERY_SMOOTH, NULL, 30, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.variety, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 0,       0, 5, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.generation_seed, SLE_UINT32, 0, 0, GENERATE_NEW_SEED,       0, UINT32_MAX, 0, STR_NULL, STR_NULL, NULL, 30, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.tree_placer, SLE_UINT8, 0, SGF_MULTISTRING, 2,       0, 2, 0, STR_CONFIG_SETTING_TREE_PLACER, STR_CONFIG_SETTING_TREE_PLACER_NONE, NULL, 30, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.heightmap_rotation, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,       0, 1, 0, STR_CONFIG_SETTING_HEIGHTMAP_ROTATION, STR_CONFIG_SETTING_HEIGHTMAP_ROTATION_COUNTER_CLOCKWISE, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.se_flat_world_height, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1,       0, 15, 0, STR_CONFIG_SETTING_SE_FLAT_WORLD_HEIGHT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.map_x, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 8,       MIN_MAP_SIZE_BITS, MAX_MAP_SIZE_BITS, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.map_y, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 8,       MIN_MAP_SIZE_BITS, MAX_MAP_SIZE_BITS, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDT_BOOL(GameSettings, construction.freeform_edges,        0, 0, true,                              STR_CONFIG_SETTING_ENABLE_FREEFORM_EDGES, STR_NULL, CheckFreeformEdges, 111, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.water_borders, SLE_UINT8, 0, 0, 15,       0, 16, 0, STR_NULL, STR_NULL, NULL, 111, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.custom_town_number, SLE_UINT16, 0, 0, 1,       1, 5000, 0, STR_NULL, STR_NULL, NULL, 115, SL_MAX_VERSION),
SDT_VAR(GameSettings, construction.extra_tree_placement, SLE_UINT8, 0, SGF_MULTISTRING, 2,       0, 2, 0, STR_CONFIG_SETTING_EXTRA_TREE_PLACEMENT, STR_CONFIG_SETTING_EXTRA_TREE_PLACEMENT_NONE, NULL, 132, SL_MAX_VERSION),
SDT_VAR(GameSettings, game_creation.custom_sea_level, SLE_UINT8, 0, 0, 1,       2, 90, 0, STR_NULL, STR_NULL, NULL, 149, SL_MAX_VERSION),
SDT_OMANY(GameSettings, locale.currency, SLE_UINT8, SLF_NO_NETWORK_SYNC, 0, 0,             CUSTOM_CURRENCY_ID, _locale_currencies,     STR_NULL, STR_NULL, RedrawScreen, 97, SL_MAX_VERSION, NULL),
SDT_OMANY(GameSettings, locale.units, SLE_UINT8, SLF_NO_NETWORK_SYNC, 0, 1,             2, _locale_units,     STR_NULL, STR_NULL, RedrawScreen, 97, SL_MAX_VERSION, NULL),
SDT_STR(GameSettings, locale.digit_group_separator, SLE_STRQ, SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, RedrawScreen, 118, SL_MAX_VERSION),
SDT_STR(GameSettings, locale.digit_group_separator_currency, SLE_STRQ, SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, RedrawScreen, 118, SL_MAX_VERSION),
SDT_STR(GameSettings, locale.digit_decimal_separator, SLE_STRQ, SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, RedrawScreen, 126, SL_MAX_VERSION),
SDTC_OMANY(       gui.autosave, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1,             4, _autosave_interval,     STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.threaded_saves,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_OMANY(       gui.date_format_in_default_names, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,             2, _savegame_date,     STR_CONFIG_SETTING_DATE_FORMAT_IN_SAVE_NAMES, STR_CONFIG_SETTING_DATE_FORMAT_IN_SAVE_NAMES_LONG, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.show_finances,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_SHOWFINANCES, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.autoscroll,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_AUTOSCROLL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.reverse_scroll,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_REVERSE_SCROLLING, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.smooth_scroll,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_SMOOTH_SCROLLING, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.left_mouse_btn_scrolling,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_LEFT_MOUSE_BTN_SCROLLING, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.measure_tooltip,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_MEASURE_TOOLTIP, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.errmsg_duration, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 5,       0, 20, 0, STR_CONFIG_SETTING_ERRMSG_DURATION, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.hover_delay, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED, 2,       1, 5, 0, STR_CONFIG_SETTING_HOVER_DELAY, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.toolbar_pos, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 2, 0, STR_CONFIG_SETTING_TOOLBAR_POS, STR_CONFIG_SETTING_HORIZONTAL_POS_LEFT, v_PositionMainToolbar, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.statusbar_pos, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 2, 0, STR_CONFIG_SETTING_STATUSBAR_POS, STR_CONFIG_SETTING_HORIZONTAL_POS_LEFT, v_PositionStatusbar, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.window_snap_radius, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED, 10,       1, 32, 0, STR_CONFIG_SETTING_SNAP_RADIUS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.window_soft_limit, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED, 5,       5, 255, 1, STR_CONFIG_SETTING_SOFT_LIMIT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.population_in_label,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_POPULATION_IN_LABEL, STR_NULL, PopulationInLabelActive, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.link_terraform_toolbar,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_LINK_TERRAFORM_TOOLBAR, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.smallmap_land_colour, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,       0, 2, 0, STR_CONFIG_SETTING_SMALLMAP_LAND_COLOUR, STR_CONFIG_SETTING_SMALLMAP_LAND_COLOUR_GREEN, RedrawSmallmap, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.liveries, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 2,       0, 2, 0, STR_CONFIG_SETTING_LIVERIES, STR_CONFIG_SETTING_LIVERIES_NONE, InvalidateCompanyLiveryWindow, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.prefer_teamchat,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_PREFER_TEAMCHAT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.scrollwheel_scrolling, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,       0, 2, 0, STR_CONFIG_SETTING_SCROLLWHEEL_SCROLLING, STR_CONFIG_SETTING_SCROLLWHEEL_ZOOM, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.scrollwheel_multiplier, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 5,       1, 15, 1, STR_CONFIG_SETTING_SCROLLWHEEL_MULTIPLIER, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.pause_on_newgame,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_PAUSE_ON_NEW_GAME, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.advanced_vehicle_list, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 2, 0, STR_CONFIG_SETTING_ADVANCED_VEHICLE_LISTS, STR_CONFIG_SETTING_COMPANIES_OFF, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.timetable_in_ticks,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_TIMETABLE_IN_TICKS, STR_NULL, InvalidateVehTimetableWindow, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.timetable_arrival_departure,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_TIMETABLE_SHOW_ARRIVAL_DEPARTURE, STR_NULL, InvalidateVehTimetableWindow, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.quick_goto,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_QUICKGOTO, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.loading_indicators, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 2, 0, STR_CONFIG_SETTING_LOADING_INDICATORS, STR_CONFIG_SETTING_COMPANIES_OFF, RedrawScreen, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.default_rail_type, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,       0, 2, 0, STR_CONFIG_SETTING_DEFAULT_RAIL_TYPE, STR_CONFIG_SETTING_DEFAULT_RAIL_TYPE_FIRST, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.enable_signal_gui,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_ENABLE_SIGNAL_GUI, STR_NULL, CloseSignalGUI, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.coloured_news_year, SLE_INT32, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NOCOMMA, 2000,       MIN_YEAR, MAX_YEAR, 1, STR_CONFIG_SETTING_COLOURED_NEWS_YEAR, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.drag_signals_density, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 4,       1, 20, 0, STR_CONFIG_SETTING_DRAG_SIGNALS_DENSITY, STR_NULL, DragSignalsDensityChanged, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.semaphore_build_before, SLE_INT32, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NOCOMMA, 1950,       MIN_YEAR, MAX_YEAR, 1, STR_CONFIG_SETTING_SEMAPHORE_BUILD_BEFORE_DATE, STR_NULL, ResetSignalVariant, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.vehicle_income_warn,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_WARN_INCOME_LESS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.order_review_system, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 2,       0, 2, 0, STR_CONFIG_SETTING_ORDER_REVIEW, STR_CONFIG_SETTING_ORDER_REVIEW_OFF, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.lost_vehicle_warn,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_WARN_LOST_VEHICLE, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.disable_unsuitable_building,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_DISABLE_UNSUITABLE_BUILDING, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.new_nonstop,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_NONSTOP_BY_DEFAULT, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.stop_location, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 2,       0, 2, 1, STR_CONFIG_SETTING_STOP_LOCATION, STR_CONFIG_SETTING_STOP_LOCATION_NEAR_END, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.keep_all_autosave,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.autosave_on_exit,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.max_num_autosaves, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 16,       0, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.auto_euro,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.news_message_timeout, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 2,       1, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.show_track_reservation,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_SHOW_TRACK_RESERVATION, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.default_signal_type, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 2, 1, STR_CONFIG_SETTING_DEFAULT_SIGNAL_TYPE, STR_CONFIG_SETTING_DEFAULT_SIGNAL_NORMAL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.cycle_signal_types, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 2,       0, 2, 1, STR_CONFIG_SETTING_CYCLE_SIGNAL_TYPES, STR_CONFIG_SETTING_CYCLE_SIGNAL_NORMAL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.station_numtracks, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1,       1, 7, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.station_platlength, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 5,       1, 7, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.station_dragdrop,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.station_show_coverage,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.persistent_buildingtools,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_CONFIG_SETTING_PERSISTENT_BUILDINGTOOLS, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.expenses_layout,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_CONFIG_SETTING_EXPENSES_LAYOUT, STR_NULL, RedrawScreen, 0, SL_MAX_VERSION),
#ifdef DEDICATED
SDTC_BOOL(       gui.show_date_in_logs,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifndef DEDICATED
SDTC_BOOL(       gui.show_date_in_logs,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
SDTC_VAR(       gui.developer, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 1,       0, 2, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.newgrf_developer_tools,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, InvalidateNewGRFChangeWindows, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.ai_developer_tools,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.scenario_developer,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, InvalidateNewGRFChangeWindows, 0, SL_MAX_VERSION),
SDTC_BOOL(       gui.newgrf_show_old_versions,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.newgrf_default_palette, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 1,       0, 1, 0, STR_CONFIG_SETTING_NEWGRF_DEFAULT_PALETTE, STR_CONFIG_SETTING_NEWGRF_DEFAULT_PALETTE_DOS, UpdateNewGRFConfigPalette, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.console_backlog_timeout, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 100,       10, 65500, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       gui.console_backlog_length, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 100,       10, 65500, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       music.playlist, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 0,       0, 5, 1, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       music.music_vol, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 127,       0, 127, 1, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_VAR(       music.effect_vol, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 127,       0, 127, 1, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_LIST(       music.custom_1, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_LIST(       music.custom_2, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       music.playing,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTC_BOOL(       music.shuffle,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#ifdef ENABLE_NETWORK
SDTC_VAR(       gui.network_chat_box_width, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 620,       200, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       gui.network_chat_box_height, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 25,       5, 255, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       gui.network_chat_timeout, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 20,       1, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.sync_freq, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NOT_IN_CONFIG | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 100,       0, 100, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.frame_freq, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NOT_IN_CONFIG | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 0,       0, 100, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.commands_per_frame, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 2,       1, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.max_commands_in_queue, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 16,       1, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.bytes_per_frame, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 8,       1, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.bytes_per_frame_burst, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 256,       1, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.max_join_time, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 500,       0, 32000, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.pause_on_join,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.server_port, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NETWORK_DEFAULT_PORT,       0, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.server_admin_port, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NETWORK_ADMIN_PORT,       0, 65535, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.server_admin_chat,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, true,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.server_advertise,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.lan_internet, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 0,       0, 1, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.client_name, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, UpdateClientName, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.server_password, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NULL,                              STR_NULL, STR_NULL, UpdateServerPassword, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.rcon_password, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NULL,                              STR_NULL, STR_NULL, UpdateRconPassword, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.admin_password, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.default_company_pass, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.server_name, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.connect_to_ip, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.network_id, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, NULL,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.autoclean_companies,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.autoclean_unprotected, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED | SGF_NETWORK_ONLY, 12,       0, 240, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.autoclean_protected, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED | SGF_NETWORK_ONLY, 36,       0, 240, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.autoclean_novehicles, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED | SGF_NETWORK_ONLY, 0,       0, 240, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.max_companies, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 15,       1, MAX_COMPANIES, 0, STR_NULL, STR_NULL, UpdateClientConfigValues, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.max_clients, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 25,       2, MAX_CLIENTS, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.max_spectators, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 15,       0, MAX_CLIENTS, 0, STR_NULL, STR_NULL, UpdateClientConfigValues, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.restart_game_year, SLE_INT32, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_0ISDISABLED | SGF_NETWORK_ONLY | SGF_NOCOMMA, 0,       MIN_YEAR, MAX_YEAR, 1, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.min_active_clients, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 0,       0, MAX_CLIENTS, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_OMANY(       network.server_lang, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, 0,             35, _server_langs,     STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.reload_cfg,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_NETWORK_ONLY, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_STR(       network.last_host, SLE_STRB, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, "",                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_VAR(       network.last_port, SLE_UINT16, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, 0,       0, UINT16_MAX, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef ENABLE_NETWORK
SDTC_BOOL(       network.no_http_content_downloads,        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, false,                              STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
#endif
#ifdef __APPLE__
SDTC_VAR(       gui.right_mouse_btn_emulation, SLE_UINT8, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, SGF_MULTISTRING, 0,       0, 2, 0, STR_CONFIG_SETTING_RIGHT_MOUSE_BTN_EMU, STR_CONFIG_SETTING_RIGHT_MOUSE_BTN_EMU_COMMAND, NULL, 0, SL_MAX_VERSION),
#endif
SDT_END()
};
/* win32_v.cpp only settings */
#if defined(WIN32) && !defined(DEDICATED)
extern bool _force_full_redraw, _window_maximize;
extern uint _display_hz, _fullscreen_bpp;

static const SettingDescGlobVarList _win32_settings[] = {
SDTG_VAR("display_hz", SLE_UINT, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _display_hz, 0, 0, 120, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_BOOL("force_full_redraw",        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _force_full_redraw, false,                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_VAR("fullscreen_bpp", SLE_UINT, SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _fullscreen_bpp, 8, 8, 32, 0, STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_BOOL("window_maximize",        SLF_NOT_IN_SAVE | SLF_NO_NETWORK_SYNC, 0, _window_maximize, false,                        STR_NULL, STR_NULL, NULL, 0, SL_MAX_VERSION),
SDTG_END()
};
#endif /* WIN32 */
