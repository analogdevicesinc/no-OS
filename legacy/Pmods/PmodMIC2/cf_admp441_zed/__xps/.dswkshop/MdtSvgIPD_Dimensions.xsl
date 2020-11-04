<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
       xmlns:svg="http://www.w3.org/2000/svg"
	   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
       xmlns:exsl="http://exslt.org/common"
       xmlns:dyn="http://exslt.org/dynamic"
       xmlns:math="http://exslt.org/math"
       xmlns:xlink="http://www.w3.org/1999/xlink"
       extension-element-prefixes="math dyn exsl xlink">
                           
<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"
	       doctype-public="-//W3C//DTD SVG 1.0//EN"
		   doctype-system="http://www.w3.org/TR/SVG/DTD/svg10.dtd"/>

<!-- 
	======================================================
			BUS INTERFACE DIMENSIONS
	======================================================
-->				
	
<xsl:variable name="IPD_HGAP"         select="16"/>				
<xsl:variable name="IPD_VGAP"         select="16"/>

<xsl:variable name="IPD_HPORT_GAP"    select="16"/>				

<xsl:variable name="IPD_CENTER_W"      select="180"/>

<xsl:variable name="IPD_PRT_W"         select="8"/>
<xsl:variable name="IPD_PRT_H"         select="8"/>
<xsl:variable name="IPD_PRT_SY"        select="8"/>
<xsl:variable name="IPD_PRT_SPC"       select="16"/>
	
<!-- 
	======================================================
			BUS INTERFACE DIMENSIONS
	======================================================
-->				
	
<xsl:variable name="IPD_BIF_H"     select="16"/>				
<xsl:variable name="IPD_BIF_W"     select="32"/>				
	
<xsl:variable name="IPD_BIFC_H"    select="24"/>				
<xsl:variable name="IPD_BIFC_W"    select="24"/>				

<xsl:variable name="IPD_BIFC_dx"   select="ceiling($IPD_BIFC_W div 5)"/>
<xsl:variable name="IPD_BIFC_dy"   select="ceiling($IPD_BIFC_H div 5)"/>
<xsl:variable name="IPD_BIFC_Hi"   select="($IPD_BIFC_H - ($IPD_BIFC_dy * 2))"/>	
<xsl:variable name="IPD_BIFC_Wi"   select="($IPD_BIFC_W - ($IPD_BIFC_dx * 2))"/>

<xsl:variable name="IPD_BIF_TYPE_ONEWAY"  select="'OneWay'"/>
	
<!-- 
	======================================================
			GLOLBAL BUS INTERFACE DIMENSIONS
		(Define for global MdtSVG_BifShapes.xsl which is used across all
	     diagrams to define the shapes of bifs the same across all diagrams)
	======================================================
-->	
	
<xsl:variable name="BIF_H"     select="$IPD_BIF_H"/>				
<xsl:variable name="BIF_W"     select="$IPD_BIF_W"/>
	
<xsl:variable name="BIFC_H"    select="$IPD_BIFC_H"/>
<xsl:variable name="BIFC_W"    select="$IPD_BIFC_W"/>
	
<xsl:variable name="BIFC_dx"   select="$IPD_BIFC_dx"/>
<xsl:variable name="BIFC_dy"   select="$IPD_BIFC_dy"/>
	
<xsl:variable name="BIFC_Hi"   select="$IPD_BIFC_Hi"/>	
<xsl:variable name="BIFC_Wi"   select="$IPD_BIFC_Wi"/>	
	
	
	
</xsl:stylesheet>
