const char *json_profile_active_util_init = "{ \n\
  \"radioCtrlInit\": { \n\
    \"radioCtrlModeCfg\": { \n\
      \"txRadioCtrlModeCfg\": { \n\
        \"txEnableMode\": 0, \n\
        \"txChannelMask\": 15 \n\
      }, \n\
      \"rxRadioCtrlModeCfg\": { \n\
        \"rxEnableMode\": 0, \n\
        \"rxChannelMask\": 255 \n\
      }, \n\
      \"orxRadioCtrlModeCfg\": { \n\
        \"orxEnableMode\": 0, \n\
        \"orxPinSelectSettlingDelay_armClkCycles\": 0, \n\
        \"singleChannel1PinModeOrxSel\": 0, \n\
        \"singleChannel2PinModeLowOrxSel\": 0, \n\
        \"singleChannel2PinModeHighOrxSel\": 0, \n\
        \"dualChannel2PinModeOrxSel\": 0 \n\
      } \n\
    }, \n\
    \"txToOrxMapping\": { \n\
      \"orx1Map\": 2, \n\
      \"orx2Map\": 3, \n\
      \"orx3Map\": 2, \n\
      \"orx4Map\": 3 \n\
    }, \n\
    \"streamGpioCfg\": { \n\
      \"streamGpInput0\": 19, \n\
      \"streamGpInput1\": 19, \n\
      \"streamGpInput2\": 19, \n\
      \"streamGpInput3\": 19, \n\
      \"streamGpInput4\": 19, \n\
      \"streamGpInput5\": 19, \n\
      \"streamGpInput6\": 19, \n\
      \"streamGpInput7\": 19, \n\
      \"streamGpInput8\": 19, \n\
      \"streamGpInput9\": 19, \n\
      \"streamGpInput10\": 19, \n\
      \"streamGpInput11\": 19, \n\
      \"streamGpInput12\": 19, \n\
      \"streamGpInput13\": 19, \n\
      \"streamGpInput14\": 19, \n\
      \"streamGpInput15\": 19 \n\
    }, \n\
    \"lo1PllFreq_Hz\": 3500000000, \n\
    \"lo2PllFreq_Hz\": 3550000000, \n\
    \"auxPllFreq_Hz\": 0 \n\
  }, \n\
  \"initCals\": { \n\
    \"calMask\": 862207, \n\
    \"channelMask\": 15, \n\
    \"warmBoot\": 0 \n\
  }, \n\
  \"calRsltsFile\": null, \n\
  \"checkFwVer\": 0 \n\
}\n";
