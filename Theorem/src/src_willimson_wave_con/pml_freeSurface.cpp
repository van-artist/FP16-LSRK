/*================================================================
*   ESS, Southern University of Science and Technology
*   
*   File Name:pml_freeSurface.cpp
*   Author: Wenqiang Wang, 11849528@mail.sustech.edu.cn
*   Created Time:2021-11-17
*   Discription:
*
================================================================*/

#include "header.h"
__GLOBAL__ 
void pml_free_surface_x(		 									
	WAVE h_W, WAVE W, AUXILIARY h_Aux_x, AUXILIARY Aux_x,			
	FLOAT * ZT_X, FLOAT * ZT_Y, FLOAT * ZT_Z, MEDIUM_FLOAT medium, 		
	Mat3x3 _rDZ_DX, Mat3x3 _rDZ_DY,									
	float *  pml_d_x, int nPML, int _nx_, int _ny_, int _nz_, int FLAG, float rDH, int FB1, float DT )
{																	
#ifdef GPU_CUDA
	int i0 = threadIdx.x + blockIdx.x * blockDim.x;
	int j0 = threadIdx.y + blockIdx.y * blockDim.y;
#else
	int i0 = 0;
	int j0 = 0;
#endif

	int k0;										
	int i, j, k;								
	long long index;
	long long pos;

	int nx = _nx_ - HALO - HALO;
	int ny = _ny_ - HALO - HALO;
	int nz = _nz_ - HALO - HALO;


	int indexOnSurf; 												
	float mu = 0.0f;												
	float lambda = 0.0f;											
																	
	float d_x = 0.0f;												
																	
	float Vx_xi  = 0.0f; float Vy_xi  = 0.0f; float Vz_xi  = 0.0f;	
	float Vx_zt  = 0.0f; float Vy_zt  = 0.0f; float Vz_zt  = 0.0f;	
	float zt_x  = 0.0f;  float zt_y  = 0.0f;  float zt_z  = 0.0f;	

	float Txx3 = 0.0;
	float Tyy3 = 0.0;
	float Tzz3 = 0.0;
	float Txy3 = 0.0;
	float Txz3 = 0.0;
	float Tyz3 = 0.0;


	float h_Aux_xTxx;																									
	float h_Aux_xTyy;																									
	float h_Aux_xTzz;																									
	float h_Aux_xTxy;																									
	float h_Aux_xTxz;																									
	float h_Aux_xTyz;																									
																	
	int stride = FLAG * ( nx - nPML );					
	k0 = nz - 1;										
	CALCULATE2D( i0, j0, 0, nPML, 0, ny )		
		i = i0 + HALO + stride;								
		j = j0 + HALO;										
		k = k0 + HALO;										
		index = INDEX(i, j, k);								
		indexOnSurf = INDEX( i, j, 0 ); 					
		//pos	= i0 + j0 * nPML + k0 * nPML * ny;
		pos	= Index3D( i0, j0, k0, nPML, ny, nz );//i0 + j0 * nPML + k0 * nPML * ny;			
																	
		mu = (float)medium.mu[index]; 										
		lambda = (float)medium.lambda[index];								
																	
		d_x = pml_d_x[i];											
		

		zt_x = (float)ZT_X[index]; 	zt_y = (float)ZT_Y[index]; 	zt_z = (float)ZT_Z[index];															
		
		 Vx_xi = L( ( float )W.Vx, FB1, xi ) * d_x;																							
		 Vy_xi = L( ( float )W.Vy, FB1, xi ) * d_x;																							
		 Vz_xi = L( ( float )W.Vz, FB1, xi ) * d_x;																							
																																	
		Vx_zt = DOT_PRODUCT3D( _rDZ_DX.M11[indexOnSurf], _rDZ_DX.M12[indexOnSurf], _rDZ_DX.M13[indexOnSurf], Vx_xi, Vy_xi, Vz_xi );	
		Vy_zt = DOT_PRODUCT3D( _rDZ_DX.M21[indexOnSurf], _rDZ_DX.M22[indexOnSurf], _rDZ_DX.M23[indexOnSurf], Vx_xi, Vy_xi, Vz_xi );	
		Vz_zt = DOT_PRODUCT3D( _rDZ_DX.M31[indexOnSurf], _rDZ_DX.M32[indexOnSurf], _rDZ_DX.M33[indexOnSurf], Vx_xi, Vy_xi, Vz_xi );	
		

		Txx3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_x * Vx_zt );
		Tyy3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_y * Vy_zt );
		Tzz3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_z * Vz_zt );

		Txy3 = DOT_PRODUCT2D( zt_y, zt_x, Vx_zt, Vy_zt ) * mu;
		Txz3 = DOT_PRODUCT2D( zt_z, zt_x, Vx_zt, Vz_zt ) * mu;
		Tyz3 = DOT_PRODUCT2D( zt_z, zt_y, Vy_zt, Vz_zt ) * mu;
																																	
		h_Aux_xTxx = h_Aux_x.Txx[pos];																									
		h_Aux_xTyy = h_Aux_x.Tyy[pos];																									
		h_Aux_xTzz = h_Aux_x.Tzz[pos];																									
		h_Aux_xTxy = h_Aux_x.Txy[pos];																									
		h_Aux_xTxz = h_Aux_x.Txz[pos];																									
		h_Aux_xTyz = h_Aux_x.Tyz[pos];																									

		h_Aux_xTxx += Txx3 * DT;																									
		h_Aux_xTyy += Tyy3 * DT;																									
		h_Aux_xTzz += Tzz3 * DT;																									
		h_Aux_xTxy += Txy3 * DT;																									
		h_Aux_xTxz += Txz3 * DT;																									
		h_Aux_xTyz += Tyz3 * DT;																									

		h_Aux_x.Txx[pos] = h_Aux_xTxx;																									
		h_Aux_x.Tyy[pos] = h_Aux_xTyy;																									
		h_Aux_x.Tzz[pos] = h_Aux_xTzz;																									
		h_Aux_x.Txy[pos] = h_Aux_xTxy;																									
		h_Aux_x.Txz[pos] = h_Aux_xTxz;																									
		h_Aux_x.Tyz[pos] = h_Aux_xTyz;																									
									
	END_CALCULATE2D( )												
}


__GLOBAL__	
void pml_free_surface_y( 											
	WAVE h_W, WAVE W, AUXILIARY h_Aux_y, AUXILIARY Aux_y,			
	FLOAT * ZT_X, FLOAT * ZT_Y, FLOAT * ZT_Z, MEDIUM_FLOAT medium, 		
	Mat3x3 _rDZ_DX, Mat3x3 _rDZ_DY,									
	float *  pml_d_y, int nPML, int _nx_, int _ny_, int _nz_, int FLAG, float rDH, int FB2, float DT )
{																	
#ifdef GPU_CUDA
	int i0 = threadIdx.x + blockIdx.x * blockDim.x;
	int j0 = threadIdx.y + blockIdx.y * blockDim.y;
#else
	int i0 = 0;
	int j0 = 0;
#endif

	int k0;											
	int i, j, k;									
	long long index;
	long long pos;

	int nx = _nx_ - HALO - HALO;
	int ny = _ny_ - HALO - HALO;
	int nz = _nz_ - HALO - HALO;



	int indexOnSurf; 								
	float mu = 0.0f;												
	float lambda = 0.0f;											
																	
	float d_y = 0.0f;												
																	
	float Vx_et  = 0.0f; float Vy_et  = 0.0f; float Vz_et  = 0.0f; 	
	float Vx_zt  = 0.0f; float Vy_zt  = 0.0f; float Vz_zt  = 0.0f;	
	float zt_x  = 0.0f;  float zt_y  = 0.0f;  float zt_z  = 0.0f;	
		
	float Txx3 = 0.0f;
	float Tyy3 = 0.0f;
	float Tzz3 = 0.0f;
	float Txy3 = 0.0f;
	float Txz3 = 0.0f;
	float Tyz3 = 0.0f; 




	float h_Aux_yTxx;																									
	float h_Aux_yTyy;																									
	float h_Aux_yTzz;																									
	float h_Aux_yTxy;																									
	float h_Aux_yTxz;																									
	float h_Aux_yTyz;																									




	int stride = FLAG * ( ny - nPML );					
	k0 = nz - 1;											
	CALCULATE2D( i0, j0, 0, nx, 0, nPML )	
		i = i0 + HALO;									
		j = j0 + HALO + stride;							
		k = k0 + HALO;									
		index = INDEX(i, j, k);							
		indexOnSurf = INDEX( i, j, 0 ); 				
		//pos	= i0 + j0 * nx + k0 * nx * nPML;
		pos	= Index3D( i0, j0, k0, nx, nPML, nz );//i0 + j0 * nx + k0 * nx * nPML;
																	
		mu = (float)medium.mu[index]; 										
		lambda = (float)medium.lambda[index];								
																	
		d_y = pml_d_y[j];											
		
		zt_x = (float)ZT_X[index]; 	zt_y = (float)ZT_Y[index]; 	zt_z = (float)ZT_Z[index];															
																																	
		 Vx_et = L( ( float )W.Vx, FB2, et ) * d_y;																							
		 Vy_et = L( ( float )W.Vy, FB2, et ) * d_y;																							
		 Vz_et = L( ( float )W.Vz, FB2, et ) * d_y;																							
																																	
		Vx_zt = DOT_PRODUCT3D( _rDZ_DY.M11[indexOnSurf], _rDZ_DY.M12[indexOnSurf], _rDZ_DY.M13[indexOnSurf], Vx_et, Vy_et, Vz_et );	
		Vy_zt = DOT_PRODUCT3D( _rDZ_DY.M21[indexOnSurf], _rDZ_DY.M22[indexOnSurf], _rDZ_DY.M23[indexOnSurf], Vx_et, Vy_et, Vz_et );	
		Vz_zt = DOT_PRODUCT3D( _rDZ_DY.M31[indexOnSurf], _rDZ_DY.M32[indexOnSurf], _rDZ_DY.M33[indexOnSurf], Vx_et, Vy_et, Vz_et );	
		

		Txx3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_x * Vx_zt );
		Tyy3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_y * Vy_zt );
		Tzz3 = DOT_PRODUCT3D( zt_x, zt_y, zt_z, Vx_zt, Vy_zt, Vz_zt ) * lambda + 2.0f * mu * ( zt_z * Vz_zt );

		Txy3 = DOT_PRODUCT2D( zt_y, zt_x, Vx_zt, Vy_zt ) * mu;
		Txz3 = DOT_PRODUCT2D( zt_z, zt_x, Vx_zt, Vz_zt ) * mu;
		Tyz3 = DOT_PRODUCT2D( zt_z, zt_y, Vy_zt, Vz_zt ) * mu;


		h_Aux_yTxx = h_Aux_y.Txx[pos];																									
		h_Aux_yTyy = h_Aux_y.Tyy[pos];																									
		h_Aux_yTzz = h_Aux_y.Tzz[pos];																									
		h_Aux_yTxy = h_Aux_y.Txy[pos];																									
		h_Aux_yTxz = h_Aux_y.Txz[pos];																									
		h_Aux_yTyz = h_Aux_y.Tyz[pos];																									

		h_Aux_yTxx += Txx3 * DT;																									
		h_Aux_yTyy += Tyy3 * DT;																									
		h_Aux_yTzz += Tzz3 * DT;																									
		h_Aux_yTxy += Txy3 * DT;																									
		h_Aux_yTxz += Txz3 * DT;																									
		h_Aux_yTyz += Tyz3 * DT;																									

		h_Aux_y.Txx[pos] = h_Aux_yTxx;																									
		h_Aux_y.Tyy[pos] = h_Aux_yTyy;																									
		h_Aux_y.Tzz[pos] = h_Aux_yTzz;																									
		h_Aux_y.Txy[pos] = h_Aux_yTxy;																									
		h_Aux_y.Txz[pos] = h_Aux_yTxz;																									
		h_Aux_y.Tyz[pos] = h_Aux_yTyz;																									


	END_CALCULATE2D( )												
}

void pmlFreeSurfaceDeriv( GRID grid, WAVE h_W, WAVE W, 
						  CONTRAVARIANT_FLOAT con, MEDIUM_FLOAT medium, 
						  AUX4 Aux4_1, AUX4 Aux4_2, 
						  Mat3x3 _rDZ_DX, Mat3x3 _rDZ_DY,	
						  PML_D pml_d, MPI_BORDER border, int FB1, int FB2, float DT )
{
	int _nx_ = grid._nx_;
	int _ny_ = grid._ny_;
	int _nz_ = grid._nz_;

	int nPML = grid.nPML;

	float rDH = grid.rDH;

	FLOAT * ZT_X = con.zt_x; FLOAT * ZT_Y = con.zt_y; FLOAT * ZT_Z = con.zt_z;

	float * pml_d_x = pml_d.x;
	float * pml_d_y = pml_d.y;


#ifdef GPU_CUDA
	int nx = _nx_ - 2 * HALO;
	int ny = _ny_ - 2 * HALO;
	int nz = _nz_ - 2 * HALO;


	dim3 thread( 8, 8, 1);
	dim3 blockX;
	blockX.x = ( nPML + thread.x - 1 ) / thread.x;
	blockX.y = ( ny   + thread.y - 1 ) / thread.y;
	blockX.z = 1;

	dim3 blockY;
	blockY.x = ( nx   + thread.x - 1 ) / thread.x;
	blockY.y = ( nPML + thread.y - 1 ) / thread.y;
	blockY.z = 1;


	if ( border.isx1 ) pml_free_surface_x<<< blockX, thread >>>	
					  ( h_W, W, 
						Aux4_1.h_Aux_x, Aux4_1.Aux_x,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,	
						pml_d_x, nPML, 
						_nx_, _ny_, _nz_, 0, rDH, FB1, DT );

	if ( border.isy1 ) pml_free_surface_y<<< blockY, thread >>>
					  ( h_W, W, 
						Aux4_1.h_Aux_y, Aux4_1.Aux_y,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,				
						pml_d_y, nPML, 
						_nx_, _ny_, _nz_, 0, rDH, FB2, DT );

	if ( border.isx2 ) pml_free_surface_x<<< blockX, thread >>>
	                  ( h_W, W, 
						Aux4_2.h_Aux_x, Aux4_2.Aux_x,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,	
						pml_d_x, nPML, 
						_nx_, _ny_, _nz_, 1, rDH, FB1, DT );

	if ( border.isy2 ) pml_free_surface_y<<< blockY, thread >>>	
					  ( h_W, W, 
						Aux4_2.h_Aux_y, Aux4_2.Aux_y,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,				
						pml_d_y, nPML, 
						_nx_, _ny_, _nz_, 1, rDH, FB2, DT );
#else
	if ( border.isx1 ) pml_free_surface_x
					  ( h_W, W, 
						Aux4_1.h_Aux_x, Aux4_1.Aux_x,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,	
						pml_d_x, nPML, 
						_nx_, _ny_, _nz_, 0, rDH, FB1, DT );

	if ( border.isy1 ) pml_free_surface_y
					  ( h_W, W, 
						Aux4_1.h_Aux_y, Aux4_1.Aux_y,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,				
						pml_d_y, nPML, 
						_nx_, _ny_, _nz_, 0, rDH, FB2, DT );

	if ( border.isx2 ) pml_free_surface_x
	                  ( h_W, W, 
						Aux4_2.h_Aux_x, Aux4_2.Aux_x,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,	
						pml_d_x, nPML, 
						_nx_, _ny_, _nz_, 1, rDH, FB1, DT );

	if ( border.isy2 ) pml_free_surface_y
					  ( h_W, W, 
						Aux4_2.h_Aux_y, Aux4_2.Aux_y,	
						ZT_X, ZT_Y, ZT_Z, 
						medium, _rDZ_DX, _rDZ_DY,				
						pml_d_y, nPML, 
						_nx_, _ny_, _nz_, 1, rDH, FB2, DT );


#endif //GPU_CUDA

}

