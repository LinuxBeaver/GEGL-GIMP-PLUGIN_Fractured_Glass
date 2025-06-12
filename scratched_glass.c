/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * 2024, beaver, Scratched Glass

id=0 over aux=[ ref=0
 shadows-highlights 
shadows=-100
highlights=-88.969999
whitepoint=-10
radius=29.940000000000001
compress=0
shadows-ccorrect=29.73
highlights-ccorrect=18.48
median-blur radius=0
opacity value=0.55
]

gaussian-blur std-dev-x=3 std-dev-y=4
id=1 gimp:layer-mode layer-mode=color-erase aux=[ ref=1s 
cubism tile-size=183 tile-saturation=3.6  seed=22
emboss azimuth=42 elevation=24 depth=95
 ]

dst-over aux=[ ref=1 ]

id=2 gimp:layer-mode layer-mode=hardlight opacity=0.1 aux=[ ref=2
difference-of-gaussians radius1=40 radius2=40  ]

denoise-dct sigma=4

hue-chroma lightness=1
saturation scale=0.7

]

end of syntax
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

/*Properties go here*/

property_double (blur_x, _("Horizontal Blur"), 3.0)
    description (_("Horizontally Blur image behind glass"))
    value_range (0.0, 5.0)
    ui_meta ("unit", "degree")

property_double (blur_y, _("Vertical Blur"), 2.0)
    description (_("Veritically Blur image behind glass"))
    value_range (0.0, 5.0)
    ui_meta ("unit", "degree")

property_double (azimuth, _("Azimuth"), 42.0)
    description (_("Light angle (degrees) of glass marking"))
    value_range (0, 180)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")

property_double (elevation, _("Elevation"), 24.0)
    description (_("Light rotation of glass markings"))
    value_range (24, 180)
    ui_meta ("unit", "degree")


property_int (depth, _("Depth"), 95)
    description (_("Depth of glass marking"))
    value_range (5, 100)
    ui_meta ("unit", "degree")

property_double (marking_size, _("Marking size"), 183.0)
    description (_("Markings on glass work through a internal cubism filter, this is the tile size, which is the average diameter of each tile (in pixels)"))
    value_range (50.0, 256.0)
    ui_meta     ("unit", "pixel-distance")

property_double (marking_expansion, _("Marking expansion"), 3.6)
    description (_("Markings on glass work through internal cubism filter, this expands tiles by a given amount"))
    value_range (3.0, 10.0)


property_seed (seed, _("Random seed"), rand)
    description (_("Random seed of the markings"))

property_double (desaturation, _("Desaturation"), 0.7)
    description (_("Desaturate the image"))
    value_range (0.0, 1.0)
    ui_meta ("unit", "degree")

property_double (light, _("Lightness"), 7)
    description (_("Lightness adjustment for the entire image"))
    value_range (-7.0, 15.0)
    ui_meta ("unit", "degree")


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     scratched_glass
#define GEGL_OP_C_SOURCE scratched_glass.c

#include "gegl-op.h"

/*starred nodes go inside typedef struct */

typedef struct
{
 GeglNode *input;
 GeglNode *shadowhighlightstuff;
 GeglNode *hardlightdog;
 GeglNode *lightness;
 GeglNode *saturation;
 GeglNode *idref;
 GeglNode *behind;
 GeglNode *blur;
 GeglNode *emboss;
 GeglNode *cubism;
 GeglNode *colorerase;
 GeglNode *whitebg;
 GeglNode *crop;
 GeglNode *output;
}State;

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);


  State *state = o->user_data = g_malloc0 (sizeof (State));

/*new child node list is here, this is where starred nodes get defined

 state->newchildname = gegl_node_new_child (gegl, "operation", "lb:name", NULL);*/
  state->input    = gegl_node_get_input_proxy (gegl, "input");
  state->output   = gegl_node_get_output_proxy (gegl, "output");

#define EMBEDDEDGRAPH \
" id=0 over aux=[ ref=0 shadows-highlights shadows=-100 highlights=-88.969999 whitepoint=-10 radius=29.940000000000001 compress=0 shadows-ccorrect=29.73 highlights-ccorrect=18.48 median-blur radius=0 opacity value=0.55 ] "\

  state->shadowhighlightstuff    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", EMBEDDEDGRAPH, 
                                  NULL);
#define EMBEDDEDGRAPH2 \
"  id=2 gimp:layer-mode layer-mode=hardlight opacity=0.1 aux=[ ref=2 difference-of-gaussians radius1=40 radius2=40  ] denoise-dct sigma=4 "\

  state->hardlightdog    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", EMBEDDEDGRAPH2, 
                                  NULL);

#define EMBEDDEDGRAPH3 \
" id=x dst-over aux=[ color value=white ] crop aux=[ ref=x ] "\

  state->whitebg    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", EMBEDDEDGRAPH3, 
                                  NULL);
 state->lightness = gegl_node_new_child (gegl, "operation", "gegl:hue-chroma", NULL);

 state->saturation = gegl_node_new_child (gegl, "operation", "gegl:saturation", NULL);

 state->idref = gegl_node_new_child (gegl, "operation", "gegl:nop", NULL);

 state->behind = gegl_node_new_child (gegl, "operation", "gegl:dst-over",  NULL);

 state->blur = gegl_node_new_child (gegl, "operation", "gegl:gaussian-blur",  NULL);

 state->emboss = gegl_node_new_child (gegl, "operation", "gegl:emboss", NULL);

 state->cubism = gegl_node_new_child (gegl, "operation", "gegl:cubism", NULL);

 state->crop = gegl_node_new_child (gegl, "operation", "gegl:crop", NULL);

 state->colorerase = gegl_node_new_child (gegl, "operation", "gimp:layer-mode", "layer-mode", 57, "blend-space", 1, NULL);


 gegl_operation_meta_redirect (operation, "light", state->lightness,  "lightness");
 gegl_operation_meta_redirect (operation, "desaturation", state->saturation,  "scale");
 gegl_operation_meta_redirect (operation, "seed", state->cubism,  "seed");
 gegl_operation_meta_redirect (operation, "marking_size", state->cubism,  "tile_size");
 gegl_operation_meta_redirect (operation, "marking_expansion", state->cubism,  "tile_saturation");
 gegl_operation_meta_redirect (operation, "depth", state->emboss,  "depth");
 gegl_operation_meta_redirect (operation, "elevation", state->emboss,  "elevation");
 gegl_operation_meta_redirect (operation, "azimuth", state->emboss,  "azimuth");
 gegl_operation_meta_redirect (operation, "blur_x", state->blur,  "std-dev-x");
 gegl_operation_meta_redirect (operation, "blur_y", state->blur,  "std-dev-y");
}    


static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  gegl_node_link_many (state->input, state->whitebg, state->shadowhighlightstuff, state->blur, state->idref, state->colorerase, state->behind, state->hardlightdog, state->lightness, state->saturation, state->crop,  state->output,  NULL);
  gegl_node_connect (state->colorerase, "aux", state->emboss, "output"); 
  gegl_node_connect (state->behind, "aux", state->idref, "output"); 
  gegl_node_link_many (state->idref, state->cubism, state->emboss,  NULL);
  gegl_node_connect (state->crop, "aux", state->input, "output"); 
/*optional connect from and too is here
  gegl_node_connect (state->blendmode, "aux", state->lastnodeinlist, "output"); */



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:glass",
    "title",       _("Fractured Glass"),
    "reference-hash", "amberolisgreatsoftwareheartsebassi",
    "description", _("Make a fractured glass effect over your image"),
/*<Image>/Colors <Image>/Filters are top level menus in GIMP*/
    "gimp:menu-path", "<Image>/Filters/Blur",
    "gimp:menu-label", _("Fractured Glass..."),
    NULL);
}

#endif
