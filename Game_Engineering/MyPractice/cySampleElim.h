// cyCodeBase by Cem Yuksel
// [www.cemyuksel.com]
//-------------------------------------------------------------------------------
//! \file   cySampleElim.h 
//! \author Cem Yuksel
//!
//! \brief  Implementation of the weighted sample elimination method.
//!
//! This file includes an implementation of the weighted sample elimination 
//! method for generating Poisson disk sample sets.
//!
//! Blue noise (Poisson disk) sample sets produce high-quality sampling. They
//! often lead to lower noise and better convergence with Monte Carlo sampling.
//! They provide a uniform sample distribution over a sampling domain. Unlike
//! regular random sampling, Poisson disk sample sets avoid placing any two
//! samples too close together (determined by a Poisson disk radius).
//! 
//! The weighted sample elimination method implemented in this file generates a 
//! subset of samples with blue noise (Poisson disk) characteristics from a given 
//! input sample set. The weighted sample elimination method is simple, 
//! computationally efficient, and suitable for any sampling domain. It produces 
//! high-quality blue noise sample sets with a relatively large average Poisson 
//! disk radius without the need for specifying a Poisson disk radius. It also 
//! allows progressive (adaptive) sampling and it is efficient for high-
//! dimensional sampling. However, it does not guarantee maximal coverage.
//!
//! More details can be found in the original publication:
//!
//! Cem Yuksel. 2015. Sample Elimination for Generating Poisson Disk Sample Sets. 
//! Computer Graphics Forum 34, 2 (May 2015), 25-32. 
//! http://www.cemyuksel.com/research/sampleelimination/
//!
//-------------------------------------------------------------------------------
//
// Copyright (c) 2016, Cem Yuksel <cem@cemyuksel.com>
// All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
// 
//-------------------------------------------------------------------------------

#ifndef _CY_SAMPLE_ELIM_H_INCLUDED_
#define _CY_SAMPLE_ELIM_H_INCLUDED_

//-------------------------------------------------------------------------------

// #include "cyCore.h"
// #include "cyHeap.h"
// #include "cyPointCloud.h"

/*dependencies path editing*/
#include "cyPointCloud.h"
#include "cyCore.h"
#include "cyHeap.h"
#include <vector>

//-------------------------------------------------------------------------------
namespace cy {
//-------------------------------------------------------------------------------

//! 가중치 샘플 제거 방법의 구현
//!
//! Cem Yuksel. 2015. Sample Elimination for Generating Poisson Disk Sample Sets.
//! Computer Graphics Forum 34, 2 (May 2015), 25-32.
//! http://www.cemyuksel.com/research/sampleelimination/
//!
//! 이 클래스는 가중치 샘플 제거 알고리즘에 대한 여러 매개 변수를 유지합니다.
//! Eliminate 메서드에서 주요 알고리즘이 구현되어 있습니다.


template <typename PointType, typename FType, int DIMENSIONS, typename SIZE_TYPE=size_t>
class WeightedSampleElimination
{
public:
	//! 생성자는 기본 매개 변수를 설정합니다.
	WeightedSampleElimination()
	{
		for ( int d=0; d<DIMENSIONS; d++ ) {
			boundsMin[d] = FType(0);
			boundsMax[d] = FType(1);
		}
		alpha = FType(8);
		beta  = FType(0.65);
		gamma = FType(1.5);
		tiling = false;
		weightLimiting = true;
	}
	//! Tiling은 생성된 샘플이 타일링 가능한지 여부를 결정합니다.
	//! 기본적으로 Tiling은 꺼져 있지만 상자 모양의 샘플링 도메인의 경우 켜는 것이 좋습니다.
	//! Tiling이 꺼져 있으면, 가중치 샘플 제거는 샘플 도메인 경계 근처의 샘플을 제거하지 않을 가능성이 더 큽니다.
	//! Tiling을 켜면, 정확한 샘플 도메인 경계를 설정해야 합니다.
	void SetTiling( bool on=true ) { tiling = on; }

	//! Tiling 매개 변수가 켜져 있는지 여부를 반환합니다.
	bool IsTiling() const { return tiling; }

	//! Weight limiting은 기본 가중치 함수에서 사용되며 기본적으로 켜져 있습니다.
	//! 가중치 제한을 사용하면 일반적으로 더 강한 블루 노이즈 특성을 가집니다.
	//! 따라서 권장됩니다. 베타 매개 변수는 가중치 제한의 양을 결정합니다.
	//! 베타 매개 변수를 0으로 설정하면 가중치 제한이 꺼집니다.
	void SetWeightLimiting( bool on=true ) { weightLimiting = on; }

	//! Weight limiting이 켜져 있는지 여부를 반환합니다.
	bool IsWeightLimiting() const { return weightLimiting; }

	//! 샘플링 도메인의 최소 경계를 반환합니다.
	//! 샘플링 도메인 경계는 타일링 및 샘플링 도메인의 최대 가능한
	//! 포아송 디스크 반지름을 계산하는 데 사용됩니다. 기본 경계는 0과 1 사이입니다.
	PointType const & GetBoundsMin() const { return boundsMin; }

	//! 샘플링 도메인의 최대 경계를 반환합니다.
	//! 샘플링 도메인 경계는 타일링 및 샘플링 도메인의 최대 가능한
	//! 포아송 디스크 반지름을 계산하는 데 사용됩니다. 기본 경계는 0과 1 사이입니다.
	PointType const & GetBoundsMax() const { return boundsMax; }

	//! 샘플링 도메인의 최소 경계를 설정합니다.
	//! 샘플링 도메인 경계는 타일링 및 샘플링 도메인의 최대 가능한
	//! 포아송 디스크 반지름을 계산하는 데 사용됩니다. 기본 경계는 0과 1 사이입니다.
	void SetBoundsMin( PointType const &bmin ) { boundsMin = bmin; }

	//! 샘플링 도메인의 최대 경계를 설정합니다.
	//! 샘플링 도메인 경계는 타일링 및 샘플링 도메인의 최대 가능한
	//! 포아송 디스크 반지름을 계산하는 데 사용됩니다. 기본 경계는 0과 1 사이입니다.
	void SetBoundsMax( PointType const &bmax ) { boundsMax = bmax; }

	//! 기본 가중치 함수에서 사용되는 알파 매개 변수를 설정합니다. 
	void  SetParamAlpha( FType a ) { alpha = a; }

	//! 기본 가중치 함수에 사용되는 알파 매개변수를 반환합니다.
	FType GetParamAlpha() const { return alpha; }

	//! 기본 가중치 함수의 가중치 제한에 사용되는 베타 매개변수를 설정합니다.
	//! 베타 매개변수를 0으로 설정하면 가중치 제한이 꺼집니다.
	//! 가중치 제한이 꺼져 있으면 이 매개변수는 아무런 효과가 없습니다.
	void SetParamBeta ( FType b ) { beta = b; }

	//! 기본 가중치 함수의 가중치 제한에 사용되는 베타 매개변수를 반환합니다.
	FType GetParamBeta () const { return beta; }

	//! 기본 가중치 함수의 가중치 제한에 사용되는 감마 매개변수를 설정합니다.
	//! 감마 매개변수는 입력 및 출력 개수의 비율을 기반으로 가중치 제한을 조정합니다.
	//! 가중치 제한이 꺼져 있으면 이 매개변수는 아무런 효과가 없습니다.
	void SetParamGamma( FType c ) { gamma = c; }

	//! 기본 가중치 함수의 가중치 제한에 사용되는 감마 매개변수를 반환합니다.
	FType GetParamGamma() const { return gamma; }


	//! 이것은 샘플의 하위 집합을 선택하기 위해 가중 샘플 제거를 사용하는 주요 방법입니다.
	//주어진 입력 샘플 세트(입력 포인트)에서 블루 노이즈(푸아송 디스크) 특성을 가진 샘플 하위 집합을 선택하는 방법입니다. 
	//! 선택된 샘플이 출력 포인트에 복사됩니다. 출력 크기는 입력 크기보다 작아야 합니다.
	//! 
	//! 프로그레시브 파라미터가 참이면 출력 샘플 포인트는 프로그레시브 샘플링을 위해 정렬됩니다,
	// 샘플이 이 순서대로 하나씩 도입될 때 시퀀스의 각 부분 집합은 //!
	//! 블루 노이즈 특성을 나타냅니다.
	//! 
	//! d_max 매개변수는 가중 함수가 0이 아닌 반경을 정의합니다.
	//! 
	//! dimensions 매개변수는 샘플링 도메인의 차원을 지정합니다. 이 매개변수
	//는 일반적으로 샘플링 도메인의 차원(DIMENSIONS로 지정됨)과 동일합니다.
	//! 그러나 고차원 공간에서 저차원 다양체를 샘플링할 때는 더 작은 값을 사용할 수 있습니다.
	//공간에서 저차원 다양체를 샘플링할 때는 더 작은 값을 사용할 수 있습니다.
	//! 
	//! 가중치 함수는 가중치 샘플 제거의 중요한 구성 요소입니다. 이 함수는 샘플 포인트의 가중치를 계산합니다.
	//! 샘플 포인트의 최대 반경 내에 있는 이웃의 배치에 따라 가중치를 계산합니다. 가중치 함수
	//는 다음과 같은 형식을 가져야 합니다:
	//!
	//! FType weightFunction( PointType const &p0, PointType const &p1, FType dist2, FType d_max )
	//!
	//! 인자 p0 과 p1 은 인접한 두 점이고, dist2 는 이 두 점 사이의 유클리드 거리의 제곱입니다. 
	//! 제곱이며, d_max는 가중치 함수의 현재 반경입니다.
	//! 프로그레시브 매개변수가 켜져 있으면 가중치 함수에 전송되는 d_max 값이
	//이 메서드에 전달된 d_max 값과 다를 수 있습니다.
	template <typename WeightFunction>
	void Eliminate ( 
		PointType const *inputPoints, 
		SIZE_TYPE        inputSize, 
		PointType       *outputPoints, 
		SIZE_TYPE        outputSize, 
		bool             progressive,
		FType            d_max,
		int              dimensions,
		WeightFunction   weightFunction
		) const
	{
		assert( outputSize < inputSize );
		assert( dimensions <= DIMENSIONS && dimensions >= 2 );
		if ( d_max <= FType(0) ) d_max = 2 * GetMaxPoissonDiskRadius( dimensions, outputSize );
		DoEliminate( inputPoints, inputSize, outputPoints, outputSize, d_max, weightFunction, false );
		if ( progressive ) {
			std::vector<PointType> tmpPoints( outputSize );
			PointType *inPts  = outputPoints;
			PointType *outPts = tmpPoints.data();
			SIZE_TYPE inSize  = outputSize;
			SIZE_TYPE outSize = 0;
			while ( inSize >= 3 ) {
				outSize = inSize / 2;
				d_max *= ProgressiveRadiusMultiplier( dimensions );
				DoEliminate( inPts, inSize, outPts, outSize, d_max, weightFunction, true );
				if ( outPts != outputPoints ) MemCopy( outputPoints+outSize, outPts+outSize, inSize-outSize );
				PointType *tmpPts = inPts; inPts = outPts; outPts = tmpPts;
				inSize = outSize;
			}
			if ( inPts != outputPoints ) MemCopy( outputPoints, inPts, outSize );
		}
	}

	//! 이것은 샘플의 하위 집합을 선택하기 위해 가중 샘플 제거를 사용하는 주요 방법입니다.
	//주어진 입력 샘플 세트(입력 포인트)에서 블루 노이즈(푸아송 디스크) 특성을 가진 샘플 하위 집합을 선택하는 방법입니다. 
	//! 선택된 샘플이 출력 포인트에 복사됩니다. 출력 크기는 입력 크기보다 작아야 합니다.
	//! 이 메서드는 기본 가중치 함수를 사용합니다.
	//! 
	//! 프로그레시브 매개변수가 참이면 출력 샘플 포인트는 프로그레시브 샘플링을 위해 정렬됩니다,
	//샘플이 이 순서대로 하나씩 도입될 때 시퀀스의 각 하위 집합은
	//! 블루 노이즈 특성을 나타냅니다.
	//! 
	//! d_max 매개변수는 가중 함수가 0이 아닌 반경을 정의합니다. 이 매개변수
	//! 가 0(또는 음수)이면 샘플링 치수와 출력 세트의 크기인
	//! 출력 세트의 크기를 사용하여 자동으로 계산됩니다.
	//! 
	//! dimensions 매개변수는 샘플링 도메인의 차원을 지정합니다. 이 매개변수
	//! 는 일반적으로 샘플링 도메인의 차원(DIMENSIONS로 지정됨)과 동일합니다.
	//! 그러나 고차원 공간에서 저차원 다양체를 샘플링할 때는 더 작은 값을 사용할 수 있습니다.
	//공간에서 저차원 다양체를 샘플링할 때는 이보다 작은 값을 사용할 수 있습니다.
	void Eliminate ( 
		PointType const *inputPoints, 
		SIZE_TYPE        inputSize, 
		PointType       *outputPoints, 
		SIZE_TYPE        outputSize, 
		bool             progressive = false,
		FType            d_max = FType(0),
		int              dimensions = DIMENSIONS
		) const
	{
		if ( d_max <= FType(0) ) d_max = 2 * GetMaxPoissonDiskRadius( dimensions, outputSize );
		FType alpha = this->alpha;
		if ( weightLimiting ) {
			FType d_min = d_max * GetWeightLimitFraction( inputSize, outputSize );
			Eliminate( inputPoints, inputSize, outputPoints, outputSize, progressive, d_max, dimensions,
				[d_min, alpha] (PointType const &, PointType const &, FType d2, FType d_max)
				{
					FType d = Sqrt(d2);
					if ( d < d_min ) d = d_min;
					return std::pow( FType(1) - d/d_max, alpha );
				}
			);
		} else {
			Eliminate( inputPoints, inputSize, outputPoints, outputSize, progressive, d_max, dimensions,
				[alpha] (PointType const &, PointType const &, FType d2, FType d_max)
				{
					FType d = Sqrt(d2);
					return std::pow( FType(1) - d/d_max, alpha );
				}
			);
		}
	}

	//! 주어진 샘플 수에 대해 주어진 치수에서 가능한 최대 푸아송 디스크 반경을 반환합니다.
	//에 대해 주어진 도메인 사이즈에 퍼질 수 있는 최대 푸아송 디스크 반경을 반환합니다. domainSize 인자가 0이거나 음수이면, 이 값은 //!
	//최소 및 최대 경계로 정의된 상자의 면적 또는 N차원 부피로 계산됩니다.
	//! 이 메서드는 기본 가중치 함수에 사용됩니다.
	FType GetMaxPoissonDiskRadius( int dimensions, SIZE_TYPE sampleCount, FType domainSize = 0 ) const
	{
		assert( dimensions >= 2 );
		if ( domainSize <= FType(0) ) {
			domainSize = boundsMax[0] - boundsMin[0];
			for ( int d=1; d<DIMENSIONS; d++ ) domainSize *= boundsMax[d] - boundsMin[d];
		}
		FType sampleArea = domainSize / (FType) sampleCount;
		FType r_max;
		switch ( dimensions ) {
		case 2: r_max = Sqrt( sampleArea / ( FType(2) * Sqrt(FType(3)) ) ); break;
		case 3: r_max = std::pow ( sampleArea / ( FType(4) * Sqrt(FType(2)) ), FType(1)/FType(3) ); break;
		default:
			FType c;
			int d_start;
			if ( (dimensions & 1) ) { c = FType(2);      d_start = 3; }
			else                    { c = Pi<FType>(); d_start = 4; }
			for ( int d=d_start; d<=dimensions; d+=2 ) c *= FType(2) * Pi<FType>() / FType(d);
			r_max = std::pow( sampleArea / c, FType(1)/FType(dimensions) );
			break;
		}
		return r_max;
	}


private:
	PointType boundsMin;			// The minimum bounds of the sampling domain.
	PointType boundsMax;			// The maximum bounds of the sampling domain.
	FType     alpha, beta, gamma;	// Parameters of the default weight function.
	bool      weightLimiting;		// Specifies whether weight limiting is used with the default weight function.
	bool      tiling;				// Specifies whether the sampling domain is tiled.

	// Reflects a point near the bounds of the sampling domain off of all domain bounds for tiling.
	template <typename OPERATION>
	void TilePoint( SIZE_TYPE index, PointType const &point, FType d_max, OPERATION operation, int dim=0 ) const
	{
		for ( int d=dim; d<DIMENSIONS; d++ ) {
			if ( boundsMax[d] - point[d] < d_max ) {
				PointType p = point;
				p[d] -= boundsMax[d] - boundsMin[d];
				operation( index, p );
				TilePoint( index, p, d_max, operation, d+1 );
			}
			if ( point[d] - boundsMin[d] < d_max ) {
				PointType p = point;
				p[d] += boundsMax[d] - boundsMin[d];
				operation( index, p );
				TilePoint( index, p, d_max, operation, d+1 );
			}
		}
	}

	// This is the method that performs weighted sample elimination.
	template <typename WeightFunction>
	void DoEliminate( 
		PointType const *inputPoints, 
		SIZE_TYPE        inputSize, 
		PointType       *outputPoints, 
		SIZE_TYPE        outputSize, 
		FType            d_max,
		WeightFunction   weightFunction,
		bool             copyEliminated
		) const
	{
		// Build a k-d tree for samples
		PointCloud<PointType,FType,DIMENSIONS,SIZE_TYPE> kdtree;
		if ( tiling ) {
			std::vector<PointType> point(inputPoints, inputPoints + inputSize);
			std::vector<SIZE_TYPE> index(inputSize);
			for ( SIZE_TYPE i=0; i<inputSize; i++ ) index[i] = i;
			auto AppendPoint = [&]( SIZE_TYPE ix, PointType const &pt ) {
				point.push_back(pt);
				index.push_back(ix);
			};
			for ( SIZE_TYPE i=0; i<inputSize; i++ ) TilePoint( i, inputPoints[i], d_max, AppendPoint );
			kdtree.Build( point.size(), point.data(), index.data() );
		} else {
			kdtree.Build( inputSize, inputPoints );
		}

		// Assign weights to each sample
		std::vector<FType> w( inputSize, FType(0) );
		auto AddWeights = [&]( SIZE_TYPE index, PointType const &point ) {
			kdtree.GetPoints( point, d_max, [&weightFunction,d_max,&w,index,&point,&inputSize]( SIZE_TYPE i, PointType const &p, FType d2, FType & ){
				if ( i >= inputSize ) return;
				if ( i != index ) w[index] += weightFunction(point,p,d2,d_max);
			} );
		};
		for ( SIZE_TYPE i=0; i<inputSize; i++ ) AddWeights( i, inputPoints[i] );

		// Build a heap for the samples using their weights
		MaxHeap<FType,SIZE_TYPE> heap;
		heap.SetDataPointer( w.data(), inputSize );
		heap.Build();

		// While the number of samples is greater than desired
		auto RemoveWeights = [&]( SIZE_TYPE index, PointType const &point ) {
			kdtree.GetPoints( point, d_max, [&weightFunction,d_max,&w,index,&point,&heap,&inputSize]( SIZE_TYPE i, PointType const &p, FType d2, FType & ){
				if ( i >= inputSize ) return;
				if ( i != index ) {
					w[i] -= weightFunction(point,p,d2,d_max);
					heap.MoveItemDown(i);
				}
			} );
		};
		SIZE_TYPE sampleSize = inputSize;
		while ( sampleSize > outputSize ) {
			// Pull the top sample from heap
			SIZE_TYPE i = heap.GetTopItemID();
			heap.Pop();
			// For each sample around it, remove its weight contribution and update the heap
			RemoveWeights( i, inputPoints[i] );
			sampleSize--;
		}

		// Copy the samples to the output array
		SIZE_TYPE targetSize = copyEliminated ? inputSize : outputSize;
		for ( SIZE_TYPE i=0; i<targetSize; i++ ) {
			outputPoints[i] = inputPoints[ heap.GetIDFromHeap(i) ];
		}
	}

	// Returns the change in weight function radius using half of the number of samples. It is used for progressive sampling.
	FType ProgressiveRadiusMultiplier(int dimensions) const { return dimensions==2 ? Sqrt(FType(2)) : std::pow(FType(2), FType(1)/FType(dimensions)); }

	// Returns the minimum radius fraction used by the default weight function.
	FType GetWeightLimitFraction( SIZE_TYPE inputSize, SIZE_TYPE outputSize ) const
	{
		FType ratio = FType(outputSize) / FType(inputSize);
		return ( 1 - std::pow( ratio, gamma ) ) * beta;
	}
};

//-------------------------------------------------------------------------------

#ifdef _CY_VECTOR_H_INCLUDED_
template <typename T> _CY_TEMPLATE_ALIAS( WeightedSampleElimination2, (WeightedSampleElimination<Vec2<T>,T,2>) );	//!< Weighted sample elimination in 2D
template <typename T> _CY_TEMPLATE_ALIAS( WeightedSampleElimination3, (WeightedSampleElimination<Vec3<T>,T,3>) );	//!< Weighted sample elimination in 3D
template <typename T> _CY_TEMPLATE_ALIAS( WeightedSampleElimination4, (WeightedSampleElimination<Vec4<T>,T,4>) );	//!< Weighted sample elimination in 4D

typedef WeightedSampleElimination<Vec2f,float,2>  WeightedSampleElimination2f;	//!< Weighted sample elimination in 2D with float  type elements
typedef WeightedSampleElimination<Vec3f,float,3>  WeightedSampleElimination3f;	//!< Weighted sample elimination in 3D with float  type elements
typedef WeightedSampleElimination<Vec4f,float,4>  WeightedSampleElimination4f;	//!< Weighted sample elimination in 4D with float  type elements

typedef WeightedSampleElimination<Vec2d,double,2> WeightedSampleElimination2d;	//!< Weighted sample elimination in 2D with double type elements
typedef WeightedSampleElimination<Vec3d,double,3> WeightedSampleElimination3d;	//!< Weighted sample elimination in 3D with double type elements
typedef WeightedSampleElimination<Vec4d,double,4> WeightedSampleElimination4d;	//!< Weighted sample elimination in 4D with double type elements

template <typename T, uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( WeightedSampleEliminationN, (WeightedSampleElimination<Vec<T,DIMENSIONS>,T,DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions
template <uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( WeightedSampleEliminationNf , (WeightedSampleEliminationN<float,   DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions with single precision (float)
template <uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( WeightedSampleEliminationNd , (WeightedSampleEliminationN<double,  DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions with double precision (double)
#endif

//-------------------------------------------------------------------------------
} // namespace cy
//-------------------------------------------------------------------------------

#ifdef _CY_VECTOR_H_INCLUDED_
template <typename T> _CY_TEMPLATE_ALIAS( cyWeightedSampleElimination2, (cy::WeightedSampleElimination<cy::Vec2<T>,T,2>) );	//!< Weighted sample elimination in 2D
template <typename T> _CY_TEMPLATE_ALIAS( cyWeightedSampleElimination3, (cy::WeightedSampleElimination<cy::Vec3<T>,T,3>) );	//!< Weighted sample elimination in 3D
template <typename T> _CY_TEMPLATE_ALIAS( cyWeightedSampleElimination4, (cy::WeightedSampleElimination<cy::Vec4<T>,T,4>) );	//!< Weighted sample elimination in 4D

typedef cy::WeightedSampleElimination<cy::Vec2f,float,2>  cyWeightedSampleElimination2f;	//!< Weighted sample elimination in 2D with float  type elements
typedef cy::WeightedSampleElimination<cy::Vec3f,float,3>  cyWeightedSampleElimination3f;	//!< Weighted sample elimination in 3D with float  type elements
typedef cy::WeightedSampleElimination<cy::Vec4f,float,4>  cyWeightedSampleElimination4f;	//!< Weighted sample elimination in 4D with float  type elements

typedef cy::WeightedSampleElimination<cy::Vec2d,double,2> cyWeightedSampleElimination2d;	//!< Weighted sample elimination in 2D with double type elements
typedef cy::WeightedSampleElimination<cy::Vec3d,double,3> cyWeightedSampleElimination3d;	//!< Weighted sample elimination in 3D with double type elements
typedef cy::WeightedSampleElimination<cy::Vec4d,double,4> cyWeightedSampleElimination4d;	//!< Weighted sample elimination in 4D with double type elements

template <typename T, uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( cyWeightedSampleEliminationN, (cy::WeightedSampleElimination<cy::Vec<T,DIMENSIONS>,T,DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions
template <uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( cyWeightedSampleEliminationNf , (cyWeightedSampleEliminationN<float,   DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions with single precision (float)
template <uint32_t DIMENSIONS> _CY_TEMPLATE_ALIAS( cyWeightedSampleEliminationNd , (cyWeightedSampleEliminationN<double,  DIMENSIONS>) );	//!< Weighted sample elimination in N dimensions with double precision (double)
#endif

//-------------------------------------------------------------------------------

#endif
