<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match='/'>

<html>
<head>
<title>Dialog Preview -
</title>
<script type='text/javascript'>
	document.querySelector('title').innerHTML += window.location.pathname.split("/").pop();
</script>
<link rel='stylesheet' type='text/css' href='dialog.css'/>
</head>

<body>
<xsl:if test='/dialog/@skin'>
	<xsl:attribute name='style'>
	background-image: url('img/bg/<xsl:value-of select='/dialog/@skin'/>.png');
	</xsl:attribute>
</xsl:if>
<xsl:if test='/dialog/@skin = "dark" or not(/dialog/@skin)'>
	<xsl:attribute name='class'>dark</xsl:attribute>
</xsl:if>
<div class='dialog'>

<xsl:apply-templates select='*'/>

</div>
</body>
</html>

</xsl:template>

<xsl:template name='set-bounds'>
	<!-- To be called within a style attribute -->
	<xsl:param name='setHeight'>true</xsl:param>
	<xsl:param name='setWidth'>true</xsl:param>
	
	<xsl:variable name='top'>	
		<xsl:choose>
			<xsl:when test='../@top'>
				<xsl:value-of select='./@top - ../@top'/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select='./@top'/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name='left'>	
		<xsl:choose>
			<xsl:when test='../@left'>
				<xsl:value-of select='./@left - ../@left'/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select='./@left'/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	left: <xsl:value-of select='$left'/>px;
	top: <xsl:value-of select='$top'/>px;
	<xsl:if test='$setWidth = "true" and ./@width &gt; 0'>
		width: <xsl:value-of select='./@width'/>px;
	</xsl:if>
	<xsl:if test='$setHeight = "true" and ./@height &gt; 0'>
		height: <xsl:value-of select='./@height'/>px;
	</xsl:if>
</xsl:template>

<xsl:template match='pict'>
	<div>
	
	<xsl:attribute name='class'>
		pict
		<xsl:if test='@framed = "true" or not(@framed)'>framed </xsl:if>
		<xsl:value-of select='./@type'/>
		<xsl:if test='@size = "large" and @type != "monst"'> large</xsl:if>
		<xsl:if test='@size = "small" and @type = "item"'> tiny</xsl:if>
	</xsl:attribute>
	<xsl:if test='/dialog/@debug = "true" and @name'>
		<xsl:attribute name='title'>
			<xsl:value-of select='./@name'/>
		</xsl:attribute>
	</xsl:if>
	<xsl:choose>
		<xsl:when test='@type = "full"'>
			<xsl:attribute name='style'>
				background-image: url('img/other/<xsl:value-of select='./@num'/>.png');
				<xsl:call-template name='set-bounds'/>
			</xsl:attribute>
		</xsl:when>
		<xsl:when test='@type = "blank"'>
			<xsl:attribute name='style'>
				background-color: black;
				<xsl:call-template name='set-bounds'>
					<xsl:with-param name='setHeight'>false</xsl:with-param>
					<xsl:with-param name='setWidth'>false</xsl:with-param>
				</xsl:call-template>
			</xsl:attribute>
		</xsl:when>
		<xsl:when test='@size = "large" and @type != "monst"'>
			<xsl:attribute name='style'>
				background-image: url('img/<xsl:value-of select='./@type'/>/large<xsl:value-of select='./@num'/>.png');
				<xsl:call-template name='set-bounds'>
					<xsl:with-param name='setHeight'>false</xsl:with-param>
					<xsl:with-param name='setWidth'>false</xsl:with-param>
				</xsl:call-template>
			</xsl:attribute>
		</xsl:when>
		<xsl:otherwise>
			<xsl:attribute name='style'>
				background-image: url('img/<xsl:value-of select='./@type'/>/<xsl:value-of select='./@num'/>.png');
				<xsl:call-template name='set-bounds'>
					<xsl:with-param name='setHeight'>false</xsl:with-param>
					<xsl:with-param name='setWidth'>false</xsl:with-param>
				</xsl:call-template>
			</xsl:attribute>
		</xsl:otherwise>
	</xsl:choose>
	
	</div>
</xsl:template>

<xsl:template match='button'>
	<div>
	
	<xsl:attribute name='class'>
		<xsl:if test='/dialog/@debug = "true" and @type = "tiny"'>debug</xsl:if>
		button <xsl:value-of select='./@type'/>
	</xsl:attribute>
	<xsl:attribute name='style'>
		background-image: url('img/button/<xsl:value-of select='./@type'/>.png');
		<xsl:call-template name='set-bounds'>
			<xsl:with-param name='setHeight'>false</xsl:with-param>
			<xsl:with-param name='setWidth'>
				<xsl:if test='@type = "push" or @type = "tiny"'>true</xsl:if>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:attribute>
	<xsl:attribute name='title'>
		<xsl:if test='/dialog/@debug = "true" and @name'>
			<xsl:value-of select='./@name'/>
		</xsl:if>
		<xsl:if test='@def-key'>
			(<xsl:value-of select='./@def-key'/>)
		</xsl:if>
	</xsl:attribute>
	<xsl:value-of select='.'/>
	
	</div>
</xsl:template>

<xsl:template match='led'>
	<div>
	
	<xsl:attribute name='class'>
		led 
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
	</xsl:attribute>
	<xsl:if test='/dialog/@debug = "true" and @name'>
		<xsl:attribute name='title'>
			<xsl:value-of select='./@name'/>
		</xsl:attribute>
	</xsl:if>
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/> <xsl:value-of select='/dialog/@fore'/>;
		font-family:
		<xsl:choose>
			<xsl:when test='@font = "plain"'>'DejaVu Sans', Geneva</xsl:when>
			<xsl:when test='@font = "bold"'>Capriola, Silom</xsl:when>
			<xsl:otherwise><xsl:value-of select='./@font'/></xsl:otherwise>
		</xsl:choose>;
		<xsl:if test='./@state'>
			background-image: url('img/button/led-<xsl:value-of select='./@state'/>.png');
		</xsl:if>
		<xsl:call-template name='set-bounds'/>
	</xsl:attribute>
	<xsl:value-of select='.'/>
	
	</div>
</xsl:template>

<xsl:template match='group'>
	<div>
	
	<xsl:attribute name='class'>
		group 
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
		<xsl:if test='@framed = "true"'>framed </xsl:if>
	</xsl:attribute>
	
	<xsl:apply-templates select='led'/>
	
	</div>
</xsl:template>

<xsl:template match='text'>
	<div>
	
	<xsl:attribute name='class'>
		text
		<xsl:if test='@framed = "true"'>framed </xsl:if>
		<xsl:if test='@size = "large"'>large </xsl:if>
		<xsl:if test='@size = "title"'>title </xsl:if>
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
	</xsl:attribute>
	<xsl:if test='/dialog/@debug = "true" and @name'>
		<xsl:attribute name='title'>
			<xsl:value-of select='./@name'/>
		</xsl:attribute>
	</xsl:if>
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/> <xsl:value-of select='/dialog/@fore'/>;
		font-family: <xsl:value-of select='./@font'/>;
		<xsl:call-template name='set-bounds'/>
	</xsl:attribute>
	<xsl:for-each select='./child::node()'>
		<xsl:choose>
			<xsl:when test="self::br">
				<br/>
			</xsl:when>
			<xsl:when test="self::text()">
				<xsl:value-of select='translate(.,"&#9;&#10;&#13;"," ")'/>
			</xsl:when>
		</xsl:choose>
	</xsl:for-each>
	
	</div>
</xsl:template>

<xsl:template match='field'>
	<div class='tfield'>
	
	<xsl:attribute name='style'>
		<xsl:call-template name='set-bounds'/>
	</xsl:attribute>
	
	<div class='tfield-inner'>
	
	<xsl:attribute name='style'>
		width: <xsl:value-of select='./@width'/>px; height: <xsl:value-of select='./@height'/>px;
		font-family: 'DejaVu Sans', Geneva;
		font-size: 12px;
	</xsl:attribute>
	(<xsl:value-of select='@name'/>)
	
	</div>
	</div>
</xsl:template>

<xsl:template match='stack'>
	<div>
	
	<xsl:attribute name='class'>
		stack
		<xsl:if test='@framed = "true"'>framed </xsl:if>
	</xsl:attribute>
	
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/> <xsl:value-of select='/dialog/@fore'/>;
		font-family: <xsl:value-of select='./@font'/>;
		<xsl:call-template name='set-bounds'/>
	</xsl:attribute>
	
	<xsl:apply-templates select='*'/>
	
	</div>
</xsl:template>

<xsl:template match='pane'>
	<div>
	
	<xsl:attribute name='class'>
		pane
		<xsl:if test='@framed = "true"'>framed </xsl:if>
	</xsl:attribute>
	<xsl:attribute name='style'>
		overflow-x: hidden;
		overflow-y: scroll;
		color: <xsl:value-of select='./@color'/> <xsl:value-of select='/dialog/@fore'/>;
		font-family: <xsl:value-of select='./@font'/>;
		<xsl:call-template name='set-bounds'/>
	</xsl:attribute>
	
	<xsl:apply-templates select='*'/>
	
	</div>
</xsl:template>

</xsl:stylesheet>