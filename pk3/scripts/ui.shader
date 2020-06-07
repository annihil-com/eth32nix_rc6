eth32logo
{
	nopicmip
	{
		map gfx/eth32_logo.tga
		blendfunc blend
		alphaGen const 0.5
	}
}

nullshader
{
	nomipmaps
	nopicmip
	{
		map gfx/nullshader.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

eth32logo2
{
	nomipmaps
	nopicmip
	{
		clampmap gfx/eth32_logo.tga
		blendfunc blend
		alphaGen const 0.5
	}
}
