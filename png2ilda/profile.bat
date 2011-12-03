VSInstr Profile\png2ilda.exe /ExcludeSmallFuncs
VSPerfCmd /start:trace /output:profile.vsp
Profile\png2ilda.exe shortData
VSPerfCmd /shutdown
VSPerfReport profile.vsp /summary:all
pause