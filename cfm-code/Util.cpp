/*#########################################################################
# Mass Spec Prediction and Identification of Metabolites
#
# util.h
#
# Description: 	Useful functions and definitions
#
# Copyright (c) 2013, Felicity Allen
# All rights reserved.

# This file is part of the cfm-id project.
# The contents are covered by the terms of the GNU Lesser General Public
# License, which is included in the file license.txt, found at the root
# of the cfm source tree.
#########################################################################*/


#include "Util.h"
#include <GraphMol/PeriodicTable.h>
#include <GraphMol/AtomIterators.h>

double getMassTol( double abs_tol, double ppm_tol, double mass ){
	double mass_tol = (mass/1000000.0) * ppm_tol;
	if( mass_tol < abs_tol ) mass_tol = abs_tol;
	return mass_tol;
}

double getMonoIsotopicMass( romol_ptr_t mol, bool addHPlus){

	RDKit::PeriodicTable *pt = RDKit::PeriodicTable::getTable();
	double mass = 0.0;
	int natoms = mol->getNumAtoms();
	for( int i = 0; i < natoms; i++ ){
		RDKit::Atom *atom = mol->getAtomWithIdx(i);
		std::string symbol = atom->getSymbol();
		mass += pt->getMostCommonIsotopeMass(symbol);
		mass += atom->getTotalNumHs()*pt->getMostCommonIsotopeMass("H");
	}
	if(addHPlus) mass += pt->getMostCommonIsotopeMass("H");
	return mass;
}

//Helper function to find an atom with the given label
RDKit::Atom *getLabeledAtom( romol_ptr_t mol, const char *label ){
	RDKit::ROMol::AtomIterator ai;
	int root = 0;
	for( ai = mol.get()->beginAtoms(); ai != mol.get()->endAtoms(); ++ai ){	
		(*ai)->getProp(label, root);
		if( root ) break;
	}	
	if( root ) return *ai;
	else return NULL;
}
