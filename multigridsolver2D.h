/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef MULTIGRIDSOLVER0_H
#define MULTIGRIDSOLVER0_H

#include "buffer.h"

class BorderHandler2D
{
public:
	enum CellType { CellInner,CellOuter,CellDirichlet,CellNeumann };

	virtual void setarg(int arg,cl::Kernel & ker,int dimx,int dimy) = 0;

	virtual CellType cellType(int x,int y,int dimx,int dimy) const = 0;

private:
};

class MultigridSolver2D
{
public:
	MultigridSolver2D(const char * filename,BorderHandler2D & handl);

	Buffer2D iterate(Buffer2D & in,
					const Buffer2D & func,
					 real omega = 2.0/3.0,
					 int a1 = 4,
					 int a2 = 4,
					 int v = 1);

	Buffer2D mg(Buffer2D & in,
					const Buffer2D & func,
					 real omega = 2.0/3.0,
					 int a1 = 4,
					 int a2 = 4,
					 int v = 1,
					 int iters = 3) {
		for (int i=0;i < iters;++i)
			iterate(in,func,omega,a1,a2,v);
		return in;
	}

	Buffer2D fmg(const Buffer2D & func,
			real omega = 2.0/3.0,
			int a1 = 4,
			int a2 = 4,
			int v = 1,
			int iters = 3);

	void wait() { m_queue.finish();}

	cl::CommandQueue & queue() {return m_queue;}

	void smoother_iterate(Buffer2D& res, const Buffer2D& func, real omega, int a1);
	void compute_residuals(Buffer2D& res,const Buffer2D & input, const Buffer2D& func);
	void restrict(Buffer2D& res,const Buffer2D & input);
	void correct_residual(Buffer2D & res,const Buffer2D & input,Buffer2D & residual);
	void prolongate(Buffer2D & res,const Buffer2D & input);
	void zero_mem(Buffer2D & res);
	void zero_out(Buffer2D & res);

	bool m_debugPrintResiduals;

private:
	cl::Program m_theProgram;

	cl::Kernel m_iterationKernel;
	cl::Kernel m_residualKernel;
	cl::Kernel m_reductionKernel;
	cl::Kernel m_residualCorrectKernel;
	cl::Kernel m_prolongationKernel;
	cl::Kernel m_zeroOutKernel;
	cl::Buffer m_emptyBuf;

	cl::CommandQueue m_queue;

	BorderHandler2D & m_Handl;
};

#endif // MULTIGRIDSOLVER0_H
