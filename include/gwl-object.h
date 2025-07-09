
#include "gwl-import.h"

#include <glib-object.h>

#pragma once

G_BEGIN_DECLS

#define GWL_TYPE_OBJECT gwl_object_get_type()

G_DECLARE_DERIVABLE_TYPE(GwlObject, gwl_object, GWL, OBJECT, GObject)

struct _GwlObjectClass {
    GObjectClass parent_class;

    GDestroyNotify (*get_cleanup_func)(void);

    gpointer padding[16];
};

void
gwl_object_set(GwlObject *self, gpointer obj);

gpointer
gwl_object_get(GwlObject *self);

void
gwl_object_cleanup(GwlObject *self);

G_END_DECLS
