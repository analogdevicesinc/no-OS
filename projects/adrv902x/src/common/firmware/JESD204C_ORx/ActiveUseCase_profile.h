const char *json_profile_active_use_case = "{ \n\
  \"dataInterface\": { \n\
    \"framer\": [ \n\
      { \n\
        \"serializerLaneCrossbar\": { \n\
          \"lane0FramerOutSel\": 0, \n\
          \"lane1FramerOutSel\": 1, \n\
          \"lane2FramerOutSel\": 8, \n\
          \"lane3FramerOutSel\": 8 \n\
        }, \n\
        \"adcCrossbar\": { \n\
          \"conv0\": 1, \n\
          \"conv1\": 0, \n\
          \"conv2\": 3, \n\
          \"conv3\": 2, \n\
          \"conv4\": 5, \n\
          \"conv5\": 4, \n\
          \"conv6\": 7, \n\
          \"conv7\": 6, \n\
          \"conv8\": 127, \n\
          \"conv9\": 127, \n\
          \"conv10\": 127, \n\
          \"conv11\": 127, \n\
          \"conv12\": 127, \n\
          \"conv13\": 127, \n\
          \"conv14\": 127, \n\
          \"conv15\": 127, \n\
          \"conv16\": 127, \n\
          \"conv17\": 127, \n\
          \"conv18\": 127, \n\
          \"conv19\": 127, \n\
          \"conv20\": 127, \n\
          \"conv21\": 127, \n\
          \"conv22\": 127, \n\
          \"conv23\": 127 \n\
        }, \n\
        \"enableJesd204C\": 1, \n\
        \"bankId\": 0, \n\
        \"deviceId\": 1, \n\
        \"lane0Id\": 0, \n\
        \"jesd204M\": 8, \n\
        \"jesd204K\": 32, \n\
        \"jesd204F\": 8, \n\
        \"jesd204Np\": 16, \n\
        \"jesd204E\": 1, \n\
        \"scramble\": 1, \n\
        \"serializerLanesEnabled\": 3, \n\
        \"lmfcOffset\": 0, \n\
        \"syncbInSelect\": 0, \n\
        \"overSample\": 0, \n\
        \"syncbInLvdsMode\": 1, \n\
        \"syncbInLvdsPnInvert\": 0, \n\
        \"newSysrefOnRelink\": 0, \n\
        \"sysrefForStartup\": 0, \n\
        \"sysrefNShotEnable\": 0, \n\
        \"sysrefNShotCount\": 0, \n\
        \"sysrefIgnoreWhenLinked\": 0 \n\
      }, \n\
      { \n\
        \"serializerLaneCrossbar\": { \n\
          \"lane0FramerOutSel\": 8, \n\
          \"lane1FramerOutSel\": 8, \n\
          \"lane2FramerOutSel\": 0, \n\
          \"lane3FramerOutSel\": 1 \n\
        }, \n\
        \"adcCrossbar\": { \n\
          \"conv0\": 17, \n\
          \"conv1\": 16, \n\
          \"conv2\": 19, \n\
          \"conv3\": 18, \n\
          \"conv4\": 127, \n\
          \"conv5\": 127, \n\
          \"conv6\": 127, \n\
          \"conv7\": 127, \n\
          \"conv8\": 127, \n\
          \"conv9\": 127, \n\
          \"conv10\": 127, \n\
          \"conv11\": 127, \n\
          \"conv12\": 127, \n\
          \"conv13\": 127, \n\
          \"conv14\": 127, \n\
          \"conv15\": 127, \n\
          \"conv16\": 127, \n\
          \"conv17\": 127, \n\
          \"conv18\": 127, \n\
          \"conv19\": 127, \n\
          \"conv20\": 127, \n\
          \"conv21\": 127, \n\
          \"conv22\": 127, \n\
          \"conv23\": 127 \n\
        }, \n\
        \"enableJesd204C\": 1, \n\
        \"bankId\": 0, \n\
        \"deviceId\": 1, \n\
        \"lane0Id\": 0, \n\
        \"jesd204M\": 4, \n\
        \"jesd204K\": 64, \n\
        \"jesd204F\": 4, \n\
        \"jesd204Np\": 16, \n\
        \"jesd204E\": 1, \n\
        \"scramble\": 1, \n\
        \"serializerLanesEnabled\": 12, \n\
        \"lmfcOffset\": 0, \n\
        \"syncbInSelect\": 1, \n\
        \"overSample\": 0, \n\
        \"syncbInLvdsMode\": 1, \n\
        \"syncbInLvdsPnInvert\": 0, \n\
        \"newSysrefOnRelink\": 0, \n\
        \"sysrefForStartup\": 0, \n\
        \"sysrefNShotEnable\": 0, \n\
        \"sysrefNShotCount\": 0, \n\
        \"sysrefIgnoreWhenLinked\": 0 \n\
      }, \n\
      { \n\
        \"serializerLaneCrossbar\": { \n\
          \"lane0FramerOutSel\": 0, \n\
          \"lane1FramerOutSel\": 0, \n\
          \"lane2FramerOutSel\": 0, \n\
          \"lane3FramerOutSel\": 0 \n\
        }, \n\
        \"adcCrossbar\": { \n\
          \"conv0\": 127, \n\
          \"conv1\": 127, \n\
          \"conv2\": 127, \n\
          \"conv3\": 127, \n\
          \"conv4\": 127, \n\
          \"conv5\": 127, \n\
          \"conv6\": 127, \n\
          \"conv7\": 127, \n\
          \"conv8\": 127, \n\
          \"conv9\": 127, \n\
          \"conv10\": 127, \n\
          \"conv11\": 127, \n\
          \"conv12\": 127, \n\
          \"conv13\": 127, \n\
          \"conv14\": 127, \n\
          \"conv15\": 127, \n\
          \"conv16\": 127, \n\
          \"conv17\": 127, \n\
          \"conv18\": 127, \n\
          \"conv19\": 127, \n\
          \"conv20\": 127, \n\
          \"conv21\": 127, \n\
          \"conv22\": 127, \n\
          \"conv23\": 127 \n\
        }, \n\
        \"enableJesd204C\": 1, \n\
        \"bankId\": 0, \n\
        \"deviceId\": 0, \n\
        \"lane0Id\": 0, \n\
        \"jesd204M\": 0, \n\
        \"jesd204K\": 0, \n\
        \"jesd204F\": 0, \n\
        \"jesd204Np\": 0, \n\
        \"jesd204E\": 0, \n\
        \"scramble\": 0, \n\
        \"serializerLanesEnabled\": 0, \n\
        \"lmfcOffset\": 0, \n\
        \"syncbInSelect\": 0, \n\
        \"overSample\": 0, \n\
        \"syncbInLvdsMode\": 0, \n\
        \"syncbInLvdsPnInvert\": 0, \n\
        \"newSysrefOnRelink\": 0, \n\
        \"sysrefForStartup\": 0, \n\
        \"sysrefNShotEnable\": 0, \n\
        \"sysrefNShotCount\": 0, \n\
        \"sysrefIgnoreWhenLinked\": 0 \n\
      } \n\
    ], \n\
    \"deframer\": [ \n\
      { \n\
        \"deserializerLaneCrossbar\": { \n\
          \"deframerInput0LaneSel\": 0, \n\
          \"deframerInput1LaneSel\": 1, \n\
          \"deframerInput2LaneSel\": 2, \n\
          \"deframerInput3LaneSel\": 3 \n\
        }, \n\
        \"enableJesd204C\": 1, \n\
        \"bankId\": 0, \n\
        \"deviceId\": 1, \n\
        \"lane0Id\": 0, \n\
        \"jesd204M\": 8, \n\
        \"jesd204K\": 64, \n\
        \"jesd204F\": 4, \n\
        \"jesd204Np\": 16, \n\
        \"jesd204E\": 1, \n\
        \"scramble\": 1, \n\
        \"deserializerLanesEnabled\": 15, \n\
        \"lmfcOffset\": 0, \n\
        \"syncbOutSelect\": 0, \n\
        \"syncbOutLvdsMode\": 1, \n\
        \"syncbOutLvdsPnInvert\": 0, \n\
        \"syncbOutCmosSlewRate\": 0, \n\
        \"syncbOutCmosDriveLevel\": 0, \n\
        \"dacCrossbar\": { \n\
          \"tx1DacChanI\": 1, \n\
          \"tx1DacChanQ\": 0, \n\
          \"tx2DacChanI\": 3, \n\
          \"tx2DacChanQ\": 2, \n\
          \"tx3DacChanI\": 5, \n\
          \"tx3DacChanQ\": 4, \n\
          \"tx4DacChanI\": 7, \n\
          \"tx4DacChanQ\": 6 \n\
        }, \n\
        \"newSysrefOnRelink\": 0, \n\
        \"sysrefForStartup\": 0, \n\
        \"sysrefNShotEnable\": 0, \n\
        \"sysrefNShotCount\": 0, \n\
        \"sysrefIgnoreWhenLinked\": 0 \n\
      }, \n\
      { \n\
        \"deserializerLaneCrossbar\": { \n\
          \"deframerInput0LaneSel\": 8, \n\
          \"deframerInput1LaneSel\": 8, \n\
          \"deframerInput2LaneSel\": 8, \n\
          \"deframerInput3LaneSel\": 8 \n\
        }, \n\
        \"enableJesd204C\": 1, \n\
        \"bankId\": 0, \n\
        \"deviceId\": 0, \n\
        \"lane0Id\": 0, \n\
        \"jesd204M\": 0, \n\
        \"jesd204K\": 0, \n\
        \"jesd204F\": 0, \n\
        \"jesd204Np\": 0, \n\
        \"jesd204E\": 0, \n\
        \"scramble\": 0, \n\
        \"deserializerLanesEnabled\": 0, \n\
        \"lmfcOffset\": 0, \n\
        \"syncbOutSelect\": 0, \n\
        \"syncbOutLvdsMode\": 0, \n\
        \"syncbOutLvdsPnInvert\": 0, \n\
        \"syncbOutCmosSlewRate\": 0, \n\
        \"syncbOutCmosDriveLevel\": 0, \n\
        \"dacCrossbar\": { \n\
          \"tx1DacChanI\": 127, \n\
          \"tx1DacChanQ\": 127, \n\
          \"tx2DacChanI\": 127, \n\
          \"tx2DacChanQ\": 127, \n\
          \"tx3DacChanI\": 127, \n\
          \"tx3DacChanQ\": 127, \n\
          \"tx4DacChanI\": 127, \n\
          \"tx4DacChanQ\": 127 \n\
        }, \n\
        \"newSysrefOnRelink\": 0, \n\
        \"sysrefForStartup\": 1, \n\
        \"sysrefNShotEnable\": 0, \n\
        \"sysrefNShotCount\": 0, \n\
        \"sysrefIgnoreWhenLinked\": 0 \n\
      } \n\
    ], \n\
    \"serCfg\": [ \n\
      { \n\
        \"serAmplitude\": 0, \n\
        \"serPreEmphasis\": 1, \n\
        \"serPostEmphasis\": 3, \n\
        \"serInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"serAmplitude\": 0, \n\
        \"serPreEmphasis\": 1, \n\
        \"serPostEmphasis\": 3, \n\
        \"serInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"serAmplitude\": 0, \n\
        \"serPreEmphasis\": 1, \n\
        \"serPostEmphasis\": 3, \n\
        \"serInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"serAmplitude\": 0, \n\
        \"serPreEmphasis\": 1, \n\
        \"serPostEmphasis\": 3, \n\
        \"serInvertLanePolarity\": 0 \n\
      } \n\
    ], \n\
    \"desCfg\": [ \n\
      { \n\
        \"highBoost\": 0, \n\
        \"configOption1\": 0, \n\
        \"configOption2\": 0, \n\
        \"configOption3\": 0, \n\
        \"configOption4\": 0, \n\
        \"configOption5\": 0, \n\
        \"configOption6\": 0, \n\
        \"configOption7\": 0, \n\
        \"configOption8\": 0, \n\
        \"configOption9\": 0, \n\
        \"configOption10\": 0, \n\
        \"desInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"highBoost\": 0, \n\
        \"configOption1\": 0, \n\
        \"configOption2\": 0, \n\
        \"configOption3\": 0, \n\
        \"configOption4\": 0, \n\
        \"configOption5\": 0, \n\
        \"configOption6\": 0, \n\
        \"configOption7\": 0, \n\
        \"configOption8\": 0, \n\
        \"configOption9\": 0, \n\
        \"configOption10\": 0, \n\
        \"desInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"highBoost\": 0, \n\
        \"configOption1\": 0, \n\
        \"configOption2\": 0, \n\
        \"configOption3\": 0, \n\
        \"configOption4\": 0, \n\
        \"configOption5\": 0, \n\
        \"configOption6\": 0, \n\
        \"configOption7\": 0, \n\
        \"configOption8\": 0, \n\
        \"configOption9\": 0, \n\
        \"configOption10\": 0, \n\
        \"desInvertLanePolarity\": 0 \n\
      }, \n\
      { \n\
        \"highBoost\": 0, \n\
        \"configOption1\": 0, \n\
        \"configOption2\": 0, \n\
        \"configOption3\": 0, \n\
        \"configOption4\": 0, \n\
        \"configOption5\": 0, \n\
        \"configOption6\": 0, \n\
        \"configOption7\": 0, \n\
        \"configOption8\": 0, \n\
        \"configOption9\": 0, \n\
        \"configOption10\": 0, \n\
        \"desInvertLanePolarity\": 0 \n\
      } \n\
    ], \n\
    \"linkSharingCfg\": { \n\
      \"linkSharingEnabled\": 0, \n\
      \"linkSharingM\": 0, \n\
      \"linkSharingS\": 1, \n\
      \"linkSharingNp\": 0, \n\
      \"linkSharingAdcCrossbar\": { \n\
        \"conv0\": 127, \n\
        \"conv1\": 127, \n\
        \"conv2\": 127, \n\
        \"conv3\": 127, \n\
        \"conv4\": 127, \n\
        \"conv5\": 127, \n\
        \"conv6\": 127, \n\
        \"conv7\": 127, \n\
        \"conv8\": 127, \n\
        \"conv9\": 127, \n\
        \"conv10\": 127, \n\
        \"conv11\": 127, \n\
        \"conv12\": 127, \n\
        \"conv13\": 127, \n\
        \"conv14\": 127, \n\
        \"conv15\": 127 \n\
      } \n\
    }, \n\
    \"dataCfg\": { \n\
      \"enable\": 0, \n\
      \"configOption1\": 0, \n\
      \"configOption2\": 0 \n\
    }, \n\
    \"channelSelect\": 0, \n\
    \"channelMode\": 0 \n\
  }, \n\
  \"clocks\": { \n\
    \"deviceClock_kHz\": 245760, \n\
    \"clkPllVcoFreq_kHz\": 9830400, \n\
    \"serdesPllVcoFreq_kHz\": 8110080, \n\
    \"ldoSelect\": 0, \n\
    \"extLoFreq1_kHz\": 0, \n\
    \"extLoFreq2_kHz\": 0, \n\
    \"rfPll1LoMode\": 0, \n\
    \"rfPll2LoMode\": 0, \n\
    \"rfPll1LoOutDivider\": 0, \n\
    \"rfPll2LoOutDivider\": 0, \n\
    \"rfPllPhaseSyncMode\": 0, \n\
    \"rx12LoSelect\": 1, \n\
    \"rx34LoSelect\": 1, \n\
    \"tx12LoSelect\": 2, \n\
    \"tx34LoSelect\": 2, \n\
    \"orx12LoSelect\": 1, \n\
    \"orx34LoSelect\": 1 \n\
  }, \n\
  \"gpInterrupts\": { \n\
    \"gpIntMaskPin0\": 18446744073709551615, \n\
    \"gpIntMaskPin1\": 18446744073709551615 \n\
  }, \n\
  \"rx\": { \n\
    \"rxInitChannelMask\": 1023, \n\
    \"rxChannelCfg\": [ \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 1, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 1, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 245760, \n\
          \"rfBandwidth_kHz\": 300000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 150000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              -19, \n\
              34, \n\
              -8, \n\
              1, \n\
              4, \n\
              27, \n\
              -48, \n\
              16, \n\
              95, \n\
              -169, \n\
              -899, \n\
              18594, \n\
              -899, \n\
              -169, \n\
              95, \n\
              16, \n\
              -48, \n\
              27, \n\
              4, \n\
              1, \n\
              -8, \n\
              34, \n\
              -19, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 3, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 1, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 2, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 1, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 245760, \n\
          \"rfBandwidth_kHz\": 300000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 150000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              -19, \n\
              34, \n\
              -8, \n\
              1, \n\
              4, \n\
              27, \n\
              -48, \n\
              16, \n\
              95, \n\
              -169, \n\
              -899, \n\
              18594, \n\
              -899, \n\
              -169, \n\
              95, \n\
              16, \n\
              -48, \n\
              27, \n\
              4, \n\
              1, \n\
              -8, \n\
              34, \n\
              -19, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 3, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 2, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 4, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 1, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 245760, \n\
          \"rfBandwidth_kHz\": 300000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 150000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              -19, \n\
              34, \n\
              -8, \n\
              1, \n\
              4, \n\
              27, \n\
              -48, \n\
              16, \n\
              95, \n\
              -169, \n\
              -899, \n\
              18594, \n\
              -899, \n\
              -169, \n\
              95, \n\
              16, \n\
              -48, \n\
              27, \n\
              4, \n\
              1, \n\
              -8, \n\
              34, \n\
              -19, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 3, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 4, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 8, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 1, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 245760, \n\
          \"rfBandwidth_kHz\": 300000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 150000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              -19, \n\
              34, \n\
              -8, \n\
              1, \n\
              4, \n\
              27, \n\
              -48, \n\
              16, \n\
              95, \n\
              -169, \n\
              -899, \n\
              18594, \n\
              -899, \n\
              -169, \n\
              95, \n\
              16, \n\
              -48, \n\
              27, \n\
              4, \n\
              1, \n\
              -8, \n\
              34, \n\
              -19, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 3, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 8, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 16, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 16, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 32, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 32, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 64, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 64, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 128, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 128, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 256, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 256, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"channelType\": 512, \n\
          \"rxFirDecimation\": 1, \n\
          \"rxDec5Decimation\": 5, \n\
          \"rhb1Decimation\": 2, \n\
          \"rhb1WideBandMode\": 0, \n\
          \"rhb2Decimation\": 1, \n\
          \"rhb3Decimation\": 1, \n\
          \"rxFir1Decimation\": 1, \n\
          \"rxFir2Decimation\": 1, \n\
          \"rxOutputRate_kHz\": 491520, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"rxBbf3dBCorner_kHz\": 225000, \n\
          \"rxAdcBandWidth_kHz\": 225000, \n\
          \"rxFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 24, \n\
            \"coefs\": [ \n\
              33, \n\
              -7, \n\
              9, \n\
              -7, \n\
              3, \n\
              9, \n\
              -27, \n\
              62, \n\
              -138, \n\
              349, \n\
              -1663, \n\
              19282, \n\
              -1663, \n\
              349, \n\
              -138, \n\
              62, \n\
              -27, \n\
              9, \n\
              3, \n\
              -7, \n\
              9, \n\
              -7, \n\
              33, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"rxDdcMode\": 0, \n\
          \"rxNcoShifterCfg\": { \n\
            \"bandAInputBandWidth_kHz\": 0, \n\
            \"bandAInputCenterFreq_kHz\": 0, \n\
            \"bandANco1Freq_kHz\": 0, \n\
            \"bandANco2Freq_kHz\": 0, \n\
            \"bandBInputBandWidth_kHz\": 0, \n\
            \"bandBInputCenterFreq_kHz\": 0, \n\
            \"bandBNco1Freq_kHz\": 0, \n\
            \"bandBNco2Freq_kHz\": 0, \n\
            \"bandAbCombinedEnable\": 0 \n\
          }, \n\
          \"tiaPowerMode\": 4, \n\
          \"rxDataFormat\": { \n\
            \"rxChannelMask\": 512, \n\
            \"formatSelect\": 0, \n\
            \"floatingPointConfig\": { \n\
              \"fpDataFormat\": 0, \n\
              \"fpRoundMode\": 0, \n\
              \"fpNumExpBits\": 0, \n\
              \"fpAttenSteps\": 0, \n\
              \"fpHideLeadingOne\": 0, \n\
              \"fpEncodeNan\": 0 \n\
            }, \n\
            \"integerConfigSettings\": { \n\
              \"intEmbeddedBits\": 0, \n\
              \"intSampleResolution\": 2, \n\
              \"intParity\": 2, \n\
              \"intEmbeddedPos\": 0 \n\
            }, \n\
            \"slicerConfigSettings\": { \n\
              \"extSlicerStepSize\": 0, \n\
              \"intSlicerStepSize\": 0, \n\
              \"rx1ExtSlicerGpioSelect\": 0, \n\
              \"rx2ExtSlicerGpioSelect\": 0, \n\
              \"rx3ExtSlicerGpioSelect\": 0, \n\
              \"rx4ExtSlicerGpioSelect\": 0 \n\
            }, \n\
            \"embOvldMonitorSettings\": { \n\
              \"embeddedMonitorSrcLsbI\": 0, \n\
              \"embeddedMonitorSrcLsbQ\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneI\": 0, \n\
              \"embeddedMonitorSrcLsbPlusOneQ\": 0, \n\
              \"embeddedMonitorHb2LowSrcSel\": 6, \n\
              \"embeddedMonitorHb2HighSrcSel\": 3, \n\
              \"embeddedMonitorApdLowSrcSel\": 2, \n\
              \"embeddedMonitorApdHighSrcSel\": 2, \n\
              \"invertHb2Flag\": 0, \n\
              \"invertApdFlag\": 0 \n\
            }, \n\
            \"externalLnaGain\": 0, \n\
            \"tempCompensationEnable\": 0 \n\
          } \n\
        } \n\
      } \n\
    ] \n\
  }, \n\
  \"tx\": { \n\
    \"txInitChannelMask\": 15, \n\
    \"txChannelCfg\": [ \n\
      { \n\
        \"profile\": { \n\
          \"txInputRate_kHz\": 491520, \n\
          \"primarySigBandwidth_kHz\": 200000, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"txDac3dBCorner_kHz\": 450000, \n\
          \"txBbf3dBCorner_kHz\": 225000, \n\
          \"dpdHb1Interpolation\": 1, \n\
          \"dpdHb2Interpolation\": 1, \n\
          \"txFirInterpolation\": 1, \n\
          \"thb1Interpolation\": 2, \n\
          \"thb2Interpolation\": 1, \n\
          \"thb3Interpolation\": 2, \n\
          \"txInt5Interpolation\": 1, \n\
          \"txFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 20, \n\
            \"coefs\": [ \n\
              -28, \n\
              45, \n\
              -82, \n\
              152, \n\
              -262, \n\
              462, \n\
              -893, \n\
              1945, \n\
              -4588, \n\
              22882, \n\
              -4588, \n\
              1945, \n\
              -893, \n\
              462, \n\
              -262, \n\
              152, \n\
              -82, \n\
              45, \n\
              -28, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"txBbfPowerMode\": 8 \n\
        }, \n\
        \"txAttenCtrl\": { \n\
          \"txAttenStepSize\": 0, \n\
          \"txRampDownEvents\": 63, \n\
          \"reserved\": 0, \n\
          \"attenMode\": 1, \n\
          \"dacFullScale\": 0, \n\
          \"txAttenInit_mdB\": 10000 \n\
        }, \n\
        \"dacFullScale\": 0 \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"txInputRate_kHz\": 491520, \n\
          \"primarySigBandwidth_kHz\": 200000, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"txDac3dBCorner_kHz\": 450000, \n\
          \"txBbf3dBCorner_kHz\": 225000, \n\
          \"dpdHb1Interpolation\": 1, \n\
          \"dpdHb2Interpolation\": 1, \n\
          \"txFirInterpolation\": 1, \n\
          \"thb1Interpolation\": 2, \n\
          \"thb2Interpolation\": 1, \n\
          \"thb3Interpolation\": 2, \n\
          \"txInt5Interpolation\": 1, \n\
          \"txFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 20, \n\
            \"coefs\": [ \n\
              -28, \n\
              45, \n\
              -82, \n\
              152, \n\
              -262, \n\
              462, \n\
              -893, \n\
              1945, \n\
              -4588, \n\
              22882, \n\
              -4588, \n\
              1945, \n\
              -893, \n\
              462, \n\
              -262, \n\
              152, \n\
              -82, \n\
              45, \n\
              -28, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"txBbfPowerMode\": 8 \n\
        }, \n\
        \"txAttenCtrl\": { \n\
          \"txAttenStepSize\": 0, \n\
          \"txRampDownEvents\": 63, \n\
          \"reserved\": 0, \n\
          \"attenMode\": 1, \n\
          \"dacFullScale\": 0, \n\
          \"txAttenInit_mdB\": 10000 \n\
        }, \n\
        \"dacFullScale\": 0 \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"txInputRate_kHz\": 491520, \n\
          \"primarySigBandwidth_kHz\": 200000, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"txDac3dBCorner_kHz\": 450000, \n\
          \"txBbf3dBCorner_kHz\": 225000, \n\
          \"dpdHb1Interpolation\": 1, \n\
          \"dpdHb2Interpolation\": 1, \n\
          \"txFirInterpolation\": 1, \n\
          \"thb1Interpolation\": 2, \n\
          \"thb2Interpolation\": 1, \n\
          \"thb3Interpolation\": 2, \n\
          \"txInt5Interpolation\": 1, \n\
          \"txFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 20, \n\
            \"coefs\": [ \n\
              -28, \n\
              45, \n\
              -82, \n\
              152, \n\
              -262, \n\
              462, \n\
              -893, \n\
              1945, \n\
              -4588, \n\
              22882, \n\
              -4588, \n\
              1945, \n\
              -893, \n\
              462, \n\
              -262, \n\
              152, \n\
              -82, \n\
              45, \n\
              -28, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"txBbfPowerMode\": 8 \n\
        }, \n\
        \"txAttenCtrl\": { \n\
          \"txAttenStepSize\": 0, \n\
          \"txRampDownEvents\": 63, \n\
          \"reserved\": 0, \n\
          \"attenMode\": 1, \n\
          \"dacFullScale\": 0, \n\
          \"txAttenInit_mdB\": 10000 \n\
        }, \n\
        \"dacFullScale\": 0 \n\
      }, \n\
      { \n\
        \"profile\": { \n\
          \"txInputRate_kHz\": 491520, \n\
          \"primarySigBandwidth_kHz\": 200000, \n\
          \"rfBandwidth_kHz\": 450000, \n\
          \"txDac3dBCorner_kHz\": 450000, \n\
          \"txBbf3dBCorner_kHz\": 225000, \n\
          \"dpdHb1Interpolation\": 1, \n\
          \"dpdHb2Interpolation\": 1, \n\
          \"txFirInterpolation\": 1, \n\
          \"thb1Interpolation\": 2, \n\
          \"thb2Interpolation\": 1, \n\
          \"thb3Interpolation\": 2, \n\
          \"txInt5Interpolation\": 1, \n\
          \"txFir\": { \n\
            \"gain_dB\": 6, \n\
            \"numFirCoefs\": 20, \n\
            \"coefs\": [ \n\
              -28, \n\
              45, \n\
              -82, \n\
              152, \n\
              -262, \n\
              462, \n\
              -893, \n\
              1945, \n\
              -4588, \n\
              22882, \n\
              -4588, \n\
              1945, \n\
              -893, \n\
              462, \n\
              -262, \n\
              152, \n\
              -82, \n\
              45, \n\
              -28, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0, \n\
              0 \n\
            ] \n\
          }, \n\
          \"txBbfPowerMode\": 8 \n\
        }, \n\
        \"txAttenCtrl\": { \n\
          \"txAttenStepSize\": 0, \n\
          \"txRampDownEvents\": 63, \n\
          \"reserved\": 0, \n\
          \"attenMode\": 1, \n\
          \"dacFullScale\": 0, \n\
          \"txAttenInit_mdB\": 10000 \n\
        }, \n\
        \"dacFullScale\": 0 \n\
      } \n\
    ] \n\
  }, \n\
  \"adcProfiles\": { \n\
    \"RxChannel1\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"RxChannel2\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"RxChannel3\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"RxChannel4\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel1Profile0\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel1Profile1\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel1Profile2\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel1Profile3\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel2Profile0\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel2Profile1\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel2Profile2\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel2Profile3\": [ \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0, \n\
      0 \n\
    ], \n\
    \"OrxChannel1Index\": 0, \n\
    \"OrxChannel2Index\": 0 \n\
  } \n\
} \n\
\n";

