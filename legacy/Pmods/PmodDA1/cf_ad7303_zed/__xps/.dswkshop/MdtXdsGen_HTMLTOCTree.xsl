<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns:xlink="http://www.w3.org/1999/xlink">

<xsl:strip-space elements="*"/>

<xsl:variable name="FRAME_TOCTREE"><xsl:value-of select="$PARAM_PREFIX"/>_toc</xsl:variable>

<!--
<xsl:variable name="FNM_TOCTREE_JS"><xsl:value-of select="$FRAME_TOCTREE"/>.js</xsl:variable>
<xsl:variable name="FNM_TOCTREE_CSS"><xsl:value-of select="$FRAME_TOCTREE"/>.css</xsl:variable>
-->

<xsl:variable name="FNM_TOCTREE_HTML"><xsl:value-of select="$FRAME_TOCTREE"/>.html</xsl:variable>

<xsl:variable name="IMG_TOCTREE_BROPEN">imgs/IMG_openBranch.png</xsl:variable>
<xsl:variable name="IMG_TOCTREE_BRCLSE">imgs/IMG_closeBranch.png</xsl:variable>

<xsl:template name="WriteFrame_TOCTREE">
	<HTML>
		<HEAD>
			<TITLE>Table of Contents</TITLE>
			
			<BASE target="{$FRAME_FRMMAIN}"></BASE>
			
			<!--specify a css for the TOC -->
			<STYLE type="text/css">
				<xsl:call-template name="WriteContents_TOCTree_CSS"/>
			</STYLE>
			
			<!--specify the javascript for the TOC-->
			<SCRIPT type="text/javascript">
				<xsl:call-template name="WriteContents_TOCTree_Script"/>
			</SCRIPT>
		</HEAD>	
	
		<!--Layout Table of contents   -->
		<BODY class="main_body">
			<xsl:call-template name="WriteContents_TOCTree_HTML"/>
		</BODY>
	</HTML>
</xsl:template>

<!-- ======================= LAYOUT TABLE OF CONTENTS ====================================== -->

<xsl:template name="WriteContents_TOCTree_HTML">
	
	<xsl:variable name="toctree_col_"    select="$COL_WHITE"/>
	<xsl:variable name="toctree_width_"  select="$G_TOC_WIDTH"/>
	
	<TABLE BGCOLOR="{$toctree_col_}" WIDTH="{$toctree_width_}" COLS="1" cellspacing="0" cellpadding="0" border="0">
		<TD COLSPAN="1" VALIGN="Top">
		<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_OVERVIEW"      style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Overview</SPAN></A><BR/>
		<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_BLOCKDIAGRAM"  style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Block Diagram</SPAN></A><BR/>
		<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_EXTERNALPORTS" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">External Ports</SPAN></A><BR/>
		
		<!-- 
			========================================
						PROCESSORS	
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_PROCESSORS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Processors'); swapBranchImg('BranchImg_Processors');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Processors"></IMG>
				<xsl:if test="($G_NUMOF_PROCESSORS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Processors</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_PROCESSORS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Processor</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Processors">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'PROCESSOR')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
<!-- 
					<xsl:if test="MEMORYMAP/MEMRANGE[(@INSTANCE)]">
						<A HREF="{$FNM_FRMMAIN_HTML}#_{@INSTANCE}_MemoryMap" style="text-decoration:one"><SPAN style="color:{$DS_COL_BLACK}; font: italic14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;memory map</SPAN></A><xsl:if test="LICENSEINFO"><IMG SRC="imgs/IMG_LicensedCore.bmp" border="0" vspace="0" hspace="0"/></xsl:if><BR></BR>
					</xsl:if>
-->					
				</xsl:for-each>
			</SPAN>	
		</xsl:if>
		
		
		<!-- 
			========================================
						DEBUGGERS	
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_DEBUGGERS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Debuggers'); swapBranchImg('BranchImg_Debuggers');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Debuggers"></IMG>
				<xsl:if test="($G_NUMOF_DEBUGGERS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Debuggers</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_DEBUGGERS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Debuggers</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Debuggers">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'DEBUG')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>
		
		<!-- 
			========================================
						INTERRUPT CONTROLLERS	
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_INTERRUPT_CNLTRS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Interrupt_Cntlrs'); swapBranchImg('BranchImg_Interrupt_Cntlrs');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Interrupt_Cntlrs"></IMG>
				<xsl:if test="($G_NUMOF_INTERRUPT_CNLTRS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Interrupt Controllers</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_INTERRUPT_CNLTRS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Interrupt Controllers</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Interrupt_Cntlrs">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'INTERRUPT_CNTLR')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>
		
		<!-- 
			========================================
						BUSSES
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_BUSSES &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Busses'); swapBranchImg('BranchImg_Busses');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Busses"></IMG>
				<xsl:if test="($G_NUMOF_BUSSES &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Busses</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_BUSSES = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Bus</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Busses">		
				<xsl:for-each select="$G_MODULES/MODULE[((@MODCLASS = 'BUS') or (@MODCLASS = 'BUS_ARBITER'))]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>
		
		
		<!-- 
			========================================
						BUS BRIDGES
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_BRIDGES &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Bridges'); swapBranchImg('BranchImg_Bridges');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Bridges"></IMG>
				<xsl:if test="($G_NUMOF_BRIDGES &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Bridges</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_BRIDGES = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Bridge</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Bridges">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'BUS_BRIDGE')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>
		
		<!-- 
			========================================
						MEMORY
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_MEMORYS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Memory'); swapBranchImg('BranchImg_Memory');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Memory"></IMG>
				<xsl:if test="($G_NUMOF_MEMORYS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Memorys</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_MEMORYS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Memory</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Memory">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'MEMORY')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>		
		
		<!-- 
			========================================
						MEMORY CONTROLLERS
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_MEMORY_CNTLRS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Memory_Cntlrs'); swapBranchImg('BranchImg_Memory_Cntlrs');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Memory_Cntlrs"></IMG>
				<xsl:if test="($G_NUMOF_MEMORY_CNTLRS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Memory Controllers</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_MEMORY_CNTLRS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Memory Controller</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Memory_Cntlrs">		
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'MEMORY_CNTLR')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>		
		
		<!-- 
			========================================
						PERIPHERALS 
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_PERIPHERALS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('Peripherals'); swapBranchImg('BranchImg_Peripherals');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Peripherals"></IMG>
				<xsl:if test="($G_NUMOF_PERIPHERALS &gt; 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;Peripherals</SPAN>
				</xsl:if>
				<xsl:if test="($G_NUMOF_PERIPHERALS = 1)">
					<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana ArialHelvetica,sans-serif">&#160;Peripheral</SPAN>
				</xsl:if>
			</DIV>
			<SPAN class="branch" id="Peripherals">
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'PERIPHERAL')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>		
		
		<!-- 
			========================================
						IP 
			========================================
		-->
		
		<xsl:if test="($G_NUMOF_IPS &gt; 0)">
			<DIV class="trigger" onClick="showBranch('IP'); swapBranchImg('BranchImg_Ips');">
				<IMG src="{$IMG_TOCTREE_BROPEN}" border="0" id="BranchImg_Ips"></IMG>
				<SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">&#160;IP</SPAN>
			</DIV>
			<SPAN class="branch" id="IP">
				<xsl:for-each select="$G_MODULES/MODULE[(@MODCLASS = 'IP')]">
					<xsl:sort select="@INSTANCE"/>
					<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_{@INSTANCE}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 14px Verdana Arial,Helvetica,sans-serif">&#160;&#160;&#160;<xsl:value-of select="@INSTANCE"/></SPAN></A><BR></BR>
				</xsl:for-each>
			</SPAN>	
		</xsl:if>		
		
		<A HREF="{$FNM_FRMMAIN_HTML}#ANCHOR_TIMINGINFO" style="text-decoration:none"><SPAN style="color:{$COL_BLACK}; font: bold 16px Verdana Arial,Helvetica,sans-serif">Timing Information</SPAN></A><BR></BR>
			
		</TD>
		
	</TABLE>
</xsl:template>

<xsl:template name="WriteContents_TOCTree_Script">
	var openImg  = new Image();
	var clseImg  = new Image();
	
	openImg.src   = "<xsl:value-of select="$IMG_TOCTREE_BROPEN"/>";
	clseImg.src   = "<xsl:value-of select="$IMG_TOCTREE_BRCLSE"/>";
		
	function showBranch(iBranchId) {
		
		var branchObj = document.getElementById(iBranchId).style;
			
		if(branchObj.display== "block")
		   branchObj.display = "none";
		else   
		   branchObj.display = "block";
	}
		
	function swapBranchImg(iBranchImgId) {
		
		branchImg = document.getElementById(iBranchImgId);
		
		if(branchImg.src.indexOf('<xsl:value-of select="$IMG_TOCTREE_BRCLSE"/>') > -1)
			branchImg.src = openImg.src;
		else   
		    branchImg.src = clseImg.src;
	}
</xsl:template>

<xsl:template name="WriteContents_TOCTree_CSS">
	.trigger {
		cursor : hand;
		cursor : pointer;
	}
	
	.branch {
		display     : none;
		margin-left : 16px;
	}
</xsl:template>

</xsl:stylesheet>
<!-- ======================= END LAYOUT TABLE OF CONTENT TREE =================================== -->
