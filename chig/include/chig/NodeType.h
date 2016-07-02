#ifndef CHIG_NODE_H
#define CHIG_NODE_H

#pragma once

enum ChigNodeType {
	CHIG_NODE_TYPE_BULIT_IN,
	CHIG_NODE_TYPE_IMPORTED
};

typedef struct {
	
	
} ChigNodeTypeBuiltIn;

typedef struct {
	
} ChigNodeTypeImported;

typedef struct {
	
	ChigNodeType type;
	
	union {
		ChigNodeTypeBuiltIn builtInNode;
		ChigNodeTypeImported importedNode;
	}
	
} ChigNodeType;


#endif // CHIG_NODE_H
