//Contents of ENUMS.H
//	©2014, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __ENUMS_H_INCLUDED
#define __ENUMS_H_INCLUDED

enum vbStates
	{	vbsIdle,						//0
		vbsPending,						//1
		vbsComplete=0xFF				//0xFF
	};

enum enable_disable {
	enable,
	disable,
	disable_initiation
};

enum	eventnotificationtypes {
	entypeCOB,
	entypeCOS,
	entypeCOV,
	entypeCF,
	entypeFL,
	entypeOOR,
	entypeCET,
    //entype7 is deprecated
    entypeCOLS=8,                                                               //  ***1202 Begin
    entypeEX,
    entypeBR,
    entypeUR                                                                    // ***1202 End
};

enum notifytype {                                                               //  ***1202 Begin
	ntAlarm,
	ntEvent,
	ntAck_notification
};                                                                              //  ***1202 End

enum	alarmstates	{
	alarmstatenormal,
	alarmstatefault,
	alarmstateoffnormal,
	alarmstatehighlimit,
	alarmstatelowlimit,
	alarmstatelifesafetyalarm													//	***603
};		//																			***210 End

enum _reliability {																//	***1201
	no_fault_detected = 0,
	no_sensor,
	over_range,
	under_range,
	open_loop,
	shorted_loop,
	no_output,
	unreliabile_other,
	process_error,
	multi_state_fault,
	configuration_error,
	communication_failure=12,
	member_fault=13,
	monitored_object_fault=14,
	tripped=15
};																				//	***1201		

enum	vsblt {								//	Logging Type						***400 Begin
	loggingtypepolled,
	loggingtypecov,
	loggingtypetriggered
};											//										***400 End

typedef enum _vsbpolarity
{
	vsbpol_normal,
	vsbpol_reverse
} _vsbpolarity;

typedef enum _vsbnt {						//	Node Type							***600 Begin
	vsbnt_UNKNOWN,
	vsbnt_SYSTEM,
	vsbnt_NETWORK,
	vsbnt_DEVICE,
	vsbnt_ORGANIZATIONAL, 
	vsbnt_AREA, 
	vsbnt_EQUIPMENT, 
	vsbnt_POINT, 
	vsbnt_COLLECTION, 
	vsbnt_PROPERTY, 
	vsbnt_FUNCTIONAL, 
	vsbnt_OTHER
} vsbnt;									//										***600 End

enum	vsbeu {								//										***400 Begin
   vsbeu_square_meters = 0,
   vsbeu_square_feet = 1,
   vsbeu_milliamperes = 2,
   vsbeu_amperes = 3,
   vsbeu_ohms = 4,
   vsbeu_volts = 5,
   vsbeu_kilovolts = 6,
   vsbeu_megavolts = 7,
   vsbeu_volt_amperes = 8,
   vsbeu_kilovolt_amperes = 9,
   vsbeu_megavolt_amperes = 10,
   vsbeu_volt_amperes_reactive = 11,
   vsbeu_kilovolt_amperes_reactive = 12,
   vsbeu_megavolt_amperes_reactive = 13,
   vsbeu_degrees_phase = 14,
   vsbeu_power_factor = 15,
   vsbeu_joules = 16,
   vsbeu_kilojoules = 17,
   vsbeu_watt_hours = 18,
   vsbeu_kilowatt_hours = 19,
   vsbeu_btus = 20,
   vsbeu_therms = 21,
   vsbeu_ton_hours = 22,
   vsbeu_joules_per_kilogram_dry_air = 23,
   vsbeu_btus_per_pound_dry_air = 24,
   vsbeu_cycles_per_hour = 25,
   vsbeu_cycles_per_minute = 26,
   vsbeu_hertz = 27,
   vsbeu_grams_of_water_per_kilogram_dry_air = 28,
   vsbeu_percent_relative_humidity = 29,
   vsbeu_millimeters = 30,
   vsbeu_meters = 31,
   vsbeu_inches = 32,
   vsbeu_feet = 33,
   vsbeu_watts_per_square_foot = 34,
   vsbeu_watts_per_square_meter = 35,
   vsbeu_lumens = 36,
   vsbeu_luxes = 37,
   vsbeu_foot_candles = 38,
   vsbeu_kilograms = 39,
   vsbeu_pounds_mass = 40,
   vsbeu_tons = 41,
   vsbeu_kilograms_per_second = 42,
   vsbeu_kilograms_per_minute = 43,
   vsbeu_kilograms_per_hour = 44,
   vsbeu_pounds_mass_per_minute = 45,
   vsbeu_pounds_mass_per_hour = 46,
   vsbeu_watts = 47,
   vsbeu_kilowatts = 48,
   vsbeu_megawatts = 49,
   vsbeu_btus_per_hour = 50,
   vsbeu_horsepower = 51,
   vsbeu_tons_refrigeration = 52,
   vsbeu_pascals = 53,
   vsbeu_kilopascals = 54,
   vsbeu_bars = 55,
   vsbeu_pounds_force_per_square_inch = 56,
   vsbeu_centimeters_of_water = 57,
   vsbeu_inches_of_water = 58,
   vsbeu_millimeters_of_mercury = 59,
   vsbeu_centimeters_of_mercury = 60,
   vsbeu_inches_of_mercury = 61,
   vsbeu_degrees_Celsius = 62,
   vsbeu_degrees_Kelvin = 63,
   vsbeu_degrees_Fahrenheit = 64,
   vsbeu_degree_days_Celsius = 65,
   vsbeu_degree_days_Fahrenheit = 66,
   vsbeu_years = 67,
   vsbeu_months = 68,
   vsbeu_weeks = 69,
   vsbeu_days = 70,
   vsbeu_hours = 71,
   vsbeu_minutes = 72,
   vsbeu_seconds = 73,
   vsbeu_meters_per_second = 74,
   vsbeu_kilometers_per_hour = 75,
   vsbeu_feet_per_second = 76,
   vsbeu_feet_per_minute = 77,
   vsbeu_miles_per_hour = 78,
   vsbeu_cubic_feet = 79,
   vsbeu_cubic_meters = 80,
   vsbeu_imperial_gallons = 81,
   vsbeu_liters = 82,
   vsbeu_us_gallons = 83,
   vsbeu_cubic_feet_per_minute = 84,
   vsbeu_cubic_meters_per_second = 85,
   vsbeu_imperial_gallons_per_minute = 86,
   vsbeu_liters_per_second = 87,
   vsbeu_liters_per_minute = 88,
   vsbeu_us_gallons_per_minute = 89,
   vsbeu_degrees_angular = 90,
   vsbeu_degrees_Celsius_per_hour = 91,
   vsbeu_degrees_Celsius_per_minute = 92,
   vsbeu_degrees_Fahrenheit_per_hour = 93,
   vsbeu_degrees_Fahrenheit_per_minute = 94,
   vsbeu_no_units = 95,
   vsbeu_parts_per_million = 96,
   vsbeu_parts_per_billion = 97,
   vsbeu_percent = 98,
   vsbeu_percent_per_second = 99,
   vsbeu_per_minute = 100,
   vsbeu_per_second = 101,
   vsbeu_psi_per_degree_Fahrenheit = 102,
   vsbeu_radians = 103,
   vsbeu_revolutions_per_minute = 104,
   vsbeu_currency1 = 105,
   vsbeu_currency2 = 106,
   vsbeu_currency3 = 107,
   vsbeu_currency4 = 108,
   vsbeu_currency5 = 109,
   vsbeu_currency6 = 110,
   vsbeu_currency7 = 111,
   vsbeu_currency8 = 112,
   vsbeu_currency9 = 113,
   vsbeu_currency10 = 114,
   vsbeu_square_inches = 115,
   vsbeu_square_centimeters = 116,
   vsbeu_btus_per_pound = 117,
   vsbeu_centimeters = 118,
   vsbeu_pounds_mass_per_second = 119,
   vsbeu_delta_degrees_Fahrenheit = 120,
   vsbeu_delta_degrees_Kelvin = 121,
   vsbeu_kilohms = 122,
   vsbeu_megohms = 123,
   vsbeu_millivolts = 124,
   vsbeu_kilojoules_per_kilogram = 125,
   vsbeu_megajoules = 126,
   vsbeu_joules_per_degree_Kelvin = 127,
   vsbeu_joules_per_kilogram_degree_Kelvin = 128,
   vsbeu_kilohertz = 129,
   vsbeu_megahertz = 130,
   vsbeu_per_hour = 131,
   vsbeu_milliwatts = 132,
   vsbeu_hectopascals = 133,
   vsbeu_millibars = 134,
   vsbeu_cubic_meters_per_hour = 135,
   vsbeu_liters_per_hour = 136,
   vsbeu_kilowatt_hours_per_square_meter = 137,
   vsbeu_kilowatt_hours_per_square_foot = 138,
   vsbeu_megajoules_per_square_meter = 139,
   vsbeu_megajoules_per_square_foot = 140,
   vsbeu_watts_per_square_meter_degree_kelvin = 141,
   vsbeu_cubic_feet_per_second = 142,
   vsbeu_percent_obscuration_per_foot = 143,
   vsbeu_percent_obscuration_per_meter = 144,
   vsbeu_milliohms = 145,
   vsbeu_megawatt_hours = 146,
   vsbeu_kilo_btus = 147,
   vsbeu_mega_btus = 148,
   vsbeu_kilojoules_per_kilogram_dry_air = 149,
   vsbeu_megajoules_per_kilogram_dry_air = 150,
   vsbeu_kilojoules_per_degree_Kelvin = 151,
   vsbeu_megajoules_per_degree_Kelvin = 152,
   vsbeu_newton = 153,
   vsbeu_grams_per_second = 154,
   vsbeu_grams_per_minute = 155,
   vsbeu_tons_per_hour = 156,
   vsbeu_kilo_btus_per_hour = 157,
   vsbeu_hundredths_seconds = 158,
   vsbeu_milliseconds = 159,
   vsbeu_newton_meters = 160,
   vsbeu_millimeters_per_second = 161,
   vsbeu_millimeters_per_minute = 162,
   vsbeu_meters_per_minute = 163,
   vsbeu_meters_per_hour = 164,
   vsbeu_cubic_meters_per_minute = 165,
   vsbeu_meters_per_second_per_second = 166,
   vsbeu_amperes_per_meter = 167,
   vsbeu_amperes_per_square_meter = 168,
   vsbeu_ampere_square_meters = 169,
   vsbeu_farads = 170,
   vsbeu_henrys = 171,
   vsbeu_ohm_meters = 172,
   vsbeu_siemens = 173,
   vsbeu_siemens_per_meter = 174,
   vsbeu_teslas = 175,
   vsbeu_volts_per_degree_Kelvin = 176,
   vsbeu_volts_per_meter = 177,
   vsbeu_webers = 178,
   vsbeu_candelas = 179,
   vsbeu_candelas_per_square_meter = 180,
   vsbeu_degrees_Kelvin_per_hour = 181,
   vsbeu_degrees_Kelvin_per_minute = 182,
   vsbeu_joule_seconds = 183,
   vsbeu_radians_per_second = 184,
   vsbeu_square_meters_per_Newton = 185,
   vsbeu_kilograms_per_cubic_meter = 186,
   vsbeu_newton_seconds = 187,
   vsbeu_newtons_per_meter = 188,
   vsbeu_watts_per_meter_per_degree_Kelvin = 189,
   vsbeu_micro_siemens = 190,							//	135-2008h
   vsbeu_cubic_feet_per_hour = 191,
   vsbeu_us_gallons_per_hour = 192,
   vsbeu_kilometers = 193,								//	135-2008z
   vsbeu_micrometers = 194,
   vsbeu_grams = 195,
   vsbeu_milligrams = 196,
   vsbeu_milliliters = 197,
   vsbeu_millliters_per_second = 198,
   vsbeu_decibels = 199,
   vsbeu_decibels_millivolt = 200,
   vsbeu_decibels_volt = 201,
   vsbeu_millisiemens = 202,
   vsbeu_watt_hours_reactive = 203,
   vsbeu_kilowatt_hours_reactive = 204,
   vsbeu_megawatt_hours_reactive = 205,
   vsbeu_millimeters_of_water = 206,
   vsbeu_per_mile = 207,
   vsbeu_grams_per_gram = 208,
   vsbeu_kilograms_per_kilogram = 209,
   vsbeu_grams_per_kilogram = 210,
   vsbeu_milligrams_per_gram = 211,
   vsbeu_milligrams_per_kilogram = 212,
   vsbeu_grams_per_milliliter = 213,
   vsbeu_grams_per_liter = 214,
   vsbeu_milligrams_per_liter = 215,
   vsbeu_micrograms_per_liter = 216,
   vsbeu_grams_per_cubic_meter = 217,
   vsbeu_milligrams_per_cubic_meter = 218,
   vsbeu_micrograms_per_cubic_meter = 219,
   vsbeu_nanograms_per_cubic_meter = 220,
   vsbeu_grams_per_cubic_centimeter = 221,
   vsbeu_becquerels = 222,
   vsbeu_kilobecquerels = 223,
   vsbeu_megabecquerels = 224,
   vsbeu_gray = 225,
   vsbeu_milligray = 226,
   vsbeu_microgray = 227,
   vsbeu_sieverts = 228,
   vsbeu_millisieverts = 229,
   vsbeu_microsieverts = 230,
   vsbeu_microsieverts_per_hour = 231,
   vsbeu_decibels_a = 232,
   vsbeu_nephelometric_turbidity_unit = 233,
   vsbeu_pH = 234,
   vsbeu_grams_per_square_meter = 235,
   vsbeu_minutes_per_degree_kelvin = 236
};															//					***400 End

enum vsbrr_rangetypes
{	
	vsbrrByPosition,				//0
	vsbrrOldByTime,					//1 //deprecated
	vsbrrTimeRange,					//2	
	vsbrrBySequence,				//3
	vsbrrByTime,					//4
	vsbrrNoRange=0xFF				//0xFF
};

typedef enum _vsbrn_restartreasons												//	***1200 Begin
{
	vsbrnUnknown,
	vsbrnColdStart,
	vsbrnWarmStart,
	vsbrnPowerLost,
	vsbrnPoweredOff,
	vsbrnHWWatchDog,
	vsbrnSWWatchDog,
	vsbrnSuspended
} vsbrn_restartreasons;															//	***1200 End


#endif //__UENUMS_H_INCLUDED
