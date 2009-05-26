<?xml version="1.0" encoding="UTF-8"?>
<!-- <?xml-stylesheet href="http://www.w3.org/StyleSheets/TR/W3C-REC.css" type="text/css"?>
<?xml-stylesheet href="" type="text/css"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"> -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">


<xsl:template match='/'>

<html> <!-- xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" -->
<head>
<title>Dialog Preview</title>
<link rel='stylesheet' type='text/css' href='dialog.css'/>
</head>

<body>
<xsl:attribute name='style'>
background-image: url('bg/<xsl:value-of select='/dialog/@skin'/>.png');
</xsl:attribute>
<div class='dialog'>
<xsl:for-each select='dialog/pict'>
	<xsl:choose>
		<xsl:when test='@type = "full"'>
			<div>
			<xsl:attribute name='class'>pict <xsl:value-of select='./@type'/></xsl:attribute>
			<xsl:attribute name='style'>
			background-image: url('other/<xsl:value-of select='./@num'/>.png');
			left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
			width: <xsl:value-of select='./@width'/>px; height: <xsl:value-of select='./@height'/>px;
			</xsl:attribute>
			</div>
		</xsl:when>
		<xsl:when test='@type = "blank"'>
			<div>
			<xsl:attribute name='class'>pict <xsl:value-of select='./@type'/></xsl:attribute>
			<xsl:attribute name='style'>
			background-color: black;
			left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
			</xsl:attribute>
			</div>
		</xsl:when>
		<xsl:when test='@size = "large" and @type != "monst"'>
			<div>
			<xsl:attribute name='class'>pict large <xsl:value-of select='./@type'/></xsl:attribute>
			<xsl:attribute name='style'>
			background-image: url('<xsl:value-of select='./@type'/>/large<xsl:value-of select='./@num'/>.png');
			left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
			</xsl:attribute>
			</div>
		</xsl:when>
		<xsl:otherwise>
			<div>
			<xsl:attribute name='class'>pict <xsl:value-of select='./@type'/></xsl:attribute>
			<xsl:attribute name='style'>
			background-image: url('<xsl:value-of select='./@type'/>/<xsl:value-of select='./@num'/>.png');
			left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
			</xsl:attribute>
			</div>
		</xsl:otherwise>
	</xsl:choose>
</xsl:for-each>

<xsl:for-each select='dialog/button'>
	<div>
	<xsl:attribute name='class'>
		button <xsl:value-of select='./@type'/>
	</xsl:attribute>
	<xsl:attribute name='style'>
		background-image: url('button/<xsl:value-of select='./@type'/>.png');
		left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
		<xsl:if test='@type = "push"'>width: <xsl:value-of select='./@width'/>;</xsl:if>
	</xsl:attribute>
	<xsl:value-of select='.'/>
	</div>
</xsl:for-each>

<xsl:for-each select='dialog/led'>
	<div>
	<xsl:attribute name='class'>
		led 
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
	</xsl:attribute>
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/>;
		font-family:
		<xsl:choose>
			<xsl:when test='@font = "plain"'>Geneva</xsl:when>
			<xsl:when test='@font = "bold"'>Silom</xsl:when>
			<xsl:otherwise><xsl:value-of select='./@font'/></xsl:otherwise>
		</xsl:choose>;
		background-image: url('button/led-<xsl:value-of select='./@state'/>.png');
		left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
		width: <xsl:value-of select='./@width'/>px;
	</xsl:attribute>
	<xsl:value-of select='.'/>
	</div>
</xsl:for-each>

<xsl:for-each select='dialog/group/led'>
	<div>
	<xsl:attribute name='class'>
		led 
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
	</xsl:attribute>
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/>;
		font-family:
		<xsl:choose>
			<xsl:when test='@font = "plain"'>Geneva</xsl:when>
			<xsl:when test='@font = "bold"'>Silom</xsl:when>
			<xsl:otherwise><xsl:value-of select='./@font'/></xsl:otherwise>
		</xsl:choose>;
		border-color: red;
		background-image: url('button/led-<xsl:value-of select='./@state'/>.png');
		left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
		width: <xsl:value-of select='./@width'/>px;
	</xsl:attribute>
	<xsl:value-of select='.'/>
	</div>
</xsl:for-each>

<xsl:for-each select='dialog/text'>
	<div>
	<xsl:attribute name='class'>
		text
		<xsl:if test='@framed = "true"'>framed </xsl:if>
		<xsl:if test='@size = "large"'>large </xsl:if>
		<xsl:if test='/dialog/@debug = "true"'>debug</xsl:if>
	</xsl:attribute>
	<xsl:attribute name='style'>
		color: <xsl:value-of select='./@color'/>;
		font-family: <xsl:value-of select='./@font'/>;
		left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
		width: <xsl:value-of select='./@width'/>px; height: <xsl:value-of select='./@height'/>px;
	</xsl:attribute>
	<xsl:value-of select='.'/>
	</div>
</xsl:for-each>

<xsl:for-each select='dialog/field'>
	<div class='field'>
	<xsl:attribute name='style'>
		left: <xsl:value-of select='./@left'/>px; top: <xsl:value-of select='./@top'/>px;
		width: <xsl:value-of select='./@width'/>px; height: <xsl:value-of select='./@height'/>px;
	</xsl:attribute>
	<div class='field-inner'>
	<xsl:attribute name='style'>
		width: <xsl:value-of select='./@width'/>px; height: <xsl:value-of select='./@height'/>px;
		font-family: Geneva;
		font-size: 12px;
	</xsl:attribute>
	(<xsl:value-of select='@name'/>)
	</div>
	</div>
</xsl:for-each>
</div>
</body>
</html>

</xsl:template>

</xsl:stylesheet>