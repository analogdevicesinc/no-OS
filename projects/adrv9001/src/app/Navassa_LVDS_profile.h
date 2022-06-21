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
          \"primarySigBandwidth_Hz\": 9000000, \
          \"rxOutputRate_Hz\": 15360000, \
          \"rxInterfaceSampleRate_Hz\": 15360000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 20000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 10000, \
          \"rxCorner3dBLp_kHz\": 10000, \
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
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
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
                  \"sampleFreq\": 15360000, \
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
            \"minGainIndex\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 2, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 1, \
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
            \"ddrEn\": true, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 9000000, \
          \"rxOutputRate_Hz\": 15360000, \
          \"rxInterfaceSampleRate_Hz\": 15360000, \
          \"rxOffsetLo_kHz\": 0, \
          \"rxNcoEnable\": false, \
          \"outputSignaling\": 0, \
          \"filterOrder\": 1, \
          \"filterOrderLp\": 1, \
          \"hpAdcCorner\": 20000000, \
          \"lpAdcCorner\": 0, \
          \"adcClk_kHz\": 2211840, \
          \"rxCorner3dB_kHz\": 10000, \
          \"rxCorner3dBLp_kHz\": 10000, \
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
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
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
                  \"sampleFreq\": 15360000, \
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
            \"minGainIndex\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 2, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 1, \
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
            \"ddrEn\": true, \
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
            \"minGainIndex\": 0 \
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
            \"minGainIndex\": 0 \
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
          \"primarySigBandwidth_Hz\": 9000000, \
          \"rxOutputRate_Hz\": 15360000, \
          \"rxInterfaceSampleRate_Hz\": 15360000, \
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
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
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
            \"minGainIndex\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 2, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 1, \
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
            \"ddrEn\": true, \
            \"rxMaskStrobeEn\": false \
          } \
        } \
      }, { \
        \"profile\": { \
          \"primarySigBandwidth_Hz\": 9000000, \
          \"rxOutputRate_Hz\": 15360000, \
          \"rxInterfaceSampleRate_Hz\": 15360000, \
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
              \"decBy2Blk25En\": 0, \
              \"decBy2Blk27En\": 0, \
              \"decBy2Blk29En\": 0, \
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
            \"minGainIndex\": 0 \
          }, \
          \"rxSsiConfig\": { \
            \"ssiType\": 2, \
            \"ssiDataFormatSel\": 4, \
            \"numLaneSel\": 1, \
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
            \"ddrEn\": true, \
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
            \"minGainIndex\": 0 \
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
            \"minGainIndex\": 0 \
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
        \"primarySigBandwidth_Hz\": 9000000, \
        \"txInputRate_Hz\": 15360000, \
        \"txInterfaceSampleRate_Hz\": 15360000, \
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
            \"txInterpBy2Blk30En\": 0, \
            \"txInterpBy2Blk28En\": 0, \
            \"txInterpBy2Blk26En\": 0, \
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
          \"ssiType\": 2, \
          \"ssiDataFormatSel\": 4, \
          \"numLaneSel\": 1, \
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
          \"ddrEn\": true, \
          \"rxMaskStrobeEn\": false \
        } \
      }, { \
        \"primarySigBandwidth_Hz\": 9000000, \
        \"txInputRate_Hz\": 15360000, \
        \"txInterfaceSampleRate_Hz\": 15360000, \
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
            \"txInterpBy2Blk30En\": 0, \
            \"txInterpBy2Blk28En\": 0, \
            \"txInterpBy2Blk26En\": 0, \
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
          \"ssiType\": 2, \
          \"ssiDataFormatSel\": 4, \
          \"numLaneSel\": 1, \
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
          \"ddrEn\": true, \
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
    \"pllLockTime_us\": 750, \
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
      \"coefficients\": [ 475, 312, -782, -39, 1201, -777, -1182, 1981, 177, -2874, 1941, 2393, -4416, 225, 5594, -4581, -3668, 8650, -1992, -9342, 9646, 4213, -15137, 6404, 13615, -18199, -2610, 23969, -15142, -17198, 31204, -3269, -34604, 30213, 17955, -49337, 16361, 45636, -53954, -12567, 72920, -40769, -54562, 89506, -4148, -102269, 83183, 57280, -142874, 41767, 139213, -158628, -45955, 231679, -125964, -193870, 320642, -4532, -442087, 390927, 347244, -1055854, 429729, 4391599, 4391599, 429729, -1055854, 347244, 390927, -442087, -4532, 320642, -193870, -125964, 231679, -45955, -158628, 139213, 41767, -142874, 57280, 83183, -102269, -4148, 89506, -54562, -40769, 72920, -12567, -53954, 45636, 16361, -49337, 17955, 30213, -34604, -3269, 31204, -17198, -15142, 23969, -2610, -18199, 13615, 6404, -15137, 4213, 9646, -9342, -1992, 8650, -3668, -4581, 5594, 225, -4416, 2393, 1941, -2874, 177, 1981, -1182, -777, 1201, -39, -782, 312, 0 ] \
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
      \"coefficients\": [ 475, 312, -782, -39, 1201, -777, -1182, 1981, 177, -2874, 1941, 2393, -4416, 225, 5594, -4581, -3668, 8650, -1992, -9342, 9646, 4213, -15137, 6404, 13615, -18199, -2610, 23969, -15142, -17198, 31204, -3269, -34604, 30213, 17955, -49337, 16361, 45636, -53954, -12567, 72920, -40769, -54562, 89506, -4148, -102269, 83183, 57280, -142874, 41767, 139213, -158628, -45955, 231679, -125964, -193870, 320642, -4532, -442087, 390927, 347244, -1055854, 429729, 4391599, 4391599, 429729, -1055854, 347244, 390927, -442087, -4532, 320642, -193870, -125964, 231679, -45955, -158628, 139213, 41767, -142874, 57280, 83183, -102269, -4148, 89506, -54562, -40769, 72920, -12567, -53954, 45636, 16361, -49337, 17955, 30213, -34604, -3269, 31204, -17198, -15142, 23969, -2610, -18199, 13615, 6404, -15137, 4213, 9646, -9342, -1992, 8650, -3668, -4581, 5594, 225, -4416, 2393, 1941, -2874, 177, 1981, -1182, -777, 1201, -39, -782, 312, 0 ] \
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
        \"coefficients\": [ -346, 1329, -1178, -1281, 1308, 2727, -1341, -5852, -1018, 12236, 19599, 12236, -1018, -5852, -1341, 2727, 1308, -1281, -1178, 1329, -346 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -346, 1329, -1178, -1281, 1308, 2727, -1341, -5852, -1018, 12236, 19599, 12236, -1018, -5852, -1341, 2727, 1308, -1281, -1178, 1329, -346 ] \
      } ], \
    \"pfirRxMagLowTiaHighSRHp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -2474, 892, 6049, 4321, -7599, -15881, -6074, 13307, 18926, 6338, -2843, 6338, 18926, 13307, -6074, -15881, -7599, 4321, 6049, 892, -2474 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -2474, 892, 6049, 4321, -7599, -15881, -6074, 13307, 18926, 6338, -2843, 6338, 18926, 13307, -6074, -15881, -7599, 4321, 6049, 892, -2474 ] \
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
        \"coefficients\": [ -346, 1329, -1178, -1281, 1308, 2727, -1341, -5851, -1018, 12236, 19599, 12236, -1018, -5851, -1341, 2727, 1308, -1281, -1178, 1329, -346 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -346, 1329, -1178, -1281, 1308, 2727, -1341, -5851, -1018, 12236, 19599, 12236, -1018, -5851, -1341, 2727, 1308, -1281, -1178, 1329, -346 ] \
      } ], \
    \"pfirRxMagLowTiaHighSRLp\": [ { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -2473, 892, 6048, 4321, -7598, -15879, -6072, 13306, 18924, 6338, -2843, 6338, 18924, 13306, -6072, -15879, -7598, 4321, 6048, 892, -2473 ] \
      }, { \
        \"numCoeff\": 21, \
        \"coefficients\": [ -2473, 892, 6048, 4321, -7598, -15879, -6072, 13306, 18924, 6338, -2843, 6338, 18924, 13306, -6072, -15879, -7598, 4321, 6048, 892, -2473 ] \
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