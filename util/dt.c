/***************************************************************************//**
 *   @file   dt.c
 *   @brief  Linux device tree like functionality.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "dt.h"

static int _cmp(const void *va, const void *vb)
{
	const struct dt_property *a = va, *b = vb;
	return strcmp(a->name, b->name);
}

void dt_init(struct dt_properties *ps, struct dt_property *table,
	     unsigned int size, int dflt)
{
	ps->table = table;
	ps->size = size;
	ps->dflt = dflt;
	qsort (table, size, sizeof (struct dt_property), _cmp);
}

int dt_value(struct dt_properties ps, const char *name)
{
	struct dt_property name_dt_property[1] = {{name}};
	struct dt_property *dt_property = bsearch(name_dt_property, ps.table,
					  ps.size, sizeof(struct dt_property), _cmp);
	return dt_property ? dt_property->value : ps.dflt;
}

int dt_array(struct dt_properties ps, const char *name, void *array,
	     size_t item_sz)
{
	struct dt_property name_dt_property[1] = {{name}};
	struct dt_property *dt_property = bsearch(name_dt_property, ps.table,
					  ps.size, sizeof(struct dt_property), _cmp);
	if(dt_property && dt_property->array) {
		memcpy(array, dt_property->array, dt_property->value * item_sz);
		return dt_property->value;
	}
	return ps.dflt;
}
