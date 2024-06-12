# IoT Device Profiles

The device profile is a JSON file that contains
* the device's meta information (e.g. the model, version, etc.)
* the device channels' meta information (e.g. display name, units, vtype, etc)
* the device's capabilities and behaviors (e.g. a list of commands that a device supports).
* a mapping of the PXWhite "tag" identifiers to local channel identifiers (eg, DCI parameter names) since both are fields of each described channel.

The repository for today's IoT Device Profiles are located [here](https://bitbucket-prod.tcc.etn.com/projects/IOT/repos/platform_device_profile/browse).

A [sample IoT Device Profile](https://bitbucket-prod.tcc.etn.com/projects/IOT/repos/platform_device_profile/browse/InnovaUnity/InnovaUPS.json) illustrates this, for the device and some channels:

```json
{
  "_id": "c234f396-bfbf-4ea3-aecb-cd4842693132",
  "device": {
    "vendor": "Santak",
    "family": "Power Quality",
    "role": "device",
    "model": "Innova",
    "model_lname": "Innova UPS",
    "model_sname": "Innova UPS",
  	"hardware": "UPS",
  	"software": "Real Time OS",
    "mcl_id": "master",
  	"capabilities": {
    	"direct_method": true
     }
  },
  "channels": [
    {
      "tag": "102919",
      "name": "UPS.PowerConverter.Input[1].Voltage",
      "lname": "Main AC voltage",
      "sname": "Main AC voltage",
      "aname": "mACVin",
      "vtype": "UINT16",
      "category": "Voltage",
      "lcategory": "Voltage",
      "scategory": "Voltage",
      "unit": "volts*0.1",
      "lunit": "volts*0.1",
      "sunit": "dV"
    },
    ...
    {
      "tag": "102977",
      "name": "UPS.BatterySystem.Battery.TestInformation.Testduration",
      "lname": "Battery test duration record",
      "sname": "Battery test duration record",
      "aname": "iBATTR",
	  "vtype": "UINT16",
      "category": "Operations",
      "lcategory": "Operations",
      "scategory": "Operations",
      "unit": "seconds",
      "lunit": "second",
      "sunit": "s"
    }
  ]
}
```

## Future plans for Management of IoT Device Profiles

The PXWhite team is planning to revise the way Device Profiles are created and described, to better support the adopting product groups.
Many of the fields shown in the example above will be stripped out, and the
process should be easier for adopting groups to work with.

Those plans are discussed [here](https://confluence-prod.tcc.etn.com/display/IoTPlatform/Future+Device+Profile) and [here](https://confluence-prod.tcc.etn.com/display/IoTPlatform/R13+Device+Profile+Management).


