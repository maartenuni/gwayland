/*
 * GWayland library gobject wrappers around waylandclient library.
 * Copyright (C) 2025 Maarten Duijndam
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "gwl-object.h"

typedef struct {
    gpointer *wrapped_object;
} GwlObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlObject, gwl_object, G_TYPE_OBJECT)

typedef enum { FIRST_PROPERTY, PROP_OBJECT, NUM_PROPS } ObjectProperty;

static GParamSpec *properties[NUM_PROPS] = {0};

static void
gwl_object_set_property(GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    GwlObject *self = GWL_OBJECT(object);

    switch ((ObjectProperty) property_id) {
    case PROP_OBJECT:
        gwl_object_set(self, g_value_get_pointer(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_object_get_property(GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    GwlObject *self = GWL_OBJECT(object);
    (void) self;
    (void) value;

    switch ((ObjectProperty) property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

void
gwl_object_finalize(GObject *object)
{
    GwlObject        *self = GWL_OBJECT(object);
    GwlObjectPrivate *priv = gwl_object_get_instance_private(self);

    if (priv->wrapped_object) {
        gwl_object_cleanup(self);
        priv->wrapped_object = 0;
    }

    G_OBJECT_CLASS(gwl_object_parent_class)->finalize(object);
}

static void
gwl_object_init(GwlObject *self)
{
    (void) self;
}

static void
gwl_object_class_init(GwlObjectClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = gwl_object_set_property;
    object_class->get_property = gwl_object_get_property;
    object_class->finalize     = gwl_object_finalize;

    properties[PROP_OBJECT]
        = g_param_spec_pointer("object",
                               "Object",
                               "The wrapped libwayland object",
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, NUM_PROPS, properties);
}

void
gwl_object_set(GwlObject *self, gpointer object)
{
    g_return_if_fail(GWL_IS_OBJECT(self));
    GwlObjectPrivate *priv = gwl_object_get_instance_private(self);

    g_warn_if_fail(priv->wrapped_object == NULL);

    priv->wrapped_object = object;
}

gpointer
gwl_object_get(GwlObject *self)
{
    g_return_val_if_fail(GWL_IS_OBJECT(self), NULL);
    GwlObjectPrivate *priv = gwl_object_get_instance_private(self);

    return priv->wrapped_object;
}

void
gwl_object_cleanup(GwlObject *self)
{
    g_return_if_fail(GWL_IS_OBJECT(self));
    GwlObjectPrivate *priv = gwl_object_get_instance_private(self);

    GwlObjectClass *gwl_obj_class = GWL_OBJECT_GET_CLASS(self);

    g_return_if_fail(gwl_obj_class->get_cleanup_func);
    GDestroyNotify cleanup_func = gwl_obj_class->get_cleanup_func();

    cleanup_func(priv->wrapped_object);
    priv->wrapped_object = NULL;
}
