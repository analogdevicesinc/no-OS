const char *json_profile = "{ \
  \"clocks\": { \
    \"deviceClock_kHz\": 38400, \
    \"clkPllVcoFreq_daHz\": 884736000, \
    \"clkPllHsDiv\": 0, \
    \"clkPllMode\": 0, \
    \"clk1105Div\": 2, \
    \"armClkDiv\": 6, \
    \"armPowerSavingClkDiv\": 1, \
    \"refClockOutEnable\": true, \
    \"auxPllPower\": 2, \
    \"clkPllPower\": 2, \
    \"padRefClkDrv\": 0, \
    \"extLo1OutFreq_kHz\": 0, \
    \"extLo2OutFreq_kHz\": 0, \
    \"rfPll1LoMode\": 0, \
    \"rfPll2LoMode\": 0, \
    \"ext1LoType\": 0, \
    \"ext2LoType\": 0, \
    \"rx1RfInputSel\": 0, \
    \"rx2RfInputSel\": 0, \
    \"extLo1Divider\": 2, \
    \"extLo2Divider\": 2, \
    \"rfPllPhaseSyncMode\": 0, \
    \"rx1LoSelect\": 2, \
    \"rx2LoSelect\": 2, \
    \"tx1LoSelect\": 1, \
    \"tx2LoSelect\": 1, \
    \"rx1LoDivMode\": 1, \
    \"rx2LoDivMode\": 1, \
    \"tx1LoDivMode\": 1, \
    \"tx2LoDivMode\": 1, \
    \"loGen1Select\": 1, \
    \"loGen2Select\": 1 \
  }, \
  \"rx\": { \
    \"rxInitChannelMask\": 195, \
    \"rxChannelCfg\": [ { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 1008000, \
          \"rxOutputRate_Hz\": 1920000, \
          \"rxInterfaceSampleRate_Hz\": 1920000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 20000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 40000, \
          \"rxCorner3dBLp_kHz\": 40000, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 1, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 1, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 1, \
              \"decBy2Blk27En\": 1, \
              \"decBy2Blk29En\": 1, \
              \"decBy2Blk31En\": 1, \
              \"decBy2Blk33En\": 1, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 1, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 4, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 1, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 1920000, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 0, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 1 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 1, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 1008000, \
          \"rxOutputRate_Hz\": 1920000, \
          \"rxInterfaceSampleRate_Hz\": 1920000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 20000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 40000, \
          \"rxCorner3dBLp_kHz\": 40000, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 2, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 1, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 1, \
              \"decBy2Blk27En\": 1, \
              \"decBy2Blk29En\": 1, \
              \"decBy2Blk31En\": 1, \
              \"decBy2Blk33En\": 1, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 1, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 4, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 1, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 1920000, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 2, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 1 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 1, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 12500, \
          \"rxOutputRate_Hz\": 0, \
          \"rxInterfaceSampleRate_Hz\": 0, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 0, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 0, \
          \"rxCorner3dB_kHz\": 0, \
          \"rxCorner3dBLp_kHz\": 0, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 0, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 0, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
              \"decBy2Blk31En\": 0, \
              \"decBy2Blk33En\": 0, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 0, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 0, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 0, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 0, \
            \"ssiDataFormatSel\": 0, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 12500, \
          \"rxOutputRate_Hz\": 0, \
          \"rxInterfaceSampleRate_Hz\": 0, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 0, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 0, \
          \"rxCorner3dB_kHz\": 0, \
          \"rxCorner3dBLp_kHz\": 0, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 0, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 0, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
              \"decBy2Blk31En\": 0, \
              \"decBy2Blk33En\": 0, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 0, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 0, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 0, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 0, \
            \"ssiDataFormatSel\": 0, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 1008000, \
          \"rxOutputRate_Hz\": 1920000, \
          \"rxInterfaceSampleRate_Hz\": 1920000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 50000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 100000, \
          \"rxCorner3dBLp_kHz\": 100000, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 64, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 1, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 1, \
              \"decBy2Blk27En\": 1, \
              \"decBy2Blk29En\": 1, \
              \"decBy2Blk31En\": 1, \
              \"decBy2Blk33En\": 1, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 1, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 4, \
              \"hbMux\": 2, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 1, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 1, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 1008000, \
          \"rxOutputRate_Hz\": 1920000, \
          \"rxInterfaceSampleRate_Hz\": 1920000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 50000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 100000, \
          \"rxCorner3dBLp_kHz\": 100000, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 128, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 1, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 1, \
              \"decBy2Blk27En\": 1, \
              \"decBy2Blk29En\": 1, \
              \"decBy2Blk31En\": 1, \
              \"decBy2Blk33En\": 1, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 1, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 4, \
              \"hbMux\": 2, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 3, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 1, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 12500, \
          \"rxOutputRate_Hz\": 0, \
          \"rxInterfaceSampleRate_Hz\": 0, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 0, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 0, \
          \"rxCorner3dB_kHz\": 0, \
          \"rxCorner3dBLp_kHz\": 0, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 0, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 0, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
              \"decBy2Blk31En\": 0, \
              \"decBy2Blk33En\": 0, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 0, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 0, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 0, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 0, \
            \"ssiDataFormatSel\": 0, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 12500, \
          \"rxOutputRate_Hz\": 0, \
          \"rxInterfaceSampleRate_Hz\": 0, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 0, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 0, \
          \"rxCorner3dB_kHz\": 0, \
          \"rxCorner3dBLp_kHz\": 0, \
          \"tiaPower\": 2, \
          \"tiaPowerLp\": 2, \
          \"channelType\": 0, \
          \"adcType\": 1, \
          \"lpAdcCalMode\": 0, \
          \"gainTableType\": 0, \
          \"rxDpProfile\": { \
            \"rxNbDecTop\": { \
              \"scicBlk23En\": 0, \
              \"scicBlk23DivFactor\": 0, \
              \"scicBlk23LowRippleEn\": 0, \
              \"decBy2Blk35En\": 0, \
              \"decBy2Blk37En\": 0, \
              \"decBy2Blk39En\": 0, \
              \"decBy2Blk41En\": 0, \
              \"decBy2Blk43En\": 0, \
              \"decBy3Blk45En\": 0, \
              \"decBy2Blk47En\": 0 \
            }, \
            \"rxWbDecTop\": { \
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
              \"decBy2Blk31En\": 0, \
              \"decBy2Blk33En\": 0, \
              \"wbLpfBlk33p1En\": 0 \
            }, \
            \"rxDecTop\": { \
              \"decBy3Blk15En\": 0, \
              \"decBy2Hb3Blk17p1En\": 0, \
              \"decBy2Hb4Blk17p2En\": 0, \
              \"decBy2Hb5Blk19p1En\": 0, \
              \"decBy2Hb6Blk19p2En\": 0 \
            }, \
            \"rxSincHBTop\": { \
              \"sincGainMux\": 1, \
              \"sincMux\": 0, \
              \"hbMux\": 4, \
              \"isGainCompEnabled\": 0, \
              \"gainComp9GainI\": [ 16384, 16384, 16384, 16384, 16384, 16384 ], \
              \"gainComp9GainQ\": [ 0, 0, 0, 0, 0, 0 ] \
            }, \
            \"rxNbDem\": { \
              \"dpInFifo\": { \
                \"dpInFifoEn\": 0, \
                \"dpInFifoMode\": 0, \
                \"dpInFifoTestDataSel\": 0 \
              }, \
              \"rxNbNco\": { \
                \"rxNbNcoEn\": 0, \
                \"rxNbNcoConfig\": { \
                  \"freq\": 0, \
                  \"sampleFreq\": 0, \
                  \"phase\": 0, \
                  \"realOut\": 0 \
                } \
              }, \
              \"rxWbNbCompPFir\": { \
                \"bankSel\": 0, \
                \"rxWbNbCompPFirInMuxSel\": 0, \
                \"rxWbNbCompPFirEn\": 0 \
              }, \
              \"resamp\": { \
                \"rxResampEn\": 0, \
                \"resampPhaseI\": 0, \
                \"resampPhaseQ\": 0 \
              }, \
              \"gsOutMuxSel\": 1, \
              \"rxOutSel\": 0, \
              \"rxRoundMode\": 0, \
              \"dpArmSel\": 0 \
            } \
          }, \
          \"lnaConfig\": { \
            \"externalLnaPresent\": false, \
            \"gpioSourceSel\": 0, \
            \"externalLnaPinSel\": 0, \
            \"settlingDelay\": 0, \
            \"numberLnaGainSteps\": 0, \
            \"lnaGainSteps_mdB\": [ 0, 0, 0, 0 ], \
            \"lnaDigitalGainDelay\": 0, \
            \"minGainIndex\": 0, \
            \"lnaType\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 0, \
            \"ssiDataFormatSel\": 0, \
            \"numLaneSel\": 0, \
            \"strobeType\": 0, \
            \"lsbFirst\": 0, \
            \"qFirst\": 0, \
            \"txRefClockPin\": 0, \
            \"lvdsIBitInversion\": false, \
            \"lvdsQBitInversion\": false, \
            \"lvdsStrobeBitInversion\": false, \
            \"lvdsUseLsbIn12bitMode\": 0, \
            \"lvdsRxClkInversionEn\": false, \
            \"cmosDdrPosClkEn\": false, \
            \"cmosClkInversionEn\": false, \
            \"ddrEn\": false, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      } ] \
  }, \
  \"tx\": { \
    \"txInitChannelMask\": 12, \
    \"txProfile\": [ { \
        \"primarySigBandwidth_Hz\": 1008000, \
        \"txInputRate_Hz\": 1920000, \
        \"txInterfaceSampleRate_Hz\": 1920000, \
        \"txOffsetLo_kHz\": 0, \
        \"validDataDelay\": 0, \
        \"txBbf3dBCorner_kHz\": 50000, \
        \"outputSignaling\": 0, \
        \"txPdBiasCurrent\": 1, \
        \"txPdGainEnable\": 0, \
        \"txPrePdRealPole_kHz\": 1000000, \
        \"txPostPdRealPole_kHz\": 530000, \
        \"txBbfPower\": 2, \
        \"txExtLoopBackType\": 0, \
        \"txExtLoopBackForInitCal\": 0, \
        \"txPeakLoopBackPower\": 0, \
        \"frequencyDeviation_Hz\": 0, \
        \"txDpProfile\": { \
          \"txPreProc\": { \
            \"txPreProcSymbol0\": 0, \
            \"txPreProcSymbol1\": 0, \
            \"txPreProcSymbol2\": 0, \
            \"txPreProcSymbol3\": 0, \
            \"txPreProcSymMapDivFactor\": 1, \
            \"txPreProcMode\": 1, \
            \"txPreProcWbNbPfirIBankSel\": 0, \
            \"txPreProcWbNbPfirQBankSel\": 1 \
          }, \
          \"txWbIntTop\": { \
            \"txInterpBy2Blk30En\": 1, \
            \"txInterpBy2Blk28En\": 1, \
            \"txInterpBy2Blk26En\": 1, \
            \"txInterpBy2Blk24En\": 1, \
            \"txInterpBy2Blk22En\": 1, \
            \"txWbLpfBlk22p1En\": 0 \
          }, \
          \"txNbIntTop\": { \
            \"txInterpBy2Blk20En\": 0, \
            \"txInterpBy2Blk18En\": 0, \
            \"txInterpBy2Blk16En\": 0, \
            \"txInterpBy2Blk14En\": 0, \
            \"txInterpBy2Blk12En\": 0, \
            \"txInterpBy3Blk10En\": 0, \
            \"txInterpBy2Blk8En\": 0, \
            \"txScicBlk32En\": 0, \
            \"txScicBlk32DivFactor\": 1 \
          }, \
          \"txIntTop\": { \
            \"interpBy3Blk44p1En\": 1, \
            \"sinc3Blk44En\": 0, \
            \"sinc2Blk42En\": 0, \
            \"interpBy3Blk40En\": 1, \
            \"interpBy2Blk38En\": 0, \
            \"interpBy2Blk36En\": 0 \
          }, \
          \"txIntTopFreqDevMap\": { \
            \"rrc2Frac\": 0, \
            \"mpll\": 0, \
            \"nchLsw\": 0, \
            \"nchMsb\": 0, \
            \"freqDevMapEn\": 0, \
            \"txRoundEn\": 1 \
          }, \
          \"txIqdmDuc\": { \
            \"iqdmDucMode\": 2, \
            \"iqdmDev\": 0, \
            \"iqdmDevOffset\": 0, \
            \"iqdmScalar\": 0, \
            \"iqdmThreshold\": 0, \
            \"iqdmNco\": { \
              \"freq\": 0, \
              \"sampleFreq\": 61440000, \
              \"phase\": 0, \
              \"realOut\": 0 \
            } \
          } \
        }, \
        \"txSsiConfig\": { \
          \"ssiType\": 1, \
          \"ssiDataFormatSel\": 4, \
          \"numLaneSel\": 0, \
          \"strobeType\": 0, \
          \"lsbFirst\": 0, \
          \"qFirst\": 0, \
          \"txRefClockPin\": 1, \
          \"lvdsIBitInversion\": false, \
          \"lvdsQBitInversion\": false, \
          \"lvdsStrobeBitInversion\": false, \
          \"lvdsUseLsbIn12bitMode\": 0, \
          \"lvdsRxClkInversionEn\": false, \
          \"cmosDdrPosClkEn\": false, \
          \"cmosClkInversionEn\": false, \
          \"ddrEn\": false, \
          \"rxMaskStrobeEn\": false \
        } \
      }, { \
        \"primarySigBandwidth_Hz\": 1008000, \
        \"txInputRate_Hz\": 1920000, \
        \"txInterfaceSampleRate_Hz\": 1920000, \
        \"txOffsetLo_kHz\": 0, \
        \"validDataDelay\": 0, \
        \"txBbf3dBCorner_kHz\": 50000, \
        \"outputSignaling\": 0, \
        \"txPdBiasCurrent\": 1, \
        \"txPdGainEnable\": 0, \
        \"txPrePdRealPole_kHz\": 1000000, \
        \"txPostPdRealPole_kHz\": 530000, \
        \"txBbfPower\": 2, \
        \"txExtLoopBackType\": 0, \
        \"txExtLoopBackForInitCal\": 0, \
        \"txPeakLoopBackPower\": 0, \
        \"frequencyDeviation_Hz\": 0, \
        \"txDpProfile\": { \
          \"txPreProc\": { \
            \"txPreProcSymbol0\": 0, \
            \"txPreProcSymbol1\": 0, \
            \"txPreProcSymbol2\": 0, \
            \"txPreProcSymbol3\": 0, \
            \"txPreProcSymMapDivFactor\": 1, \
            \"txPreProcMode\": 1, \
            \"txPreProcWbNbPfirIBankSel\": 2, \
            \"txPreProcWbNbPfirQBankSel\": 3 \
          }, \
          \"txWbIntTop\": { \
            \"txInterpBy2Blk30En\": 1, \
            \"txInterpBy2Blk28En\": 1, \
            \"txInterpBy2Blk26En\": 1, \
            \"txInterpBy2Blk24En\": 1, \
            \"txInterpBy2Blk22En\": 1, \
            \"txWbLpfBlk22p1En\": 0 \
          }, \
          \"txNbIntTop\": { \
            \"txInterpBy2Blk20En\": 0, \
            \"txInterpBy2Blk18En\": 0, \
            \"txInterpBy2Blk16En\": 0, \
            \"txInterpBy2Blk14En\": 0, \
            \"txInterpBy2Blk12En\": 0, \
            \"txInterpBy3Blk10En\": 0, \
            \"txInterpBy2Blk8En\": 0, \
            \"txScicBlk32En\": 0, \
            \"txScicBlk32DivFactor\": 1 \
          }, \
          \"txIntTop\": { \
            \"interpBy3Blk44p1En\": 1, \
            \"sinc3Blk44En\": 0, \
            \"sinc2Blk42En\": 0, \
            \"interpBy3Blk40En\": 1, \
            \"interpBy2Blk38En\": 0, \
            \"interpBy2Blk36En\": 0 \
          }, \
          \"txIntTopFreqDevMap\": { \
            \"rrc2Frac\": 0, \
            \"mpll\": 0, \
            \"nchLsw\": 0, \
            \"nchMsb\": 0, \
            \"freqDevMapEn\": 0, \
            \"txRoundEn\": 1 \
          }, \
          \"txIqdmDuc\": { \
            \"iqdmDucMode\": 2, \
            \"iqdmDev\": 0, \
            \"iqdmDevOffset\": 0, \
            \"iqdmScalar\": 0, \
            \"iqdmThreshold\": 0, \
            \"iqdmNco\": { \
              \"freq\": 0, \
              \"sampleFreq\": 61440000, \
              \"phase\": 0, \
              \"realOut\": 0 \
            } \
          } \
        }, \
        \"txSsiConfig\": { \
          \"ssiType\": 1, \
          \"ssiDataFormatSel\": 4, \
          \"numLaneSel\": 0, \
          \"strobeType\": 0, \
          \"lsbFirst\": 0, \
          \"qFirst\": 0, \
          \"txRefClockPin\": 1, \
          \"lvdsIBitInversion\": false, \
          \"lvdsQBitInversion\": false, \
          \"lvdsStrobeBitInversion\": false, \
          \"lvdsUseLsbIn12bitMode\": 0, \
          \"lvdsRxClkInversionEn\": false, \
          \"cmosDdrPosClkEn\": false, \
          \"cmosClkInversionEn\": false, \
          \"ddrEn\": false, \
          \"rxMaskStrobeEn\": false \
        } \
      } ] \
  }, \
  \"sysConfig\": { \
    \"duplexMode\": 1, \
    \"fhModeOn\": 0, \
    \"numDynamicProfiles\": 1, \
    \"mcsMode\": 0, \
    \"mcsInterfaceType\": 0, \
    \"adcTypeMonitor\": 1, \
    \"pllLockTime_us\": 380, \
    \"pllPhaseSyncWait_us\": 0, \
    \"pllModulus\": { \
      \"modulus\": [ 8388593, 8388593, 8388593, 8388593, 8388593 ], \
      \"dmModulus\": [ 8388593, 8388593 ] \
    }, \
    \"warmBootEnable\": false \
  }, \
  \"pfirBuffer\": { \
    \"pfirRxWbNbChFilterCoeff_A\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -392, -228, 1164, -426, -2275, 3118, 1517, -7492, 4673, 8616, -16569, 2081, 24816, -27732, -11318, 53353, -35059, -44826, 94798, -27039, -110096, 145573, 15827, -224287, 197850, 132551, -432387, 241249, 465740, -1004873, 265939, 4480229, 4480229, 265939, -1004873, 465740, 241249, -432387, 132551, 197850, -224287, 15827, 145573, -110096, -27039, 94798, -44826, -35059, 53353, -11318, -27732, 24816, 2081, -16569, 8616, 4673, -7492, 1517, 3118, -2275, -426, 1164, -228, -392, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirRxWbNbChFilterCoeff_B\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirRxWbNbChFilterCoeff_C\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -392, -228, 1164, -426, -2275, 3118, 1517, -7492, 4673, 8616, -16569, 2081, 24816, -27732, -11318, 53353, -35059, -44826, 94798, -27039, -110096, 145573, 15827, -224287, 197850, 132551, -432387, 241249, 465740, -1004873, 265939, 4480229, 4480229, 265939, -1004873, 465740, 241249, -432387, 132551, 197850, -224287, 15827, 145573, -110096, -27039, 94798, -44826, -35059, 53353, -11318, -27732, 24816, 2081, -16569, 8616, 4673, -7492, 1517, 3118, -2275, -426, 1164, -228, -392, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirRxWbNbChFilterCoeff_D\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirTxWbNbPulShpCoeff_A\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirTxWbNbPulShpCoeff_B\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirTxWbNbPulShpCoeff_C\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirTxWbNbPulShpCoeff_D\": { \
      \"numCoeff\": 128, \
      \"symmetricSel\": 0, \
      \"tapsSel\": 3, \
      \"gainSel\": 2, \
      \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
    }, \
    \"pfirRxNbPulShp\": [ { \
        \"numCoeff\": 128, \
        \"symmetricSel\": 0, \
        \"taps\": 128, \
        \"gainSel\": 2, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      }, { \
        \"numCoeff\": 128, \
        \"symmetricSel\": 0, \
        \"taps\": 128, \
        \"gainSel\": 2, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      } ], \
    \"pfirRxMagLowTiaLowSRHp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -12, 83, -293, 734, -1489, 2594, -3965, 5403, -6516, 5868, 27957, 5868, -6516, 5403, -3965, 2594, -1489, 734, -293, 83, -12 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -12, 83, -293, 734, -1489, 2594, -3965, 5403, -6516, 5868, 27957, 5868, -6516, 5403, -3965, 2594, -1489, 734, -293, 83, -12 ] \
      } ], \
    \"pfirRxMagLowTiaHighSRHp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -62, 194, 80, -829, 201, 1857, -179, -4602, -1259, 11431, 19102, 11431, -1259, -4602, -179, 1857, 201, -829, 80, 194, -62 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -62, 194, 80, -829, 201, 1857, -179, -4602, -1259, 11431, 19102, 11431, -1259, -4602, -179, 1857, 201, -829, 80, 194, -62 ] \
      } ], \
    \"pfirRxMagHighTiaHighSRHp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ 39, -229, 714, -1485, 2134, -1844, -219, 4147, -8514, 8496, 26292, 8496, -8514, 4147, -219, -1844, 2134, -1485, 714, -229, 39 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ 39, -229, 714, -1485, 2134, -1844, -219, 4147, -8514, 8496, 26292, 8496, -8514, 4147, -219, -1844, 2134, -1485, 714, -229, 39 ] \
      } ], \
    \"pfirRxMagLowTiaLowSRLp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -12, 83, -293, 733, -1488, 2593, -3963, 5401, -6514, 5870, 27953, 5870, -6514, 5401, -3963, 2593, -1488, 733, -293, 83, -12 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -12, 83, -293, 733, -1488, 2593, -3963, 5401, -6514, 5870, 27953, 5870, -6514, 5401, -3963, 2593, -1488, 733, -293, 83, -12 ] \
      } ], \
    \"pfirRxMagLowTiaHighSRLp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -62, 194, 80, -828, 201, 1855, -180, -4597, -1254, 11428, 19093, 11428, -1254, -4597, -180, 1855, 201, -828, 80, 194, -62 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -62, 194, 80, -828, 201, 1855, -180, -4597, -1254, 11428, 19093, 11428, -1254, -4597, -180, 1855, 201, -828, 80, 194, -62 ] \
      } ], \
    \"pfirRxMagHighTiaHighSRLp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ 39, -229, 712, -1481, 2128, -1841, -215, 4131, -8490, 8497, 26266, 8497, -8490, 4131, -215, -1841, 2128, -1481, 712, -229, 39 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ 39, -229, 712, -1481, 2128, -1841, -215, 4131, -8490, 8497, 26266, 8497, -8490, 4131, -215, -1841, 2128, -1481, 712, -229, 39 ] \
      } ], \
    \"pfirTxMagComp1\": { \
      \"numCoeff\": 21, \
      \"coefficients\": [ 69, -384, 1125, -2089, 2300, -165, -5248, 12368, -13473, 4864, 34039, 4864, -13473, 12368, -5248, -165, 2300, -2089, 1125, -384, 69 ] \
    }, \
    \"pfirTxMagComp2\": { \
      \"numCoeff\": 21, \
      \"coefficients\": [ 69, -384, 1125, -2089, 2300, -165, -5248, 12368, -13473, 4864, 34039, 4864, -13473, 12368, -5248, -165, 2300, -2089, 1125, -384, 69 ] \
    }, \
    \"pfirTxMagCompNb\": [ { \
        \"numCoeff\": 13, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      }, { \
        \"numCoeff\": 13, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      } ], \
    \"pfirRxMagCompNb\": [ { \
        \"numCoeff\": 13, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      }, { \
        \"numCoeff\": 13, \
        \"coefficients\": [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] \
      } ] \
  } \
}";
