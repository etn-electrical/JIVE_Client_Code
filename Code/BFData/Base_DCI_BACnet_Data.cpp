/**
*****************************************************************************************
*   @file
*
*   @brief Provides the BACnet DCI map.
*
*   @details This cross list is used to translate BAcnet instances to Parameter ID
*
*   @version
*   C++ Style Guide Version 1.0
*   Last Modified Date: 09-11-2021
*   @copyright 2013 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/

#include "Base_DCI_BACnet_Data.h"

uint16_t MSV_Object_State_Count[TOTAL_MSV]=
{
    MSV0_STATES,
    MSV1_STATES
};

const struct MSV_length_to_strings MSV0_len_to_State_Txt[MSV0_STATES] = 
{
    3U,64U,0U,const_cast<char*>("OFF"),
    2U,64U,0U,const_cast<char*>("ON"),
    14U,64U,0U,const_cast<char*>("High Impedance"),
};  
     
const struct MSV_length_to_strings MSV1_len_to_State_Txt[MSV1_STATES] = 
{
    7U,64U,0U,const_cast<char*>("Fault 1"),
    7U,64U,0U,const_cast<char*>("Fault 2"),
    7U,64U,0U,const_cast<char*>("Fault 3"),
};  
     
const BACnet_AI_to_DCID BACnet_AI_to_DCID_map[TOTAL_AI] =   
{   
    {    0U                                   /*Instance*/
         ,DCI_ANALOG_INPUT_0_DCID              /* Analog Input 0*/
         ,0U                                    /*Access Type*/
         ,27U                                   /*Unit*/
         ,100U                                  /*Scale*/
         ,40U                                  /*COV Increament*/
         ,14U,64U,0U,const_cast<char*>("Analog Input 0")              /*Name*/
         ,14U,64U,0U,const_cast<char*>("Analog Input 0")               /*Description*/
    }, 
    {    1U                                   /*Instance*/
         ,DCI_ANALOG_INPUT_1_DCID              /* Analog Input 1*/
         ,0U                                    /*Access Type*/
         ,27U                                   /*Unit*/
         ,100U                                  /*Scale*/
         ,40U                                  /*COV Increament*/
         ,14U,64U,0U,const_cast<char*>("Analog Input 1")              /*Name*/
         ,14U,64U,0U,const_cast<char*>("Analog Input 1")               /*Description*/
    } 
};  
     
const BACnet_AV_to_DCID BACnet_AV_to_DCID_map[TOTAL_AV] =   
{   
    {    0U                                   /*Instance*/
         ,DCI_ANALOG_VALUE_0_DCID              /* Analog Value 0*/
         ,2U                                    /*Access Type*/
         ,98U                                   /*Unit*/
         ,100U                                  /*Scale*/
         ,10U                                  /*COV Increament*/
         ,14U,64U,0U,const_cast<char*>("Analog Value 0")              /*Name*/
         ,14U,64U,0U,const_cast<char*>("Analog Value 0")               /*Description*/
    }, 
    {    1U                                   /*Instance*/
         ,DCI_ANALOG_VALUE_1_DCID              /* Analog Value 1*/
         ,1U                                    /*Access Type*/
         ,27U                                   /*Unit*/
         ,100U                                  /*Scale*/
         ,40U                                  /*COV Increament*/
         ,14U,64U,0U,const_cast<char*>("Analog Value 1")              /*Name*/
         ,14U,64U,0U,const_cast<char*>("Analog Value 1")               /*Description*/
    }, 
    {    2U                                   /*Instance*/
         ,DCI_AV_ANY_PARAM_NUM_DCID              /* AnyParam ID*/
         ,1U                                    /*Access Type*/
         ,95U                                   /*Unit*/
         ,1U                                  /*Scale*/
         ,10U                                  /*COV Increament*/
         ,11U,64U,0U,const_cast<char*>("AnyParam ID")              /*Name*/
         ,11U,64U,0U,const_cast<char*>("AnyParam ID")               /*Description*/
    }, 
    {    3U                                   /*Instance*/
         ,DCI_AV_ANY_PARAM_VAL_DCID              /* AnyParam Value*/
         ,1U                                    /*Access Type*/
         ,95U                                   /*Unit*/
         ,1U                                  /*Scale*/
         ,10U                                  /*COV Increament*/
         ,14U,64U,0U,const_cast<char*>("AnyParam Value")              /*Name*/
         ,14U,64U,0U,const_cast<char*>("AnyParam Value")               /*Description*/
    } 
};  
     
const BACnet_BV_to_DCID BACnet_BV_to_DCID_map[TOTAL_BV] =   
{   
    {    0U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,0U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 0")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 0")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    1U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,1U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 1")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 1")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    2U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,2U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 2")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 2")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    3U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,3U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 3")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 3")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    4U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,4U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 4")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 4")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    5U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,5U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 5")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 5")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    6U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,6U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 6")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 6")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    7U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_WORD_DCID              /* Digital Input word*/
         ,7U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 7")              /*Name*/
         ,24U,64U,0U,const_cast<char*>("Digital Input word bit 7")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    8U                                   /*Instance*/
         ,DCI_DIGITAL_OUTPUT_WORD_DCID              /* Digital Output word*/
         ,0U                                         /*Bit Number*/
         ,2U                                    /*Access Type*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 0")              /*Name*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 0")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    9U                                   /*Instance*/
         ,DCI_DIGITAL_OUTPUT_WORD_DCID              /* Digital Output word*/
         ,1U                                         /*Bit Number*/
         ,1U                                    /*Access Type*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 1")              /*Name*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 1")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    10U                                   /*Instance*/
         ,DCI_DIGITAL_OUTPUT_WORD_DCID              /* Digital Output word*/
         ,2U                                         /*Bit Number*/
         ,1U                                    /*Access Type*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 2")              /*Name*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 2")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    11U                                   /*Instance*/
         ,DCI_DIGITAL_OUTPUT_WORD_DCID              /* Digital Output word*/
         ,3U                                         /*Bit Number*/
         ,1U                                    /*Access Type*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 3")              /*Name*/
         ,25U,64U,0U,const_cast<char*>("Digital Output word bit 3")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    12U                                   /*Instance*/
         ,DCI_DIGITAL_INPUT_BIT_DCID              /* Digital Input bit*/
         ,0U                                         /*Bit Number*/
         ,0U                                    /*Access Type*/
         ,17U,64U,0U,const_cast<char*>("Digital Input bit")              /*Name*/
         ,17U,64U,0U,const_cast<char*>("Digital Input bit")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    }, 
    {    13U                                   /*Instance*/
         ,DCI_DIGITAL_OUTPUT_BIT_DCID              /* Digital Output bit*/
         ,0U                                         /*Bit Number*/
         ,1U                                    /*Access Type*/
         ,18U,64U,0U,const_cast<char*>("Digital Output bit")              /*Name*/
         ,18U,64U,0U,const_cast<char*>("Digital Output bit")               /*Description*/
         ,3,64U,0U,const_cast<char*>("OFF")                /*Inactive Text*/
         ,2,64U,0U,const_cast<char*>("ON")                       /*Active Text*/
    } 
};  
     
const BACnet_DEV_to_DCID BACnet_DEV_to_DCID_map[TOTAL_DEV] =   
{   
    {    0U 
         ,DCI_VENDOR_NAME_DCID              /* Product Vendor Name*/
    }, 
    {    1U 
         ,DCI_BACNET_VENDOR_IDENTIFIER_DCID              /* BACnet Vendor Identifier*/
    }, 
    {    2U 
         ,DCI_MODEL_NAME_DCID              /* Model Name*/
    }, 
    {    3U 
         ,DCI_MAX_MASTER_DCID              /* BACnet Max Master*/
    }, 
    {    4U 
         ,DCI_MAX_INFO_FRAMES_DCID              /* BACnet Max Info Frames*/
    }, 
    {    5U 
         ,DCI_PROFILE_NAME_DCID              /* BACnet Profile Name*/
    }, 
    {    6U 
         ,DCI_USER_APP_NAME_DCID              /* User Application Name or Device Tag*/
    }, 
    {    7U 
         ,DCI_PRODUCT_NAME_DCID              /* Product Name - Short description of Prod Code*/
    } 
};  
     
const BACnet_MSV_to_DCID BACnet_MSV_to_DCID_map[TOTAL_MSV] =   
{   
    {    0U                                   /*Instance*/
         ,DCI_MULTI_STATE_0_DCID              /* Multi state 0*/
         ,0U                                    /*Access Type*/
         ,0U                                   /*Unit*/
         ,0U                                  /*Scale*/
         ,13U,64U,0U,const_cast<char*>("Multi state 0")              /*Name*/
         ,13U,64U,0U,const_cast<char*>("Multi state 0")               /*Description*/
         ,MSV0_len_to_State_Txt
    }, 
    {    1U                                   /*Instance*/
         ,DCI_MULTI_STATE_1_DCID              /* Multi state 1*/
         ,0U                                    /*Access Type*/
         ,0U                                   /*Unit*/
         ,0U                                  /*Scale*/
         ,13U,64U,0U,const_cast<char*>("Multi state 1")              /*Name*/
         ,13U,64U,0U,const_cast<char*>("Multi state 1")               /*Description*/
         ,MSV1_len_to_State_Txt
    } 
};  
     
const BACNET_CLASS_ST_TD bacnet_dci_data_cip_class_list[BACNET_TOTAL_CLASSES] =
{
    BACnet_AI_to_DCID_map,
    BACnet_AV_to_DCID_map,
    BACnet_BV_to_DCID_map,
    BACnet_DEV_to_DCID_map,
    BACnet_MSV_to_DCID_map,
};
//*******************************************************
//******      Define the Class Structure List
//*******************************************************
const BACNET_TARGET_INFO_ST_TD bacnet_dci_data_target_info =
{
    BACNET_TOTAL_CLASSES,
    bacnet_dci_data_cip_class_list,
};
     
