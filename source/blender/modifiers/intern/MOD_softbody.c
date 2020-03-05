/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2005 by the Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup modifiers
 */

#include <stdio.h>

#include "BLI_utildefines.h"

#include "DNA_scene_types.h"
#include "DNA_mesh_types.h"
#include "DNA_object_force_types.h"

#include "BKE_layer.h"
#include "BKE_particle.h"
#include "BKE_softbody.h"

#include "DEG_depsgraph.h"
#include "DEG_depsgraph_build.h"
#include "DEG_depsgraph_physics.h"
#include "DEG_depsgraph_query.h"

#include "MOD_modifiertypes.h"

static void deformVerts(ModifierData *UNUSED(md),
                        const ModifierEvalContext *ctx,
                        Mesh *UNUSED(derivedData),
                        float (*vertexCos)[3],
                        int numVerts)
{
  Scene *scene = DEG_get_evaluated_scene(ctx->depsgraph);
  sbObjectStep(
      ctx->depsgraph, scene, ctx->object, DEG_get_ctime(ctx->depsgraph), vertexCos, numVerts);
}

static bool dependsOnTime(ModifierData *UNUSED(md))
{
  return true;
}

static void updateDepsgraph(ModifierData *UNUSED(md), const ModifierUpdateDepsgraphContext *ctx)
{
  if (ctx->object->soft) {
    /* Actual code uses ccd_build_deflector_hash */
    DEG_add_collision_relations(ctx->node,
                                ctx->object,
                                ctx->object->soft->collision_group,
                                eModifierType_Collision,
                                NULL,
                                "Softbody Collision");
    DEG_add_forcefield_relations(
        ctx->node, ctx->object, ctx->object->soft->effector_weights, true, 0, "Softbody Field");
  }
}

ModifierTypeInfo modifierType_Softbody = {
    /* name */ "Softbody",
    /* structName */ "SoftbodyModifierData",
    /* structSize */ sizeof(SoftbodyModifierData),
    /* type */ eModifierTypeType_OnlyDeform,
    /* flags */ eModifierTypeFlag_AcceptsCVs | eModifierTypeFlag_AcceptsLattice |
        eModifierTypeFlag_RequiresOriginalData | eModifierTypeFlag_Single,

    /* copyData */ NULL,

    /* deformVerts */ deformVerts,
    /* deformMatrices */ NULL,
    /* deformVertsEM */ NULL,
    /* deformMatricesEM */ NULL,
    /* applyModifier */ NULL,

    /* initData */ NULL,
    /* requiredDataMask */ NULL,
    /* freeData */ NULL,
    /* isDisabled */ NULL,
    /* updateDepsgraph */ updateDepsgraph,
    /* dependsOnTime */ dependsOnTime,
    /* dependsOnNormals */ NULL,
    /* foreachObjectLink */ NULL,
    /* foreachIDLink */ NULL,
    /* foreachTexLink */ NULL,
    /* freeRuntimeData */ NULL,
};