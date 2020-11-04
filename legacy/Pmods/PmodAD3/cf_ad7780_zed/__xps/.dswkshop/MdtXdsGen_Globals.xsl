<?xml version="1.0" standalone="no"?>

<!DOCTYPE stylesheet [
	<!ENTITY UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ">
	<!ENTITY LOWERCASE "abcdefghijklmnopqrstuvwxyz">
	
	<!ENTITY UPPER2LOWER " '&UPPERCASE;' , '&LOWERCASE;' ">
	<!ENTITY LOWER2UPPER " '&LOWERCASE;' , '&UPPERCASE;' ">
	
	<!ENTITY ALPHALOWER "ABCDEFxX0123456789">
	<!ENTITY HEXUPPER "ABCDEFxX0123456789">
	<!ENTITY HEXLOWER "abcdefxX0123456789">
	<!ENTITY HEXU2L " '&HEXLOWER;' , '&HEXUPPER;' ">
	
	<!ENTITY ALLMODS "MODULE[(@INSTANCE)]">
	<!ENTITY BUSMODS "MODULE[(@MODCLASS ='BUS')]">
	<!ENTITY CPUMODS "MODULE[(@MODCLASS ='PROCESSOR')]">
	
	<!ENTITY MODPRMS "MODULE/PARAMETERS/PARAMETER">
	
	<!ENTITY MODIOFS "MODULE/IOINTERFACES/IOINTERFACE">
	<!ENTITY ALLIOFS "&MODIOFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE'))]">
	
	<!ENTITY MODBIFS "MODULE/BUSINTERFACES/BUSINTERFACE">
	<!ENTITY ALLBIFS "&MODBIFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE'))]">
	<!ENTITY MSTBIFS "&MODBIFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE')) and  (@TYPE = 'MASTER')]">
	<!ENTITY SLVBIFS "&MODBIFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE')) and  (@TYPE = 'SLAVE')]">
	<!ENTITY MOSBIFS "&MODBIFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE')) and ((@TYPE = 'MASTER') or (@TYPE = 'SLAVE'))]">
	<!ENTITY P2PBIFS "&MODBIFS;[(not(@IS_VALID) or (@IS_VALID = 'TRUE')) and ((@TYPE = 'TARGET') or (@TYPE = 'INITIATOR'))]">	
	
	<!ENTITY MODPORTS "MODULE/PORTS/PORT">
	<!ENTITY ALLPORTS "&MODPORTS;[ (not(@IS_VALID) or (@IS_VALID = 'TRUE'))]">
	<!ENTITY NDFPORTS "&MODPORTS;[((not(@IS_VALID) or (@IS_VALID = 'TRUE')) and (not(@BUS) and not(@IOS)))]">
	<!ENTITY DEFPORTS "&MODPORTS;[((not(@IS_VALID) or (@IS_VALID = 'TRUE')) and ((@BUS) or (@IOS)))]">
]>

<xsl:stylesheet version="1.0"
	   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
       xmlns:exsl="http://exslt.org/common"
       xmlns:dyn="http://exslt.org/dynamic"
       xmlns:math="http://exslt.org/math"
       xmlns:xlink="http://www.w3.org/1999/xlink"
       extension-element-prefixes="math dyn exsl xlink">
                
<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"
	       doctype-public="-//W3C//DTD SVG 1.0//EN"
		   doctype-system="http://www.w3.org/TR/SVG/DTD/svg10.dtd"/>
		   
<xsl:variable name="G_ROOT"     select="/"/>
<xsl:variable name="G_EDKSYS"   select="$G_ROOT/EDKSYSTEM"/>
<xsl:variable name="G_MODULES"  select="$G_EDKSYS/MODULES"/>

<xsl:variable name="G_EDKVERSION">
<xsl:if test="$G_ROOT/EDKSYSTEM/@EDKVERSION"><xsl:value-of select="$G_ROOT/EDKSYSTEM/@EDKVERSION"/></xsl:if>
<xsl:if test="not($G_ROOT/EDKSYSTEM/@EDKVERSION)">NA</xsl:if>
</xsl:variable>

<xsl:variable name="G_TIMESTAMP">
<xsl:if test="$G_ROOT/EDKSYSTEM/@TIMESTAMP"><xsl:value-of select="$G_ROOT/EDKSYSTEM/@TIMESTAMP"/></xsl:if>
<xsl:if test="not($G_ROOT/EDKSYSTEM/@TIMESTAMP)">NA</xsl:if>
</xsl:variable>

<xsl:variable name="G_FAMILY">
<xsl:if test="$G_ROOT/EDKSYSTEM/SYSTEMINFO/@ARCH"><xsl:value-of select="$G_ROOT/EDKSYSTEM/SYSTEMINFO/@ARCH"/></xsl:if>
<xsl:if test="not($G_ROOT/EDKSYSTEM/SYSTEMINFO/@ARCH)">NA</xsl:if>
</xsl:variable>

<xsl:variable name="G_DEVICE">
<xsl:if test="$G_ROOT/EDKSYSTEM/SYSTEMINFO/@PART"><xsl:value-of select="$G_ROOT/EDKSYSTEM/SYSTEMINFO/@PART"/></xsl:if>
<xsl:if test="not($G_ROOT/EDKSYSTEM/SYSTEMINFO/@PART)">NA</xsl:if>
</xsl:variable>

<xsl:variable name="G_XPS_TITLE">XPS Project Report</xsl:variable>

<xsl:variable name="G_TOC_WIDTH" 		select="200"/>
<xsl:variable name="G_MAIN_WIDTH"		select="850"/>

<xsl:variable name="G_OVSPECFICS_WIDTH"		select="300"/>
<xsl:variable name="G_OVRESOURCES_WIDTH"	select="550"/>

<xsl:variable name="G_NUMOF_IPS"  		 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'IP')])"/>
<xsl:variable name="G_NUMOF_BUSSES"  	 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[((@MODCLASS = 'BUS') or (@MODCLASS = 'BUS_ARBITER'))])"/>
<xsl:variable name="G_NUMOF_BRIDGES"  	 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'BUS_BRIDGE')])"/>
<xsl:variable name="G_NUMOF_MEMORYS"  	 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY')])"/>
<xsl:variable name="G_NUMOF_DEBUGGERS"  	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'DEBUG')])"/>
<xsl:variable name="G_NUMOF_PROCESSORS" 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PROCESSOR')])"/>
<xsl:variable name="G_NUMOF_PERIPHERALS" 	  select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PERIPHERAL')])"/>
<xsl:variable name="G_NUMOF_MEMORY_CNTLRS"    select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY_CNTLR')])"/>
<xsl:variable name="G_NUMOF_INTERRUPT_CNLTRS" select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'INTERRUPT_CNTLR')])"/>
<xsl:variable name="G_NUMOF_MODULES_W_TIMING" select="count($G_ROOT/EDKSYSTEM/MODULES/MODULE/TIMING)"/>

<!--  INDEX KEYS FOR FAST ACCESS  -->
<xsl:key name="G_MAP_MODULES"   	match="&ALLMODS;" use="@INSTANCE"/>
<xsl:key name="G_MAP_MODULES"   	match="&ALLMODS;" use="@MODCLASS"/>
<xsl:key name="G_MAP_MODULES"   	match="&ALLMODS;" use="@MODTYPE"/>
<xsl:key name="G_MAP_PROCESSORS"   	match="&CPUMODS;" use="@INSTANCE"/>

<xsl:key name="G_MAP_BUSSES"   		match="&BUSMODS;" use="@INSTANCE"/>
<xsl:key name="G_MAP_BUSSES"   		match="&BUSMODS;" use="@BUSSTD"/>
<xsl:key name="G_MAP_BUSSES" 	  	match="&BUSMODS;" use="@BUSSTD_PSF"/>

<xsl:key name="G_MAP_PARAMETERS"   	match="&MODPRMS;" use="../../@INSTANCE"/>

<xsl:key name="G_MAP_ALL_IOFS"   	match="&ALLIOFS;" use="../../@INSTANCE"/>
<xsl:key name="G_MAP_ALL_BIFS"   	match="&ALLBIFS;" use="../../@INSTANCE"/>

<xsl:key name="G_MAP_ALL_BIFS_BY_BUS" match="&ALLBIFS;" use="@BUSNAME"/>

<xsl:key name="G_MAP_MST_BIFS"   	match="&MSTBIFS;" use="@BUSNAME"/>
<xsl:key name="G_MAP_SLV_BIFS"   	match="&SLVBIFS;" use="@BUSNAME"/>
<xsl:key name="G_MAP_MOS_BIFS"   	match="&MOSBIFS;" use="@BUSNAME"/>

<xsl:key name="G_MAP_P2P_BIFS"   	match="&P2PBIFS;" use="@BUSNAME"/>
<xsl:key name="G_MAP_P2P_BIFS"   	match="&P2PBIFS;" use="@BUSSTD"/>
<xsl:key name="G_MAP_P2P_BIFS"   	match="&P2PBIFS;" use="@BUSSTD_PSF"/>

<xsl:key name="G_MAP_ALL_PORTS"   	match="&ALLPORTS;" use="../../@INSTANCE"/>
<xsl:key name="G_MAP_DEF_PORTS"   	match="&DEFPORTS;" use="../../@INSTANCE"/> <!-- Default ports -->
<xsl:key name="G_MAP_NDF_PORTS"   	match="&NDFPORTS;" use="../../@INSTANCE"/> <!-- Non Default ports -->

<xsl:variable name="G_BIFTYPES">

	<BIFTYPE TYPE="SLAVE"/>
	<BIFTYPE TYPE="MASTER"/>
	<BIFTYPE TYPE="MASTER_SLAVE"/>
	
	<BIFTYPE TYPE="TARGET"/>
	<BIFTYPE TYPE="INITIATOR"/>
	
	<BIFTYPE TYPE="MONITOR"/>
	
	<BIFTYPE TYPE="USER"/>
	<BIFTYPE TYPE="TRANSPARENT"/>
</xsl:variable>	

<xsl:variable name="G_BUSSTDS">
	
	<BUSSTD NAME="AXI"/>
	<BUSSTD NAME="AXIS"/>
	<BUSSTD NAME="XIL"/>
	<BUSSTD NAME="OCM"/>
	<BUSSTD NAME="OPB"/>
	<BUSSTD NAME="LMB"/>
	<BUSSTD NAME="FSL"/>
	<BUSSTD NAME="DCR"/>
	<BUSSTD NAME="FCB"/>
	
	<BUSSTD NAME="PLB"/>
	<BUSSTD NAME="PLB_P2P"/>
	<BUSSTD NAME="PLBV34"/>
	<BUSSTD NAME="PLBV34_P2P"/>
	<BUSSTD NAME="PLBV46"/>
	<BUSSTD NAME="PLBV46_P2P"/>

	<BUSSTD NAME="USER"/>
	
	<BUSSTD NAME="TRS"/>
	<BUSSTD NAME="TRANS"/>
	<BUSSTD NAME="TRANSPARENT"/>
</xsl:variable>


</xsl:stylesheet>
