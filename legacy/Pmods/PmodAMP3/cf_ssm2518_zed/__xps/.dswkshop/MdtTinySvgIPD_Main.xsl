<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
	   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
       xmlns:exsl="http://exslt.org/common"
       xmlns:dyn="http://exslt.org/dynamic"
       xmlns:math="http://exslt.org/math"
       xmlns:xlink="http://www.w3.org/1999/xlink"
       extension-element-prefixes="math dyn exsl xlink">
           

		   
<xsl:include href="MdtSvgDiag_Colors.xsl"/>
<xsl:include href="MdtSvgDiag_Globals.xsl"/>
<xsl:include href="MdtSvgDiag_StyleDefs.xsl"/>

<xsl:include href="MdtSvgIPD_Dimensions.xsl"/>

<xsl:include href="MdtTinySvgDiag_BifShapes.xsl"/>

<xsl:param name="INSTANCE" select="'microblaze_0'"/>

<xsl:variable name="G_ROOT" select="/"/>

<!-- ======================= main svg block =============================== -->
<xsl:output method="xml" 
			indent="yes"
			version="1.0" 
			encoding="UTF-8" 
	        doctype-public="-//W3C//DTD SVG 1.0//EN"
		    doctype-system="http://www.w3.org/TR/SVG/DTD/svg10.dtd"/>
		    
<xsl:template match="EDKSYSTEM">
	
<xsl:if test="not($G_ROOT/EDKSYSTEM/MODULES/MODULE[(@INSTANCE= $INSTANCE)])">
	<xsl:message>XSLT WARNING: Could not find <xsl:value-of select="$INSTANCE"/> in project's XML.</xsl:message>
</xsl:if>

<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@INSTANCE= $INSTANCE)]">
	
<xsl:variable name="ports_left_cnt_" select="count(PORTS/PORT[((@IS_INSTANTIATED = 'TRUE') and ((@DIR='I' or @DIR = 'IO') and not(@SIGNAME = '__NOC__') and not(@SIGNAME = '__DEF__') and (@IS_INSTANTIATED = 'TRUE')))])"/>	
<xsl:variable name="ports_rght_cnt_" select="count(PORTS/PORT[((@IS_INSTANTIATED  = 'TRUE') and ((@DIR='O') and not(@SIGNAME = '__NOC__') and not(@SIGNAME = '__DEF__')))])"/>	
	
<xsl:variable name="bifs_left_cnt_"  select="count(BUSINTERFACES/BUSINTERFACE[((@IS_INSTANTIATED = 'TRUE') and not(@BUSNAME = '__DEF__') and not(@BUSNAME = '__NOC__') and ((@IS_VALID = 'TRUE') or not(@IS_VALID)) and ((@TYPE = 'SLAVE')  or (@TYPE = 'TARGET')       or (@TYPE ='USER')))])"/>
<xsl:variable name="bifs_rght_cnt_"  select="count(BUSINTERFACES/BUSINTERFACE[((@IS_INSTANTIATED = 'TRUE') and not(@BUSNAME = '__DEF__') and not(@BUSNAME = '__NOC__') and ((@IS_VALID = 'TRUE') or not(@IS_VALID)) and ((@TYPE = 'MASTER') or (@TYPE = 'MASTER_SLAVE') or (@TYPE = 'INITIATOR') or (@TYPE ='MONITOR')))])"/>

<!-- 
<xsl:message>Port Left Count <xsl:value-of select="$ports_left_cnt_"/></xsl:message>
<xsl:message>Port Rght Count <xsl:value-of select="$ports_rght_cnt_"/></xsl:message>
	
<xsl:message>Bifs Left Count <xsl:value-of select="$bifs_left_cnt_"/></xsl:message>
<xsl:message>Bifs Rght Count <xsl:value-of select="$bifs_rght_cnt_"/></xsl:message>
-->	

<!-- select and return the maximum between the two left and right of ports -->
<xsl:variable name="PORTS_H">
	<xsl:choose>
		
		<xsl:when test="(($ports_left_cnt_ &lt; 2) and ($ports_rght_cnt_ &lt; 2))">
			<xsl:value-of select="(($IPD_PRT_H + $IPD_PRT_SPC) * 2) + $IPD_PRT_SY"/>
		</xsl:when>
		
		<!-- make sure the module is at least 2 port spaces high -->
		<xsl:when test="($ports_left_cnt_ &gt; $ports_rght_cnt_)">
			<xsl:value-of select="(($IPD_PRT_H + $IPD_PRT_SPC) * $ports_left_cnt_) + $IPD_PRT_SY"/>
		</xsl:when>
			
		<xsl:when test="($ports_rght_cnt_ &gt; $ports_left_cnt_)">
			<xsl:value-of select="(($IPD_PRT_H + $IPD_PRT_SPC) * $ports_rght_cnt_) + $IPD_PRT_SY"/>
		</xsl:when>
		
		<xsl:when test="($ports_left_cnt_ = $ports_rght_cnt_)">
			<xsl:value-of select="(($IPD_PRT_H + $IPD_PRT_SPC) * $ports_left_cnt_) + $IPD_PRT_SY"/>
		</xsl:when>
		
	</xsl:choose>	
</xsl:variable>

<!-- select and return the maximum between the two left and right bifs -->
<xsl:variable name="BIFS_H">
	<xsl:choose>
	
		<xsl:when test="($bifs_left_cnt_ &gt; $bifs_rght_cnt_)">
			<xsl:value-of select="(($IPD_BIF_H + $IPD_PRT_SPC) * $bifs_left_cnt_)"/>
		</xsl:when>
			
		<xsl:when test="($bifs_rght_cnt_ &gt; $bifs_left_cnt_)">
			<xsl:value-of select="(($IPD_BIF_H + $IPD_PRT_SPC) * $bifs_rght_cnt_)"/>
		</xsl:when>
		
		<xsl:when test="(($bifs_left_cnt_ = $bifs_rght_cnt_) and ($bifs_left_cnt_ &gt; 0))">
			<xsl:value-of select="(($IPD_BIF_H + $IPD_PRT_SPC) * $bifs_left_cnt_)"/>
		</xsl:when>  
		
		<xsl:otherwise>0</xsl:otherwise>	
	</xsl:choose>	

</xsl:variable>

<xsl:variable name="IPD_H">
	<xsl:value-of select="$PORTS_H + $BIFS_H  + ($IPD_VGAP * 2)"/>
</xsl:variable>

<xsl:variable name="IPD_W">
	<xsl:value-of select="(($IPD_HGAP * 2) + ($IPD_BIF_W * 2)  + $IPD_CENTER_W)"/>
</xsl:variable>

<!--specify a css for the file -->

<!-- 
<xsl:processing-instruction name="xml-stylesheet">href="MdtSvgDiag_StyleDefs.css" type="text/css"</xsl:processing-instruction>
 -->

<svg width="{$IPD_W + 4}" height="{$IPD_H + 4}">
	
	<defs>
		<xsl:call-template  name="Define_ConnectedBifTypes"/>
		
		<xsl:call-template name="Define_BodyParts">
			<xsl:with-param name="iBodyWidth"  select="$IPD_W"/>
			<xsl:with-param name="iBodyHeight" select="$IPD_H"/>
		</xsl:call-template>			
	</defs>	
	
	<xsl:call-template name="Draw_Module">
		<xsl:with-param name="iBodyWidth"   select="$IPD_W"/>
		<xsl:with-param name="iBodyHeight"  select="$IPD_H"/>
		<xsl:with-param name="iBifsHeight"  select="BIFS_H"/>
		<xsl:with-param name="iPortsHeight" select="$PORTS_H"/>
	</xsl:call-template>			
</svg>

</xsl:for-each>
</xsl:template>

<!-- ======================= main svg block =============================== -->


<!-- ======================= IP DIAGRAM DEF =============================== -->


<xsl:template name="Define_BodyParts">
	
	<xsl:param name="iBodyWidth"   select="0"/>
	<xsl:param name="iBodyHeight"  select="0"/>
	<xsl:param name="iBodyBgColor" select="$COL_MOD_BG"/>
	
	<g id="HCurve" overflow="visible">
		<path d="m 0  0, 
			 a 16 16, 0,0,0, 32,0,
			 z" style="fill:{$iBodyBgColor};fill-opacity:1;stroke:black;stroke-width:1.5"/>  
		 <line x1="0" y1="0" x2="32" y2="0" style="stroke:{$iBodyBgColor};stroke-width:3"/>
	</g> 	
	
	<g id="IPD_StandardBody">
			
		<rect x="0"  
			  y="0"    
			  width ="{$iBodyWidth}"  
			  height="{$iBodyHeight}"
			  style="fill:{$iBodyBgColor};fill-opacity: 1.0; stroke:{$COL_BLACK}; stroke-width:1"/> 
				
		<rect x="{$IPD_VGAP + $IPD_BIF_W}" 
			  y="{$IPD_HGAP}"   
			  width ="{$iBodyWidth  - (($IPD_VGAP * 2) + ($IPD_BIF_W * 2))}"   
			  height="{$iBodyHeight -  ($IPD_VGAP * 2)}" 
			  style="fill:{$COL_WHITE}; fill-opacity: 1.0; stroke:{$COL_BLACK}; stroke-width:1.5"/> 	
			  
		<use  x="{ceiling($iBodyWidth div 2) - 16}" 
			  y="{$IPD_VGAP}" 
			  xlink:href="#HCurve"/>
	</g>
	
	<g id="IPD_PORT">
		<rect width ="{$IPD_PRT_W}" 
			  height="{$IPD_PRT_H}" 
			  style="fill:{$COL_MOD_MPRT};stroke-width:1;stroke:black;"/>
	</g>	
		
	<g id="IPD_SPort">
		<line x1="0"            y1="0" 
			  x2="{$IPD_PRT_W}" y2="0" 
			  style="stroke:{$COL_MOD_SPRT};stroke-width:2;stroke-opacity:1"/>
	</g>
	
    <g id="IPD_PortClk">
		<line x1="0" y1="0" 
			  x2="7" y2="3" 
			  style="stroke:{$COL_BLACK};stroke-width:1;stroke-opacity:1"/>
		<line x1="7" y1="3" x2="0" y2="7" style="stroke:{$COL_BLACK};stroke-width:1;stroke-opacity:1"/>
	</g>	
	
</xsl:template>


<xsl:template name="Draw_Ports">
	<xsl:param name="iBodyWidth"   select="0"/>
	<xsl:param name="iBodyHeight"  select="0"/>
	
	<xsl:variable name="prt_ix_pos_">
		<xsl:value-of select="($IPD_HGAP  + $IPD_BIF_W) - $IPD_PRT_W"/>
	</xsl:variable>	
	
	<xsl:variable name="prt_ox_pos_">
		<xsl:value-of select="$iBodyWidth - ($IPD_HGAP + $IPD_BIF_W)"/>
	</xsl:variable>	
	
	<xsl:variable name="prt_ini_y_">
		<xsl:value-of select="$IPD_VGAP + $IPD_PRT_SY"/>
	</xsl:variable>	
	
	<!-- layout the input ports-->
	<xsl:for-each select="PORTS/PORT[((@DIR='I'or @DIR='IO') and  (@IS_INSTANTIATED = 'TRUE'))]">
		<xsl:sort data-type="number" select="@MHS_INDEX" order="ascending"/>
		<xsl:variable name="prt_iy_pos_" select="$prt_ini_y_ + ((position() - 1) * ($IPD_PRT_SPC + $IPD_PRT_H))"/>
		
		<xsl:if test="@MSB and @LSB">
			<use x="{$prt_ix_pos_}" y="{$prt_iy_pos_}" xlink:href="#IPD_PORT"/>
		</xsl:if>
		<xsl:if test="not(@MSB) or not(@LSB)">
			<use x="{$prt_ix_pos_}" y="{$prt_iy_pos_}" xlink:href="#IPD_SPort"/>
		</xsl:if>
		<xsl:if test="@SIGIS='CLK'">
			<use x="{$prt_ix_pos_ + 8}" y="{$prt_iy_pos_ - 2}" xlink:href="#IPD_PortClk"/>
		</xsl:if>
<!--
		<text class="portlabel" 
		       x="{$prt_ix_pos_ + $IPD_PRT_W + 12}" 
  		       y="{$prt_iy_pos_ + 6}"><xsl:value-of select="@IPD_INDEX"/></text>
-->		
	   
		<xsl:call-template name="F_WriteText">
			<xsl:with-param name="iX" 		select="($prt_ix_pos_ + $IPD_PRT_W + 12)"/>
			<xsl:with-param name="iY" 		select="($prt_iy_pos_ + 6)"/>
			<xsl:with-param name="iText"	select="@MHS_INDEX"/>
			<xsl:with-param name="iClass"	select="'ipd_portlabel'"/>
		</xsl:call-template>	
			   
	</xsl:for-each>
	
	<!-- layout the output ports-->
	<xsl:for-each select="PORTS/PORT[((@DIR='O') and (@IS_INSTANTIATED = 'TRUE'))]">
		<xsl:sort data-type="number" select="@MHS_INDEX" order="ascending"/>
		<xsl:variable name="prt_oy_pos_" select="$prt_ini_y_ + ((position() - 1) * ($IPD_PRT_SPC + $IPD_PRT_H))"/>
		
		<xsl:if test="@MSB and @LSB">
			<use x="{$prt_ox_pos_}" y="{$prt_oy_pos_}" xlink:href="#IPD_PORT"/> 
		</xsl:if>
		<xsl:if test="not(@MSB) or not(@LSB)">
			<use x="{$prt_ox_pos_}" y="{$prt_oy_pos_}" xlink:href="#IPD_SPort"/>
		</xsl:if>
		
<!-- 
		<text class="portlabel" 
			x="{$prt_ox_pos_ - 12}" 
			y="{$prt_oy_pos_ + 6}"><xsl:value-of select="@IPD_INDEX"/></text>
-->			
		<xsl:call-template name="F_WriteText">
			<xsl:with-param name="iX" 		select="($prt_ox_pos_ - 12)"/>
			<xsl:with-param name="iY" 		select="($prt_oy_pos_ + 6)"/>
			<xsl:with-param name="iText"	select="@MHS_INDEX"/>
			<xsl:with-param name="iClass"	select="'ipd_portlabel'"/>
		</xsl:call-template>	
			   
	</xsl:for-each>
	
</xsl:template>

<xsl:template name="Draw_Bifs">
	<xsl:param name="iBodyWidth"    select="0"/>
	<xsl:param name="iBodyHeight"   select="0"/>
	<xsl:param name="iPortsHeight"  select="0"/>
	
	<xsl:variable name="bif_ix_pos_">
		<xsl:value-of select="($IPD_HGAP  + $IPD_BIF_W) - $IPD_BIFC_W"/>
	</xsl:variable>	
	
	<xsl:variable name="bif_ox_pos_">
		<xsl:value-of select="$iBodyWidth - ($IPD_HGAP + $IPD_BIF_W)"/>
	</xsl:variable>	
	
	<xsl:variable name="bif_ini_y_">
		<xsl:value-of select="$IPD_VGAP + $IPD_PRT_SY + $iPortsHeight"/>
	</xsl:variable>	
	
	<!-- layout the left bifs-->
	<xsl:for-each select="BUSINTERFACES/BUSINTERFACE[((@IS_INSTANTIATED = 'TRUE') and (not(@TYPE) or (@TYPE = 'SLAVE') or (@TYPE = 'TARGET') or (@TYPE = 'USER')))]">
		<xsl:variable name="bif_iy_pos_" select="$bif_ini_y_ + ((position() - 1) * ($IPD_PRT_SPC + $IPD_BIF_H))"/>
		
		
		<xsl:variable name="bifType_">
			<xsl:choose>
				<xsl:when test="@TYPE"><xsl:value-of select="@TYPE"/></xsl:when>
				<xsl:otherwise>SLAVE</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="bifBusStd_">
			<xsl:choose>
				<xsl:when test="@BUSSTD"><xsl:value-of select="@BUSSTD"/></xsl:when>
				<xsl:otherwise>TRS</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<use x="{$bif_ix_pos_}" y="{$bif_iy_pos_}" xlink:href="#{$bifBusStd_}_busconn_{$bifType_}"/>
		
<!-- 
		<text class="ipdbiflbl" 
			x="{$bif_ix_pos_ + $IPD_BIF_W + 4}" 
			y="{$bif_iy_pos_ + 14}"><xsl:value-of select="@NAME"/></text>
-->			
			
		<xsl:call-template name="F_WriteText">
			<xsl:with-param name="iClass"	select="'ipd_biflabel'"/>
			<xsl:with-param name="iX" 		select="($bif_ix_pos_ + $IPD_BIF_W + 4)"/>
			<xsl:with-param name="iY" 		select="($bif_iy_pos_ + 14)"/>
			<xsl:with-param name="iText"	select="@NAME"/>
		</xsl:call-template>	
			   
	</xsl:for-each>
	
	<!-- layout the right bifs-->
	<xsl:for-each select="BUSINTERFACES/BUSINTERFACE[((@IS_INSTANTIATED = 'TRUE') and ((@TYPE = 'MASTER') or (@TYPE = 'MASTER_SLAVE') or (@TYPE = 'INITIATOR') or (@TYPE = 'MONITOR')))]">
		<xsl:variable name="bif_oy_pos_" select="$bif_ini_y_ + ((position() - 1) * ($IPD_PRT_SPC + $IPD_BIF_H))"/>
		
		<xsl:variable name="bifType_">
			<xsl:choose>
				<xsl:when test="@TYPE"><xsl:value-of select="@TYPE"/></xsl:when>
				<xsl:otherwise>SLAVE</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="bifBusStd_">
			<xsl:choose>
				<xsl:when test="@BUSSTD"><xsl:value-of select="@BUSSTD"/></xsl:when>
				<xsl:otherwise>TRS</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:if test="not($bifType_ = 'MASTER_SLAVE')">
			<use x="{$bif_ox_pos_}" y="{$bif_oy_pos_}" xlink:href="#{$bifBusStd_}_busconn_{$bifType_}"/>
		</xsl:if>		
		<xsl:if test="($bifType_ = 'MASTER_SLAVE')">
			<use x="{$bif_ox_pos_}" y="{$bif_oy_pos_}" xlink:href="#{$bifBusStd_}_busconn_{$bifType_}" transform="rotate(90,{($bif_ox_pos_ + ceiling($IPD_BIF_W div 2))},{($bif_oy_pos_ + ceiling($IPD_BIF_H div 2))})"/>
		</xsl:if>
<!-- 
		<text class="ipdbiflbl" 
			x="{$bif_ox_pos_ - (string-length(@NAME) * 8) - 2}" 
			y="{$bif_oy_pos_ + 14}"><xsl:value-of select="@NAME"/></text>
-->			
		<xsl:call-template name="F_WriteText">
			<xsl:with-param name="iClass"	select="'ipd_biflabel'"/>
			<xsl:with-param name="iX" 		select="($bif_ox_pos_ - (string-length(@NAME) * 8) - 2)"/>
			<xsl:with-param name="iY" 		select="($bif_oy_pos_ + 14)"/>
			<xsl:with-param name="iText"	select="@NAME"/>
		</xsl:call-template>	
			   
	</xsl:for-each>
	
</xsl:template>


<xsl:template name="Draw_Module">
	
	<xsl:param name="iBodyWidth"   select="0"/>
	<xsl:param name="iBodyHeight"  select="0"/>
	<xsl:param name="iBifsHeight"  select="0"/>
	<xsl:param name="iPortsHeight" select="0"/>

	<use x="0"  
		 y="0"  
		 xlink:href="#IPD_StandardBody"/>
		 
<!-- 
	<text class="iptype" 
		  x="{ceiling($iBodyWidth  div 2)}" 
		  y="{ceiling($iBodyHeight div 2) + 4}">
		<xsl:value-of select="@MODTYPE"/>
	</text>
-->	

	<xsl:call-template name="F_WriteText">
		<xsl:with-param name="iClass"	select="'ipd_iptype'"/>
		<xsl:with-param name="iX" 		select="(ceiling($iBodyWidth  div 2))"/>
		<xsl:with-param name="iY" 		select="(ceiling($iBodyHeight div 2) + 4)"/>
		<xsl:with-param name="iText"	select="@MODTYPE"/>
	</xsl:call-template>			
	
<!-- 
	<text class="iplabel" 
		  x="{ceiling($iBodyWidth  div 2)}" 
		  y="{ceiling($iBodyHeight div 2) + 16}">
		<xsl:value-of select="@INSTANCE"/>
	</text>
-->	
	<xsl:call-template name="F_WriteText">
		<xsl:with-param name="iClass"	select="'ipd_ipname'"/>
		<xsl:with-param name="iX" 		select="(ceiling($iBodyWidth  div 2))"/>
		<xsl:with-param name="iY" 		select="(ceiling($iBodyHeight div 2) + 16)"/>
		<xsl:with-param name="iText"	select="@INSTANCE"/>
	</xsl:call-template>	
	
	<xsl:call-template name="Draw_Ports">
		<xsl:with-param name="iBodyWidth"   select="$iBodyWidth"/>
		<xsl:with-param name="iBodyHeight"  select="$iBodyHeight"/>
	</xsl:call-template>
	
	<xsl:call-template name="Draw_Bifs">
		<xsl:with-param name="iBodyWidth"    select="$iBodyWidth"/>
		<xsl:with-param name="iBodyHeight"   select="$iBodyHeight"/>
		<xsl:with-param name="iPortsHeight"  select="$iPortsHeight"/>
	</xsl:call-template>
	
</xsl:template>

</xsl:stylesheet>

