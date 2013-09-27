<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
           xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
           xmlns:exsl="http://exslt.org/common">
           
<!--  Use the same general colors for primary colors and mapped colors, 
      (e.g colors of bus standards) as the diagrams
-->
<xsl:include href="MdtSvgDiag_Colors.xsl"/>

<xsl:variable name="COL_INFO"    	select="'#2233FF'"/>

<xsl:variable name="COL_ATTR_BUF"   select="'#FF5555'"/>
<xsl:variable name="COL_ATTR_CLK"   select="'#22FF22'"/>
<xsl:variable name="COL_ATTR_INT"   select="'#5555FF'"/>
<xsl:variable name="COL_ATTR_RST"   select="'#FFCC00'"/>

<xsl:variable name="COL_CHG_USR"     select="'#FFFFAA'"/>
<xsl:variable name="COL_CHG_SYS"     select="'#AAAAFF'"/>

<!-- 
<xsl:variable name="COL_MODSYSNW"   select="'#000099'"/>
 -->

</xsl:stylesheet>
