#ifndef ___XML_H__
#define ___XML_H__

#include "stdio.h"

typedef struct xml_attribute {
	char *name;
	char *value;
}xml_attribute;

typedef struct xml_node {
	char *name;
	xml_attribute **attributes;
	uint16_t attr_cnt;
	struct xml_node **children;
	uint16_t children_cnt;
}xml_node;

typedef struct xml_document {
	char *buff;
	uint32_t size;
	uint32_t index;
}xml_document;

ssize_t xml_create_attribute(xml_attribute **attribute, char *name, char *value);
ssize_t xml_delete_attribute(xml_attribute **attribute);
ssize_t xml_add_attribute(xml_node *node, xml_attribute *attribute);
ssize_t xml_create_node(xml_node **node, char *name);
ssize_t xml_delete_node(xml_node **node);
ssize_t xml_add_node(xml_node *node_parent, xml_node *node_child);
ssize_t xml_create_document(xml_document **document, xml_node *node);
ssize_t xml_delete_document(xml_document **document);

#endif // ___XML_H__
