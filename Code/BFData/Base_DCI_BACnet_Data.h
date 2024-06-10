#ifndef BASE_DCI_BACNET_DATA_H
    #define BASE_DCI_BACNET_DATA_H

#include "Includes.h"
#include "core.h"
#include "DCI_Data.h"

#define BACNET_TOTAL_CLASSES    5U
#define TOTAL_AI 2U
#define TOTAL_AV 4U
#define TOTAL_BV 14U
#define TOTAL_DEV 8U
#define TOTAL_MSV 2U
#define MSV0_STATES  3U
#define MSV1_STATES  3U

extern uint16_t MSV_Object_State_Count[TOTAL_MSV];

typedef uint8_t BACNET_ID;


struct MSV_length_to_strings
{
    frString msv;
};

struct BACnet_AI_to_DCID
{
    BACNET_ID bacnet_id;
    DCI_ID_TD dcid;
    uint8_t accessType;
    uint16_t units;
    uint16_t scale;
    float32_t cov_increment;
    frString name;
    frString desc;
};

struct BACnet_AV_to_DCID
{
    BACNET_ID bacnet_id;
    DCI_ID_TD dcid;
    uint8_t accessType;
    uint16_t units;
    uint16_t scale;
    float32_t cov_increment;
    frString name;
    frString desc;
};

struct BACnet_BV_to_DCID
{
    BACNET_ID bacnet_id;
    DCI_ID_TD dcid;
    uint8_t bitNumber;
    uint8_t accessType;
    frString name;
    frString desc;
    frString inactive_text;
    frString active_text;
};

struct BACnet_DEV_to_DCID
{
    BACNET_ID bacnet_id;
    DCI_ID_TD dcid;
};

struct BACnet_MSV_to_DCID
{
    BACNET_ID bacnet_id;
    DCI_ID_TD dcid;
    uint8_t accessType;
    uint16_t units;
    uint16_t scale;
    frString name;
    frString desc;
    const MSV_length_to_strings *MSV_len_to_Obj;
};

extern const BACnet_AI_to_DCID BACnet_AI_to_DCID_map[TOTAL_AI];
extern const BACnet_AV_to_DCID BACnet_AV_to_DCID_map[TOTAL_AV];
extern const BACnet_BV_to_DCID BACnet_BV_to_DCID_map[TOTAL_BV];
extern const BACnet_DEV_to_DCID BACnet_DEV_to_DCID_map[TOTAL_DEV];
extern const BACnet_MSV_to_DCID BACnet_MSV_to_DCID_map[TOTAL_MSV];
extern const struct MSV_length_to_strings MSV0_len_to_State_Txt[MSV0_STATES];
extern const struct MSV_length_to_strings MSV1_len_to_State_Txt[MSV1_STATES];
     
typedef struct BACNET_CLASS_ST_TD
{
    BACnet_AI_to_DCID const *BACnet_AI_to_DCID_map;
    BACnet_AV_to_DCID const *BACnet_AV_to_DCID_map;
    BACnet_BV_to_DCID const *BACnet_BV_to_DCID_map;
    BACnet_DEV_to_DCID const *BACnet_DEV_to_DCID_map;
    BACnet_MSV_to_DCID const *BACnet_MSV_to_DCID_map;
} BACNET_CLASS_ST_TD;
     
typedef struct BACNET_TARGET_INFO_ST_TD
{
    uint16_t total_classes;
    BACNET_CLASS_ST_TD const* class_list;
} BACNET_TARGET_INFO_ST_TD;
     
extern const BACNET_TARGET_INFO_ST_TD bacnet_dci_data_target_info;
#endif
