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

using namespace std;
namespace OpenBabel
{

class MAEFormat : public OBMoleculeFormat
// Derive directly from OBFormat for objects which are not molecules.
{
public:
	//Register this format type ID in the constructor
  MAEFormat()
	{
		/* MAE is the file extension and is case insensitive. A MIME type can be
		   added as an optional third parameter.
		   Multiple file extensions can be registered by adding extra statements.*/
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

    /* This optional function is for formats which can contain more than one
       molecule. It is used to quickly position the input stream after the nth
       molecule without have to convert and discard all the n molecules.
       See obconversion.cpp for details and mdlformat.cpp for an example.*/
    virtual int SkipObjects(int n, OBConversion* pConv)
    {
        //TODO:  Make this work with maeparser
        return 0;
    };

    ////////////////////////////////////////////////////
    /// Declarations for the "API" interface functions. Definitions are below
    virtual bool ReadMolecule(OBBase* pOb, OBConversion* pConv);
    virtual bool WriteMolecule(OBBase* pOb, OBConversion* pConv);

private:
    /* Add declarations for any local function or member variables used.
       Generally only a single instance of a format class is used. Keep this in
       mind if you employ member variables. */
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

  istream& ifs = *pConv->GetInStream();

  pmol->BeginModify();

  pmol->EndModify();

	/* For multi-molecule formats, leave the input stream at the start of the
	   next molecule, ready for this routine to be called again.  */

	/* Return true if ok. Returning false means discard the OBMol and stop
	   converting, unless the -e option is set. With a multi-molecule inputstream
	   this will skip the current molecule and continue with the next, if SkipObjects()
	   has been defined. If it has not, and continuation after errors is still required,
	   it is necessary to leave the input stream at the beginning of next object when
	   returning false;*/
  return true;
}

////////////////////////////////////////////////////////////////

bool MAEFormat::WriteMolecule(OBBase* pOb, OBConversion* pConv)
{
  OBMol* pmol = dynamic_cast<OBMol*>(pOb);
  if(pmol==NULL)
      return false;

  ostream& ofs = *pConv->GetOutStream();

	/** Write the representation of the OBMol molecule to the output stream **/

	// To find out whether this is the first molecule to be output...
	if(pConv->GetOutputIndex()==1)
		ofs << "The contents of this file were derived from " << pConv->GetInFilename() << endl;

	return true; //or false to stop converting
}

} //namespace OpenBabel

