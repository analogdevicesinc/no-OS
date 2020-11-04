<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
           xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
           xmlns:dyn="http://exslt.org/dynamic"
           xmlns:exsl="http://exslt.org/common"
           xmlns:xlink="http://www.w3.org/1999/xlink">
         
<xsl:output method="html"/>

<xsl:include href="MdtXdsGen_Colors.xsl"/>
<xsl:include href="MdtXdsGen_Globals.xsl"/>
<xsl:include href="MdtXdsGen_HTMLTOCTree.xsl"/>
<xsl:include href="MdtXdsGen_HTMLMemoryMap.xsl"/>
<xsl:include href="MdtXdsGen_HTMLIPSection.xsl"/>

<!-- 
<xsl:param name="PARAM_FRAME"	select="'MAIN'"/>
<xsl:param name="PARAM_FRAME"	select="'INDEX'"/>
<xsl:param name="PARAM_FRAME"	select="'TOCTREE'"/>
<xsl:param name="PARAM_FRAME"	select="'NOFRAMES'"/>
 -->

<xsl:param name="PARAM_FRAME"	select="'MAIN'"/>
<xsl:param name="PARAM_DEBUG"	select="'FALSE'"/>
<xsl:param name="PARAM_PREFIX"	select="'system'"/>

<xsl:variable name="FRAME_FRMMAIN"><xsl:value-of select="$PARAM_PREFIX"/>_main</xsl:variable>
<xsl:variable name="FNM_FRMMAIN_HTML"><xsl:value-of select="$FRAME_FRMMAIN"/>.html</xsl:variable>
<xsl:variable name="FNM_FRMMAIN_HTML_NOFRM"><xsl:value-of select="$FRAME_FRMMAIN"/>NF.html</xsl:variable>

<xsl:variable name="IMG_FRMMAIN_BLKD">imgs/<xsl:value-of select="$PARAM_PREFIX"/>_blkd.jpg</xsl:variable>

<!-- ======================= MAIN DATASHEET SECTION  =============================== -->
<xsl:template match="EDKSYSTEM[@EDKVERSION and @EDWVERSION]">

	<xsl:if test="$PARAM_DEBUG='TRUE'">
		<xsl:message> --- Writing Frame: <xsl:value-of select="$PARAM_FRAME"/></xsl:message>
	</xsl:if>
	
	<xsl:choose>
	
		<xsl:when test="(($PARAM_FRAME='MAIN') or ($PARAM_FRAME='NOFRAMES'))">
			<xsl:call-template name="WriteFrame_MAIN"/>	
		</xsl:when>
		
		<xsl:when test="$PARAM_FRAME='TOCTREE'">
			<xsl:call-template name="WriteFrame_TOCTREE"/>	
		</xsl:when>
		
		<xsl:when test="$PARAM_FRAME='INDEX'">
			<xsl:call-template name="WriteFrame_INDEX"/>	
		</xsl:when>
		
		<xsl:when test="$PARAM_FRAME='BLKD'">
			<xsl:call-template name="WriteFrame_BLKD"/>	
		</xsl:when>
		
		<xsl:otherwise>
			<xsl:if test="$PARAM_DEBUG='TRUE'">
				<xsl:message>Unknown frame <xsl:value-of select="$PARAM_FRAME"/> requested.</xsl:message>
			</xsl:if>
		</xsl:otherwise>
	
	</xsl:choose>
	
</xsl:template>	

<!-- 
	========================================
			MAIN FRAME
	========================================
-->
<xsl:template name="WriteFrame_MAIN">
<HTML>
	<HEAD>
		<TITLE><xsl:value-of select="$G_XPS_TITLE"/></TITLE>
		<!--specify a css for the file -->
	    <meta http-equiv="PRAGMA" content="NO-CACHE"/>
	    <meta http-equiv="content-Type" content="text/html; charset=iso-8859-1"/>
	</HEAD>	
	
	<BODY BGCOLOR="{$COL_WHITE}">
<!--
		========================================
			TABLE OF CONTENTS and OVERVIEW
		========================================
-->
		<xsl:if test="($PARAM_FRAME = 'NOFRAMES')">
			<xsl:call-template name="WriteContents_TOCFLAT"/>
		</xsl:if>	
		<xsl:if test="$PARAM_DEBUG='TRUE'">
			<xsl:message> ---- Writing Overview </xsl:message>
		</xsl:if>
		<TABLE BGCOLOR="{$COL_WHITE}" WIDTH="{$G_MAIN_WIDTH}" COLS="1" CELLSPACING="0" CELLPADDING="0" BORDER="0">
			<xsl:if test="not($PARAM_FRAME = 'NOFRAMES')">
				<TD COLSPAN="1" ALIGN="LEFT" VALIGN="TOP">
					<A HREF="{$FNM_FRMMAIN_HTML_NOFRM}" target="ANCHOR_TOP" style="text-decoration:none"><SPAN style="color:{$COL_GREEN}; font: bold 14px Verdana Arial,Helvetica,sans-serif">Printable Version</SPAN></A>
				</TD>
				<TR></TR>
			</xsl:if>
			
			<TD COLSPAN="1" WIDTH="100%" VALIGN="TOP">
				<A name="ANCHOR_OVERVIEW"/>
				<xsl:call-template name="WriteContents_NavigateHeader">
					<xsl:with-param name="iHeaderName" select="'Overview'"/>
				</xsl:call-template>	
				<xsl:call-template name="WriteContents_Overview"/>
			</TD>
<!--
		========================================
				BLOCK DIAGRAM 
		========================================
-->
			<TR/>
			<TD COLSPAN="1" WIDTH="100%" VALIGN="TOP">
				<BR/>
				<A name="ANCHOR_BLOCKDIAGRAM"/>
				<xsl:call-template name="WriteContents_NavigateHeader">
					<xsl:with-param name="iHeaderName" select="'Block Diagram'"/>
				</xsl:call-template>
				<BR></BR>
				<IMG SRC="{$IMG_FRMMAIN_BLKD}" AlT="BlockDiagram" BORDER="0" VSPACE="0" HSPACE="0"/>
			</TD>
			
<!--
		========================================
					EXTERNAL PORTS
		========================================
-->
			<TR/>
			<TD COLSPAN="1" WIDTH="100%" VALIGN="TOP">
				<A name="ANCHOR_EXTERNALPORTS"/>
				<xsl:call-template name="WriteContents_NavigateHeader">
					<xsl:with-param name="iHeaderName" select="'External Ports'"/>
				</xsl:call-template>
				<BR></BR>
				<xsl:call-template name="WriteContents_ExternalPorts"/>
			</TD>			
			
<!--
		========================================
					PROCESSORS
		========================================
-->
			<xsl:if test="$PARAM_DEBUG='TRUE'">
				<xsl:message> ---- Writing Processors</xsl:message>
			</xsl:if>	
			<xsl:if test="($G_NUMOF_PROCESSORS &gt; 0)">
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_PROCESSORS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
							<xsl:with-param name="iHeaderName" select="'Processors'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
					<xsl:for-each select="$G_MODULES">
						<xsl:variable name="num_of_procs_"  select="count(key('G_MAP_MODULES', 'PROCESSOR'))"/>
						<xsl:for-each select="key('G_MAP_MODULES', 'PROCESSOR')">
							<xsl:sort select="@MODTYPE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</xsl:for-each>
<!-- 
							<xsl:message>Num of processors in the design  <xsl:value-of select="$num_of_procs_"/></xsl:message>
							<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[((@MODCLASS='PROCESSOR') and (contains(@MODTYPE,'microblaze')))]">
								<xsl:sort select="@INSTANCE"/>
								<TR/>
								<TD COLSPAN="1" ALIGN="LEFT">
									<xsl:call-template name="WriteContents_IPSection"/>
								</TD>
							</xsl:for-each>
							
							<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[((@MODCLASS='PROCESSOR') and (contains(@MODTYPE,'ppc')))]">
								<xsl:sort select="@INSTANCE"/>
								<TR/>
								<TD COLSPAN="1" ALIGN="LEFT">
									<xsl:call-template name="WriteContents_IPSection"/>
								</TD>
								
							</xsl:for-each>
-->					
					</TABLE>
				</TD>			
			</xsl:if>
			
<!--
		========================================
					DEBUGGERS
		========================================
-->
			<xsl:if test="($G_NUMOF_DEBUGGERS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Debuggers</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_DEBUGGERS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Debuggers'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='DEBUG')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>
			</xsl:if>	
			
<!--
		========================================
					INTERRUPT CONTROLLERS
		========================================
-->
			<xsl:if test="($G_NUMOF_INTERRUPT_CNLTRS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Interrupt Controllers</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_INTERRUPT_CNTLRS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Interrupt Controllers'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='INTERRUPT_CNTLR')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>
			</xsl:if>
			
<!--
		========================================
					BUSSES
		========================================
-->
			<xsl:if test="($G_NUMOF_BUSSES &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Busses</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_BUSSES"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Busses'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='BUS')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>
			</xsl:if>			
			
<!--
		========================================
					BRIDGES
		========================================
-->
			<xsl:if test="($G_NUMOF_BRIDGES &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Bridges</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_BRIDGES"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Bridges'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='BUS_BRIDGE')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>
			</xsl:if>
<!--
		========================================
					MEMORYS
		========================================
-->
			<xsl:if test="($G_NUMOF_MEMORYS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Memorys</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_MEMORYS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Memorys'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='MEMORY')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>			
			</xsl:if>
			
<!--
		========================================
					MEMORY CONTROLLERS
		========================================
-->
			<xsl:if test="($G_NUMOF_MEMORY_CNTLRS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Memory Controllers</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_MEMORY_CNTLRS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Memory Controllers'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='MEMORY_CNTLR')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>				
			</xsl:if>
			
<!--
		========================================
					PERIPHERALS
		========================================
-->
			<xsl:if test="($G_NUMOF_PERIPHERALS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing Peripherals</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_PERIPHERALS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'Peripherals'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PERIPHERAL')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>				
			</xsl:if>
			
<!--
		========================================
					IP
		========================================
-->
			<xsl:if test="($G_NUMOF_IPS &gt; 0)">
				<xsl:if test="$PARAM_DEBUG='TRUE'">
					<xsl:message> ---- Writing IPS</xsl:message>
				</xsl:if>	
				<TR/>
				<TD COLSPAN="1" WIDTH="100%">
					<BR></BR>
					<BR></BR>
					<A name="ANCHOR_PERIPHERALS"/>
					<xsl:call-template name="WriteContents_NavigateHeader">
						<xsl:with-param name="iHeaderName" select="'IP'"/>
					</xsl:call-template>
					<BR></BR>
					<TABLE ALIGN="LEFT" WIDTH="{$G_MAIN_WIDTH}" CELLSPACING="0" CELLPADDING="0" BORDER="0">
						<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'IP')]">
							<xsl:sort select="@INSTANCE"/>
							<TR/>
							<TD COLSPAN="1" ALIGN="LEFT">
								<xsl:call-template name="WriteContents_IPSection"/>
							</TD>
						</xsl:for-each>
					</TABLE>
				</TD>				
			</xsl:if>			
			
<!--
		========================================
					TIMING INFORMATION
		========================================
-->
			<xsl:if test="$PARAM_DEBUG='TRUE'">
				<xsl:message> ---- Writing Timing Information</xsl:message>
			</xsl:if>	
			<TR/>
			<TD COLSPAN="1" WIDTH="100%">
				<BR></BR>
				<BR></BR>
				<A name="ANCHOR_TIMINGINFO"/>
				<xsl:call-template name="WriteContents_NavigateHeader">
					<xsl:with-param name="iHeaderName" select="'Timing Information'"/>
				</xsl:call-template>	
				<BR></BR>
				<BR></BR>
				<xsl:call-template name="WriteContents_TimingInfo"/>
			</TD>			
			
		</TABLE>	
	</BODY>
</HTML>
</xsl:template>

<xsl:template name="WriteContents_Overview">
<TABLE WIDTH="{$G_MAIN_WIDTH}" VALIGN="TOP" ALIGN="LEFT" BORDER="0" CELLPADDING="0" CELLSPACING="1" COLS="2" BGCOLOR="{$COL_WHITE}"> 
	<TD COLSPAN="1" ALIGN="LEFT" VALIGN="TOP">
		<xsl:call-template name="WriteContents_ResourcesTable"/>
	</TD>
	<TD COLSPAN="1" ALIGN="RIGHT" VALIGN="TOP">
		<xsl:call-template name="WriteContents_SpecificsTable"/>
	</TD>
</TABLE>
</xsl:template>

<xsl:template name="WriteContents_ResourcesTable">

<xsl:variable name="BusDescs_"    select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'BUS')]/DESCRIPTION[(@TYPE ='SHORT')]"/>
<xsl:variable name="ProcDescs_"   select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PROCESSOR')]/DESCRIPTION[(@TYPE ='SHORT')]"/>
<xsl:variable name="BrdgDescs_"   select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'BUS_BRIDGE')]/DESCRIPTION[(@TYPE ='SHORT')]"/>
<xsl:variable name="BramDescs_"   select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY')]/DESCRIPTION[(@TYPE ='SHORT')]"/>
<xsl:variable name="CntlrDescs_"  select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY_CNTLR')]/DESCRIPTION[(@TYPE ='SHORT')]"/>
<xsl:variable name="OtherDescs_"  select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[not((@MODCLASS = 'BUS') or (@MODCLASS = 'PROCESSOR') or (@MODCLASS = 'BUS_BRIDGE') or (@MODCLASS = 'MEMORY') or (@MODCLASS = 'MEMORY_CNTLR'))]/DESCRIPTION[(@TYPE ='SHORT')]"/>

<xsl:if test="$PARAM_DEBUG='TRUE'">
	<xsl:message> ---- Writing Overview Resources </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($ProcDescs_)"/> Processors </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($BusDescs_)"/> Busses </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($BrdgDescs_)"/> Bridges </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($BramDescs_)"/> Memorys </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($CntlrDescs_)"/> Memory Controllers </xsl:message>
	<xsl:message> ----  Found <xsl:value-of select="count($OtherDescs_)"/> Other Modules</xsl:message>
</xsl:if>


<TABLE WIDTH="{$G_OVRESOURCES_WIDTH}" BORDER="0" CELLPADDING="0" CELLSPACING="1" COLS="0" BGCOLOR="{$COL_WHITE}"> 
	<TD COLSPAN="2" BGCOLOR="{$COL_WHITE}" ALIGN="CENTER" VALIGN="TOP">
		<SPAN style="color:{$COL_XLNX}; font: bold 14px Verdana,Arial,Helvetica,sans-serif">Resources Used</SPAN>
	</TD>
	<TR/>
	
	<!--  Processors overview  -->
	<xsl:for-each select="exsl:node-set($ProcDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($ProcDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
	
	<!--  Busses Overview  -->
	<xsl:for-each select="exsl:node-set($BusDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($BusDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
	
	<!--  Bridges Overview  -->
	<xsl:for-each select="exsl:node-set($BrdgDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($BrdgDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
	
	<!--  Memory Overview  -->
	<xsl:for-each select="exsl:node-set($BramDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($BramDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
	
	<!--  Memory Controllers Overview  -->
	<xsl:for-each select="exsl:node-set($CntlrDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($CntlrDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
	
	<!--  Other assorted modules Overview  -->
	<xsl:for-each select="exsl:node-set($OtherDescs_)[not(text() = preceding::text())]">
		<xsl:variable name="DscOf" select="text()"/>
		<xsl:variable name="NumOf" select="count($OtherDescs_[text() = $DscOf])"/>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="RIGHT" WIDTH="5%">
			<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$NumOf"/><xsl:text>&#160;&#160;</xsl:text></SPAN>
		</TD>
		<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="LEFT" WIDTH="95%">
			<SPAN style="color:{$COL_BLACK}; font: italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$DscOf"/></SPAN>
		</TD>
		<TR/>
	</xsl:for-each>
</TABLE>
</xsl:template>

<xsl:template name="WriteContents_SpecificsTable">
<xsl:if test="$PARAM_DEBUG='TRUE'">
	<xsl:message> ---- Writing Overview Specifics </xsl:message>
</xsl:if>

<TABLE WIDTH="{$G_OVSPECFICS_WIDTH}" BORDER="0" CELLPADDING="0" CELLSPACING="1" COLS="2" BGCOLOR="{$COL_BLACK}"> 
	<TD COLSPAN="2" BGCOLOR="{$COL_XLNX}" ALIGN="CENTER">
		<SPAN style="color:{$COL_WHITE}; font: bold 14px Verdana,Arial,Helvetica,sans-serif">Specifics</SPAN>
	</TD>
	<TR/>
	
	<TD COLSPAN="1" BGCOLOR="{$COL_GRAY}" ALIGN="LEFT" WIDTH="30%">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Generated</SPAN>
	</TD>
	<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="CENTER" WIDTH="70%">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$G_ROOT/EDKSYSTEM/@TIMESTAMP"/></SPAN>
	</TD>
	<TR/>
	
	<TD COLSPAN="1" BGCOLOR="{$COL_GRAY}" ALIGN="LEFT" WIDTH="30%">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">EDK Version</SPAN>
	</TD>
	<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="CENTER">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$G_EDKVERSION"/></SPAN>
	</TD>
	<TR/>
	
	<TD COLSPAN="1" BGCOLOR="{$COL_GRAY}" ALIGN="LEFT" WIDTH="30%">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Device Family</SPAN>
	</TD>
	<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="CENTER">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$G_FAMILY"/></SPAN>
	</TD>
	<TR/>
	
	<TD COLSPAN="1" BGCOLOR="{$COL_GRAY}" ALIGN="LEFT" WIDTH="30%">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Device</SPAN>
	</TD>
	<TD COLSPAN="1" BGCOLOR="{$COL_WHITE}" ALIGN="CENTER">
		<SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$G_DEVICE"/></SPAN>
	</TD>
	<TR/>
</TABLE>
</xsl:template>

<xsl:template name="WriteContents_ExternalPorts">
	
<xsl:if test="$PARAM_DEBUG='TRUE'">
	<xsl:message> ---- Writing External Ports </xsl:message>
</xsl:if>	

	<xsl:variable name="NUMOF_ALL_" select="count($G_ROOT/EXTERNALPORTS/PORT)"/>
	<xsl:variable name="NUMOF_I_"   select="count($G_ROOT/EXTERNALPORTS/PORT[(@DIR='I')])"/>
	<xsl:variable name="NUMOF_O_"   select="count($G_ROOT/EXTERNALPORTS/PORT[(@DIR='O')])"/>
	<xsl:variable name="NUMOF_IO_"  select="count($G_ROOT/EXTERNALPORTS/PORT[(@DIR='IO')])"/>
	
	<xsl:variable name="table_width_">
		<xsl:if test="$NUMOF_ALL_ &gt;  16"><xsl:value-of select="$G_MAIN_WIDTH"/></xsl:if>
		<xsl:if test="$NUMOF_ALL_ &lt;= 16"><xsl:value-of select="$G_MAIN_WIDTH"/></xsl:if>
	</xsl:variable>
	
	<xsl:variable name="left_extra_">
		<xsl:if test="($NUMOF_ALL_ mod 2)     = 1">1</xsl:if>
		<xsl:if test="not(($NUMOF_ALL_ mod 2) = 1)">0</xsl:if>
	</xsl:variable>
	
	<xsl:variable name="num_left_" select="floor($NUMOF_ALL_ div 2) + $left_extra_"/>
	<xsl:variable name="num_rhgt_" select="floor($NUMOF_ALL_ div 2)"/>
	
	<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{$table_width_}" COLS="8" CELLSPACING="1" CELLPADDING="1" BORDER="0">
		
<!-- 
		<TD COLSPAN="8" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">EXTERNAL PORTS</SPAN></TD>
-->		
	 	<TR></TR>	
		<TD COLSPAN="8" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
			<SPAN style="color:{$COL_INFO}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">
				These are the external ports defined in the MHS file.
			</SPAN>
		</TD>
		
		<TR></TR>
		<TD COLSPAN="8" ALIGN="left" BGCOLOR="{$COL_WHITE}">
			<SPAN style="color:{$COL_INFO}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Attributes Key</SPAN>
			<BR></BR>
			<SPAN style="color:{$COL_BLACK}; font: italic 10px Verdana,Arial,Helvetica,sans-serif">The attributes are obtained from the SIGIS and IOB_STATE parameters set on the PORT in the MHS file </SPAN>
			<BR></BR>
			<SPAN style="color:{$COL_ATTR_CLK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">CLK</SPAN>
			<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">&#160;&#160;indicates Clock ports, (SIGIS = CLK)&#160;</SPAN>
			<BR></BR>
			<SPAN style="color:{$COL_ATTR_INT}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">INTR</SPAN>
			<SPAN style="color:{$COL_INFO}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;&#160;indicates Interrupt ports,(SIGIS = INTR)&#160;</SPAN>
			<BR></BR>
			<SPAN style="color:{$COL_ATTR_RST}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">RESET</SPAN>
			<SPAN style="color:{$COL_INFO}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;&#160;indicates Reset ports, (SIGIS = RST)&#160;</SPAN>
			<BR></BR>
			<SPAN style="color:{$COL_ATTR_BUF}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">BUF or REG</SPAN>
			<SPAN style="color:{$COL_INFO}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;&#160;Indicates ports that instantiate or infer IOB primitives, (IOB_STATE = BUF or REG)&#160;</SPAN>
		</TD>
		
		<xsl:if test="$NUMOF_ALL_ &lt;= 16">
			
		<TR></TR>
		
		<TD COLSPAN="7" ALIGN="left" BGCOLOR="{$COL_WHITE}">
			
		<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="100%" COLS="9" CELLSPACING="1" CELLPADDING="0" BORDER="0">
			
			<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">#</SPAN></TD>
			<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">NAME</SPAN></TD>
			<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">DIR</SPAN></TD>
			<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">[LSB:MSB]</SPAN></TD>
			<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">SIG</SPAN></TD>
			<TD COLSPAN="2" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">ATTRIBUTES</SPAN></TD>
			
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/EXTERNALPORTS/PORT">
				<xsl:sort data-type="number" select="@GROUP_INDEX" order = "ascending"/>
				
				<xsl:variable name="glb_bg_col_">
					<xsl:call-template name="Group2Color">
						<xsl:with-param name="group" select="@GROUP"/>
					</xsl:call-template>
				</xsl:variable>	
				
				<TR></TR>	
				<TD COLSPAN="1" WIDTH="5%"  ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}">
					<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@INDEX"/></SPAN><SPAN style="color:{$COL_BLACK}; vertical-align: super; font: bold 10px Arial,Helvetica,sans-serif"><xsl:value-of select="@GROUP"/></SPAN>
				</TD>
				<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
				<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold   10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@DIR"/></SPAN></TD>
				<xsl:if test="@MSB and @LSB">
					<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@LSB"/>:<xsl:value-of select="@MSB"/></SPAN></TD>
				</xsl:if>			
				<xsl:if test="not(@MSB and @LSB)">
					<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">1</SPAN></TD>
				</xsl:if>			
				<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@SIGNAME"/></SPAN></TD>
				<TD COLSPAN="2" WIDTH="25%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}">
					<xsl:if test="@SIGIS='CLK'"><SPAN style="color:{$COL_ATTR_CLK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
					<xsl:if test="@SIGIS='DCMCLK'"><SPAN style="color:{$COL_ATTR_CLK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
					<xsl:if test="@SIGIS='RST'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
					<xsl:if test="@SIGIS='RESET'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
					<xsl:if test="@SIGIS='INTERRUPT'"><SPAN style="color:{$COL_ATTR_INT}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;INTR&#160;</SPAN></xsl:if>
					<xsl:if test="@IOB_STATE='BUF'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;BUF&#160;</SPAN></xsl:if>
					<xsl:if test="@IOB_STATE='REG'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;REG&#160;</SPAN></xsl:if>
				</TD>
			</xsl:for-each>
		
		</TABLE>
		</TD>
		</xsl:if>	
		
		<xsl:if test="$NUMOF_ALL_ &gt; 16">
			
			<TR></TR>	
			
			<TD COLSPAN="3" WIDTH="49%">
				<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="100%" COLS="9" CELLSPACING="1" CELLPADDING="0" BORDER="0">
					<TD COLSPAN="1" WIDTH="5"   ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">#</SPAN></TD>
					<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">NAME</SPAN></TD>
					<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">DIR</SPAN></TD>
					<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">[LSB:MSB]</SPAN></TD>
					<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">SIG</SPAN></TD>
					<TD COLSPAN="2" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">ATTRIBUTES</SPAN></TD>
			
					<xsl:for-each select="$G_ROOT/EDKSYSTEM/EXTERNALPORTS/PORT">
					<xsl:sort data-type="number" select="@GROUP_INDEX" order = "ascending"/>
					
					<xsl:if test="position() &lt;= $num_left_">
					
						<xsl:variable name="glb_bg_col_">
							<xsl:call-template name="Group2Color">	
								<xsl:with-param name="group" select="@GROUP"/>
							</xsl:call-template>		
						</xsl:variable>	
<!--
						<xsl:message>NAME: <xsl:value-of select="@NAME"/></xsl:message>						
						<xsl:message>SIGIS: <xsl:value-of select="@SIGIS"/></xsl:message>						
-->	
					    
						<TR></TR>	
						<TD COLSPAN="1" WIDTH="5%"  ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}">
							<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@INDEX"/></SPAN><SPAN style="color:{$COL_BLACK}; vertical-align: super; font: bold 10px Arial,Helvetica,sans-serif"><xsl:value-of select="@GROUP"/></SPAN>
						</TD>
						<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
						<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@DIR"/></SPAN></TD>
						<xsl:if test="@MSB and @LSB">
							<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@LSB"/>:<xsl:value-of select="@MSB"/></SPAN></TD>
						</xsl:if>			
						<xsl:if test="not(@MSB and @LSB)">
							<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">1</SPAN></TD>
						</xsl:if>			
						<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@SIGNAME"/></SPAN></TD>
						<TD COLSPAN="2" WIDTH="25%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}">
							<xsl:if test="@SIGIS='CLK'"><SPAN style="color:{$COL_GREEN}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='DCMCLK'"><SPAN style="color:{$COL_GREEN}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='RST'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='RESET'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='INTERRUPT'"><SPAN style="color:{$COL_ATTR_INT}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;INTR&#160;</SPAN></xsl:if>
							<xsl:if test="@IOB_STATE='BUF'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;BUF&#160;</SPAN></xsl:if>
							<xsl:if test="@IOB_STATE='REG'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;REG&#160;</SPAN></xsl:if>
						</TD>
					</xsl:if>	
					</xsl:for-each>
				</TABLE>	
			</TD>	
			
			<TD COLSPAN="1" WIDTH="2%" BGCOLOR="{$COL_GRAY}">
				<SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;</SPAN>
			</TD> 
			
			<TD COLSPAN="3" WIDTH="49%">
				<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="100%" COLS="9" CELLSPACING="1" CELLPADDING="0" BORDER="0">
					<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">NAME</SPAN></TD>
					<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">DIR</SPAN></TD>
					<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">[LSB:MSB]</SPAN></TD>
					<TD COLSPAN="2" WIDTH="30%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">SIG</SPAN></TD>
					<TD COLSPAN="2" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">ATTRIBUTES</SPAN></TD>
					
					<xsl:for-each select="$G_ROOT/EDKSYSTEM/EXTERNALPORTS/PORT">
					<xsl:sort data-type="number" select="@GROUP_INDEX" order = "ascending"/>
					
					<xsl:if test="position() &gt; $num_left_">
<!--					
						<xsl:variable name="glb_bg_col_">
							<xsl:choose>
								<xsl:when test="(@GROUP mod 2) = 1">
									<xsl:value-of select="$COL_ASH"/>
								</xsl:when>		
								<xsl:otherwise>
									<xsl:value-of select="$COL_WHITE"/>
								</xsl:otherwise>		
							</xsl:choose>	
						</xsl:variable>	
-->						
						
						<xsl:variable name="glb_bg_col_">
							<xsl:call-template name="Group2Color">	
								<xsl:with-param name="group" select="@GROUP"/>
							</xsl:call-template>		
						</xsl:variable>	
			
						<TR></TR>	
						<TD COLSPAN="1" WIDTH="5%"  ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}">
							<SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@INDEX"/></SPAN><SPAN style="color:{$COL_BLACK}; vertical-align: super; font: bold 10px Arial,Helvetica,sans-serif"><xsl:value-of select="@GROUP"/></SPAN>
						</TD>
						<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT"   BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
						<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold   10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@DIR"/></SPAN></TD>
						
						<xsl:if test="@MSB and @LSB">
							<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@LSB"/>:<xsl:value-of select="@MSB"/></SPAN></TD>
						</xsl:if>			
						<xsl:if test="not(@MSB and @LSB)">
							<TD COLSPAN="1" WIDTH="5%" ALIGN="CENTER" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">1</SPAN></TD>
						</xsl:if>			
						<TD COLSPAN="2" WIDTH="30%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@SIGNAME"/></SPAN></TD>
						<TD COLSPAN="2" WIDTH="25%" ALIGN="LEFT" BGCOLOR="{$glb_bg_col_}">
							<xsl:if test="@SIGIS='CLK'"><SPAN style="color:{$COL_GREEN}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='DCMCLK'"><SPAN style="color:{$COL_GREEN}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;CLK&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='RST'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='RESET'"><SPAN style="color:{$COL_ATTR_RST}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;RESET&#160;</SPAN></xsl:if>
							<xsl:if test="@SIGIS='INTERRUPT'"><SPAN style="color:{$COL_ATTR_INT}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;INTR&#160;</SPAN></xsl:if>
							<xsl:if test="@IOB_STATE='BUF'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;BUF&#160;</SPAN></xsl:if>
							<xsl:if test="@IOB_STATE='REG'"><SPAN style="color:{$COL_ATTR_BUF}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;REG&#160;</SPAN></xsl:if>
						</TD>
					</xsl:if>	
					</xsl:for-each>
					
					<xsl:if test="$left_extra_ &gt; 0">
						<TR></TR>	
						<TD COLSPAN="9" WIDTH="100%" ALIGN="left" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_WHITE}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;</SPAN></TD>
					</xsl:if>
					
				</TABLE>	
			</TD>	
		</xsl:if> 
	</TABLE>		
</xsl:template>


<!-- 
	====================================== 
	LAYOUT TABLE OF CONTENTS 
	====================================== 
-->
<xsl:template name="WriteContents_TOCFLAT">

<!-- 
<xsl:variable name="toc_col_">
	<xsl:if test="$PARAM_DS_TYPE='NOFRAMES'">
		<xsl:value-of select="$COL_LGRY"/>	
	</xsl:if>
	
	<xsl:if test="$PARAM_DS_TYPE='FRAMES'">
		<xsl:value-of select="$COL_WHITE"/>	
	</xsl:if>
</xsl:variable>

<xsl:variable name="toc_width_">
	<xsl:if test="$PARAM_DS_TYPE='NOFRAMES'">
		<xsl:value-of select="$G_MAIN_WIDTH"/>	
	</xsl:if>
	
	<xsl:if test="$PARAM_DS_TYPE='FRAMES'">
		<xsl:value-of select="$DS_TOC_WIDTH"/>	
	</xsl:if>
</xsl:variable>

<xsl:variable name="toc_target_">
	<xsl:if test="$PARAM_DS_TYPE='NOFRAMES'">
		<xsl:value-of select="$PARAM_DS_FRAME_SELF"/>	
	</xsl:if>
	
	<xsl:if test="$PARAM_DS_TYPE='FRAMES'">
		<xsl:value-of select="$PARAM_DS_FRAME_MAIN"/>	
	</xsl:if>
</xsl:variable>

<xsl:variable name="trg_html_">
	<xsl:if test="$PARAM_DS_TYPE='NOFRAMES'">
		<xsl:value-of select="''"/>	
	</xsl:if>
	
	<xsl:if test="$PARAM_DS_TYPE='FRAMES'">
		<xsl:value-of select="$DS_HTML_MAIN"/>	
	</xsl:if>
</xsl:variable>
 -->	
 
	
<A name="ANCHOR_TOC"/>
<TABLE BGCOLOR="{$COL_GRAY_LT}" WIDTH="{$G_MAIN_WIDTH}" COLS="2" CELLSPACING="0" CELLPADDING="3" BORDER="0">

	<TH COLSPAN="2" WIDTH="100%" ALIGN="CENTER"><SPAN style="color:{$COL_XLNX}; font: bold 20px Arial,Helvetica,sans-serif">TABLE OF CONTENTS</SPAN></TH>
	
	<TR></TR>
	<TD COLSPAN="1" WIDTH="40%" ALIGN="left">
		<BR></BR>
		<A HREF="#ANCHOR_OVERVIEW" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Overview</SPAN></A>
		
		<BR></BR>
		<A HREF="#ANCHOR_BLOCKDIAGRAM" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Block Diagram</SPAN></A>
		
		<BR></BR>
		<A HREF="#ANCHOR_EXTERNALPORTS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">External Ports</SPAN></A>
		
		<BR></BR>
		
		<xsl:if test="$G_NUMOF_PROCESSORS &gt; 1">
			<A HREF="#ANCHOR_PROCESSORS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Processors</SPAN></A>
		</xsl:if>	
		<xsl:if test="not($G_NUMOF_PROCESSORS &gt; 1)">
			<A HREF="#ANCHOR_PROCESSORS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Processor</SPAN></A>
		</xsl:if>	
		
		<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='PROCESSOR')]">
			<xsl:sort select="@INSTANCE"/>
			<BR></BR>
			<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			<xsl:if test="MEMORYMAP/MEMRANGE[(@INSTANCE)]">
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}_MEMORYMAP" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;memory map</SPAN></A>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'DEBUG')]">
			<BR></BR>
			<A HREF="#ACHOR_DEBUGGERS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana Arial,Helvetica,sans-serif">Debuggers</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='DEBUG')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'INTERRUPT_CNTLR')]">
			<BR></BR>
			<A HREF="#ANCHOR_INTERRUPT_CNTLRS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 14px Verdana Arial,Helvetica,sans-serif">Interrupt Controllers</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'INTERRUPT_CNTLR')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[((@MODCLASS='BUS') or (@MODCLASS='BUS_ARBITER'))]">
			<BR></BR>
			<A HREF="#ANCHOR_BUSSES" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Busses</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[((@MODCLASS='BUS') or (@MODCLASS='BUS_ARBITER'))]">
				<xsl:sort select="@INSTANCE"/>
	 			<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULE[(@MODCLASS = 'BUS_BRIDGE')]">
			<BR></BR>
			<A HREF="#ANCHOR_BRIDGES" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Bridges</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='BUS_BRIDGE')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='MEMORY')]">
			<BR></BR>
			<A HREF="#ANCHOR_MEMORYS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Memory</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS='MEMORY')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>		
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY_CNTLR')]">
				<BR></BR>	
				<A HREF="#ANCHOR_MEMORY_CNTLRS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Memory Controllers</SPAN></A>
				<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'MEMORY_CNTLR')]">
					<xsl:sort select="@INSTANCE"/>
					<BR></BR>
					<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
				</xsl:for-each>
		</xsl:if>		
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PERIPHERAL')]">
			<BR></BR>
			<A HREF="#ANCHOR_PERIPHERALS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Peripherals</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'PERIPHERAL')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Courier Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>		
		
		<xsl:if test="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'IP')]">
			<BR></BR>
			<A HREF="#ANCHOR_IPS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">IP</SPAN></A>
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE[(@MODCLASS = 'IP')]">
				<xsl:sort select="@INSTANCE"/>
				<BR></BR>
				<A HREF="#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Courier Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN><xsl:if test="LICENSEINFO[(@ICON_NAME and not(@ICON_NAME = 'ps_core_preferred'))]"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" vspace="0" hspace="0"/></xsl:if></A>
			</xsl:for-each>
		</xsl:if>			
		
		<BR></BR>
		<A HREF="#ANCHOR_TIMINGINFO" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Timing Information</SPAN></A>
	</TD>
</TABLE>
</xsl:template>

<xsl:template name="Group2Color">

	<xsl:param name="group"  select="'A'"/>
		
	<xsl:variable name="letter">
		<xsl:value-of select="substring($group,(string-length($group) -string-length($group)) + 1)"/>
	</xsl:variable>
	
	<xsl:choose>
		<xsl:when test="$group = 'GLB'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		
		<xsl:when test="$letter = 'A'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'C'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'E'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'G'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'I'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'K'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'M'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'O'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'Q'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'S'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'U'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'W'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		<xsl:when test="$letter = 'Y'"><xsl:value-of select="$COL_GRAY_LT"/></xsl:when>
		
		<xsl:when test="$letter = 'B'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'D'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'F'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'H'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'J'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'L'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'N'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'P'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'R'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'T'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'V'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'X'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		<xsl:when test="$letter = 'Z'"><xsl:value-of select="$COL_WHITE"/></xsl:when>
		
		<xsl:otherwise>
			<xsl:value-of select="$COL_WHITE"/>	
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template name="WriteContents_TimingInfo">
	
	 
	<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{ceiling($G_MAIN_WIDTH div 2)}" COLS="5" CELLSPACING="1" CELLPADDING="1" BORDER="1">
		<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">Post Synthesis Clock Limits</SPAN></TD>
	 	<TR></TR>	
		<xsl:if test="$G_NUMOF_MODULES_W_TIMING &lt; 1">
			<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
				<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
					No clocks could be identified in the design. Run platgen to generate synthesis information.
				</SPAN>
			</TD>
		</xsl:if>
		<xsl:if test="$G_NUMOF_MODULES_W_TIMING &gt; 0">
			<TD COLSPAN="5" WIDTH="100%" ALIGN="left" BGCOLOR="{$COL_WHITE}">
				<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
					These are the post synthesis clock frequencies. The critical frequencies are marked with
				</SPAN>
				<SPAN style="color:{$COL_GREEN}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
					green.
				</SPAN>
				<BR></BR>
				<SPAN style="color:{$COL_RED}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
					The values reported here are post synthesis estimates calculated for each individual module. These values will change after place and route is performed on the entire system.
				</SPAN>
			</TD>
			<TR></TR>
			<TD COLSPAN="2" WIDTH="40%" ALIGN="left" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">MODULE</SPAN></TD>
			<TD COLSPAN="2" WIDTH="40%" ALIGN="left" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">CLK Port</SPAN></TD>
			<TD COLSPAN="1" WIDTH="20%" ALIGN="left" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">MAX FREQ</SPAN></TD>
			
			<xsl:for-each select="$G_ROOT/EDKSYSTEM/MODULES/MODULE/TIMING">
			<xsl:sort data-type="number" select="@MAXFREQ" order="ascending"/>
				<xsl:variable name="maxFreq_"  select="@MAXFREQ"/>
				<xsl:variable name="freqUnit_" select="@FREQUNITS"/>
				<xsl:variable name="freqCol_">
					<xsl:if test="position() = 1">	
						<xsl:value-of select="$COL_GREEN"/>
					</xsl:if>
					<xsl:if test="not(position() = 1)">	
						<xsl:value-of select="$COL_BLACK"/>
					</xsl:if>
				</xsl:variable>
				
				<xsl:for-each select="TIMECLK">
					<TR></TR>
					
					<TD COLSPAN="2" WIDTH="40%" ALIGN="left" BGCOLOR="{$COL_WHITE}">
							<A HREF="#ANCHOR_{../../@INSTANCE}" style="text-decoration:none">
								<SPAN style="color:{$COL_XLNX}; font: italic 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="../../@INSTANCE"/></SPAN>
							</A>
					</TD>
					
					<TD COLSPAN="2" WIDTH="40%" ALIGN="left" BGCOLOR="{$COL_WHITE}">
						<SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif">
							<xsl:value-of select="@SIGNAME"/>
						</SPAN>
					</TD>
					
					<TD COLSPAN="1" WIDTH="20%" ALIGN="left" BGCOLOR="{$COL_WHITE}">
						<SPAN style="color:{$freqCol_}; font: normal 12px Verdana,Arial,Helvetica,sans-serif">
							<xsl:value-of select="$maxFreq_"/><xsl:value-of select="$freqUnit_"/>
						</SPAN>
					</TD>
					
			    </xsl:for-each>
			</xsl:for-each>
		</xsl:if>
		
	</TABLE>	
</xsl:template>

<xsl:template name="WriteContents_NavigateHeader">
	<xsl:param name="iHeaderName" select="' '"/>
	<TABLE BGCOLOR="{$COL_XLNX}" WIDTH="{$G_MAIN_WIDTH}" COLS="3" CELLSPACING="0" CELLPADDING="0" BORDER="0">
		<TH COLSPAN="1" WIDTH="90%" ALIGN="left"><SPAN style="color:{$COL_WHITE}; font: bold 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$iHeaderName"/></SPAN></TH>
		<xsl:choose>
			<xsl:when test="$PARAM_FRAME = 'NOFRAMES'">
				<TH COLSPAN="1" WIDTH="10%"  ALIGN="CENTER"><A HREF="#ANCHOR_TOC" style="text-decoration:none"><SPAN style="color:{$COL_GRAY_LT}; font: bold italic 14px Verdana Arial,Helvetica,sans-serif">TOC</SPAN></A></TH>
			</xsl:when>	
			<xsl:when test="(not($PARAM_FRAME = 'NOFRAMES') and not($iHeaderName = 'Overview'))">
				<TH COLSPAN="1" WIDTH="10%"  ALIGN="CENTER"><A HREF="#ANCHOR_OVERVIEW" style="text-decoration:none"><SPAN style="color:{$COL_GRAY_LT}; font: bold italic 14px Verdana Arial,Helvetica,sans-serif">TOP</SPAN></A></TH>
			</xsl:when>	
		</xsl:choose>
	</TABLE>
</xsl:template>

<xsl:template name="WriteFrame_BLKD">
<HTML>
<HEAD><TITLE>Block Diagram</TITLE></HEAD>
	
<!--
	<xsl:message> Embedded image name <xsl:value-of select="$DS_BLKD_NAME"/></xsl:message>
	<xsl:message> Embedded image name <xsl:value-of select="$DS_BLKD_NAME"/></xsl:message>
	<xsl:message> Embedded image type <xsl:value-of select="$DS_BLKD_TYPE"/></xsl:message>
	<xsl:message> Embedded image width <xsl:value-of select="$DS_BLKD_WIDTH"/></xsl:message>
	<xsl:message> Embedded image height <xsl:value-of select="$DS_BLKD_HEIGHT"/></xsl:message>
-->	
	
	<BODY class="main_body">
		
	<xsl:if test="$DS_BLKD_TYPE = 'svg'">
		<EMBED src="{$DS_BLKD_NAME}" WIDTH="{$DS_BLKD_WIDTH}" height="{$DS_BLKD_HEIGHT}" type="image/svg+xml"/>
	</xsl:if>
	
	<xsl:if test="not($DS_BLKD_TYPE = 'svg')">
		<IMG SRC="{$DS_BLKD_NAME}" alt="BlockDiagram" WIDTH="{$DS_BLKD_WIDTH}" height="{$DS_BLKD_HEIGHT}" BORDER="0" vspace="0" hspace="0"/>
	</xsl:if>
		<BR></BR>
		<BR></BR>
		<xsl:call-template name="WriteContents_ExternalPorts"/>
		<BR></BR>
	</BODY>		
</HTML>
</xsl:template>

<xsl:template name="WriteFrame_TOC">
	<HTML>
		<HEAD>
			<TITLE>Table of Contents</TITLE>
			<BASE target="{$PARAM_FRAME_MAIN}"/>
		</HEAD>
	
		<!-- Layout Table of contents   -->
		<BODY class="main_body">
			<xsl:call-template name="WriteContents_TOCFLAT"/>
		</BODY>		
	</HTML>
</xsl:template>

<xsl:template name="WriteFrame_INDEX">
<HTML>
<HEAD><TITLE><xsl:value-of select="$G_XPS_TITLE"/></TITLE></HEAD>
<HEAD><TITLE>XPS Project Report</TITLE></HEAD>

	<FRAMESET COLS="20%,80%" BORDER="0" FRAMESPACING="0">
	
		<FRAME SRC="{$FNM_TOCTREE_HTML}"
		   MARGINWIDTH="0" 
		   MARINHEIGHT="0" 
		   FRAMEBORDER="NO"
		   BORDER="NO"
		   NAME="{$FRAME_TOCTREE}"
		   SCROLLING="YES"/>
		   
		<FRAME SRC="{$FNM_FRMMAIN_HTML}"
		   MARGINWIDTH="0" 
		   MARINHEIGHT="0"
		   FRAMEBORDER="NO"
		   BORDER="NO"
		   NAME="{$FRAME_FRMMAIN}" 
		   SCROLLING="YES"/>
		   
	</FRAMESET>
</HTML>
</xsl:template>	



</xsl:stylesheet>
