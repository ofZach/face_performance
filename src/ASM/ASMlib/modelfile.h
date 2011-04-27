/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MODELFILE_H
#define MODELFILE_H

#include <cstdio>
#include "cv.h"
using cv::PCA;
using cv::Mat_;

#include <iostream>
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::fstream;

class ModelFile
{
    public:
        void writeInt(int i){ fs<<i<<endl; }
        int readInt(int &i) { fs>>i; return i; }
        
        void writeBool(bool b){ fs<<b<<endl; }
        int readBool(bool &b) { fs>>b; return b; }
        
        void writeReal(double d) { fs<<d<<endl; }
        double readReal(double &d) { fs>>d; return d; }
        
        void writePCA(const PCA *p);
        PCA * readPCA(PCA * &p);
        
        template < class T >
        void writeMat(const Mat_<T> &m){
            writeInt(m.rows);
            writeInt(m.cols);
            for (int i=0;i<m.rows;i++)
                for (int j=0;j<m.cols;j++)
                    fs<<m(i, j)<<endl;
        }
        

        template < class T >
        Mat_<T> & readMat(Mat_<T> &m){
            int r,c;
            readInt(r);
            readInt(c);
            m.create(r, c);
            for (int i=0;i<r;i++)
                for (int j=0;j<c;j++)
                    fs>>m(i,j);
            return m;
        }
        
        void openFile(const char *fName, const char *mode){
            if (mode[0]=='r')
                fs.open(fName, std::ios_base::in);
            else
                fs.open(fName, std::ios_base::out);
            if (!fs){
                printf("Model file %s not found!!\n", fName);
                throw("Model file %s not found!!");
            }
        }
        void closeFile(){ fs.close(); }
        
        //ModelFile(){  }
        ~ModelFile(){ if (fs) fs.close(); }
    private:
        fstream fs;
};

typedef ModelFile ModelFileAscii;

#endif // MODELFILE_H
