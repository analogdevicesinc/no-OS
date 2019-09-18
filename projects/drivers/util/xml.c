#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "xml.h"

/**
 * create attribute
 * @param **attribute pointer to new attribute
 * @param *name attribute name
 * @param *value attribute value
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_create_attribute(xml_attribute **attribute, char *name, char *value) {
	*attribute = malloc(sizeof(xml_attribute));
	if (!(*attribute))
		return -ENOMEM;

	(*attribute)->name = malloc(strlen(name) + 1);
	if (!(*attribute)->name)
		return -ENOMEM;
	strcpy((*attribute)->name, name);

	(*attribute)->value = malloc(strlen(value) + 1);

	if (!(*attribute)->value)
		return -ENOMEM;
	strcpy((*attribute)->value, value);

	return 0;
}

/**
 * add attribute to a xml node
 * @param *node pointer to the node, where the attribute is inserted
 * @param *attribute attribute
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_add_attribute(xml_node *node, xml_attribute *attribute) {
	if (!node->attributes) {
		node->attributes = malloc(sizeof(xml_attribute*));
		if (!node->attributes)
			return -ENOMEM;
	}
	else {
		xml_attribute **buff = realloc(node->attributes, (node->attr_cnt + 1) * sizeof(xml_attribute*));
		if (!buff)
			return -ENOMEM;
		node->attributes = buff;
	}
	node->attributes[node->attr_cnt] = attribute;
	node->attr_cnt++;

	return 0;
}

/**
 * create new xml node
 * @param **node pointer to the new node
 * @param *name
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_create_node(xml_node **node, char *name) {
	*node = malloc(sizeof(xml_node));
	if (!(*node))
		return -ENOMEM;
	(*node)->attributes = NULL;
	(*node)->attr_cnt = 0;
	(*node)->children = NULL;
	(*node)->children_cnt = 0;
	(*node)->name = malloc(strlen(name) + 1);
	if (!(*node)->name)
		return -ENOMEM;
	strcpy((*node)->name, name);

	return 0;
}

/**
 * add child node to a parent node
 * @param *node_parent
 * @param *node_child
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_add_node(xml_node *node_parent, xml_node *node_child) {
	if (!node_parent->children) {
		node_parent->children = malloc(sizeof(xml_node*));
		if (!node_parent->children)
			return -ENOMEM;
	}
	else {
		struct xml_node **buff = realloc(node_parent->children, (node_parent->children_cnt + 1) * sizeof(xml_node*));
		if (!buff)
			return -ENOMEM;
		node_parent->children = buff;
	}
	node_parent->children[node_parent->children_cnt] = node_child;
	node_parent->children_cnt++;

	return 0;
}

/**
 * delete attribute
 * @param *attribute
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_delete_attribute(xml_attribute *attribute) {
	free(attribute->name);
	free(attribute->value);

	return 0;
}

/**
 * delete xml node
 * @param *node
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_delete_node(xml_node *node) {
	uint8_t i;

	free(node->name);
	for (i = 0; i < node->attr_cnt; i++) {
		xml_delete_attribute(node->attributes[i]);
	}
	free(node->attributes);
	node->attr_cnt = 0;
	for (i = 0; i < node->children_cnt; i++) {
		xml_delete_node(node->children[i]);
	}
	free(node->children);
	node->children_cnt = 0;

	return 0;
}

/**
 * print xml tree into a xml document
 * @param **document
 * @param *node pointer to parent node, that contains the xml tree
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_create_document(xml_document **document, xml_node *node) {
	uint8_t i;

	const uint16_t buff_increments = 1024;
	if (!(*document)) {
		*document = malloc(sizeof(xml_document));
		if (!(*document))
			return -ENOMEM;
		(*document)->buff = malloc(buff_increments * 2);
		if (!(*document)->buff)
			return -ENOMEM;
		(*document)->size = buff_increments * 2;
		(*document)->index = 0;
	}

	/* increase memory */
	if ((*document)->index > (*document)->size - buff_increments)
	{
		char *buff = realloc((*document)->buff, (*document)->size + buff_increments);
		if (!buff)
			return -ENOMEM;
		(*document)->buff = buff;
		(*document)->size += buff_increments;
	}

	(*document)->index += sprintf(&(*document)->buff[(*document)->index], "<%s ", node->name);
	for (i = 0; i < node->attr_cnt; i++) {
		(*document)->index += sprintf(&(*document)->buff[(*document)->index], "%s=\"%s\" ", node->attributes[i]->name, node->attributes[i]->value);
	}
	if (node->children_cnt == 0)
	{
		(*document)->index += sprintf(&(*document)->buff[(*document)->index], "/>\n");
		return 0;
	}

	(*document)->index += sprintf(&(*document)->buff[(*document)->index], ">\n");
	for (i = 0; i < node->children_cnt; i++) {
		xml_create_document(document, node->children[i]);
	}
	(*document)->index += sprintf(&(*document)->buff[(*document)->index], "</%s>\n", node->name);

	return 0;
}

/**
 * delete xml document
 * @param **document pointer ti document
 * @return 0 in case of success or negative value otherwise
 */
ssize_t xml_delete_document(xml_document **document) {
	free((*document)->buff);
	free(*document);

	return 0;
}
