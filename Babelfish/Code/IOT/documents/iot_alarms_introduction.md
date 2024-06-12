# Introduction to Alarms over IOT

## Getting started

[IOT Alarms step-by-step guide and demo videos](­https://confluence-prod.tcc.etn.com/display/LTK/Alarms+over+IOT)

## Alarm rule configuration using DCI workbook

Refer "Alarms" sheet in  RTK DCI DB Creator workbook. Each alarm rule has the following properties:

* Alarm rule name in the free-form text
* Alarm custom message in the free-form text
* Alarm severity levels - Event, Notice, Cautionary, Critical. Any of these can be selected using drop-down
* Alarm priority - This is customer-configured through Alarm priority DCIDs(e.g DCI_AE_PRIORITY0 - DCI_AE_PRIORITY7) defined in "DCI Descriptors" sheet. Customer's choice of values.
* Alarm delay - This is configured through Alarm delay DCIDs(e.g DCI_AE_DELAY0 - DCI_AE_DELAY4) defined in "DCI Descriptors" sheet or its value can be entered directly

### How to add new alarm rule

In order to add a new alarm rule, you need to do modifications in RTK DCI DB Creator workbook. Suppose we want to add a new alarm rule named "High temperature". Below are the steps that you need to follow:

1. Navigate to Alarms sheet. Copy any row containing existing alarm rule and insert it just above "BEGIN_IGNORED_DATA"

2. Edit the newly created rule and enter its name in "Rule name" column. For e.g, you can name it as "HighTemperature"

3. Enter custom alarm message in "Message" column

4. Select severity level from the drop-down

5. Navigate to DCI descriptors sheet. Create a new priority DCID for e.g DCI_AE_PRIORITY9 and  define it. Click on "create DB" button. Navigate to "Alarms" sheet. Enter the newly created priority DCID for e.g DCI_AE_PRIORITY9 in the "Priority" column. Normally, you will want to add this DCID to "NVM Address" sheet to persist its value between power cycles.

6. Enter alarm delay in the "Delay" column. You can enter the value directly or an existing delay DCID can be reused or a new DCID for delay can be created and then used here

7. Save and click on "Create Alarms DB" button. This will update Alarms_DCI_Data.cpp and Alarms_DCI_Data.h files. Make sure that you don't get any errors

8. Navigate back to DCI Descriptors sheet. Modify the RO value of DCI_ALARM_ENABLE to 0x001F(Set 9th bit to 1) in order to enable newly created 9th rule. The DCID DCI_ALARM_ENABLE uses bitfields to enable individual rules. Save and click on "Create DB" button

### How to delete an existing alarm rule

1. Navigate to Alarms sheet. Delete the entire row of alarm rule that you wish to delete. Save and click on "Create Alarms DB" button

2. Optinally, you can also delete the priority and delay DCIDs used by that deleted alarm rule from DCI Descriptors sheet and NVM Address sheet

3. Navigate to DCI Descriptors sheet. Modify the RO value of DCI_ALARM_ENABLE to 0x000F since we have removed 1 rule. Save and click on "Create DB" button

## Alarm operations supported by IOT

* Alarm publishing to IOT hub as per configured severity level
* Alarm acknowledgement
* Close alarms
* Clear alarm history

## Alarm dynamic cadence

[How-to guide on alarm publishing dynamic cadence configuration and working](https://confluence-prod.tcc.etn.com/display/LTK/Alarms+dynamic+cadence+per+severity+level)
