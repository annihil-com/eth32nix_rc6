//eth32nix Shaders 
// Credits: *nixCoders Team 2008

matrixChams
{
	q3map_surfacelight 1800
        deformVertexes wave 300 sin 2.5 0 0 0
    	{
		map gfx/effects/matrix.tga
		blendfunc GL_ONE GL_ONE
				rgbgen entity
				tcGen environment
				tcmod rotate 4
				tcmod scroll 0.8 .1
	}
}

crystalChams
{
	cull none
	deformVertexes wave 3 sin 3.5 0 0 1
	noPicmip
	surfaceparm trans
	{
		map textures/sfx/construction.tga
		blendFunc GL_SRC_ALPHA GL_ONE 
		rgbGen entity
		tcGen environment 
		tcMod scroll 0.025 -0.07625 
	} 

}
solidChams
{
	{
		map *white
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen entity
		tcGen environment
	}
}

hackChams
{
		deformVertexes wave 100 sin 4 0 0 0
		{
			map *white
			rgbGen entity
			blendfunc GL_ONE GL_ONE
		}
	}

fusionChams
{
	q3map_surfacelight 1800
        deformVertexes wave 66 sin 1 0 0 0
	{
		map gfx/damage/burn_med_mrk.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		
				rgbgen entity
				tcGen environment
				tcmod rotate 15
				tcmod scroll 1 .1
	}
}

waveChams
{
	cull none
	deformVertexes wave 45 sin 2.0 10 0 -.6
	nomipmap
	nopicmip
		{
			map textures/liquids_sd/seawall_foam.tga
			blendFunc GL_SRC_ALPHA GL_ONE
		alphaGen normalzfade 1.0 -0.7 0.5
				rgbgen entity
				tcGen environment
				tcmod rotate 15
				tcmod scroll 1 .1
		}
	}

quadChams
{
	q3map_surfacelight 1800
        deformVertexes wave 150 sin 2.4 0 0 0
	{
		map gfx/effects/quad.tga
		blendfunc GL_ONE GL_ONE
				rgbgen entity
				tcGen environment
				tcmod rotate 4
				tcmod scroll 0.5 .1

	}
}


plasmaChams
{
      q3map_surfacelight 1800
        deformVertexes wave 100 sin 1 0 0 0
        {
		map gfx/effects/plasma.tga
		blendfunc GL_ONE GL_ONE
				rgbgen entity
				tcGen environment
				tcmod rotate 15
				tcmod scroll 1 .1
	}
}
thermalChams
{
 q3map_surfacelight 1800
        deformVertexes wave 100 sin 1 0 0 0
	{
		map gfx/effects/thermal.tga
		blendfunc GL_ONE GL_ONE
				rgbgen entity
				tcGen environment
				tcmod rotate 15
				tcmod scroll 1 .1
	}
}

statbar
{
	nomipmaps
	nopicmip
	{
		map gui/common/statbar.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

statbar2
{
	nomipmaps
	nopicmip
	{
		map gui/common/statbar2.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

wa_play
{
	nomipmaps
	nopicmip
	{
		map gui/common/wa_play.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

wa_stop
{
	nomipmaps
	nopicmip
	{
		map gui/common/wa_stop.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

wa_pause
{
	nomipmaps
	nopicmip
	{
		map gui/common/wa_pause.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

wa_next
{
	nomipmaps
	nopicmip
	{
		map gui/common/wa_next.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

wa_prev
{
	nomipmaps
	nopicmip
	{
		map gui/common/wa_prev.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

portal_marker
{
	nomipmaps
	nopicmip
	{
		map gui/common/portal_marker.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_left
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_left.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_center
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_cen.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_right
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_right.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_left
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_left.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_center
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_cen.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_right
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_right.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}