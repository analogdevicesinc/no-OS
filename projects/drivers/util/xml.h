/***************************************************************************//**
 *   @file   xml.h
 *   @brief  Header file of xml
 *   @author CPop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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

#ifndef ___XML_H__
#define ___XML_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stdio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct xml_attribute {
	char *name;
	char *value;
} xml_attribute;

struct xml_node {
	char *name;
	struct xml_attribute **attributes;
	uint16_t attr_cnt;
	struct xml_node **children;
	uint16_t children_cnt;
} xml_node;

struct xml_document {
	char *buff;
	uint32_t size;
	uint32_t index;
} xml_document;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Create xml attribute. */
ssize_t xml_create_attribute(struct xml_attribute **attribute, char *name,
			     const char *value);

/* Delete xml attribute. */
ssize_t xml_delete_attribute(struct xml_attribute **attribute);

/* Add xml attribute to node. */
ssize_t xml_add_attribute(struct xml_node *node,
			  struct xml_attribute *attribute);

/* Create xml node. */
ssize_t xml_create_node(struct xml_node **node, char *name);

/* Delete xml node. */
ssize_t xml_delete_node(struct xml_node **node);

/* Add xml node to a parent node. */
ssize_t xml_add_node(struct xml_node *node_parent, struct xml_node *node_child);

/* Create xml document. */
ssize_t xml_create_document(struct xml_document **document,
			    struct xml_node *node);

/* Delete xml document. */
ssize_t xml_delete_document(struct xml_document **document);

#endif // ___XML_H__
