/*#########################################################################
# Mass Spec Prediction and Identification of Metabolites
#
# FragmentTreeNode.h
#
# Description: 	Contains Break and FragmentTreeNode classes, for use in
#				the fragment generation process.
#
# Copyright (c) 2013, Felicity Allen
# All rights reserved.

# This file is part of the cfm-id project.
# The contents are covered by the terms of the GNU Lesser General Public
# License, which is included in the file license.txt, found at the root
# of the cfm source tree.
#########################################################################*/

#ifndef __FRAG_TREE_NODE_H__
#define __FRAG_TREE_NODE_H__

#include "Util.h"

//Class for storing information about a particular break
class Break{
public:
	//Constructor for a Non-Ring Break
	Break( int bond_idx );
	//Constructor for a Ring Break
	Break( int bond1_idx, int bond2_idx, int ring_idx );

	//Access functions
	int getBondIdx() const { return bond_idxs[0]; };
	int getSecondBondIdx() const { return bond_idxs[1]; };
	int isRingBreak() const { return ring_break; };
	int getRingIdx() const { return ring_idx; };

private:
	int ring_break;		// Flag indicating ring break
	int bond_idxs[2];   // Indexes of the broken bond(s)
	int ring_idx;		// Index of the ring that is broken
};

//Class for generating fragments via the systematic bond disconnection approach
//with extra checks for hydrogen and bond allocations via MILP.
class FragmentTreeNode{
public:
	//Constructors
	FragmentTreeNode(){};
	FragmentTreeNode( romol_ptr_t an_ion, romol_ptr_t a_nl, int a_ion_free_ep, int a_depth ) :
	    ion(an_ion), nl(a_nl), ion_free_epairs(a_ion_free_ep), depth(a_depth) {};
	FragmentTreeNode( romol_ptr_t an_ion, int a_ion_free_ep, int a_depth ) :
	    ion(an_ion), ion_free_epairs(a_ion_free_ep), depth(a_depth) {};
	
	std::vector<FragmentTreeNode> children;
	romol_ptr_t ion;		//The ion fragment
	romol_ptr_t nl;			//The neutral loss resulting in this ion
	int ion_free_epairs;	//The number of free electron pairs in the ion
	int depth;				//The depth of the tree at which the node was created

	//Generate all possible breaks of the ion
	void generateBreaks(std::vector<Break> &output);
	
	//Record a break in the properties of the ion
	void applyBreak( Break &brk );

	//Undo any changes made during applyBreak
	void undoBreak( Break &brk );

	//For an already applied break, generate the possible child fragments
	//and add them to the children field in the node
	void generateChildrenOfBreak( Break &brk );

	void setTmpTheta( double val, int energy ){ 
		if(tmp_thetas.size() < energy+1 ) tmp_thetas.resize( energy+1 );
		tmp_thetas[energy] = val; 
	};
	double getTmpTheta( int energy ) const { return tmp_thetas[energy]; };
	bool hasTmpThetas() const { return tmp_thetas.size() > 0; };

	const std::vector<double> *getAllTmpThetas() const { return &tmp_thetas; };

private:

	//Temporary storage for the current theta value in each node
	std::vector<double> tmp_thetas;

	//Helper functions:
	//Used to produce FragIdx labels for a broken molecule
	void allocatedCtdToFragment( RDKit::ROMol *romol, RDKit::Atom *atom );

	//Creates all the details and adds the children (with charge on either side) 
	//of the node. -- e_f0 specifies the number of epairs to assigne to F0, 
	//e_to_allocate is the total number to allocate, and output_bmax specifies 
	//where the epairs can go.
	void addBothChildren( int e_f0, int e_to_allocate, std::vector<int> &output_bmax, Break &brk );
	
	//For the given molecule, find a good place to allocate the
	//extra H+ on the specified fragment
	int findChargeLocation( RDKit::RWMol &rwmol, int charge_side );
};

#endif // __FRAG_TREE_NODE_H__