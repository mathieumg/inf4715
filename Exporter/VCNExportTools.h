#pragma once

inline Matrix3 TransformMatrixLHY(const Matrix3& orig)
{
  Matrix3 cc;
  cc.SetTrans( Point3(0,0,0) );
  cc.SetRow(0, Point3(1, 0, 0));
  cc.SetRow(1, Point3(0, 0, 1));
  cc.SetRow(2, Point3(0, 1, 0));

  Matrix3 cci = Inverse(cc);
    
  const Matrix3 mat = cc * orig * cci;
  
  return mat;
}