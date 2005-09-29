/**********************************************************************
base.cpp - Base classes to build a graph
 
Copyright (C) 1998-2001 by OpenEye Scientific Software, Inc.
Some portions Copyright (C) 2001-2005 by Geoffrey R. Hutchison
 
This file is part of the Open Babel project.
For more information, see <http://openbabel.sourceforge.net/>
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/

#include "babelconfig.h"
#include "base.h"

#if HAVE_IOSTREAM
#include <iostream>
#elif HAVE_IOSTREAM_H
#include <iostream.h>
#endif

#if HAVE_FSTREAM
#include <fstream>
#elif HAVE_FSTREAM_H
#include <fstream.h>
#endif

using namespace std;

namespace OpenBabel
{
/*
bool OBGraphBase::Match(OBGraphBase &g,bool singleMatch)
{
    SetFinishedMatch(false);
    SetSingleMatch(singleMatch);
    ClearMatches();
    g.SetVisitLock(true);
    g.ResetVisitFlags();

    OBNodeBase *node;
    OBNodeBase *seed = GetFirstSeed();
    vector<OBNodeBase*>::iterator i;

    for (node = g.Begin(i);node;node = g.Next(i))
        if (!node->Visit && seed->Eval(node))
        {
            node->Visit = true;
            seed->SetMatch(node);
            Match(g,BgnMatch(),BgnMatch()->second.begin());
            seed->SetMatch((OBNodeBase*)NULL);
            node->Visit = false;
            if (SingleMatch() && FinishedMatch())
                break;
        }

    g.SetVisitLock(false);

    return(FinishedMatch());
}

bool OBGraphBase::Match(OBGraphBase &g,
                        vector<pair<OBNodeBase*,vector<OBEdgeBase*> > >::iterator i,
                        vector<OBEdgeBase*>::iterator j)
{
    //bail if only one match has been requested
    if (SingleMatch() && FinishedMatch())
        return(true);

    //full match completed
    if (i == EndMatch() || (j == i->second.end() && (i+1) == EndMatch()))
    {
        SetFinishedMatch(true);
        OBNodeBase *node;
        vector<OBNodeBase*> vn;
        vector<OBNodeBase*>::iterator i;
        for (node = Begin(i);node;node = Next(i))
            vn.push_back(node->GetMatch());
        PushBack(vn);

        return(true);
    }

    //handle next seed of disconnected pattern
    if (j == i->second.end())
    {
        i++;
        OBNodeBase *node;
        OBNodeBase *seed = i->first;
        vector<OBNodeBase*>::iterator k;
        for (node = g.Begin(k);node;node = g.Next(k))
            if (!node->Visit && seed->Eval(node))
            {
                node->Visit = true;
                seed->SetMatch(node);
                Match(g,i,i->second.begin());
                seed->SetMatch((OBNodeBase*)NULL);
                node->Visit = false;
                if (SingleMatch() && FinishedMatch())
                    break;
            }
        return(true);
    }

    OBEdgeBase *edge = *j++;
    if (edge->IsClosure()) //check to see if matched atoms are bonded
    {
        if (edge->GetBgn()->GetMatch()->IsConnected(edge->GetEnd()->GetMatch()))
            Match(g,i,j);
    }
    else //bond hasn't been covered yet
    {
        OBNodeBase *nbr;
        OBNodeBase *curr = edge->GetBgn();
        OBNodeBase *next = edge->GetEnd();
        OBNodeBase *match = curr->GetMatch();
        vector<OBEdgeBase*>::iterator k;

        for (nbr = match->BeginNbr(k);nbr;nbr = match->NextNbr(k))
            if (!nbr->Visit && next->Eval(nbr) && edge->Eval(*k))
            {
                nbr->Visit = true;
                next->SetMatch(nbr);
                Match(g,i,j);
                next->SetMatch(NULL);
                nbr->Visit = false;
            }
    }

    return(false);
}
*/
OBNodeBase *OBGraphBase::Begin(vector<OBNodeBase*>::iterator &i)
{
    i = _vatom.begin();
    return((i != _vatom.end()) ? *i : NULL);
}

OBNodeBase *OBGraphBase::Next(vector<OBNodeBase*>::iterator &i)
{
    i++;
    return((i != _vatom.end()) ? *i : NULL);
}

OBEdgeBase *OBGraphBase::Begin(vector<OBEdgeBase*>::iterator &i)
{
    i = _vbond.begin();
    return((i != _vbond.end()) ? *i : NULL);
}

OBEdgeBase *OBGraphBase::Next(vector<OBEdgeBase*>::iterator &i)
{
    i++;
    return((i != _vbond.end()) ? *i : NULL);
}

OBNodeBase *OBNodeBase::BeginNbr(vector<OBEdgeBase*>::iterator &i)
{
    i = _vbond.begin();

    if (i == _vbond.end())
        return(NULL);
    return((this == (*i)->GetBgn()) ? (*i)->GetEnd() : (*i)->GetBgn());
}

OBNodeBase *OBNodeBase::NextNbr(vector<OBEdgeBase*>::iterator &i)
{
    i++;
    if (i == _vbond.end())
        return(NULL);
    return((this == (*i)->GetBgn()) ? (*i)->GetEnd() : (*i)->GetBgn());
}

void OBNodeBase::SetParent(OBGraphBase *p)
{
    _parent = p;
}

void OBEdgeBase::SetParent(OBGraphBase *p)
{
    _parent = p;
}

bool OBNodeBase::IsConnected(OBNodeBase *nb)
{
    vector<OBEdgeBase*>::iterator i;
    for (i = _vbond.begin();i != _vbond.end();i++)
        if (nb == (*i)->GetBgn() || nb == (*i)->GetEnd())
            return(true);

    return(false);
}

void OBGraphBase::ResetVisitFlags()
{
    OBNodeBase *nb;
    vector<OBNodeBase*>::iterator i;
    for (nb = Begin(i);nb;nb = Next(i))
        nb->Visit = false;

    OBEdgeBase *eb;
    vector<OBEdgeBase*>::iterator j;
    for (eb = Begin(j);eb;eb = Next(j))
        eb->Visit = false;
}

bool OBGraphBase::SetVisitLock(bool v)
{
    if (v && _vlock)
        return(false);
    _vlock = v;
    return(true);
}

/*! \mainpage

  \section intro Introduction and History
 
It is fair to say that Open Babel (and before it, OELib) is a direct
result of the original Babel. Application development is facilitated
by building software on top of libraries rich in functionality. Babel
was the first experience for Matt Stahl in designing a molecule
library. In addition to developing Babel, Pat Walters and Matt
developed `OBabel' at Vertex Pharmaceuticals. OBabel was the first
attempt at developing an object oriented molecule library -- while
successful, Matt later designed a new molecule class library. 
OELib was then designed to be flexible, extensible, portable, and
efficient class library for processing small molecules.
 
OELib was released under the GNU General Public License (GPL) by Matt Stahl
and Open Eye Scientific Software, Inc. to take advantage of many of
the "great minds writing chemical software." Eventually Open Eye
decided to write a "next-generation" class library as proprietary
software. The result was that Open Babel took up where OELib and Babel
left off, using the existing GPL version of OELib, and has continued
to evolve and improve into a separate high-quality chemistry class
library and tool.

Open Babel is now a separate project and library and has changed
considerably from the OELib days.
 
There are several advantages to having the source code to Open Babel
available. First, development time can be shortened by basing projects
on Open Babel. Many chemical and molecular concepts and code are
already implemented. The fewer people who have to reinvent the wheel
(or the function), the better. Second, as free software, hopefully 
other programmers will contribute to the project. The Open Babel development
process is completely open and suggestions, bug reports, and contributions
are always welcomed.
 
Thanks to all who have helped with Babel, OBabel, OELib and Open Babel.
The list is long and growing.
 
  \section pointers Key Modules
 
The heart of Open Babel lies in the \link OpenBabel::OBMol OBMol\endlink, 
\link OpenBabel::OBAtom OBAtom\endlink, and 
\link OpenBabel::OBBond OBBond\endlink classes,
which handle operations on atoms, bonds and molecules. Newcomers should 
start with looking at the \link OpenBabel::OBMol OBMol\endlink class, 
designed to store the basic information
in a molecule and to perceive information about a molecule.

Transformations and automatic perception of properties are performed in a 
"lazy" manner. That is, until you call for partial atomic charges, no 
charges are calculated. This ensures faster transformations of chemical data
-- properties that are not needed for your code will typically not be 
calculated. When such data is needed, appropriate routines are called, and a
"flag" is set (e.g., via OBMol::SetFlag() or OBAtom::SetFlag() etc.) so that
the code is only run once.

Conversion between various chemical file formats is accomplished through
the \link OpenBabel::OBConversion OBConversion\endlink and \link 
OpenBabel::OBFormat OBFormat\endlink classes, often through use of the \link 
OpenBabel::OBMoleculeFormat OBMoleculeFormat\endlink subclass which is designed
for easy read/write access to one or more \link OpenBabel::OBMol OBMol\endlink
objects.

*/

} // namespace OpenBabel

//! \file base.cpp
//! \brief Implementation of base classes.
