<?xml version="1.0" standalone="no"?>
<!DOCTYPE stylesheet [
	<!ENTITY HEXUPPER "ABCDEFxx0123456789">
	<!ENTITY HEXLOWER "abcdefxX0123456789">
	<!ENTITY HEXU2L " '&HEXLOWER;' , '&HEXUPPER;' ">
]>	
<xsl:stylesheet version="1.0"
           xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
           xmlns:exsl="http://exslt.org/common"
           xmlns:dyn="http://exslt.org/dynamic"
           xmlns:math="http://exslt.org/math"
           xmlns:xlink="http://www.w3.org/1999/xlink"
           extension-element-prefixes="math dyn">
           
<xsl:output method="html"/>

<!--
 	======================= MAIN PERIPHERAL SECTION =============================== 
-->

<xsl:template name="WriteContents_IPSection">
        
<TABLE BGCOLOR="{$COL_WHITE}" WIDTH="{$G_MAIN_WIDTH}" COLS="4" CELLSPACING="0" CELLPADDING="0" BORDER="0">
	
	<TD COLSPAN="4" WIDTH="5%" ALIGN="LEFT" VALIGN="BOTTOM">
		<A name="ANCHOR_{@INSTANCE}"/>
<!-- 
		<xsl:if test="LICENSEINFO"><IMG SRC="imgs/IMG_LicensedCore.bmp" BORDER="0" VSPACE="0" HSPACE="0"/></xsl:if>
 -->	
		<SPAN style="color:{$COL_XLNX}; font: bold italic 14px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@INSTANCE"/></SPAN>
		<xsl:if test="DESCRIPTION[(@TYPE = 'SHORT')]">
			<SPAN style="color:{$COL_XLNX}; font: normal italic 12px Verdana,Arial,Helvetica,sans-serif">
				&#160;&#160;<xsl:value-of select="DESCRIPTION[(@TYPE = 'SHORT')]"/>
			</SPAN>
		</xsl:if>
		<BR></BR>
		<xsl:if test="DESCRIPTION[(@TYPE = 'LONG')]">
			<SPAN style="color:{$COL_BLACK}; font: normal italic 12px Verdana,Arial,Helvetica,sans-serif">
				<xsl:value-of select="DESCRIPTION[(@TYPE = 'LONG')]" disable-output-escaping="yes"/>
			</SPAN>
		</xsl:if>
		<BR></BR>
		<BR></BR>
	</TD>	
	<TR/>
	<!-- Layout the Module Specs table-->
	<TD COLSPAN="2" WIDTH="40%" ALIGN="LEFT" VALIGN="TOP">
		<xsl:call-template name="WriteContents_IpSpecs"/>
		<BR></BR>
		<BR></BR>
	</TD>
	<TR/>
	
	<!-- Layout the Module Ports table-->
	<TD COLSPAN="2" WIDTH="40%" ALIGN="LEFT" VALIGN="TOP">
		<IMG SRC="imgs/{@INSTANCE}.jpg" alt="{@INSTANCE} IP Image" BORDER="0" VSPACE="0" HSPACE="0"/>
	</TD>
	<TD COLSPAN="2" WIDTH="60%" ALIGN="CENTER" VALIGN="TOP">
		<xsl:call-template name="WriteContents_IpPortBifTable"/>
		<BR></BR>
		<BR></BR>
	</TD>
	
	<TR/>
	<!-- Layout the Module Parameters table-->
	<TD COLSPAN="4" WIDTH="100%" ALIGN="LEFT" VALIGN="BOTTOM">
		<xsl:call-template name="WriteContents_IpParametersTable"/>
	</TD>
</TABLE>	
<BR></BR>
<BR></BR>
</xsl:template>


<xsl:template name="WriteContents_IpSpecs">

	<xsl:variable name="numof_docs_" select="count(DOCUMENTATION/DOCUMENT)"/>
	<xsl:variable name="numof_cols_" select="(2 + $numof_docs_)"/>
	
	<xsl:variable name="table_width_"    select="ceiling($G_MAIN_WIDTH div 2)"/>
	<xsl:variable name="col_w_percent_"  select="ceiling(100 div $numof_cols_)"/>
	<xsl:variable name="doc_w_percent_">
		<xsl:if test="($numof_docs_ &gt; 0)"><xsl:value-of select="ceiling((100 - (2 * (100 div $numof_cols_))) div $numof_docs_)"/></xsl:if>	
		<xsl:if test="not($numof_docs_ &gt; 0)">0</xsl:if>	
	</xsl:variable>
	
<!--
	<xsl:if test="$PARAM_DEBUG='TRUE'">
		<xsl:message>Num of columns <xsl:value-of select="$numof_cols_"/></xsl:message>
	</xsl:if>
-->

<!--
	<xsl:if test="$PARAM_DEBUG='TRUE'">
	<xsl:if test="$PARAM_DEBUG='TRUE'">
		<xsl:message>Col width percent <xsl:value-of select="$col_w_percent_"/></xsl:message>
		<xsl:message>Doc width percent <xsl:value-of select="$doc_w_percent_"/></xsl:message>
		<xsl:message>================</xsl:message>
	</xsl:if>
-->

	<xsl:variable name="ipDoc_">
		<xsl:choose>
			<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL">
				<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL"></xsl:value-of>
			</xsl:when>
			<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE">
				<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE"></xsl:value-of>
			</xsl:when>
			<xsl:otherwise>__NODOC__</xsl:otherwise>
	   </xsl:choose>
	</xsl:variable>	
	
	<xsl:variable name="drvDoc_">
		<xsl:choose>
			<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'DRIVER')]/@SRC_LOCAL">
				<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'DRIVER')]/@SRC_LOCAL"></xsl:value-of>
			</xsl:when>
			<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'DRIVER')]/@SOURCE">
				<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'DRIVER')]/@SOURCE"></xsl:value-of>
			</xsl:when>
			<xsl:otherwise>__NODOC__</xsl:otherwise>
	   </xsl:choose>
	</xsl:variable>		
	
	<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{$table_width_}" COLS="{$numof_cols_}" CELLSPACING="1" CELLPADDING="0" BORDER="0">
		<TH COLSPAN="{$numof_cols_}" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">IP Specs</SPAN></TH>
		<TR/>
		<TH COLSPAN="1" WIDTH="{$col_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Core</SPAN></TH>
		<TH COLSPAN="1" WIDTH="{$col_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Version</SPAN></TH>
		<xsl:if test="($numof_docs_ &gt;= 1)">
			<TH COLSPAN="{$numof_docs_}" WIDTH="{($doc_w_percent_ * $numof_docs_)}%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Documentation</SPAN></TH>
		</xsl:if>
		<TR/>
		<TD COLSPAN="1" WIDTH="{$col_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@MODTYPE"/></SPAN></TD>
		<TD COLSPAN="1" WIDTH="{$col_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@HWVERSION"/></SPAN></TD>
		<xsl:if test="not($ipDoc_ = '__NODOC__')">
			<TD COLSPAN="1" WIDTH="{$doc_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="{$ipDoc_}" style="text-decoration:none; color:{$COL_XLNX}"><SPAN style="color:{$COL_XLNX}; font: bold italic 12px Verdana,Arial,Helvetica,sans-serif">IP</SPAN></A></TD>
		</xsl:if>
		<xsl:if test="not($drvDoc_ = '__NODOC__')">
			<TD COLSPAN="1" WIDTH="{$doc_w_percent_}%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="{$drvDoc_}" style="text-decoration:none; color:{$COL_XLNX}"><SPAN style="color:{$COL_XLNX}; font: bold italic 12px Verdana,Arial,Helvetica,sans-serif">DRIVER</SPAN></A></TD>
		</xsl:if>
	</TABLE>
</xsl:template>

<!-- ======================= PERIHERAL TABLE PARTS   =============================== -->

<!-- Layout the Module's Information table -->
<xsl:template name="WriteContents_IpParametersTable">

    <xsl:variable name="moduleRef_" select="self::node()"/>
    <xsl:variable name="moduleIns_"><xsl:value-of select="$moduleRef_/@INSTANCE"/></xsl:variable>
    
 	<xsl:for-each select="$G_MODULES">
	
	    <xsl:variable name="paramsRef_">
	        <xsl:choose>
	            <xsl:when test="$moduleRef_/PARAMETERS"><xsl:text>$moduleRef_/PARAMETERS</xsl:text></xsl:when>
	            <xsl:otherwise><xsl:text>$moduleRef_</xsl:text></xsl:otherwise>
	        </xsl:choose>
	    </xsl:variable>
	    	
		<xsl:variable name="mhsParamCNT_" select="count(dyn:evaluate($paramsRef_)/PARAMETER)"/>
		
		<xsl:variable name="num_of_params_"  select="count(key('G_MAP_PARAMETERS',$moduleIns_))"/>
		
		<xsl:variable name="table_is_split_">
			<xsl:if test="$mhsParamCNT_     &gt;  10">1</xsl:if>
			<xsl:if test="not($mhsParamCNT_ &gt;  10)">0</xsl:if>
		</xsl:variable>
		
		<xsl:variable name="table_width_">
			<xsl:if test="$mhsParamCNT_  &gt;  10"><xsl:value-of select="$G_MAIN_WIDTH"/></xsl:if>
			<xsl:if test="$mhsParamCNT_  &lt;= 10"><xsl:value-of select="ceiling($G_MAIN_WIDTH div 2)"/></xsl:if>
		</xsl:variable>
		
		<xsl:variable name="left_extra_">
			<xsl:if test="($mhsParamCNT_ mod 2)     = 1">1</xsl:if>
			<xsl:if test="not(($mhsParamCNT_ mod 2) = 1)">0</xsl:if>
		</xsl:variable>
		
		<xsl:variable name="num_left_" select="floor($mhsParamCNT_ div 2) + $left_extra_"/>
		<xsl:variable name="num_rhgt_" select="floor($mhsParamCNT_ div 2)"/>
		
		<xsl:variable name="mdr_main_col_">
			<xsl:if test="$mhsParamCNT_  &gt;  10">4</xsl:if>
			<xsl:if test="$mhsParamCNT_  &lt;= 10">2</xsl:if>
		</xsl:variable>
		
		<xsl:variable name="mdr_othr_col_">
			<xsl:if test="$mhsParamCNT_  &gt;  10">2</xsl:if>
			<xsl:if test="$mhsParamCNT_  &lt;= 10">1</xsl:if>
		</xsl:variable>
		
		<xsl:variable name="ipDoc_">
			<xsl:choose>
				<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL">
					<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL"></xsl:value-of>
				</xsl:when>
				<xsl:when test="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE">
					<xsl:value-of select="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE"></xsl:value-of>
				</xsl:when>
				<xsl:otherwise>__NODOC__</xsl:otherwise>
		   </xsl:choose>
		</xsl:variable>	
		
		
		<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{$table_width_}" COLS="5" CELLSPACING="1" CELLPADDING="1" BORDER="1">
	<!-- 
			<xsl:message><xsl:value-of select="$moduleIns_"/> has <xsl:value-of select="$num_of_params_"/> parameters</xsl:message>
			<xsl:if test="LICENSEINFO">
				<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">License</SPAN></TD>
				
				<TR></TR>
				<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">TYPE</SPAN></TD>
				<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
					<SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="LICENSEINFO/@TYPE"/></SPAN>
				</TD>	
				
				<xsl:if test="LICENSEINFO/@EXPIRESON">
					<TR></TR>
					<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER"  BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">EXPIRES ON</SPAN></TD>
					<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
						<SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="LICENSEINFO/@EXPIRESON"/></SPAN>
					</TD>	
				</xsl:if>
			</xsl:if>	
			
	-->		
			<xsl:if test="$mhsParamCNT_ &gt; 0">
			<TR></TR>	
			<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Parameters</SPAN></TD>
				<TR></TR>
				<TD COLSPAN="5" WIDTH="100%" ALIGN="LEFT" BGCOLOR="{$COL_WHITE}">
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
						These are the current parameter settings for this module.
					</SPAN>
					<BR></BR>
					<xsl:if test="not($ipDoc_ ='__NODOC__')">
						 <SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">Please refer to the IP</SPAN> <A HREF="{$ipDoc_}" style="text-decoration:none; color:{$COL_XLNX}"><SPAN style="color:{$COL_XLNX}; font: bold italic 9px Verdana,Arial,Helvetica,sans-serif"> documentation </SPAN></A><SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">for complete information about module parameters.</SPAN>
					</xsl:if>
					<BR></BR>
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
							Parameters marked with 
					</SPAN>
					<SPAN style="color:#FFBB00; font: bold 9px Verdana,Arial,Helvetica,sans-serif">yellow</SPAN>
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
						indicate parameters set by the user.
					</SPAN>
					<BR></BR>
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
							Parameters marked with 
					</SPAN>
					<SPAN style="color:{$COL_CHG_SYS}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">blue</SPAN>
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
						indicate parameters set by the system.
					</SPAN>
				</TD>
				
			<xsl:if test="$mhsParamCNT_ &lt;= 10">
				<TR></TR>	
				<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Name</SPAN></TD>
				<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Value</SPAN></TD>
			
				<xsl:for-each select="dyn:evaluate($paramsRef_)/PARAMETER">
					<xsl:sort select="@INDEX" data-type="number"/>
						<xsl:variable name="name_bg_col_">
							<xsl:choose>
							
								<xsl:when test="@CHANGEDBY='USER'">
									<xsl:value-of select="$COL_CHG_USR"/>
								</xsl:when>		
									
								<xsl:when test="@CHANGEDBY='SYSTEM'">
									<xsl:value-of select="$COL_CHG_SYS"/>
								</xsl:when>		
								
								<xsl:when test="((position() - 1) mod 2) = 0">
									<xsl:value-of select="$COL_GRAY"/>
								</xsl:when>		
								
								<xsl:otherwise>
									<xsl:value-of select="$COL_WHITE"/>
								</xsl:otherwise>		
							</xsl:choose>	
						</xsl:variable>
							
						<xsl:variable name="value_bg_col_">
							<xsl:choose>
								
								<xsl:when test="@CHANGEDBY='USER'">
									<xsl:value-of select="$COL_CHG_USR"/>
								</xsl:when>		
									
								<xsl:when test="@CHANGEDBY='SYSTEM'">
									<xsl:value-of select="$COL_CHG_SYS"/>
								</xsl:when>	
								
								<xsl:when test="((position() - 1) mod 2) = 0">
									<xsl:value-of select="$COL_GRAY"/>
								</xsl:when>		
								
								<xsl:otherwise>
									<xsl:value-of select="$COL_WHITE"/>
								</xsl:otherwise>		
									
							</xsl:choose>	
						</xsl:variable>
							
					<TR></TR>	
					<TD COLSPAN="2" WIDTH="40%" ALIGN="left"   BGCOLOR="{$name_bg_col_}">
						<SPAN style="color:{$COL_BLACK};  font: normal 12px Verdana,Arial,Helvetica,sans-serif">
								<xsl:value-of select="@NAME"/>
						</SPAN>
						<xsl:if test="DESCRIPTION">
							<BR/>
							<SPAN style="color:{$COL_BLACK};  font: bold italic 9px Verdana,Arial,Helvetica,sans-serif">
								<xsl:value-of select="DESCRIPTION"/>
							</SPAN>
						</xsl:if>	
					</TD>
					<xsl:choose>
						<xsl:when test="((@TYPE ='std_logic_vector') or starts-with(@VALUE,'0x') or starts-with(@VALUE,'OX'))">
							<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="translate(@VALUE,&HEXU2L;)"/></SPAN></TD>
						</xsl:when>							
						<xsl:otherwise>
							<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@VALUE"/></SPAN></TD>
						</xsl:otherwise>
					</xsl:choose>					
				</xsl:for-each>
		</xsl:if>			
		
		<xsl:if test="$mhsParamCNT_ &gt; 10">
		<TR></TR>	
		<TD COLSPAN="5" WIDTH="100%">
		<TABLE BGCOLOR="{$COL_GRAY}" WIDTH="100%" COLS="5" CELLSPACING="0" CELLPADDING="0" BORDER="0">
			
			<TD COLSPAN="2" WIDTH="49%">
				<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="100%" COLS="2" CELLSPACING="1" CELLPADDING="0" BORDER="0">
					<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Name</SPAN></TD>
					<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Value</SPAN></TD>
					
					<xsl:for-each select="dyn:evaluate($paramsRef_)/PARAMETER">
						<xsl:sort select="@INDEX" data-type="number"/>
						<xsl:if test="position() &lt;= $num_left_">	
							
							<xsl:variable name="name_bg_col_">
								<xsl:choose>
									<xsl:when test="@CHANGEDBY='USER'">
										<xsl:value-of select="$COL_CHG_USR"/>
									</xsl:when>
									
									<xsl:when test="@CHANGEDBY='SYSTEM'">
										<xsl:value-of select="$COL_CHG_SYS"/>
									</xsl:when>
								
									<xsl:when test="((position() - 1) mod 2) = 0">
										<xsl:value-of select="$COL_GRAY"/>
									</xsl:when>
								
									<xsl:otherwise>
										<xsl:value-of select="$COL_WHITE"/>
									</xsl:otherwise>
								</xsl:choose>
							</xsl:variable>
							
							<xsl:variable name="value_bg_col_">
								<xsl:choose>
									<xsl:when test="@CHANGEDBY='USER'">
										<xsl:value-of select="$COL_CHG_USR"/>
									</xsl:when>		
									
									<xsl:when test="@CHANGEDBY='SYSTEM'">
										<xsl:value-of select="$COL_CHG_SYS"/>
									</xsl:when>		
								
									<xsl:when test="((position() - 1) mod 2) = 0">
										<xsl:value-of select="$COL_GRAY"/>
									</xsl:when>		
								
									<xsl:otherwise>
										<xsl:value-of select="$COL_WHITE"/>
									</xsl:otherwise>		
								</xsl:choose>	
							</xsl:variable>
							
						<TR></TR>	
						<TD COLSPAN="1" WIDTH="50%" ALIGN="left"   BGCOLOR="{$name_bg_col_}">
							<SPAN style="color:{$COL_BLACK};  font: normal 12px Verdana,Arial,Helvetica,sans-serif">
								<xsl:value-of select="@NAME"/>
							</SPAN>
							<xsl:if test="DESCRIPTION">
								<BR/>
								<SPAN style="color:{$COL_BLACK};  font: bold italic 9px Verdana,Arial,Helvetica,sans-serif">
									<xsl:value-of select="DESCRIPTION"/>
								</SPAN>
							</xsl:if>	
						</TD>
						<xsl:choose>
							<xsl:when test="((@TYPE ='std_logic_vector') or starts-with(@VALUE,'0x') or starts-with(@VALUE,'OX'))">
								<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="translate(@VALUE,&HEXU2L;)"/></SPAN></TD>
							</xsl:when>							
							<xsl:otherwise>
								<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@VALUE"/></SPAN></TD>
							</xsl:otherwise>
						</xsl:choose>							
					</xsl:if>
				</xsl:for-each>
				</TABLE>
			</TD> 
		
			<TD COLSPAN="1" WIDTH="2%">
				<TABLE BGCOLOR="{$COL_GRAY}"  WIDTH="100%" COLS="1" CELLSPACING="0" CELLPADDING="0" BORDER="0">
					<TD COLSPAN="1" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_XLNX}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;</SPAN></TD>
				</TABLE>
			</TD> 
		
			<TD COLSPAN="2" WIDTH="49%">
				<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="100%" COLS="2" CELLSPACING="1" CELLPADDING="0" BORDER="0">
					<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Name</SPAN></TD>
					<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Value</SPAN></TD>
					<xsl:for-each select="dyn:evaluate($paramsRef_)/PARAMETER">
						<xsl:sort select="@INDEX" data-type="number"/>
						<xsl:if test="position() &gt; $num_left_">	
							
							<xsl:variable name="name_bg_col_">
								<xsl:choose>
									<xsl:when test="@CHANGEDBY='USER'">
										<xsl:value-of select="$COL_CHG_USR"/>
									</xsl:when>
									
									<xsl:when test="@CHANGEDBY='SYSTEM'">
										<xsl:value-of select="$COL_CHG_SYS"/>
									</xsl:when>
								
									<xsl:when test="((position() - $num_left_ - 1) mod 2) = 0">
										<xsl:value-of select="$COL_GRAY"/>
									</xsl:when>
								
									<xsl:otherwise>
										<xsl:value-of select="$COL_WHITE"/>
									</xsl:otherwise>
								</xsl:choose>	
							</xsl:variable>
							
							<xsl:variable name="value_bg_col_">
								<xsl:choose>
								
									<xsl:when test="@CHANGEDBY='USER'">
										<xsl:value-of select="$COL_CHG_USR"/>
									</xsl:when>		
									
									<xsl:when test="@CHANGEDBY='SYSTEM'">
										<xsl:value-of select="$COL_CHG_SYS"/>
									</xsl:when>		
								
									<xsl:when test="((position() - $num_left_ - 1) mod 2) = 0">
										<xsl:value-of select="$COL_GRAY"/>
									</xsl:when>		
								
									<xsl:otherwise>
										<xsl:value-of select="$COL_WHITE"/>
									</xsl:otherwise>		
								</xsl:choose>	
							</xsl:variable>
						<TR></TR>	
						<TD COLSPAN="1" WIDTH="50%" ALIGN="LEFT"  BGCOLOR="{$name_bg_col_}">
							<SPAN style="color:{$COL_BLACK};  font: normal 12px Verdana,Arial,Helvetica,sans-serif">
								<xsl:value-of select="@NAME"/>
							</SPAN>
							<xsl:if test="DESCRIPTION">
								<BR/>
								<SPAN style="color:{$COL_BLACK};  font: bold italic 9px Verdana,Arial,Helvetica,sans-serif">
									<xsl:value-of select="DESCRIPTION"/>
								</SPAN>
							</xsl:if>	
						</TD>
						<xsl:choose>
							<xsl:when test="@TYPE ='std_logic_vector'">
								<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="translate(@VALUE,&HEXU2L;)"/></SPAN></TD>
							</xsl:when>
							<xsl:when test="(starts-with(@VALUE,'0x') or starts-with(@VALUE,'OX'))">
								<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="translate(@VALUE,&HEXU2L;)"/></SPAN></TD>
							</xsl:when>							
							<xsl:otherwise>
								<TD COLSPAN="1" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$value_bg_col_}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@VALUE"/></SPAN></TD>
							</xsl:otherwise>
						</xsl:choose>
						
					</xsl:if>
				</xsl:for-each>
				
				<xsl:if test="$left_extra_ &gt; 0">
					<TR></TR>	
					<TD COLSPAN="2" WIDTH="100%" ALIGN="left" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_WHITE}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;</SPAN></TD>
				</xsl:if>
				</TABLE>
			</TD> 
			
		</TABLE>
		</TD>	
		</xsl:if>			
		</xsl:if>
		</TABLE>
	<!--	
			<BR></BR>	
	-->		
			<xsl:if test="(@MODCLASS = 'PROCESSOR') and MEMORYMAP">
	<!--			
				<xsl:message>Found a memory map</xsl:message>
	-->			
			<xsl:call-template name="WriteContents_MemoryMap">
					<xsl:with-param name="table_width" select="$table_width_"/>
				</xsl:call-template>	
			</xsl:if>
	<!--		
			<BR></BR>
	-->	
			<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{$table_width_}" COLS="5" CELLSPACING="1" CELLPADDING="1" BORDER="0">
			<TD WIDTH="100%" COLSPAN="5" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Post Synthesis Device Utilization</SPAN></TD>
			<xsl:choose>
				<xsl:when test="not(RESOURCES)">
					<TR></TR>
					<TD WIDTH="100%" COLSPAN="5" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
					<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">
						Device utilization information is not available for this IP. Run platgen to generate synthesis information.
					</SPAN>
					</TD>
				</xsl:when>	
				<xsl:otherwise>
					<TR></TR>
					<TD COLSPAN="2" WIDTH="55%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Resource Type</SPAN></TD>
					<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Used</SPAN></TD>
					<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Available</SPAN></TD>
					<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Percent</SPAN></TD>
					
					<xsl:for-each select="RESOURCES/RESOURCE">
						<TR></TR>	
						<TD COLSPAN="2" WIDTH="55%" ALIGN="left"   BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@TYPE"/></SPAN></TD>
						<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@USED"/></SPAN></TD>
						<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@TOTAL"/></SPAN></TD>
						<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@PERCENT"/></SPAN></TD>
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
	<!--		
		    <BR></BR>	
			<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"></SPAN></TD>
	-->		
		</TABLE>
  </xsl:for-each>	
</xsl:template>

<!-- Layout the Module's Port list table -->
<xsl:template name="WriteContents_IpPortBifTable">

    <xsl:variable name="moduleRef_" select="self::node()"/>
	<xsl:variable name="moduleIns_"><xsl:value-of select="$moduleRef_/@INSTANCE"/></xsl:variable>
    
	<xsl:variable name="portsRef_">
	    <xsl:choose>
	        <xsl:when test="$moduleRef_/PORTS"><xsl:text>$moduleRef_/PORTS</xsl:text></xsl:when>
	        <xsl:otherwise><xsl:text>$moduleRef_</xsl:text></xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
	
    <xsl:variable name="portsAllRef_">
        <xsl:choose>
            <xsl:when test="$G_MODULES/MODULE/PORTS"><xsl:text>$G_MODULES/MODULE/PORTS</xsl:text></xsl:when>
            <xsl:otherwise><xsl:text>$G_MODULES/MODULE/</xsl:text></xsl:otherwise>
        </xsl:choose>
    </xsl:variable>	
	
	<xsl:variable name="busifsRef_">
	    <xsl:choose>
	        <xsl:when test="$moduleRef_/BUSINTERFACES"><xsl:text>$moduleRef_/BUSINTERFACES</xsl:text></xsl:when>
	        <xsl:otherwise><xsl:text>$moduleRef_</xsl:text></xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
	
    <xsl:variable name="busifsAllRef_">
        <xsl:choose>
            <xsl:when test="$G_MODULES/MODULE/BUSINTERFACES"><xsl:text>$G_MODULES/MODULE/BUSINTERFACES</xsl:text></xsl:when>
            <xsl:otherwise><xsl:text>$G_MODULES/MODULE</xsl:text></xsl:otherwise>
        </xsl:choose>
    </xsl:variable>	
    
    <xsl:variable name="ipDoc_">
		<xsl:choose>
			<xsl:when test="$moduleRef_/DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL">
				<xsl:value-of select="$moduleRef_/DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SRC_LOCAL"/>
			</xsl:when>
			<xsl:when test="$moduleRef_/DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE">
				<xsl:value-of select="$moduleRef_/DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE"/>
			</xsl:when>
			<xsl:otherwise>__NODOC__</xsl:otherwise>
	   </xsl:choose>
	</xsl:variable>	
	
<!--
	<xsl:message><xsl:value-of select="$ipDoc_"/></xsl:message>
-->	

	<TABLE BGCOLOR="{$COL_BLACK}" WIDTH="{ceiling($G_MAIN_WIDTH div 2)}" COLS="7" CELLSPACING="1" CELLPADDING="1" BORDER="0">
	
		<xsl:if test="dyn:evaluate($portsRef_)/PORT[(@IS_INMHS or @IS_INSTANTIATED)]">
			<TH COLSPAN="7" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">PORT LIST</SPAN></TH>
		 	<TR/>
			<TH COLSPAN="7" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
				<SPAN style="color:{$COL_INFO}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">These are the ports listed in the MHS file. 
					<xsl:if test="not($ipDoc_ = '__NODOC__')">
						Please refer to the IP <A HREF="{$ipDoc_}" style="text-decoration:none; color:{$COL_XLNX}"> <SPAN style="color:{$COL_XLNX}; font: bold italic 9px Verdana,Arial,Helvetica,sans-serif"> documentation </SPAN> </A>for complete information about module ports.
					</xsl:if>
				</SPAN>
			</TH>
			<TR/>
			<TH COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">#</SPAN></TH>
			<TH COLSPAN="2" WIDTH="25%" ALIGN="CENTER"   BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">NAME</SPAN></TH>
			<TH COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">DIR</SPAN></TH>
			<TH COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">[LSB:MSB]</SPAN></TH>
			<TH COLSPAN="2" WIDTH="50%" ALIGN="CENTER"   BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">SIGNAL</SPAN></TH>
			<xsl:for-each select="dyn:evaluate($portsRef_)/PORT[(not(@SIGNAME = '__NOC__') and not(@SIGNAME = '__DEF__') and (@IS_INMHS or @IS_INSTANTIATED))]">
				<xsl:sort data-type="number" select="@MHS_INDEX" order="ascending"/>
				<TR></TR>	
				<TD COLSPAN="1" WIDTH="5%"  ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK};   font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@MHS_INDEX"/></SPAN></TD>
				<TD COLSPAN="2" WIDTH="25%" ALIGN="LEFT" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK};   font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
				<TD COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@DIR"/></SPAN></TD>
				
				<xsl:if test="@MSB and @LSB">
					<TD COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@LSB"/>:<xsl:value-of select="@MSB"/></SPAN></TD>
				</xsl:if>			
				<xsl:if test="not(@MSB and @LSB)">
					<TD COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">1</SPAN></TD>
				</xsl:if>			
				
				<TD COLSPAN="2" WIDTH="50%" ALIGN="LEFT" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@SIGNAME"/></SPAN></TD>
			</xsl:for-each>
		</xsl:if>
		
		<xsl:if test="dyn:evaluate($busifsRef_)/BUSINTERFACE[(not(@BUSNAME = '__NOC__') and not(@IS_VALID = 'FALSE'))]">
			<TR></TR>
			<TH COLSPAN="7" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}">
				<SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Bus Interfaces</SPAN>
			</TH>	
			<TR></TR>
																														
			<TH COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;NAME&#160;</SPAN></TH>
			<TH COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">&#160;TYPE&#160;</SPAN></TH>
			<TH COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">BUSSTD</SPAN></TH>
			<TH COLSPAN="3" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">BUS</SPAN></TH>
			<TH COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY_DK}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Connected To</SPAN></TH>
			
			<xsl:for-each select="dyn:evaluate($busifsRef_)/BUSINTERFACE[(not(@BUSNAME = '__NOC__') and not(@BUSNAME = '__DEF__'))]">
				<xsl:sort select="@TYPE"/>
					
				<xsl:variable name="busname_"><xsl:value-of select="@BUSNAME"/></xsl:variable>
				<xsl:variable name="numCons_" select="count($G_MODULES/MODULE[not(@INSTANCE = $moduleIns_) and (BUSINTERFACE[(@BUSNAME = $busname_)] or BUSINTERFACES/BUSINTERFACE[(@BUSNAME = $busname_)])])"/>
					
				<xsl:variable name="connections_">
					<xsl:choose>
						<xsl:when test="($numCons_ = '1')">
							<xsl:value-of select="$G_MODULES/MODULE[not(@INSTANCE = $moduleIns_) and (BUSINTERFACE[(@BUSNAME = $busname_)] or BUSINTERFACES/BUSINTERFACE[(@BUSNAME = $busname_)])]/@INSTANCE"/>
						</xsl:when>
						<xsl:otherwise><xsl:value-of select="$numCons_"/> Peripherals.</xsl:otherwise>
				   </xsl:choose>
				</xsl:variable>					
				
				<xsl:variable name="busStd_">
					<xsl:if test="@BUSSTD_PSF">
						<xsl:value-of select="@BUSSTD_PSF"/>
					</xsl:if>
					<xsl:if test="not(@BUSSTD)">NA</xsl:if>
				</xsl:variable>					
				
				<TR></TR>
				<TD COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
				<TD COLSPAN="1" WIDTH="15%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@TYPE"/></SPAN></TD>
				<TD COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$busStd_"/></SPAN></TD>
                <!-- The bus name -->
                <xsl:choose>
					<xsl:when test="$busStd_ = 'NA'">
						<TD COLSPAN="3" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@BUSNAME"/></SPAN></TD>
					</xsl:when>
					<xsl:otherwise>
						<TD COLSPAN="3" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="#ANCHOR_{@BUSNAME}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@BUSNAME"/></SPAN></A></TD>
					</xsl:otherwise>
                </xsl:choose>
				
                <!-- The bus connection -->
                <xsl:choose>
					<xsl:when test="($numCons_ &gt; '1')">
						<TD COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold italic 11px san-serif Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$connections_"/></SPAN></TD>
					</xsl:when>
					<xsl:otherwise> <!-- Add a link to the actual element, when there is only one  -->
					    <TD COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="#ANCHOR_{$connections_}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$connections_"/></SPAN></A></TD>
					</xsl:otherwise>
                </xsl:choose>
			</xsl:for-each>
		</xsl:if>
		
		<!-- 
		      If this is a bus, list out is connections
		 -->
		<xsl:if test="@MODCLASS='BUS'">
			<xsl:variable name="instance_"><xsl:value-of select="@INSTANCE"/></xsl:variable>
			<TR></TR>
			<TH COLSPAN="7" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">Bus Connections</SPAN></TH>	
			<TR></TR>
			<TH COLSPAN="5" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">INSTANCE</SPAN></TH>
			<TH COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">INTERFACE TYPE</SPAN></TH>
			<TH COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">INTERFACE NAME</SPAN></TH>
			<xsl:for-each select="dyn:evaluate($busifsAllRef_)/BUSINTERFACE[(@BUSNAME = $instance_)]">
				<xsl:sort select="@TYPE"/>
				
			    <xsl:variable name="cnctdInst_">
			        <xsl:choose>
			            <xsl:when test="../@INSTANCE"><xsl:value-of select="../@INSTANCE"/></xsl:when>
			            <xsl:otherwise><xsl:value-of select="../../@INSTANCE"/></xsl:otherwise>
			        </xsl:choose>
			    </xsl:variable> 				
			    
				<!-- <xsl:variable name="buscName_"><xsl:value-of select="../@INSTANCE"/></xsl:variable> -->
				
				<TR></TR>
				<TD COLSPAN="5" WIDTH="50%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="#ANCHOR_{$cnctdInst_}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: italic 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$cnctdInst_"/></SPAN></A></TD>
				<TD COLSPAN="1" WIDTH="25%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@TYPE"/></SPAN></TD>
				<TD COLSPAN="1" WIDTH="25%" ALIGN="CENTER"   BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@NAME"/></SPAN></TD>
			</xsl:for-each>
		</xsl:if>
		
		
        <!-- 
            If this is an interrupt controller, list out the sources and targets of its interrupts
        -->		
		<xsl:if test="INTERRUPTINFO[(@TYPE = 'CONTROLLER')]">
			<TR></TR>
			<TH COLSPAN="7" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}">
				<SPAN style="color:{$COL_WHITE}; font: bold 9px Verdana,Arial,Helvetica,sans-serif">Interrupt Priorities</SPAN>
				<TR></TR>
				<TH COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Priority</SPAN></TH>
				<TH COLSPAN="3" WIDTH="55%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">SIG</SPAN></TH>
				<TH COLSPAN="3" WIDTH="35%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">MODULE</SPAN></TH>
				<TR></TR>
				<xsl:for-each select="INTERRUPTINFO[(@TYPE = 'CONTROLLER')]/SOURCE">
				    <xsl:sort data-type="number" select="@PRIORITY" order="ascending"/>
				
					<xsl:variable name="intrins_"><xsl:value-of select="@INSTANCE"/></xsl:variable>
					<xsl:variable name="intrsig_"><xsl:value-of select="@SIGNAME"/></xsl:variable>
					<xsl:variable name="intrpri_"><xsl:value-of select="@PRIORITY"/></xsl:variable>
					<!-- 
					   <xsl:for-each select="dyn:evaluate($portsAllRef_)/PORT[((@DIR='O') and (@SIGNAME = $intrsrc_))]">
					   </xsl:for-each>
					 -->
					<TR></TR>
					<TH COLSPAN="1" WIDTH="10%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$intrpri_"/></SPAN></TH>
					<TH COLSPAN="3" WIDTH="55%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$intrsig_"/></SPAN></TH>
					<TH COLSPAN="3" WIDTH="35%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><A HREF="#ANCHOR_{$intrins_}" style="text-decoration:none"><SPAN style="color:{$COL_XLNX}; font: normal 12px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="$intrins_"/></SPAN></A></TH>
				</xsl:for-each>
			</TH>
		</xsl:if>
	</TABLE>	
	
</xsl:template>
</xsl:stylesheet>

<!-- 
		<TD COLSPAN="5" WIDTH="100%" ALIGN="CENTER" BGCOLOR="{$COL_XLNX}"><SPAN style="color:{$COL_WHITE}; font: bold 12px Verdana,Arial,Helvetica,sans-serif">General</SPAN></TD>
		<TR></TR>
		<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER"  BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">IP Core</SPAN></TD>
		<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER"  BGCOLOR="{$COL_WHITE}">
			<xsl:if test="DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]">
				<SPAN style="color:{$COL_XLNX}; font: bold italic 10px Verdana,Arial,Helvetica,sans-serif">
					<A HREF="{DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')]/@SOURCE}" style="text-decoration:none; color:{$COL_XLNX}"><xsl:value-of select="@MODTYPE"/></A>
				</SPAN>
			</xsl:if>
			<xsl:if test="not(DOCUMENTATION/DOCUMENT[(@TYPE = 'IP')])">
				<SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">
					<xsl:value-of select="@MODTYPE"/>
				</SPAN>
			</xsl:if>
		</TD>
		<TR></TR>	
		
		<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Version</SPAN></TD>
		<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif"><xsl:value-of select="@HWVERSION"/></SPAN></TD>
		
		<xsl:if test="DOCUMENTATION/DOCUMENT[(@TYPE = 'DRIVER')]">
			<TR></TR>	
			<TD COLSPAN="2" WIDTH="40%" ALIGN="CENTER" BGCOLOR="{$COL_GRAY}"><SPAN style="color:{$COL_BLACK}; font: bold 10px Verdana,Arial,Helvetica,sans-serif">Driver</SPAN></TD>
			<TD COLSPAN="3" WIDTH="60%" ALIGN="CENTER" BGCOLOR="{$COL_WHITE}">
				<SPAN style="color:{$COL_XLNX}; font: bold italic 10px Verdana,Arial,Helvetica,sans-serif">
					<A HREF="{DOCUMENTATION/DOCUMENT[@TYPE = 'DRIVER']/@SOURCE}" style="text-decoration:none; color:{$COL_XLNX}">API</A>
				</SPAN>
			</TD>	
		</xsl:if>	
		
		<TR></TR>	
-->
