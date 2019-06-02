/**********************************************************************
Copyright (C) 2019 by Pat Lorton

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/

/* Populate an OBMol from a Schrödinger Maestro file using MaeParser.
   More information can be found at: https://github.com/schrodinger/maeparser
*/

#include <openbabel/babelconfig.h>
#include <openbabel/obmolecformat.h>

#include <openbabel/obmolecformat.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/elements.h>
#include <openbabel/generic.h>

#include <iostream>

#include <MaeConstants.hpp>
#include <Reader.hpp>
#include <Writer.hpp>

using namespace std;
using namespace schrodinger::mae;
namespace OpenBabel
{

class MAEFormat : public OBMoleculeFormat
{
public:
  //Register this format type ID in the constructor
  MAEFormat()
	{
		OBConversion::RegisterFormat("MAE", this);
	}

	virtual const char* Description() //required
	{
		return
		"Maestro format\n"
		"File format of Schrödinger Software\n";
    };

    //URL where the file format is specified
    virtual const char* SpecificationURL()
    {
        return "https://github.com/schrodinger/maeparser";
    };

    virtual int SkipObjects(int n, OBConversion* pConv)
    {
        shared_ptr<istream> ifs(shared_ptr<istream>(), pConv->GetInStream());
        Reader r(ifs);
        r.next(CT_BLOCK);
        return 0;
    };

    ////////////////////////////////////////////////////
    /// Declarations for the "API" interface functions. Definitions are below
    virtual bool ReadMolecule(OBBase* pOb, OBConversion* pConv);
    virtual bool WriteMolecule(OBBase* pOb, OBConversion* pConv);

private:
    shared_ptr<Writer> m_writer;

};
	////////////////////////////////////////////////////

//Make an instance of the format class
MAEFormat theMAEFormat;

/////////////////////////////////////////////////////////////////

bool MAEFormat::ReadMolecule(OBBase* pOb, OBConversion* pConv)
{
  OBMol* pmol = pOb->CastAndClear<OBMol>();
  if(pmol==NULL)
      return false;

  // Required for the MaeParser interface, create a shared_ptr w/o management
  shared_ptr<istream> ifs(shared_ptr<istream>(), pConv->GetInStream());
  Reader r(ifs);

  auto mae_block = r.next(CT_BLOCK);

  pmol->BeginModify();
  pmol->SetDimension(3);
  pmol->SetTitle(mae_block->getStringProperty("s_m_title").c_str());

  const auto atom_data = mae_block->getIndexedBlock(ATOM_BLOCK);
  // All atoms are gauranteed to have these three field names:
  const auto atomic_numbers = atom_data->getIntProperty(ATOM_ATOMIC_NUM);
  const auto xs = atom_data->getRealProperty(ATOM_X_COORD);
  const auto ys = atom_data->getRealProperty(ATOM_Y_COORD);
  const auto zs = atom_data->getRealProperty(ATOM_Z_COORD);
  const auto natoms = atomic_numbers->size();

  pmol->ReserveAtoms(natoms);
  // atomic numbers, and x, y, and z coordinates
  for (size_t i = 0; i < natoms; ++i) {
      OBAtom* patom = pmol->NewAtom();
      patom->SetVector(xs->at(i), ys->at(i), zs->at(i));
      patom->SetAtomicNum(atomic_numbers->at(i));
  }

  const auto bond_data = mae_block->getIndexedBlock(BOND_BLOCK);
  // All bonds are gauranteed to have these three field names:
  auto bond_atom_1s = bond_data->getIntProperty(BOND_ATOM_1);
  auto bond_atom_2s = bond_data->getIntProperty(BOND_ATOM_2);
  auto orders = bond_data->getIntProperty(BOND_ORDER);
  const auto bond_count = bond_atom_1s->size();

  for (size_t i = 0; i < bond_count; ++i) {
      // Atom indices in the bond data structure are 1 indexed
      const auto bond_atom_1 = bond_atom_1s->at(i);
      const auto bond_atom_2 = bond_atom_2s->at(i);
      if(bond_atom_1 > bond_atom_2) continue; // Bonds are duplicated in MAE format
      const auto order = orders->at(i);
      const unsigned int flag = 0; // Need to do work here around stereo/kekule
      if (!pmol->AddBond(bond_atom_1, bond_atom_2, order, flag)) {
          return false;
      }
  }

  pmol->EndModify();
  return true;
}

////////////////////////////////////////////////////////////////

bool MAEFormat::WriteMolecule(OBBase* pOb, OBConversion* pConv)
{
  OBMol* pmol = dynamic_cast<OBMol*>(pOb);
  if(pmol==NULL)
      return false;

  // The Writer automatically writes the format block at instantiation, so
  // must use a single writer for all writing
  if(pConv->GetOutputIndex()==1) {
      shared_ptr<ostream> ofs(shared_ptr<ostream>(), pConv->GetOutStream());
      m_writer = std::make_shared<Writer>(ofs);
  }

  /** Write the representation of the OBMol molecule to the output stream **/


  return true; //or false to stop converting
}

} //namespace OpenBabel

