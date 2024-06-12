/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_XML_H__
#define __HTTPD_XML_H__

#ifdef XML_STANDALONE
#define XML_DECL \
	"<\?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"\?>\n"
#else
#define XML_DECL \
	"<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n"
#endif
#define XML_DECL_LEN            STRLEN( XML_DECL )

#define AMP                     '&'
#define LT                      '<'
#define GT                      '>'
#define APOS                    '\''
#define QUOT                    '\"'

#define LT_STR                  "<"
#define GT_STR                  ">"
#define LT_SLASH                "</"
#define SLASH_GT                "/>"
#define TAB_LT                  "\t<"
#define TAB_TAB_LT              "\t\t<"
#define GT_EOL                  ">\n"
#define SLASH_GT_EOL            "/>\n"
#define QUOTE_STR               "\""
#define SLASH_STR               "/"
#define COMMA_STR               ","
#define PERIOD_STR              "."
#define COLON_STR               ":"

#define IP_FORMAT               "IPV4_BIG_ENDIAN_U8"
#define IP_STATIC_FORMAT        "IPV4_BIG_ENDIAN_U8"
#define IP_LENGTH               4U
#define MAC_FORMAT              "MAC_ADDRESS"
#define MAC_LENGTH              6U
#define DUMMY_STRING            "********:********"

#define ONE                     "1"
#define ZERO                    "0"

#define XML_AMP                 "&amp;"
#define XML_LT                  "&lt;"
#define XML_GT                  "&gt;"
#define XML_APOS                "&apos;"
#define XML_QUOT                "&quot;"

#define CD_START                "<![CDATA["
#define CD_END                  "]]>"

#define TAG_START_CHAR          '<'
#define EMPTY_TAG_END_HINT_CHAR '/'
#define TAG_END_CHAR            '>'

#define INT_SUBSET_OPEN_CHAR    '['
#define INT_SUBSET_CLOSE_CHAR   ']'

#define SINGLE_QUOTE_CHAR       '\''
#define DOUBLE_QUOTE_CHAR       '\"'

#define PE_REFERENCE_OPEN_CHAR  '%'
#define PE_REFERENCE_CLOSE_CHAR ';'

#define COMMENT_START           "<!--"
#define COMMENT_END_HINT        "--"

#define XMLDECL_START           "<?xml"

#define PI_START                "<?"
#define PI_XMLDECL_END          "?>"

#define DTD_DOCTYPE             "<!DOCTYPE"
#define DTD_ELEMENT             "<!ELEMENT"
#define DTD_ATTLIST             "<!ATTLIST"
#define DTD_ENTITY              "<!ENTITY"
#define DTD_NOTATION            "<!NOTATION"

#define VERSION_STR             "version"
#define VERSION_PREFIX          "1."
#define ENCODING_STR            "encoding"
#define STANDALONE_STR          "standalone"
#define YES_STR                 "yes"
#define NO_STR                  "no"
#define SYSTEM_STR              "SYSTEM"
#define PUBLIC_STR              "PUBLIC"

#define UTF_8_BOM_BYTE_0        0xEFU
#define UTF_8_BOM_BYTE_1        0xBBU
#define UTF_8_BOM_BYTE_2        0xBFU

#define UTF_16_BOM_LSB          0xFFU
#define UTF_16_BOM_MSB          0xFEU

#define AMP_SUB_STR             "amp"
#define LT_SUB_STR              "lt"
#define GT_SUB_STR              "gt"
#define APOS_SUB_STR            "apos"
#define QUOT_STR                "quot"

#define PID_STR                 "pid"


/******************************************************************************/
#define XML_END_PREFIX_AND_NAME "\">\n\t<Name>"
/******************************************************************************/


/******************************************************************************/
#define ATTRIB_XMLNS_XLINK " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
#define ATTRIB_XLINK_HREF  " xlink:href=\""
#define ATTRIB_PID         " pid=\""

/******************************************************************************/
#define XML_RS_START "<RS"
/******************************************************************************/

#if defined ( REST_I18N_LANGUAGE )
#define XML_RS_ROOT \
	"xlink:href=\"/rs\">\n" \
	"\t<DeviceList xlink:href=\"/rs/device\"/>\n" \
	"\t<ParamList xlink:href=\"/rs/param\"/>\n" \
	"\t<AssyList xlink:href=\"/rs/assy\"/>\n" \
	"\t<FwList xlink:href=\"/rs/fw\"/>\n" \
	"\t<UsersList xlink:href=\"/rs/users\"/>\n" \
	"\t<LogList xlink:href=\"/rs/log\"/>\n" \
	"\t<Langlist xlink:href=\"/rs/lang\"/>\n" \
	/* "\t<StructList xlink:href=\"/rs/struct\"/>\n" */ \
	"</RS>\n"
#else
#define XML_RS_ROOT \
	"xlink:href=\"/rs\">\n" \
	"\t<DeviceList xlink:href=\"/rs/device\"/>\n" \
	"\t<ParamList xlink:href=\"/rs/param\"/>\n" \
	"\t<AssyList xlink:href=\"/rs/assy\"/>\n" \
	"\t<FwList xlink:href=\"/rs/fw\"/>\n" \
	"\t<UsersList xlink:href=\"/rs/users\"/>\n" \
	"\t<LogList xlink:href=\"/rs/log\"/>\n" \
	/* "\t<StructList xlink:href=\"/rs/struct\"/>\n" */ \
	"</RS>\n"
#endif	// REST_I18N_LANGUAGE


#define XML_RS_USERS \
	"<Users " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/users\">\n" \
	"\t<Accounts xlink:href=\"/rs/users/accounts\"/>\n" \
	"\t<Roles xlink:href=\"/rs/users/roles\"/>\n" \
	"\t<Session xlink:href=\"/rs/users/session\"/>\n" \
	"</Users>\n"
#define USER_ACCOUNT_PREFIX \
	"<UserAccountList " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/users/accounts\">\n"

#define USER_ACCOUNT_ID_PREFIX      \
	"<UserAccount user_id=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/users/accounts/\bu\">\n\t"
#define USER_ACCOUNT_ID_SUFFIX      "</UserAccount>"
#define FULL_NAME_PREFIX            "<FullName>"
#define FULL_NAME_SUFFIX            "</FullName>\n\t"
#define USER_NAME_PREFIX            "<UserName>"
#define USER_NAME_SUFFIX            "</UserName>\n\t"
#define PWD_PREFIX                  "<Pwd>"
#define PWD_SUFFIX                  "</Pwd>\n\t"
#define PWD_COMPLEXITY_PREFIX       "<PwdComplexity>"
#define PWD_COMPLEXITY_SUFFIX       "</PwdComplexity>\n\t"
#define PWD_TOUTENABLE_PREFIX       "<PwdTimeoutEnable>"
#define PWD_TOUTENABLE_SUFFIX       "</PwdTimeoutEnable>\n\t"
#define PWD_TOUTDAYS_PREFIX         "<PwdTimeoutDays>"
#define PWD_TOUTDAYS_SUFFIX         "</PwdTimeoutDays>\n\t"
#define PWD_SET_EPOCH_TIME_PREFIX   "<PwdSetEpochTime>"
#define PWD_SET_EPOCH_TIME_SUFFIX   "</PwdSetEpochTime>\n\t"
#define LAST_LOGIN_TIME_PREFIX      "<LastLoginTime>"
#define LAST_LOGIN_TIME_SUFFIX      "</LastLoginTime>\n\t"
#define FAILED_ATTEMPT_COUNT_PREFIX "<FailedAttemptCount>"
#define FAILED_ATTEMPT_COUNT_SUFFIX "</FailedAttemptCount>\n\t"
#define TRIGGER_CHANGE_PWD_PREFIX   "<ChangePwd>"
#define TRIGGER_CHANGE_PWD_SUFFIX   "</ChangePwd>\n"
#define ROLE_TAG                    "<Role xlink:href=\"/rs/users/roles/\bu\">\bs</Role>\n\t"

#define USER_ACCOUNT_SUFFIX         "</UserAccountList>\n"
#define TOTAL_USERS_PREFIX          "\t<TotalUsers>"
#define TOTAL_USERS_SUFFIX          "</TotalUsers>\n"
#define MAX_USERS_PREFIX            "\t<MaxAllowedUsers>"
#define MAX_USERS_SUFFIX            "</MaxAllowedUsers>\n"
#define MAX_ALLOWED_USERS_PREFIX    "<MaxAllowedUsers>"
#define MAX_ALLOWED_USERS_SUFFIX    "</MaxAllowedUsers>\n"
#define USERNAME_SIZE_LIMIT_PREFIX  "\t<UserNameSizeLimit>"
#define USERNAME_SIZE_LIMIT_SUFFIX  "</UserNameSizeLimit>\n"
#define FULLNAME_SIZE_LIMIT_PREFIX  "\t<FullNameSizeLimit>"
#define FULLNAME_SIZE_LIMIT_SUFFIX  "</FullNameSizeLimit>\n"
#define USER_NAME_TAG               "\t<User user_name=\"\bs\" xlink:href=\"/rs/users/accounts/\bu\"/>\n"

#define ROLES_PREFIX \
	"<UserRoles " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/users/roles\">\n"
#define ROLES_SUFFIX  "</UserRoles>\n"
#define ROLE_NAME_TAG "\t<Role role_name=\"\bs\" xlink:href=\"/rs/users/roles/\bu\"/>\n"
#define USER_ROLE_PREFIX \
	"<UserRole role_id=\"\bu\" " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/users/roles/\bu\">"

#define XML_RS_USERS_HELP \
	"<UsersHelp " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/users/help\">\n" \
	"\t<FullNameLimit>32</FullNameLimit>\n" \
	"\t<UserNameLimit>20</UserNameLimit>\n" \
	"\t<PwdLimit>20</PwdLimit>\n" \
	"\t<PwdComplexityLevels>\n" \
	"\t\t<Complexity Level=\"0\" MinLen=\"6\" Alpha=\"0\" UpperCase=\"0\" LowerCase=\"0\" Num=\"0\" SplChar=\"0\" />\n" \
	"\t\t<Complexity Level=\"1\" MinLen=\"8\" Alpha=\"1\" UpperCase=\"0\" LowerCase=\"0\" Num=\"1\" SplChar=\"0\" />\n" \
	"\t\t<Complexity Level=\"2\" MinLen=\"12\" Alpha=\"1\" UpperCase=\"1\" LowerCase=\"0\" Num=\"1\" SplChar=\"1\" />\n" \
	"\t\t<Complexity Level=\"3\" MinLen=\"16\" Alpha=\"2\" UpperCase=\"1\" LowerCase=\"1\" Num=\"1\" SplChar=\"2\" />\n" \
	"\t</PwdComplexityLevels>\n" \
	"</UsersHelp>"


/******************************************************************************/

/******************************************************************************/
#define XML_DEVICE_ALL_PREFIX \
	"<DeviceList " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/device\">\n"
#define XML_DEVICE_ALL_SUFFIX      "\n</DeviceList>"
#define XML_DEVICE_LINE_SEGMENT0   "\t<Device dev_id=\""
#define XML_DEVICE_LINE_SEGMENT1   "\" name="
#define XML_DEVICE_LINE_SEGMENT2   " xlink:href=\"/rs/device/"
#define XML_DEVICE_LINE_SEGMENT3   "\"/>"
/******************************************************************************/
#define XML_PRODUCT_PREFIX_XLINK   "<Product " "xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/fw\">"
#define XML_PROCESSOR_PREFIX_XLINK "<Processor xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/fw/"
#define XML_IMAGE_PREFIX_XLINK     "<Image xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/fw/"

#define XML_PROCESSOR_XLINK        "\n\t\t<Processor xlink:href=\"/rs/fw/"
#define XML_IMAGE_XLINK            "\n\t\t<Image xlink:href=\"/rs/fw/"
#define XML_BIN_XLINK              "\n\t<Bin xlink:href=\"/rs/fw/"
#define XML_VALID_XLINK            "\n\t<Valid xlink:href=\"/rs/fw/"
#define XML_STATUS_XLINK           "\n\t<Status xlink:href=\"/rs/fw/"
#define XML_COMMIT_XLINK           "\n\t<Commit xlink:href=\"/rs/fw/"

#define XML_GUID_START             "\n\t<Guid>"
#define XML_GUID_END               "</Guid>"
#define XML_PROD_GUID_START        "\n\t<ProductGuid>"
#define XML_PROD_GUID_END          "</ProductGuid>"
#define XML_SERIAL_NUM_START       "\n\t<SerialNum>"
#define XML_SERIAL_NUM_END         "</SerialNum>"
#define XML_NAME_START             "\n\t<Name>"
#define XML_NAME_END               "</Name>"
#define XML_PRODUCT_CODE_START     "\n\t<ProductCode>"
#define XML_PRODUCT_CODE_END       "</ProductCode>"
#define XML_VERSION_START          "\n\t<Ver>"
#define XML_VERSION_END            "</Ver>"
#define XML_COMPATIBILITY_START    "\n\t<Compatibility>"
#define XML_COMPATIBILITY_END      "</Compatibility>"


/******************************************************************************/
#define XML_DEVICE_ROOT_PREFIX_SEGMENT0 \
	"<Device " \
	"xmlns:xlink=\"http://www.w3.org/1999/xlink\" dev_id=\""
#define XML_DEVICE_ROOT_PREFIX_SEGMENT1 "\" xlink:href=\"/rs/device/"
#define XML_DEVICE_ROOT_PREFIX_SEGMENT2 "\">\n"
#define XML_DEVICE_ROOT_SUFFIX          "</Device>\n"
/******************************************************************************/
// "<FirmwareList "\"xmlns:xlink=\"http://www.w3.org/1999/xlink\" Firmware List=\""
/******************************************************************************/
#define XML_DEVICE_FW_PREFIX_SEGMENT \
	"<FirmwareList xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/fw\">\n" \
	"\t<ApplicationFirmware xlink:href=\"/fw/app\"/>\n" \
	"\t<WebFirmware xlink:href=\"/fw/www\"/>\n"
#define XML_DEVICE_HOST_PREFIX_SEGMENT \
	"\t<HostFirmware xlink:href=\"/fw/host\"/>\n"
#define XML_DEVICE_FW_SUFIX_SEGMENT "</FirmwareList>\n"
/******************************************************************************/

/******************************************************************************/
// "<Firmware Informwation "\"xmlns:xlink=\"http://www.w3.org/1999/xlink\" Firmware Informwation=\""
/******************************************************************************/
#define XML_DEVICE_FWINF_PREFIX_SEGMENT0 \
	"<FirmwareInformation xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/fw"
#define XML_DEVICE_FWINF_PREFIX_SEGMENT1A "Version =\""
#define XML_DEVICE_FWINF_PREFIX_SEGMENT2  " xlink:href=\"/fw"
#define XML_DEVICE_FWINF_PREFIX_SEGMENT3  "/version\"/>\n"

#define XML_DEVICE_FWINF_PREFIX_SEGMENT3A "Date =\""
#define XML_DEVICE_FWINF_PREFIX_SEGMENT3B "Time =\""

#define XML_DEVICE_FWINF_PREFIX_SEGMENT4  " xlink:href=\"/fw"
#define XML_DEVICE_FWINF_PREFIX_SEGMENT5  "/datetime\"/>\n"

#define XML_DEVICE_FWINF_PREFIX_SEGMENT5A "User =\""
#define XML_DEVICE_FWINF_PREFIX_SEGMENT6  " xlink:href=\"/fw"
#define XML_DEVICE_FWINF_PREFIX_SEGMENT7  "/user\"/>\n"

#define XML_DEVICE_FWINF_SUFIX_SEGMENT    "</FirmwareInformation>\n"


/******************************************************************************/
#define XML_STRUCT_ALL_PREFIX \
	"<StructList xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/struct\">\n"
#define XML_STRUCT_ALL_SUFFIX \
	"</StructList>\n"
#define XML_NAME_PREFIX     "\t<Name>"
#define XML_NAME_SUFFIX     "</Name>\n"
#define XML_STRUCT_SEGMENT1 "\t<Struct sid=\""
#define XML_STRUCT_SEGMENT2 "\" name=\""
#define XML_STRUCT_SEGMENT3 "\" xlink:href=\"/rs/struct/"
#define XML_STRUCT_SEGMENT4 "\"/>\n"
/******************************************************************************/

/******************************************************************************/
#define XML_STRUCT_ROOT_PREFIX \
	"<StructItem xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/struct/"
#define XML_STRUCT_ROOT_END_PREFIX XML_END_PREFIX_AND_NAME
#define XML_STRUCT_ROOT_SUFFIX \
	"</StructItem>\n"
#define XML_STURCT_DISPNAME_PREFIX "\t<DisplayName>"
#define XML_STURCT_DISPNAME_SUFFIX "</DisplayName>\n"
#define XML_ITEM_LIST_PREFIX       "\t<ItemList>\n"
#define XML_ITEM_LIST_SUFFIX       "\t</ItemList>\n"
#define XML_STRUCT_P_SEGMENT1      "\t\t<Param pid=\""
// #define XML_STRUCT_P_SEGMENT2 XML_STRUCT_SEGMENT2
#define XML_STRUCT_P_SEGMENT3      "\" xlink:href=\"/rs/param/"
// #define XML_STRUCT_P_SEGMENT4 XML_STRUCT_SEGMENT4
#define XML_STRUCT_S_SEGMENT1      "\t\t<Struct sid=\""
// #define XML_STRUCT_S_SEGMENT2 XML_STRUCT_SEGMENT2
#define XML_STRUCT_S_SEGMENT3      "\" xlink:href=\"/rs/struct/"
// #define XML_STRUCT_S_SEGMENT4 XML_STRUCT_SEGMENT4
/******************************************************************************/


/******************************************************************************/
#define XML_PARAM_ROOT_PREFIX1 \
	"<Param pid=\""
#define XML_PARAM_ROOT_PREFIX2 \
	"\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" " \
	"xlink:href=\"/rs/param/"
#define XML_PARAM_ROOT_END_PREFIX XML_END_PREFIX_AND_NAME

/******************************************************************************/

/******************************************************************************/
#define XML_PARAM_ALL_PREFIX \
	"<ParamList xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/param\">\n"
#define XML_PARAM_ALL_SUFFIX \
	"</ParamList>\n"

#define XML_PARAM_ALL_LINE_SEG1 \
	"\t<Param pid=\""
#define XML_PARAM_ALL_LINE_SEG2 \
	"\" name=\""
#define XML_PARAM_ALL_LINE_SEG3 \
	"\" xlink:href=\"/rs/param/"
#define XML_PARAM_ALL_LINE_SEG4 \
	"\"/>\n"

#define XML_PARAM_ALL_LINE_SIZE_BARE \
	( STRLEN( XML_PARAM_ALL_LINE_SEG1 ) \
	  + STRLEN( XML_PARAM_ALL_LINE_SEG2 ) \
	  + STRLEN( XML_PARAM_ALL_LINE_SEG3 ) \
	  + STRLEN( XML_PARAM_ALL_LINE_SEG4 ) )

/******************************************************************************/

/******************************************************************************/
#define XML_ASSY_ALL_PREFIX \
	"<AssyList xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/assy\">\n"
#define XML_ASSY_ALL_ASSY_SLOTS_TOTAL_PREFIX "\t<AssySlotsTotal>"
#define XML_ASSY_ALL_ASSY_SLOTS_TOTAL_SUFFIX "</AssySlotsTotal>\n"
#define XML_ASSY_ALL_ASSY_SLOTS_LEFT_PREFIX  "\t<AssySlotsLeft>"
#define XML_ASSY_ALL_ASSY_SLOTS_LEFT_SUFFIX  "</AssySlotsLeft>\n"

#define XML_ASSY_PARAM_ALL_PREFIX \
	"<Assy xmlns:xlink=\"http://www.w3.org/1999/xlink\" aid=\""
#define XML_ASSY_ALL_SUFFIX \
	"</AssyList>\n"

#define XML_ASSY_ALL_LINE_SEG1 \
	"\t<Assy aid=\""
#define XML_ASSY_ALL_LINE_SEG2 \
	"\" name=\""
#define XML_ASSY_ALL_LINE_SEG3 \
	"\" del_supp=\""
#define XML_ASSY_ALL_LINE_SEG4 \
	"\" xlink:href=\"/rs/assy/"
#define XML_ASSY_ALL_LINE_SEG5 \
	"\"/>\n"

#define XML_ASSY_ALL_LINE_SIZE_BARE \
	( STRLEN( XML_ASSY_ALL_LINE_SEG1 ) \
	  + STRLEN( XML_ASSY_ALL_LINE_SEG2 ) \
	  + STRLEN( XML_ASSY_ALL_LINE_SEG3 ) \
	  + STRLEN( XML_ASSY_ALL_LINE_SEG4 ) \
	  + STRLEN( XML_ASSY_ALL_LINE_SEG5 ) )

#define XML_ASSY_CLOSE_TAG "</Assy>\n"
/******************************************************************************/

/******************************************************************************/
#define XML_VACCESS " v=\""
#define XML_RACCESS " r=\""
#define XML_WACCESS " w=\""
/******************************************************************************/


/******************************************************************************/
#define XML_ASSY            "Assy"
#define XML_ASSY_AID        " aid=\""
#define XML_ASSY_NAME       " name=\""
#define XML_ASSY_DEL_SUPP   " del_supp=\""
#define XML_ASSY_XLINK      " xlink:href=\"/rs/assy/"
#define XML_ASSY_ZERO       "0\""
#define XML_ASSY_ONE        "1\""
#define XML_LIST_RO         "\" list_ro=\""
#define XML_OPEN_SLOTS      " open_slots=\""
#define XML_ASSY_PARAM_NAME " name=\""
#define TAG_VALUE_LIST      "ValueList"
#define QUOTE_XML_WACCESS   "\" w=\""
#define QUOTE_GT_EOL        "\">\n"
/******************************************************************************/

#define SESSION_RESP_PREFIX "<SessionResp>\n"
#define SESSION_RESP_SUFFIX "</SessionResp>"
#define SESSION_ID_PREFIX   "\t<SessionID>"
#define SESSION_ID_SUFFIX   "</SessionID>\n"
#define SESSION_PREFIX      "<Session>\n"
#define SESSION_SUFFIX      "</Session>"
#define WAIT_PREFIX         "\t<Wait>"
#define WAIT_SUFFIX         "</Wait>\n"


/******************************************************************************/
#define XML_RS_LOG \
	"<LogList xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/log\">\n"

#define XML_LOG_DETAILS_SEG1        "\t<Log lid=\"\bu\" name=\"\bs\" pid=\"\bu\" xlink:href=\"/rs/log/\bu\"/>\n"
#define XML_LOG_DETAILS_SEG5        "</LogList>\n"

#define XML_LOG_X_DETAIL_PREFIX     \
	"<LogId lid=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/log/\bu\">\n"
#define XML_LOG_X_NAME              "\t<Name>\bs</Name>\n"
#define XML_LOG_X_DESCRIPTION       "\t<Description>\"\bs\"</Description>\n"
#define XML_LOG_X_MAX_ENTRIES       "\t<MaxEntries>\bu</MaxEntries>\n"
#define XML_LOG_X_CURRENT_ENTRIES   "\t<CurrentEntries>\bu</CurrentEntries>\n"
#define XML_LOG_X_DETAIL_SUFFIX     "</LogId>\n"

#define XML_LOG_X_GET_DETAIL_PREFIX \
	"<LogTail lid=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/log/\bu"
#define XML_LOG_X_GET_DETAIL_TAIL   "/tail"
#define XML_LOG_X_GET_DETAIL_END    "\">\n"
#define XML_LOG_X_NEXT_EID          "\t<NextEID>\bu</NextEID>\n"
#define XML_LOG_X_REM_ENTRIES       "\t<RemEntries>\bu</RemEntries>\n"
#define XML_LOG_X_ENTRY_PREFIX      "\t<Entry eid=\"\bu\">\n"
#define XML_LOG_X_ENTRY             "\t\t<e>\bs</e>\n"
#define XML_LOG_X_ENTRY_SUFFIX      "\t</Entry>\n"
#define XML_LOG_X_GET_DETAIL_SUFFIX "</LogTail>\n"
#define XML_LOG_X_DELETE_SUFFIX     \
	"<DeleteLog xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/log/\bu/tail\">\n"
#define XML_LOG_X_DELETE_PREFIX     "</DeleteLog>\n"
#define XML_LOG_X_DELETE_WAIT       "\t<OpTime>\bu</OpTime>\n"

/******************************************************************************/
#define XML_ENUM_PREFIX_1     "<Enum pid=\""
#define XML_ENUM_PREFIX_2     "\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/param/"
#define XML_ENUM_PREFIX_3     "/enum\">\n"
#define XML_ENUM_SUFFIX       "</Enum>\n"
#define XML_ENUM_TOTAL_PREFIX "<Total>"
#define XML_ENUM_TOTAL_SUFFIX "</Total>\n"
#define XML_ENUM_VALUE_1      "<Val xlink:href=\"/rs/param/"
#define XML_ENUM_VALUE_2      "/enum/"
#define XML_ENUM_VALUE_3      "\">"
#define XML_ENUM_VALUE_4      "</Val>\n"
// #define XML_ENUM_VALUE "\t<Val enum=\"\bu\" xlink:href=\"/rs/param/\bu/enum/\bu\"/>\n"

#define XML_ENUM_DESCRIPTION_PREFIX \
	"<Val pid=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/param/\bu/enum/\bu\">\n"
#define XML_ENUM_DESCRIPTION_SUFFIX "</Val>\n"
#define XML_ENUM_DESCRIPTION        "\t<Desc>\bs</Desc>\n"

/******************************************************************************/
#define XML_BITFIELD_PREFIX             \
	"<Bitfield pid=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/param/\bu/bitfield\">\n"
#define XML_BITFIELD_SUFFIX             "</Bitfield>\n"
#define XML_BITFIELD_TOTAL              "\t<Total>\bu</Total>\n"
#define XML_BITFIELD_BIT_VALUE          "\t<Bit xlink:href=\"/rs/param/\bu/bitfield/\bu\">\bu</Bit>\n"

#define XML_BITFIELD_DESCRIPTION_PREFIX \
	"<Bit pid=\"\bu\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/param/\bu/bitfield/\bu\">\n"
#define XML_BITFIELD_DESCRIPTION_SUFFIX "</Bit>\n"
#define XML_BITFIELD_DESCRIPTION        "\t<Desc>\bs</Desc>\n"
/******************************************************************************/

/******************************************************************************/
#ifdef REST_I18N_LANGUAGE
#define XML_LANG_ALL_PREFIX \
	"<LangList xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/lang\">\n"
#define XML_LANG_TOTAL_PREFIX          "\t<Total>"
#define XML_LANG_TOTAL_SUFFIX          "</Total>\n"

#define XML_LANG_SETTING_PREFIX        "\t<Lang_Select_Source>"
#define XML_LANG_SETTING_SUFFIX        "</Lang_Select_Source>\n"

#define XML_LANG_COMMON_SETTING_PREFIX "\t<Comm_Pref_Lang>"
#define XML_LANG_COMMON_SETTING_SUFFIX "</Comm_Pref_Lang>"

#define XML_LANG_ALL_SUFFIX \
	"</LangList>\n"

#define XML_LANG_ALL_LINE_SEG1 \
	"\t<Lang Name=\""
#define XML_LANG_ALL_LINE_SEG2 \
	"\" Code=\""
#define XML_LANG_ALL_LINE_SEG3 \
	"\" xlink:href=\"/rs/lang/"
#define XML_LANG_ALL_LINE_SEG4 \
	"\"/>\n"
#define XML_LANG_ALL_LINE_SEG5 \
	"/file"
#define XML_LANG_ALL_LINE_SEG6 \
	"\t<JSON Name=\""
#define XML_LANG_ALL_LINE_SEG7 \
	".json"

#define XML_LANG_CODE_PREFIX_SEG1 \
	"<Lang xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"/rs/lang/"
#define XML_LANG_CODE_PREFIX_SEG2 \
	"\">\n"
#define XML_LANG_CODE_SUFFIX \
	"</Lang>\n"

#define XML_LANG_VERSION_PREFIX       "\t<Version>"
#define XML_LANG_VERSION_SUFFIX       "</Version>\n"

#define XML_LANG_SPEC_VERSION_PREFIX  "\t<SpecVersion>"
#define XML_LANG_SPEC_VERSION_SUFFIX  "</SpecVersion>\n"

#define XML_LANG_FORMAT_PREFIX        "\t<Format>"
#define XML_LANG_FORMAT_SUFFIX        "</Format>\n"

#define XML_LANG_COMPRESSION_PREFIX   "\t<Compression>"
#define XML_LANG_COMPRESSION_SUFFIX   "</Compression>\n"

#define XML_LANG_COMPATIBILITY_PREFIX "\t<Compatibility>"
#define XML_LANG_COMPATIBILITY_SUFFIX "</Compatibility>\n"
#endif	// REST_I18N_LANGUAGE
/******************************************************************************/

#endif	/* __HTTPD_XML_H__ */



